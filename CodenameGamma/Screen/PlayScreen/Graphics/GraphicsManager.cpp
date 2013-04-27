#include "GraphicsManager.h"
#include "RenderStates.h"
#include "Effects.h"


GraphicsManager::GraphicsManager(ID3D11Device *device, ID3D11DeviceContext *deviceContext, ID3D11RenderTargetView *renderTargetView, int width, int height)
{
	m_Device			= device;
	m_DeviceContext		= deviceContext;
	m_RenderTargetView	= renderTargetView;

	m_Width = width;
	m_Height = height;

	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = (float)0;
	m_ViewPort.TopLeftY = (float)0;
	m_ViewPort.Width	= (float)width;
	m_ViewPort.Height	= (float)height;

	BoundingBox world = BoundingBox(XMFLOAT3(2000,0,2000), XMFLOAT3(2000, 2000, 2000));

	m_InstanceTree = new QuadTree(world, 8);


	InitFullScreenQuad();
	InitBuffers();

	m_DirLightBuffer = new StructuredBuffer<DirectionalLight>(m_Device, 1, D3D11_BIND_SHADER_RESOURCE, true);
	m_PointLightBuffer = new StructuredBuffer<PointLight>(m_Device, 1, D3D11_BIND_SHADER_RESOURCE, true);
	m_SpotLightBuffer = new StructuredBuffer<SpotLight>(m_Device, 1, D3D11_BIND_SHADER_RESOURCE, true);

	m_DirLights		= NULL;
	m_PointLights	= NULL;
	m_SpotLights	= NULL;
	m_ShadowMapSRV	= NULL;
	m_ShadowMapDSV	= NULL;

}

GraphicsManager::~GraphicsManager(void)
{
}

void GraphicsManager::InitFullScreenQuad()
{

	std::vector<Vertex::Quad> vertices(4);

	Vertex::Quad v1,v2,v3,v4;

	//upper left
	v1.Pos = XMFLOAT2(-1, 1);
	v1.Tex = XMFLOAT2(0, 0);
	//upper right
	v2.Pos = XMFLOAT2(1, 1);
	v2.Tex = XMFLOAT2(1, 0);
	//lower left
	v3.Pos = XMFLOAT2(-1, -1);
	v3.Tex = XMFLOAT2(0, 1);
	//lower right
	v4.Pos = XMFLOAT2(1, -1);
	v4.Tex = XMFLOAT2(1, 1);

	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
	vertices[3] = v4;

	/*
	Vertex::Quad vertices[4];
	//upper left
	vertices[0].Pos = XMFLOAT2(-1, 1);
	vertices[0].Tex = XMFLOAT2(0, 0);
	//upper right
	vertices[1].Pos = XMFLOAT2(1, 1);
	vertices[1].Tex = XMFLOAT2(1, 0);
	//lower left
	vertices[2].Pos = XMFLOAT2(-1, -1);
	vertices[2].Tex = XMFLOAT2(0, 1);
	//lower right
	vertices[3].Pos = XMFLOAT2(1, -1);
	vertices[3].Tex = XMFLOAT2(1, 1);
	*/

	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Quad) * 4;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];

	m_Device->CreateBuffer(&vbd, &vinitData, &m_FullSceenQuad);
}

void GraphicsManager::InitBuffers()
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = m_Width;
	texDesc.Height = m_Height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	//CREATES TEXTURES
	ID3D11Texture2D* albedoTex		= NULL;
	ID3D11Texture2D* depthTex		= NULL;
	ID3D11Texture2D* normalSpecTex	= NULL;
	ID3D11Texture2D* finalTex		= NULL;

	
	//R16G16B16A16_UNORM
	//texDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	m_Device->CreateTexture2D(&texDesc, NULL, &normalSpecTex);

	//R8G8B8A8_UNORM
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_Device->CreateTexture2D(&texDesc, NULL, &albedoTex);

	//D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_Device->CreateTexture2D(&texDesc, NULL, &finalTex);

	//R24G8_TYPELESS & D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	m_Device->CreateTexture2D(&texDesc, NULL, &depthTex);


	//RENDER TARGETS
	m_Device->CreateRenderTargetView( albedoTex,		NULL, &m_AlbedoRTV );
	m_Device->CreateRenderTargetView( normalSpecTex,	NULL, &m_NormalSpecRTV );
	
	//GBUFFER
	GBuffer[0] = m_AlbedoRTV;
	GBuffer[1] = m_NormalSpecRTV;

	//SHADERRESOURCEVIEWS
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;
	
	//R8G8B8A8_UNORM
	m_Device->CreateShaderResourceView(albedoTex, &SRVDesc, &m_AlbedoSRV);
	m_Device->CreateShaderResourceView(finalTex, 0, &m_FinalSRV);

	//DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	m_Device->CreateShaderResourceView(depthTex, &SRVDesc, &m_DepthSRV);

	//DXGI_FORMAT_R16G16B16A16_UNORM
	SRVDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	m_Device->CreateShaderResourceView(normalSpecTex, 0, &m_NormalSpecSRV);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	m_Device->CreateDepthStencilView( depthTex, &descDSV, &m_DepthStencilView );

	//Create the UnorderedAccessView
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	m_Device->CreateUnorderedAccessView(finalTex, 0, &m_FinalUAV);


	//RELEASE
	albedoTex->Release();
	normalSpecTex->Release();	
	depthTex->Release();
	finalTex->Release();
}

void GraphicsManager::InitShadowMap(int width, int height)
{
	SAFE_RELEASE(m_ShadowMapSRV);
	SAFE_RELEASE(m_ShadowMapDSV);
	//Texture desc
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	//Create texture
	ID3D11Texture2D* texture = NULL;
	m_Device->CreateTexture2D(&texDesc, NULL, &texture);

	//SRV desc
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	//Create SRV
	m_Device->CreateShaderResourceView(texture, &SRVDesc, &m_ShadowMapSRV);

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc;
	ZeroMemory(&DSVdesc, sizeof(DSVdesc));
	DSVdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVdesc.Texture2D.MipSlice = 0;

	m_Device->CreateDepthStencilView( texture, &DSVdesc, &m_ShadowMapDSV );

	//Release texture
	texture->Release();
}

void GraphicsManager::UpdateLights()
{
	//Update DirecionalLights.
	if (m_DirLights != NULL)
	{
		if (m_DirLightBuffer->Size() != m_DirLights->size())
		{
			delete m_DirLightBuffer;
			m_DirLightBuffer = new StructuredBuffer<DirectionalLight>(m_Device, m_DirLights->size() == 0 ? 1 : m_DirLights->size(), D3D11_BIND_SHADER_RESOURCE, true);
		}
		if (m_DirLightBuffer->Size() > 0)
		{
			DirectionalLight* dirLight = m_DirLightBuffer->MapDiscard(m_DeviceContext);
			for (unsigned int i = 0; i < m_DirLights->size(); ++i) 
			{
				dirLight[i] = *m_DirLights->at(i);
				//dirLight[i]->Direction TRANSFORM TO VIEWSPACE
			}
			m_DirLightBuffer->Unmap(m_DeviceContext);
		}
	}
	else if (m_DirLightBuffer->Size() != 0)
	{
		delete m_DirLightBuffer;
		m_DirLightBuffer = new StructuredBuffer<DirectionalLight>(m_Device, 1, D3D11_BIND_SHADER_RESOURCE, true);
	}

	//Update PointLights.
	if (m_PointLights != NULL)
	{
		if (m_PointLightBuffer->Size() != m_PointLights->size())
		{
			delete m_PointLightBuffer;
			m_PointLightBuffer = new StructuredBuffer<PointLight>(m_Device, m_PointLights->size() == 0 ? 1 : m_PointLights->size(), D3D11_BIND_SHADER_RESOURCE, true);
		}
		if (m_PointLightBuffer->Size() > 0)
		{
			PointLight* pointLight = m_PointLightBuffer->MapDiscard(m_DeviceContext);
			for (unsigned int i = 0; i < m_PointLights->size(); ++i) 
			{
				pointLight[i] = *m_PointLights->at(i);
				//pointLight[i].Position TRANSFORM TO VIEWSPACE
			}
			m_PointLightBuffer->Unmap(m_DeviceContext);
		}
	}
	else if (m_PointLightBuffer->Size() != 0)
	{
		delete m_PointLightBuffer;
		m_PointLightBuffer = new StructuredBuffer<PointLight>(m_Device, 1, D3D11_BIND_SHADER_RESOURCE, true);
	}

	//Update SpotLights.
	if (m_SpotLights != NULL)
	{
		if (m_SpotLightBuffer->Size() != m_SpotLights->size())
		{
			delete m_SpotLightBuffer;
			m_SpotLightBuffer = new StructuredBuffer<SpotLight>(m_Device, m_SpotLights->size() == 0 ? 1 : m_SpotLights->size(), D3D11_BIND_SHADER_RESOURCE, true);
		}
		if (m_SpotLightBuffer->Size() > 0)
		{
			SpotLight* spotLight = m_SpotLightBuffer->MapDiscard(m_DeviceContext);
			for (unsigned int i = 0; i < m_SpotLights->size(); ++i) 
			{
				spotLight[i] = *m_SpotLights->at(i);			
				//spotLight[i].Position TRANSFORM TO VIEWSPACE
				//spotLight[i].Direction TRANSFORM TO VIEWSPACE
			}
			m_SpotLightBuffer->Unmap(m_DeviceContext);
		}
	}
	else if (m_SpotLightBuffer->Size() != 0)
	{
		delete m_SpotLightBuffer;
		m_SpotLightBuffer = new StructuredBuffer<SpotLight>(m_Device, 1, D3D11_BIND_SHADER_RESOURCE, true);
	}



	/*
	DirectionalLight* dirLight = gDirLightBuffer->MapDiscard(gDeviceContext);
        for (unsigned int i = 0; i < gDirLights.size(); ++i) 
		{
            dirLight[i] = *gDirLights[i];
			dirLight->Direction
        }
        mLightBuffer->Unmap(d3dDeviceContext);
		*/
}

void GraphicsManager::ClearBuffers()
{
	m_DeviceContext->RSSetViewports( 1, &m_ViewPort );
	m_DeviceContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	float AlbedoClearColor[4] = {0.1f,  0.1f, 0.1f, 0.0f};
	float ClearColor[4] = {0.0f,  0.0f, 0.0f, 0.0f};
	m_DeviceContext->ClearRenderTargetView( m_RenderTargetView, ClearColor );
	m_DeviceContext->ClearRenderTargetView( m_AlbedoRTV,		AlbedoClearColor );
	m_DeviceContext->ClearRenderTargetView( m_NormalSpecRTV,	ClearColor );
	m_DeviceContext->ClearUnorderedAccessViewFloat( m_FinalUAV, ClearColor );
	/*
	m_DeviceContext->OMSetRenderTargets( 2, GBuffer, NULL );
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	

	ID3DX11EffectTechnique* tech;
	D3DX11_TECHNIQUE_DESC techDesc;

	tech = Effects::ClearGBufferFX->ClearTech;
	tech->GetDesc( &techDesc );

	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		tech->GetPassByIndex(p)->Apply(0, m_DeviceContext);

		UINT offset = 0;
		UINT vertexStride = sizeof(Vertex::Quad);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_FullSceenQuad, &vertexStride, &offset);

		m_DeviceContext->Draw(4, 0);
	}
	*/
}

void GraphicsManager::FillGBuffer(vector<Player*>& players)
{
	m_DeviceContext->OMSetRenderTargets( 2, GBuffer, m_DepthStencilView );

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_DeviceContext->OMSetDepthStencilState(RenderStates::LessDDS, 0);

	m_DeviceContext->RSSetState(RenderStates::NoCullRS);
	//m_DeviceContext->RSSetState(RenderStates::WireframeRS);
	
	for (int i = 0; i < (int)players.size(); ++i)
	{
		RenderTerrain(players[i]);

		Camera* camera = players[i]->GetCamera();
		m_DeviceContext->RSSetViewports( 1, &camera->GetViewPort() );
		XMMATRIX view;
		XMMATRIX proj;

		view = XMLoadFloat4x4(&camera->GetView());
		proj = XMLoadFloat4x4(&camera->GetProjection());

		ID3DX11EffectTechnique* tech;
		D3DX11_TECHNIQUE_DESC techDesc;
	
		m_DeviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
		tech = Effects::ObjectDeferredFX->TexTech;
		//tech = Effects::ObjectDeferredFX->TexNormalTech;
		tech->GetDesc( &techDesc );

	
		vector<ModelInstance*> instances;
		BoundingFrustum frustum = camera->GetFrustum();	
		m_InstanceTree->GetIntersectingInstances(frustum, instances);
		sort( instances.begin(), instances.end() );
		instances.erase( unique( instances.begin(), instances.end() ), instances.end() );

		cout << instances.size() << "\t";

		for(UINT p = 0; p < techDesc.Passes; ++p)
		{
			//for each (ModelInstance* instance in m_modelInstances)
			for each (ModelInstance* instance in instances)
				RenderModel(*instance, view, proj, tech, p);
		}
	}
	cout << endl;

	m_DeviceContext->OMSetRenderTargets( 0, 0, 0 );
}

void GraphicsManager::RenderModel(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass)
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;
	XMMATRIX tex = XMMatrixTranslation(1, 1, 1);

	world = XMLoadFloat4x4(&instance.m_World);
	//float a = MathHelper::InverseTranspose(world);
	
	
	worldView     = XMMatrixMultiply(world, view);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	
	//worldInvTransposeView = worldInvTranspose*view;
	worldViewProj = XMMatrixMultiply(worldView, proj);

	Effects::ObjectDeferredFX->SetWorld(world);
	Effects::ObjectDeferredFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::ObjectDeferredFX->SetTexTransform(tex);
	Effects::ObjectDeferredFX->SetWorldViewProj(worldViewProj);

	for(UINT subset = 0; subset < instance.m_Model->SubsetCount; ++subset)
	{
		//UINT subset = 6;
		Effects::ObjectDeferredFX->SetMaterial(instance.m_Model->Mat[subset]);

		Effects::ObjectDeferredFX->SetDiffuseMap(instance.m_Model->DiffuseMapSRV[subset]);
		Effects::ObjectDeferredFX->SetNormalMap(instance.m_Model->NormalMapSRV[subset]);

		tech->GetPassByIndex(pass)->Apply(0, m_DeviceContext);
		instance.m_Model->ModelMesh.Draw(m_DeviceContext, subset);
	}
}

void GraphicsManager::RenderTerrain(Player* player)
{
	Camera* camera = player->GetCamera();
	m_DeviceContext->RSSetViewports( 1, &camera->GetViewPort() );
	XMMATRIX view;
	XMMATRIX proj;

	view = XMLoadFloat4x4(&camera->GetView());
	proj = XMLoadFloat4x4(&camera->GetProjection());

	ID3DX11EffectTechnique* tech;
	D3DX11_TECHNIQUE_DESC techDesc;
	
	m_DeviceContext->IASetInputLayout(InputLayouts::Terrain);
	tech = Effects::TerrainDeferredFX->TexTech;
	//tech = Effects::TerrainDeferredFX->TexNormalTech;
	tech->GetDesc( &techDesc );
	
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		XMMATRIX world;

		XMMATRIX worldView;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		XMMATRIX tex = m_Terrain->GetTexTransform();

		world = XMMatrixTranslation(0, 0, 0);
		//float a = MathHelper::InverseTranspose(world);
	
	
		worldView     = XMMatrixMultiply(world, view);
		worldInvTranspose = MathHelper::InverseTranspose(world);
	
		//worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = XMMatrixMultiply(worldView, proj);

		Effects::TerrainDeferredFX->SetWorld(world);
		Effects::TerrainDeferredFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::TerrainDeferredFX->SetTexTransform(tex);
		Effects::TerrainDeferredFX->SetWorldViewProj(worldViewProj);


		//UINT subset = 6;
		Effects::TerrainDeferredFX->SetMaterial(m_Terrain->GetMaterial());

		Effects::TerrainDeferredFX->SetGround1Map(m_Terrain->GetGroundTexture(0));
		Effects::TerrainDeferredFX->SetGround2Map(m_Terrain->GetGroundTexture(1));
		Effects::TerrainDeferredFX->SetGround3Map(m_Terrain->GetGroundTexture(2));
		Effects::TerrainDeferredFX->SetGround4Map(m_Terrain->GetGroundTexture(3));
		Effects::TerrainDeferredFX->SetNormal1Map(m_Terrain->GetGroundNormal(0));
		Effects::TerrainDeferredFX->SetNormal2Map(m_Terrain->GetGroundNormal(1));
		Effects::TerrainDeferredFX->SetNormal3Map(m_Terrain->GetGroundNormal(2));
		Effects::TerrainDeferredFX->SetNormal4Map(m_Terrain->GetGroundNormal(3));

		tech->GetPassByIndex(p)->Apply(0, m_DeviceContext);
		m_Terrain->Draw(m_DeviceContext);
		
	}
}

void GraphicsManager::ComputeLight(vector<Player*>& players)
{
	std::vector<XMFLOAT4>	camPositions;
	std::vector<XMFLOAT4X4> invViewProjs;
	for each (Player* player in players)
	{
		XMFLOAT3 temp	= player->GetCamera()->GetPosition();
		XMFLOAT4 camPos = XMFLOAT4(temp.x, temp.y, temp.z, 1);
		camPositions.push_back(camPos);

		XMMATRIX	viewMatrix;
		XMMATRIX	projMatrix;
		XMMATRIX	invViewProjMatrix;
		XMFLOAT4X4	invViewProj;

		viewMatrix = XMLoadFloat4x4(&player->GetCamera()->GetView());
		projMatrix = XMLoadFloat4x4(&player->GetCamera()->GetProjection());

		invViewProjMatrix = XMMatrixMultiply(viewMatrix, projMatrix);

		XMVECTOR det		= XMMatrixDeterminant(invViewProjMatrix);
		invViewProjMatrix	= XMMatrixInverse(&det, invViewProjMatrix);

		XMStoreFloat4x4(&invViewProj, invViewProjMatrix);
		invViewProjs.push_back(invViewProj);
	}


	D3DX11_TECHNIQUE_DESC	techDesc;
	ID3DX11EffectTechnique* tech;
	int numViewports = players.size();

	switch (numViewports)
	{
	case 1:
		tech = Effects::TiledLightningFX->Viewport1;
		break;
	case 2:
		tech = Effects::TiledLightningFX->Viewport2;
		break;
	case 3:
		tech = Effects::TiledLightningFX->Viewport3;
		break;
	case 4:
		tech = Effects::TiledLightningFX->Viewport4;
		break;
	default:
		return;
	}
	
	tech->GetDesc( &techDesc );

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		Effects::TiledLightningFX->SetInvViewProjs(&invViewProjs[0], invViewProjs.size());
		Effects::TiledLightningFX->SetCamPositions(&camPositions[0], camPositions.size());
		Effects::TiledLightningFX->SetResolution(XMFLOAT2((float)m_Width, (float)m_Height));

		Effects::TiledLightningFX->SetAlbedoMap(m_AlbedoSRV);
		Effects::TiledLightningFX->SetNormalSpecMap(m_NormalSpecSRV);
		Effects::TiledLightningFX->SetDepthMap(m_DepthSRV);
		Effects::TiledLightningFX->SetOutputMap(m_FinalUAV);

		Effects::TiledLightningFX->SetDirLightMap(m_DirLightBuffer->GetShaderResource());
		Effects::TiledLightningFX->SetPointLightMap(m_PointLightBuffer->GetShaderResource());
		Effects::TiledLightningFX->SetSpotLightMap(m_SpotLightBuffer->GetShaderResource());

		tech->GetPassByIndex(p)->Apply(0, m_DeviceContext);

		// How many groups do we need to dispatch to cover a row of pixels, where each
		// group covers 256 pixels (the 256 is defined in the ComputeShader).
		UINT numGroupsX = (UINT)ceilf(m_Width / 16.0f);
		UINT numGroupsY = (UINT)ceilf(m_Height / 16.0f);
		m_DeviceContext->Dispatch(numGroupsX, numGroupsY, 1);
	}


	// Unbind the input texture from the CS for good housekeeping.
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	m_DeviceContext->CSSetShaderResources( 0, 1, nullSRV );

	// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
	// and a resource cannot be both an output and input at the same time.
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	m_DeviceContext->CSSetUnorderedAccessViews( 0, 1, nullUAV, 0 );

	
	Effects::TiledLightningFX->SetAlbedoMap(NULL);
	Effects::TiledLightningFX->SetNormalSpecMap(NULL);
	Effects::TiledLightningFX->SetDepthMap(NULL);
	Effects::TiledLightningFX->SetOutputMap(NULL);

	tech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
	m_DeviceContext->CSSetShader(0, 0, 0);
}

void GraphicsManager::CombineFinal()
{
	m_DeviceContext->RSSetState(RenderStates::NoCullRS);
	m_DeviceContext->RSSetViewports( 1, &m_ViewPort );
	m_DeviceContext->OMSetRenderTargets( 1, &m_RenderTargetView, NULL );
	m_DeviceContext->IASetInputLayout(InputLayouts::Quad);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	

	ID3DX11EffectTechnique* tech;
	D3DX11_TECHNIQUE_DESC techDesc;

	tech = Effects::CombineFinalFX->CombineTech;
	tech->GetDesc( &techDesc );

	Effects::CombineFinalFX->SetAlbedo(m_FinalSRV);

	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		tech->GetPassByIndex(p)->Apply(0, m_DeviceContext);

		UINT offset = 0;
		UINT vertexStride = sizeof(Vertex::Quad);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_FullSceenQuad, &vertexStride, &offset);
		m_DeviceContext->Draw(4, 0);
	}

	Effects::CombineFinalFX->SetAlbedo(NULL);
	Effects::CombineFinalFX->SetDiffuse(NULL);
	Effects::CombineFinalFX->SetSpecular(NULL);
	tech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
}

void GraphicsManager::Render(vector<Player*>& players)
{
	UpdateLights();
	ClearBuffers();
	FillGBuffer(players);
	ComputeLight(players);
	CombineFinal();
}