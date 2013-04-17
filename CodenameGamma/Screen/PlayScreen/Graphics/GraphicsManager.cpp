#include "GraphicsManager.h"
#include "RenderStates.h"
#include "Effects.h"


GraphicsManager::GraphicsManager(ID3D11Device *device, ID3D11DeviceContext *deviceContext, ID3D11RenderTargetView *renderTargetView, ID3D11DepthStencilView *depthStencilView, int width, int height)
{
	m_Device			= device;
	m_DeviceContext		= deviceContext;
	m_RenderTargetView	= renderTargetView;
	m_DepthStencilView	= depthStencilView;

	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = (float)0;
	m_ViewPort.TopLeftY = (float)0;
	m_ViewPort.Width	= (float)width;
	m_ViewPort.Height	= (float)height;

	BoundingBox world = BoundingBox(XMFLOAT3(2000,0,2000), XMFLOAT3(2000, 2000, 2000));

	m_InstanceTree = new QuadTree(world, 8);


	InitFullScreenQuad();
	InitBuffers(width, height);


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

void GraphicsManager::InitBuffers(int width, int height)
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	//CREATES TEXTURES
	ID3D11Texture2D* albedoTex		= NULL;
	ID3D11Texture2D* depthTex		= NULL;
	ID3D11Texture2D* normalSpecTex	= NULL;
	ID3D11Texture2D* diffuseTex		= NULL;
	ID3D11Texture2D* specularTex	= NULL;

	//R8G8B8A8_UNORM
	m_Device->CreateTexture2D(&texDesc, NULL, &albedoTex);
	m_Device->CreateTexture2D(&texDesc, NULL, &diffuseTex);
	m_Device->CreateTexture2D(&texDesc, NULL, &specularTex);
	
	//R16G16B16A16_UNORM
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	m_Device->CreateTexture2D(&texDesc, NULL, &normalSpecTex);

	//R24G8_TYPELESS
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	m_Device->CreateTexture2D(&texDesc, NULL, &depthTex);


	//RENDER TARGETS
	m_Device->CreateRenderTargetView( albedoTex,		NULL, &m_AlbedoRTV );
	m_Device->CreateRenderTargetView( normalSpecTex,	NULL, &m_NormalSpecRTV );	
	m_Device->CreateRenderTargetView( diffuseTex,		NULL, &m_DiffuseLightRTV );
	m_Device->CreateRenderTargetView( specularTex,		NULL, &m_SpecularLightRTV );
	
	//GBUFFER
	GBuffer[0] = m_AlbedoRTV;
	GBuffer[1] = m_NormalSpecRTV;
	//LIGHTBUFFER
	LBuffer[0] = m_DiffuseLightRTV;
	LBuffer[1] = m_SpecularLightRTV;

	//SHADERRESOURCEVIEWS
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;
	
	//R8G8B8A8_UNORM
	m_Device->CreateShaderResourceView(albedoTex, &SRVDesc, &m_AlbedoSRV);
	m_Device->CreateShaderResourceView(diffuseTex, &SRVDesc, &m_DiffuseLightSRV);
	m_Device->CreateShaderResourceView(specularTex, &SRVDesc, &m_SpecularLightSRV);

	//DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	m_Device->CreateShaderResourceView(depthTex, &SRVDesc, &m_DepthSRV);

	//DXGI_FORMAT_R16G16B16A16_UNORM
	SRVDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	m_Device->CreateShaderResourceView(normalSpecTex, &SRVDesc, &m_NormalSpecSRV);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	m_Device->CreateDepthStencilView( depthTex, &descDSV, &m_DepthSV );


	//RELEASE
	albedoTex->Release();
	normalSpecTex->Release();	
	depthTex->Release();
	diffuseTex->Release();	
	specularTex->Release();
}


void GraphicsManager::ClearBuffers()
{
	m_DeviceContext->RSSetViewports( 1, &m_ViewPort );
	m_DeviceContext->ClearDepthStencilView( m_DepthSV, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	float AlbedoClearColor[4] = {0.1f,  0.1f, 0.1f, 0.0f};
	float ClearColor[4] = {0.0f,  0.0f, 0.0f, 0.0f};
	m_DeviceContext->ClearRenderTargetView( m_RenderTargetView, ClearColor );
	m_DeviceContext->ClearRenderTargetView( m_AlbedoRTV,		AlbedoClearColor );
	m_DeviceContext->ClearRenderTargetView( m_NormalSpecRTV,	ClearColor );
	m_DeviceContext->ClearRenderTargetView( m_DiffuseLightRTV,	ClearColor );
	m_DeviceContext->ClearRenderTargetView( m_SpecularLightRTV,	ClearColor );

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
	m_DeviceContext->OMSetRenderTargets( 2, GBuffer, m_DepthSV );

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_DeviceContext->OMSetDepthStencilState(RenderStates::LessDDS, 0);
	
	m_DeviceContext->RSSetState(RenderStates::NoCullRS);
	//m_DeviceContext->RSSetState(RenderStates::WireframeRS);

	for (int i = 0; i < players.size(); ++i)
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

	Effects::CombineFinalFX->SetAlbedo(m_AlbedoSRV);
	Effects::CombineFinalFX->SetDiffuse(m_DiffuseLightSRV);
	Effects::CombineFinalFX->SetSpecular(m_SpecularLightSRV);


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





void GraphicsManager::SetQuadTree(QuadTree *instanceTree)
{
	m_InstanceTree = instanceTree;
}

void GraphicsManager::RenderModel(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass)
{
	XMMATRIX world;
	XMMATRIX worldViewInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;
	XMMATRIX tex = XMMatrixTranslation(1, 1, 1);

	world = XMLoadFloat4x4(&instance.m_World);
	//float a = MathHelper::InverseTranspose(world);
	
	
	worldView     = XMMatrixMultiply(world, view);
	worldViewInvTranspose = MathHelper::InverseTranspose(worldView);
	
	//worldInvTransposeView = worldInvTranspose*view;
	worldViewProj = XMMatrixMultiply(worldView, proj);

	Effects::ObjectDeferredFX->SetWorldView(worldView);
	Effects::ObjectDeferredFX->SetWorldInvTranspose(worldViewInvTranspose);
	Effects::ObjectDeferredFX->SetTexTransform(tex);
	Effects::ObjectDeferredFX->SetWorldViewProj(worldViewProj);

	for(UINT subset = 0; subset < instance.m_Model->SubsetCount; ++subset)
	{
		//UINT subset = 6;
		Effects::NormalMapFX->SetMaterial(instance.m_Model->Mat[subset]);

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
		XMMATRIX worldViewInvTranspose;
		XMMATRIX worldViewProj;
		XMMATRIX tex = m_Terrain->GetTexTransform();

		world = XMMatrixTranslation(0, 0, 0);
		//float a = MathHelper::InverseTranspose(world);
	
	
		worldView     = XMMatrixMultiply(world, view);
		worldViewInvTranspose = MathHelper::InverseTranspose(worldView);
	
		//worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = XMMatrixMultiply(worldView, proj);

		Effects::TerrainDeferredFX->SetWorldView(worldView);
		Effects::TerrainDeferredFX->SetWorldInvTranspose(worldViewInvTranspose);
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

void GraphicsManager::Render(vector<Player*>& players)
{
	ClearBuffers();
	FillGBuffer(players);	
	CombineFinal();
}