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
	m_ShadowWidth = 0;
	m_ShadowHeight = 0;

	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = (float)0;
	m_ViewPort.TopLeftY = (float)0;
	m_ViewPort.Width	= (float)width;
	m_ViewPort.Height	= (float)height;

	BoundingBox world = BoundingBox(XMFLOAT3(2000,0,2000), XMFLOAT3(2000, 2000, 2000));

	g_QuadTree = NULL;


	m_DirLightBuffer = NULL;
	m_PointLightBuffer = NULL;
	m_SpotLightBuffer = NULL;

	m_DirLights		= NULL;
	m_PointLights	= NULL;
	m_SpotLights	= NULL;
	m_ShadowMapSRV0	= NULL;
	m_ShadowMapSRV1	= NULL;
	m_ShadowMapSRV2	= NULL;
	m_ShadowMapSRV3	= NULL;
	m_ShadowMapDSV0	= NULL;
	m_ShadowMapDSV1	= NULL;
	m_ShadowMapDSV2	= NULL;
	m_ShadowMapDSV3	= NULL;

	InitFullScreenQuad();
	InitBuffers();
}

GraphicsManager::~GraphicsManager(void)
{
	//	Delete the light buffers
	if ( m_DirLightBuffer )
		delete	m_DirLightBuffer;
	if ( m_PointLightBuffer )
		delete	m_PointLightBuffer;
	if ( m_SpotLightBuffer )
		delete	m_SpotLightBuffer;

	//	Remove the quad tree
	if ( g_QuadTree )
		g_QuadTree->~QuadTree();

	if ( m_FullSceenQuad )
		SAFE_RELEASE( m_FullSceenQuad );

	if ( m_AlbedoRTV )
		SAFE_RELEASE( m_AlbedoRTV );
	if ( m_NormalSpecRTV )
		SAFE_RELEASE( m_NormalSpecRTV );
	if ( m_AlbedoSRV )
		SAFE_RELEASE( m_AlbedoSRV );
	if ( m_NormalSpecSRV )
		SAFE_RELEASE( m_NormalSpecSRV );
	if ( m_DepthSRV )
		SAFE_RELEASE( m_DepthSRV );
	if ( m_FinalSRV )
		SAFE_RELEASE( m_FinalSRV );
	if ( m_ShadowMapSRV0 )
		SAFE_RELEASE( m_ShadowMapSRV0 );
	if ( m_ShadowMapSRV1 )
		SAFE_RELEASE( m_ShadowMapSRV1 );
	if ( m_ShadowMapSRV2 )
		SAFE_RELEASE( m_ShadowMapSRV2 );
	if ( m_ShadowMapSRV3 )
		SAFE_RELEASE( m_ShadowMapSRV3 );
	if ( m_ShadowMapDSV0 )
		SAFE_RELEASE( m_ShadowMapDSV0 );
	if ( m_ShadowMapDSV1 )
		SAFE_RELEASE( m_ShadowMapDSV1 );
	if ( m_ShadowMapDSV2 )
		SAFE_RELEASE( m_ShadowMapDSV2 );
	if ( m_ShadowMapDSV3 )
		SAFE_RELEASE( m_ShadowMapDSV3 );
	if ( m_DepthStencilView ) 
		SAFE_RELEASE( m_DepthStencilView );


	if ( m_FinalUAV )
		SAFE_RELEASE( m_FinalUAV );

	if ( m_DirLights )
	{
		for ( UINT i = 0; i < m_DirLights->size(); ++i )
			m_DirLights->at(i)	=	0;

		m_DirLights->clear();
		m_DirLights	=	0;
	}
	if ( m_PointLights )
	{
		for ( UINT i = 0; i < m_PointLights->size(); ++i )
			m_PointLights->at(i)	=	0;

		m_PointLights->clear();
		m_PointLights	=	0;
	}
	if ( m_SpotLights )
	{
		for ( UINT i = 0; i < m_SpotLights->size(); ++i )
			m_SpotLights->at(i)	=	0;

		m_SpotLights->clear();
		m_SpotLights	=	0;
	}
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

	InitShadowMap(8192, 4096);
	//InitShadowMap(16384, 4096);
	//InitShadowMap(16384, 8192);
}

void GraphicsManager::InitShadowMap(int width, int height)
{
	m_ShadowWidth  = width;
	m_ShadowHeight = height;

	SAFE_RELEASE(m_ShadowMapSRV0);
	SAFE_RELEASE(m_ShadowMapSRV1);
	SAFE_RELEASE(m_ShadowMapSRV2);
	SAFE_RELEASE(m_ShadowMapSRV3);
	SAFE_RELEASE(m_ShadowMapDSV0);
	SAFE_RELEASE(m_ShadowMapDSV1);
	SAFE_RELEASE(m_ShadowMapDSV2);
	SAFE_RELEASE(m_ShadowMapDSV3);
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
	ID3D11Texture2D* texture0 = NULL;
	ID3D11Texture2D* texture1 = NULL;
	ID3D11Texture2D* texture2 = NULL;
	ID3D11Texture2D* texture3 = NULL;
	m_Device->CreateTexture2D(&texDesc, NULL, &texture0);
	m_Device->CreateTexture2D(&texDesc, NULL, &texture1);
	m_Device->CreateTexture2D(&texDesc, NULL, &texture2);
	m_Device->CreateTexture2D(&texDesc, NULL, &texture3);

	//SRV desc
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	//Create SRV
	m_Device->CreateShaderResourceView(texture0, &SRVDesc, &m_ShadowMapSRV0);
	m_Device->CreateShaderResourceView(texture1, &SRVDesc, &m_ShadowMapSRV1);
	m_Device->CreateShaderResourceView(texture2, &SRVDesc, &m_ShadowMapSRV2);
	m_Device->CreateShaderResourceView(texture3, &SRVDesc, &m_ShadowMapSRV3);

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc;
	ZeroMemory(&DSVdesc, sizeof(DSVdesc));
	DSVdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVdesc.Texture2D.MipSlice = 0;

	m_Device->CreateDepthStencilView( texture0, &DSVdesc, &m_ShadowMapDSV0 );
	m_Device->CreateDepthStencilView( texture1, &DSVdesc, &m_ShadowMapDSV1 );
	m_Device->CreateDepthStencilView( texture2, &DSVdesc, &m_ShadowMapDSV2 );
	m_Device->CreateDepthStencilView( texture3, &DSVdesc, &m_ShadowMapDSV3 );

	//Release texture
	texture0->Release();
	texture1->Release();
	texture2->Release();
	texture3->Release();
}

void GraphicsManager::RenderShadowMaps(vector<Camera*>& cameras)
{
	m_CurrentShadowMap = 0;
	m_ShadowMapSwitches = XMFLOAT3(9999, 9999, 9999);
	//m_ViewProjTexs.clear();
	//m_ViewProj.clear();
	//m_Texs.clear();

	//Lists with lights that have a shadowmap and will be visible
	vector<DirectionalLight*>	dirLights;
	vector<PointLight*>			pointLights;
	vector<SpotLight*>			spotLights;

#pragma region
	//Add directionallights with shadows
	for each (DirectionalLight* light in *m_DirLights)
	{
		light->ShadowIndex[0] = 9999;
		light->ShadowIndex[1] = 9999;
		light->ShadowIndex[2] = 9999;
		light->ShadowIndex[3] = 9999;
		if (light->HasShadow)
			dirLights.push_back(light);
	}

	//Add pointlights with shadows
	for each (PointLight* light in *m_PointLights)
	{
		light->ShadowIndex[0] = 9999;
		light->ShadowIndex[1] = 9999;
		light->ShadowIndex[2] = 9999;
		light->ShadowIndex[3] = 9999;
		light->ShadowIndex[4] = 9999;
		light->ShadowIndex[5] = 9999;
		if (light->HasShadow)
		{
			BoundingSphere lightBounds = light->GetBoundingSphere();
			bool added = false;
			light->Resolution = SHADOWMAP_512;
			for each (Camera* camera in cameras)
			{
				if (camera->GetFrustum().Intersects(lightBounds))
				{
					if (!added)
						pointLights.push_back(light);

					XMFLOAT2 tempRes = ChooseResolution(light, camera->GetPosition());
					light->Resolution = HighestRes(light->Resolution, tempRes);

					break;
				}
			}
		}			
	}

	//Add spotlights with shadows
	for each (SpotLight* light in *m_SpotLights)
	{
		light->ShadowIndex = 9999;
		if (light->HasShadow)
		{
			BoundingFrustum lightBounds = light->GetBoundingFrustum();
			bool added = false;
			light->Resolution = SHADOWMAP_512;
			for each (Camera* camera in cameras)
			{
				if (camera->GetFrustum().Intersects(lightBounds))
				{
					if (!added)
						spotLights.push_back(light);

					XMFLOAT2 tempRes = ChooseResolution(light, camera->GetPosition());
					light->Resolution = HighestRes(light->Resolution, tempRes);

					break;
				}
			}
		}
	}
#pragma endregion Select lights with shadows
	
	vector<XMFLOAT2> Resolutions;
	Resolutions.push_back(SHADOWMAP_4096);
	Resolutions.push_back(SHADOWMAP_2048);
	Resolutions.push_back(SHADOWMAP_1024);
	Resolutions.push_back(SHADOWMAP_512);

	int ShadowIndex = 0;
	int ShadowTileIndex = 0;
	//int ShadowTileMaxTotalSize = (m_ShadowWidth / 512) * (m_ShadowHeight / 512);

	//Går igenom upplösningarna i storlerksordning.
	for each (XMFLOAT2 Resolution in Resolutions)
	{
		RenderDirShadowMaps(dirLights, cameras, Resolution, ShadowIndex, ShadowTileIndex);	
		RenderPointShadowMaps(pointLights, Resolution, ShadowIndex, ShadowTileIndex);	
		RenderSpotShadowMaps(spotLights, Resolution, ShadowIndex, ShadowTileIndex);	
	}	
}

void GraphicsManager::RenderDirShadowMaps(vector<DirectionalLight*>& dirLights, vector<Camera*>& cameras, XMFLOAT2 Resolution, int& ShadowIndex, int& ShadowTileIndex)
{
	int TileSize = ShadowTileSize(Resolution);
	XMVECTOR CurrentRes = XMLoadFloat2(&Resolution);
	//spotlights för vald upplösning ritats ut.
	for each (DirectionalLight* light in dirLights)
	{	
		XMVECTOR LightRes = XMLoadFloat2(&light->Resolution);

		if (XMVector2Equal(LightRes, CurrentRes))
		{
			for (int i = 0; i < cameras.size(); ++i)
			//for each (Camera* camera in cameras)
			{
				Camera* camera = cameras[i];

				//kolla om shadowmapen får plats
				//else light->ShadowIndex = -1 (kolla på GPU)



				XMFLOAT2 shadowCoord = CalculateShadowCoord(ShadowTileIndex);

				SetCurrentShadowMap(ShadowTileIndex, ShadowIndex);

				//räkna ut viewport
				D3D11_VIEWPORT vp = ShadowViewPort((int)shadowCoord.x, (int)shadowCoord.y, (int)Resolution.x, (int)Resolution.y);
				
				//räkna ut ViewProj-matris.
				//1.0f, light->Range ska vara närsta avstånd till objekt, största avstånd. Terräng? GOLDPLATING! Drar prestanda, ökar kvalité på skuggorna.
				XMFLOAT4X4 V, P;
				BoundingOrientedBox OBB;

				light->GetViewProjOBB(camera->GetFrustum(), 4000.0f, V, P, OBB);
				
				XMFLOAT3 tempPos = camera->GetPosition();

				/*
				system("cls");
				cout << "Camera:	  " << tempPos.x << ", " << tempPos.y << ", " << tempPos.z << endl;
				cout << "OBB center:  " << OBB.Center.x << ", " << OBB.Center.y << ", " << OBB.Center.z << endl;
				cout << "OBB extents: " << OBB.Extents.x << ", " << OBB.Extents.y << ", " << OBB.Extents.z << endl;
				*/

				XMMATRIX View = XMLoadFloat4x4(&V);
				XMMATRIX Proj = XMLoadFloat4x4(&P);

				XMMATRIX ViewProj = View * Proj;

				//räkna ut viewprojtex
				XMFLOAT4X4 ViewProjTex = ShadowViewProjTex(vp, ViewProj);
				XMFLOAT4X4 Tex = ShadowTex(vp);				
				XMFLOAT4X4 viewprojf44;
				XMStoreFloat4x4(&viewprojf44, ViewProj);

				//spara viewprojtex till listan med viewprojtex. denna ska till GPU:n. index i listan ska vara samma som light->ShadowIndex
				m_ViewProjTexs[ShadowIndex] = ViewProjTex;
				m_ViewProj[ShadowIndex] = viewprojf44;
				m_Texs[ShadowIndex] = Tex;
				
				//drawshadowmap med vp
				RenderShadowMap(0, View, Proj, vp, OBB);

				//Sätter ShadowIndex
				light->ShadowIndex[i] = ShadowIndex;

				//Räknar upp ShadowIndex
				++ShadowIndex;
				ShadowTileIndex += TileSize;
			}			
		}	
	}
}

void GraphicsManager::RenderPointShadowMaps(vector<PointLight*>& pointLights, XMFLOAT2 Resolution, int& ShadowIndex, int& ShadowTileIndex)
{
	int TileSize = ShadowTileSize(Resolution);
	XMVECTOR CurrentRes = XMLoadFloat2(&Resolution);
	//spotlights för vald upplösning ritats ut.
	for each (PointLight* light in pointLights)
	{					
		XMVECTOR LightRes = XMLoadFloat2(&light->Resolution);

		if (XMVector2Equal(LightRes, CurrentRes))
		{
			std::vector<XMFLOAT4X4> Views = light->GetViewMatrixes();
			XMMATRIX Proj = light->GetProjectionMatrix(1.0f, light->Range);

			for (int i = 0; i < Views.size(); ++i)
			{
				XMFLOAT2 shadowCoord = CalculateShadowCoord(ShadowTileIndex);

				SetCurrentShadowMap(ShadowTileIndex, ShadowIndex);

				D3D11_VIEWPORT vp = ShadowViewPort((int)shadowCoord.x, (int)shadowCoord.y, (int)Resolution.x, (int)Resolution.y);

				XMMATRIX View = XMLoadFloat4x4(&Views[i]);

				XMMATRIX ViewProj = View * Proj;

				//räkna ut viewprojtex
				XMFLOAT4X4 ViewProjTex = ShadowViewProjTex(vp, ViewProj);
				XMFLOAT4X4 Tex = ShadowTex(vp);
				XMFLOAT4X4 viewprojf44;
				XMStoreFloat4x4(&viewprojf44, ViewProj);

				//spara viewprojtex till listan med viewprojtex. denna ska till GPU:n. index i listan ska vara samma som light->ShadowIndex
				m_ViewProjTexs[ShadowIndex] =ViewProjTex;
				m_ViewProj[ShadowIndex] = viewprojf44;
				m_Texs[ShadowIndex] = Tex;

				//drawshadowmap med vp
				RenderShadowMap(1, View, Proj, vp);

				//Sätter ShadowIndex
				light->ShadowIndex[i] = ShadowIndex;

				//Räknar upp ShadowIndex
				++ShadowIndex;
				ShadowTileIndex += TileSize;
			}
		}	
	}
}

void GraphicsManager::RenderSpotShadowMaps(vector<SpotLight*>& spotLights, XMFLOAT2 Resolution, int& ShadowIndex, int& ShadowTileIndex)
{
	int TileSize = ShadowTileSize(Resolution);
	XMVECTOR CurrentRes = XMLoadFloat2(&Resolution);
	//spotlights för vald upplösning ritats ut.
	for each (SpotLight* light in spotLights)
	{					
		XMVECTOR LightRes = XMLoadFloat2(&light->Resolution);

		if (XMVector2Equal(LightRes, CurrentRes))
		{
			//kolla om shadowmapen får plats
			//else light->ShadowIndex = -1 (kolla på GPU)
			XMFLOAT2 shadowCoord = CalculateShadowCoord(ShadowTileIndex);
			SetCurrentShadowMap(ShadowTileIndex, ShadowIndex);

			//räkna ut viewport
			D3D11_VIEWPORT vp = ShadowViewPort((int)shadowCoord.x, (int)shadowCoord.y, (int)Resolution.x, (int)Resolution.y);

			//räkna ut ViewProj-matris.
			//1.0f, light->Range ska vara närsta avstånd till objekt, största avstånd. Terräng? GOLDPLATING! Drar prestanda, ökar kvalité på skuggorna.
			XMMATRIX View = light->GetViewMatrix();
			XMMATRIX Proj = light->GetProjectionMatrix(1.0f, light->Range);
			XMMATRIX ViewProj = View * Proj;

			//räkna ut viewprojtex
			XMFLOAT4X4 ViewProjTex = ShadowViewProjTex(vp, ViewProj);
			XMFLOAT4X4 Tex = ShadowTex(vp);				
			XMFLOAT4X4 viewprojf44;
			XMStoreFloat4x4(&viewprojf44, ViewProj);

			//spara viewprojtex till listan med viewprojtex. denna ska till GPU:n. index i listan ska vara samma som light->ShadowIndex
			m_ViewProjTexs[ShadowIndex] =ViewProjTex;
			m_ViewProj[ShadowIndex] = viewprojf44;
			m_Texs[ShadowIndex] = Tex;

			//drawshadowmap med vp
			RenderShadowMap(2, View, Proj, vp);

			//Sätter ShadowIndex
			light->ShadowIndex = ShadowIndex;

			//Räknar upp ShadowIndex
			++ShadowIndex;
			ShadowTileIndex += TileSize;
		}	
	}
}

void GraphicsManager::RenderShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, D3D11_VIEWPORT vp, BoundingFrustum& frustum)
{
	m_DeviceContext->RSSetViewports( 1, &vp );
	SetShadowRTV();
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->OMSetDepthStencilState(RenderStates::LessDSS, 0);
	m_DeviceContext->RSSetState(RenderStates::NoCullRS);	
	//m_DeviceContext->RSSetState(RenderStates::WireframeRS);	

	m_DeviceContext->IASetInputLayout(InputLayouts::Pos);

	RenderTerrainShadowMap(lightType, View, Proj);
	RenderModelsShadowMap(lightType, View, Proj, frustum);

	m_DeviceContext->OMSetRenderTargets( 0, 0, 0 );
}

void GraphicsManager::RenderShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, D3D11_VIEWPORT vp, BoundingOrientedBox& OBB)
{
	m_DeviceContext->RSSetViewports( 1, &vp );
	SetShadowRTV();
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->OMSetDepthStencilState(RenderStates::LessDSS, 0);
	m_DeviceContext->RSSetState(RenderStates::NoCullRS);	
	//m_DeviceContext->RSSetState(RenderStates::WireframeRS);	

	RenderTerrainShadowMap(lightType, View, Proj);
	RenderModelsShadowMap(lightType, View, Proj, OBB);

	m_DeviceContext->OMSetRenderTargets( 0, 0, 0 );
}

void GraphicsManager::RenderTerrainShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj)
{
	m_DeviceContext->IASetInputLayout(InputLayouts::Pos);

	ID3DX11EffectTechnique* tech;
	D3DX11_TECHNIQUE_DESC techDesc;

	switch (lightType)
	{
		case 0:
			tech = Effects::ShadowMapFX->BasicShadowDirTech;
			break;
		case 1:
			tech = Effects::ShadowMapFX->BasicShadowPointTech;
			break;
		case 2:
			tech = Effects::ShadowMapFX->BasicShadowSpotTech;
			break;
	}
	
	tech->GetDesc( &techDesc );

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		XMMATRIX world = XMMatrixTranslation(0, 0, 0);

		Effects::ShadowMapFX->SetWorldViewProj(world * View * Proj);

		tech->GetPassByIndex(p)->Apply(0, m_DeviceContext);
		m_Terrain->Draw(m_DeviceContext);		
	}
}

void GraphicsManager::RenderModelsShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, vector<GameObject*>& instances)
{

	ID3DX11EffectTechnique* tech;
	ID3DX11EffectTechnique* tech2;
	D3DX11_TECHNIQUE_DESC techDesc;

	switch (lightType)
	{
		case 0:
			tech = Effects::ShadowMapFX->AlphaClipShadowDirTech;
			tech2 = Effects::ShadowMapFX->SkinnedAlphaClipShadowDirTech;
			break;
		case 1:
			tech = Effects::ShadowMapFX->AlphaClipShadowPointTech;
			tech2 = Effects::ShadowMapFX->SkinnedAlphaClipShadowPointTech;
			break;
		case 2:
			tech = Effects::ShadowMapFX->AlphaClipShadowSpotTech;
			tech2 = Effects::ShadowMapFX->SkinnedAlphaClipShadowSpotTech;
			break;
	}

	tech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		//for each (ModelInstance* instance in m_modelInstances)
		for each (GameObject* instance in instances)
		{
			ModelInstance* modelInstance = instance->GetModelInstance();
			if (modelInstance != NULL)
			{
				if (!modelInstance->UsingAnimationOrPose())
					RenderModelShadowMap(*modelInstance, View, Proj, tech, p);
			}
		}
	}

	tech2->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		//for each (ModelInstance* instance in m_modelInstances)
		for each (GameObject* instance in instances)
		{
			ModelInstance* modelInstance = instance->GetModelInstance();
			if (modelInstance != NULL)
			{
				if (modelInstance->UsingAnimationOrPose())
					RenderAnimatedModelShadowMap(*modelInstance, View, Proj, tech2, p);
			}
		}
	}


	Effects::ShadowMapFX->SetDiffuseMap(NULL);
}

void GraphicsManager::RenderModelShadowMap(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass)
{
	m_DeviceContext->IASetInputLayout(InputLayouts::Basic32);
	XMMATRIX world;
	world = XMLoadFloat4x4(&instance.m_World);

	XMMATRIX tex = XMMatrixTranslation(1, 1, 1);

	Effects::ShadowMapFX->SetWorldViewProj(world * view * proj);
	Effects::ShadowMapFX->SetTexTransform(tex);

	for(UINT subset = 0; subset < instance.m_Model->SubsetCount; ++subset)
	{
		Effects::ShadowMapFX->SetDiffuseMap(instance.m_Model->DiffuseMapSRV[subset]);
		tech->GetPassByIndex(pass)->Apply(0, m_DeviceContext);
		instance.m_Model->ModelMesh.Draw(m_DeviceContext, subset);
	}
}

void GraphicsManager::RenderAnimatedModelShadowMap(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass)
{
	m_DeviceContext->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);
	XMMATRIX world;
	world = XMLoadFloat4x4(&instance.m_World);

	XMMATRIX tex = XMMatrixTranslation(1, 1, 1);

	Effects::ShadowMapFX->SetWorldViewProj(world * view * proj);
	Effects::ShadowMapFX->SetTexTransform(tex);
	Effects::ShadowMapFX->SetBoneTransforms(&instance.FinalTransforms[0], instance.FinalTransforms.size());

	for(UINT subset = 0; subset < instance.m_Model->SubsetCount; ++subset)
	{
		Effects::ShadowMapFX->SetDiffuseMap(instance.m_Model->DiffuseMapSRV[subset]);
		tech->GetPassByIndex(pass)->Apply(0, m_DeviceContext);
		instance.m_Model->ModelMesh.Draw(m_DeviceContext, subset);
	}
}

void GraphicsManager::UpdateLights()
{
	//Update DirecionalLights.
	if (m_DirLights != NULL)
	{
		//Update size of m_DirLightBuffer
		if (m_DirLightBuffer != NULL)
		{

			if (m_DirLightBuffer->Size() != m_DirLights->size())
			{
				delete m_DirLightBuffer;

				if (m_DirLights->size() > 0)
					m_DirLightBuffer = new StructuredBuffer<DirectionalLight>(m_Device, m_DirLights->size(), D3D11_BIND_SHADER_RESOURCE, true);
				else
					m_DirLightBuffer = NULL;
			}
		}
		else if (m_DirLights->size() > 0)
			m_DirLightBuffer = new StructuredBuffer<DirectionalLight>(m_Device, m_DirLights->size(), D3D11_BIND_SHADER_RESOURCE, true);

		//Update lights in m_DirLightBuffer
		if (m_DirLightBuffer != NULL)
		{
			if (m_DirLightBuffer->Size() > 0)
			{
				DirectionalLight* dirLight = m_DirLightBuffer->MapDiscard(m_DeviceContext);
				for (unsigned int i = 0; i < m_DirLights->size(); ++i) 
				{
					dirLight[i] = *m_DirLights->at(i);
				}
				m_DirLightBuffer->Unmap(m_DeviceContext);
			}
		}
	}
	else if (m_DirLightBuffer != NULL)
	{
		delete m_DirLightBuffer;
		m_DirLightBuffer = NULL;
	}

	//Update PointLights.
	if (m_PointLights != NULL)
	{
		//Update size of m_PointLightBuffer
		if (m_PointLightBuffer != NULL)
		{
			if (m_PointLightBuffer->Size() != m_PointLights->size())
			{
				delete m_PointLightBuffer;

				if (m_PointLights->size() > 0)
					m_PointLightBuffer = new StructuredBuffer<PointLight>(m_Device, m_PointLights->size(), D3D11_BIND_SHADER_RESOURCE, true);
				else
					m_PointLightBuffer = NULL;
			}
		}
		else if (m_PointLights->size() > 0)
			m_PointLightBuffer = new StructuredBuffer<PointLight>(m_Device, m_PointLights->size(), D3D11_BIND_SHADER_RESOURCE, true);


		//Update lights in m_PointLightBuffer
		if (m_PointLightBuffer != NULL)
		{
			if (m_PointLightBuffer->Size() > 0)
			{
				PointLight* pointLight = m_PointLightBuffer->MapDiscard(m_DeviceContext);
				for (unsigned int i = 0; i < m_PointLights->size(); ++i) 
				{
					pointLight[i] = *m_PointLights->at(i);
				}
				m_PointLightBuffer->Unmap(m_DeviceContext);
			}
		}
	}
	else if (m_PointLightBuffer != NULL)
	{
		delete m_PointLightBuffer;
		m_PointLightBuffer = NULL;
	}

	//Update SpotLights.
	if (m_SpotLights != NULL)
	{
		//Update size of m_SpotLightBuffer
		if (m_SpotLightBuffer != NULL)
		{
			if (m_SpotLightBuffer->Size() != m_SpotLights->size())
			{
				delete m_SpotLightBuffer;

				if (m_SpotLights->size() > 0)
					m_SpotLightBuffer = new StructuredBuffer<SpotLight>(m_Device, m_SpotLights->size(), D3D11_BIND_SHADER_RESOURCE, true);
				else
					m_SpotLightBuffer = NULL;
			}
		}
		else if (m_SpotLights->size() > 0)
			m_SpotLightBuffer = new StructuredBuffer<SpotLight>(m_Device, m_SpotLights->size(), D3D11_BIND_SHADER_RESOURCE, true);

		//Update lights in m_SpotLightBuffer
		if (m_SpotLightBuffer != NULL)
		{
			if (m_SpotLightBuffer->Size() > 0)
			{
				SpotLight* spotLight = m_SpotLightBuffer->MapDiscard(m_DeviceContext);
				for (unsigned int i = 0; i < m_SpotLights->size(); ++i) 
				{
					spotLight[i] = *m_SpotLights->at(i);
				}
				m_SpotLightBuffer->Unmap(m_DeviceContext);
			}
		}
	}
	else if (m_SpotLightBuffer != NULL)
	{
		delete m_SpotLightBuffer;
		m_SpotLightBuffer = NULL;
	}
}

void GraphicsManager::ClearBuffers()
{
	m_DeviceContext->RSSetViewports( 1, &m_ViewPort );
	m_DeviceContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	m_DeviceContext->ClearDepthStencilView( m_ShadowMapDSV0, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	m_DeviceContext->ClearDepthStencilView( m_ShadowMapDSV1, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	m_DeviceContext->ClearDepthStencilView( m_ShadowMapDSV2, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	m_DeviceContext->ClearDepthStencilView( m_ShadowMapDSV3, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	float AlbedoClearColor[4] = {0.1f,  0.1f, 0.1f, 0.0f};
	float ClearColor[4] = {0.0f,  0.0f, 0.0f, 0.0f};
	//m_DeviceContext->ClearRenderTargetView( m_RenderTargetView, ClearColor );
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

void GraphicsManager::FillGBuffer(vector<Camera*>& Cameras)
{
	m_DeviceContext->OMSetRenderTargets( 2, GBuffer, m_DepthStencilView );
	

	m_DeviceContext->OMSetDepthStencilState(RenderStates::LessDSS, 0);

	//m_DeviceContext->RSSetState(RenderStates::NoCullRS);
	//m_DeviceContext->RSSetState(RenderStates::WireframeRS);
	
	for (int i = 0; i < (int)Cameras.size(); ++i)
	{
		if (m_Terrain != NULL)
			RenderTerrain(Cameras[i]);

		if (g_QuadTree)
			RenderModels(Cameras[i]);
	}

	m_DeviceContext->OMSetRenderTargets( 0, 0, 0 );
}

void GraphicsManager::RenderModels(Camera* tCamera)
{
	m_DeviceContext->RSSetViewports( 1, &tCamera->GetViewPort() );
	XMMATRIX view;
	XMMATRIX proj;

	view = XMLoadFloat4x4(&tCamera->GetView());
	proj = XMLoadFloat4x4(&tCamera->GetProjection());


	vector<GameObject*> instances;
	BoundingFrustum frustum = tCamera->GetFrustum();	
	g_QuadTree->GetIntersectingInstances(frustum, instances);
	sort( instances.begin(), instances.end() );
	instances.erase( unique( instances.begin(), instances.end() ), instances.end() );

	ID3DX11EffectTechnique* tech;
	D3DX11_TECHNIQUE_DESC techDesc;
	
	tech = Effects::ObjectDeferredFX->TexAlphaClipTech;
	//tech = Effects::ObjectDeferredFX->TexNormalTech;
	tech->GetDesc( &techDesc );

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		//for each (ModelInstance* instance in m_modelInstances)
		for each (GameObject* instance in instances)
		{
			ModelInstance* modelInstance = instance->GetModelInstance();
			if (modelInstance != NULL)
			{
				if (!modelInstance->UsingAnimationOrPose())
					RenderModel(*modelInstance, view, proj, tech, p);
			}
		}
	}

	tech = Effects::ObjectDeferredFX->TexAlphaClipSkinnedTech;
	tech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		//for each (ModelInstance* instance in m_modelInstances)
		for each (GameObject* instance in instances)
		{
			ModelInstance* modelInstance = instance->GetModelInstance();
			if (modelInstance != NULL)
			{
				if (modelInstance->UsingAnimationOrPose())
					RenderAnimatedModel(*modelInstance, view, proj, tech, p);
			}
		}
	}

	Effects::ObjectDeferredFX->SetDiffuseMap(NULL);
	Effects::ObjectDeferredFX->SetNormalMap(NULL);
}

void GraphicsManager::RenderModel(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass)
{
	m_DeviceContext->RSSetState(RenderStates::NoCullRS);
	m_DeviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;
	XMMATRIX tex = XMMatrixTranslation(1, 1, 1);

	world = XMLoadFloat4x4(&instance.m_World);
	//float a = MathHelper::InverseTranspose(world);
	
	worldView     = XMMatrixMultiply(world, view);
	//worldInvTranspose = MathHelper::InverseTranspose(world);
	worldInvTranspose	=	XMLoadFloat4x4(&instance.m_WorldInverseTranspose);
	
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
	RenderDebugBox(instance.GetBoundingOrientedBox(), XMMatrixMultiply(view, proj));
}

void GraphicsManager::RenderAnimatedModel(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass)
{
	m_DeviceContext->RSSetState(RenderStates::NoCullRS);
	m_DeviceContext->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;
	XMMATRIX tex = XMMatrixTranslation(1, 1, 1);

	world = XMLoadFloat4x4(&instance.m_World);
	//float a = MathHelper::InverseTranspose(world);
	
	worldView     = XMMatrixMultiply(world, view);
	//worldInvTranspose = MathHelper::InverseTranspose(world);
	worldInvTranspose	=	XMLoadFloat4x4(&instance.m_WorldInverseTranspose);
	
	//worldInvTransposeView = worldInvTranspose*view;
	worldViewProj = XMMatrixMultiply(worldView, proj);

	Effects::ObjectDeferredFX->SetWorld(world);
	Effects::ObjectDeferredFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::ObjectDeferredFX->SetTexTransform(tex);
	Effects::ObjectDeferredFX->SetWorldViewProj(worldViewProj);
	Effects::ObjectDeferredFX->SetBoneTransforms(&instance.FinalTransforms[0], instance.FinalTransforms.size());

	for(UINT subset = 0; subset < instance.m_Model->SubsetCount; ++subset)
	{
		//UINT subset = 6;
		Effects::ObjectDeferredFX->SetMaterial(instance.m_Model->Mat[subset]);

		Effects::ObjectDeferredFX->SetDiffuseMap(instance.m_Model->DiffuseMapSRV[subset]);
		Effects::ObjectDeferredFX->SetNormalMap(instance.m_Model->NormalMapSRV[subset]);

		tech->GetPassByIndex(pass)->Apply(0, m_DeviceContext);
		instance.m_Model->ModelMesh.Draw(m_DeviceContext, subset);
	}
	RenderDebugBox(instance.GetBoundingOrientedBox(), XMMatrixMultiply(view, proj));

	for each (BoundingOrientedBox OBB in instance.m_BoneBoxes)
	{
		RenderDebugBox(OBB, XMMatrixMultiply(view, proj));
	}
}

void GraphicsManager::RenderDebugBox(BoundingOrientedBox& OBB, CXMMATRIX viewproj)
{
	//m_DeviceContext->RSSetState(RenderStates::WireframeRS);
	m_DeviceContext->IASetInputLayout(InputLayouts::Pos);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	ID3DX11EffectTechnique* tech;
	D3DX11_TECHNIQUE_DESC techDesc;

	tech = Effects::BoxDebugFX->BasicTech;
	tech->GetDesc( &techDesc );
	
	Effects::BoxDebugFX->SetViewProj(viewproj);

	ID3D11Buffer* VB;
	ID3D11Buffer* IB;

	//Indexbuffer
	XMFLOAT3 cornersW[OBB.CORNER_COUNT];
	OBB.GetCorners(&cornersW[0]);

	UINT VertexStride = sizeof(XMFLOAT3);

	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * OBB.CORNER_COUNT;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = cornersW;

	m_Device->CreateBuffer(&vbd, &vinitData, &VB);

#define iSize 24
	//Indexbuffer
	USHORT indices[iSize] = 
	{	0,1,
		1,2,
		2,3,
		0,3,

		4,5,
		5,6,
		6,7,
		4,7,

		0,4,
		1,5,
		2,6,
		3,7,
		/*
		4,5,6,
		4,6,7,

		0,4,3,
		8,10,11,*/
	};



	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * iSize;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

    m_Device->CreateBuffer(&ibd, &iinitData, &IB);

	 UINT offset = 0;


	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, m_DeviceContext);
		m_DeviceContext->IASetVertexBuffers(0, 1, &VB, &VertexStride, &offset);
		m_DeviceContext->IASetIndexBuffer(IB, DXGI_FORMAT_R16_UINT, 0);
		m_DeviceContext->DrawIndexed(iSize, 0, 0);
	}	
								

	ReleaseCOM(VB);
	ReleaseCOM(IB);
}

void GraphicsManager::RenderTerrain(Camera* tCamera)
{
	m_DeviceContext->RSSetState(RenderStates::NoCullRS);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->RSSetViewports( 1, &tCamera->GetViewPort() );
	XMMATRIX view;
	XMMATRIX proj;

	view = XMLoadFloat4x4(&tCamera->GetView());
	proj = XMLoadFloat4x4(&tCamera->GetProjection());

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

		Effects::TerrainDeferredFX->SetBlendMap(m_Terrain->GetBlendMap());

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

void GraphicsManager::ComputeLight(vector<Camera*>& Cameras)
{
	std::vector<XMFLOAT4>	camPositions;
	std::vector<XMFLOAT4X4> invViewProjs;
	for each (Camera* tCam in Cameras)
	{
		XMFLOAT3 temp	=	tCam->GetPosition();
		XMFLOAT4 camPos	=	XMFLOAT4(temp.x, temp.y, temp.z, 1);
		camPositions.push_back(camPos);

		XMMATRIX	viewMatrix;
		XMMATRIX	projMatrix;
		XMMATRIX	invViewProjMatrix;
		XMFLOAT4X4	invViewProj;

		viewMatrix = XMLoadFloat4x4(&tCam->GetView());
		projMatrix = XMLoadFloat4x4(&tCam->GetProjection());

		invViewProjMatrix = XMMatrixMultiply(viewMatrix, projMatrix);

		XMVECTOR det		= XMMatrixDeterminant(invViewProjMatrix);
		invViewProjMatrix	= XMMatrixInverse(&det, invViewProjMatrix);

		XMStoreFloat4x4(&invViewProj, invViewProjMatrix);
		invViewProjs.push_back(invViewProj);
	}


	D3DX11_TECHNIQUE_DESC	techDesc;
	ID3DX11EffectTechnique* tech;
	int numViewports = Cameras.size();

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
		Effects::TiledLightningFX->SetViewProjTexs(&m_ViewProjTexs[0], MAX_SHADOWMAPS);
		Effects::TiledLightningFX->SetViewProj(&m_ViewProj[0], MAX_SHADOWMAPS);
		Effects::TiledLightningFX->SetTexs(&m_Texs[0], MAX_SHADOWMAPS);
		Effects::TiledLightningFX->SetInvViewProjs(&invViewProjs[0], invViewProjs.size());
		Effects::TiledLightningFX->SetCamPositions(&camPositions[0], camPositions.size());
		Effects::TiledLightningFX->SetResolution(XMFLOAT2((float)m_Width, (float)m_Height));
		Effects::TiledLightningFX->SetShadowMapSwitches(m_ShadowMapSwitches);
		Effects::TiledLightningFX->SetShadowMapResolution(XMFLOAT2((float)m_ShadowWidth, (float)m_ShadowHeight));

		Effects::TiledLightningFX->SetAlbedoMap(m_AlbedoSRV);
		Effects::TiledLightningFX->SetNormalSpecMap(m_NormalSpecSRV);
		Effects::TiledLightningFX->SetDepthMap(m_DepthSRV);
		Effects::TiledLightningFX->SetOutputMap(m_FinalUAV);

		switch (m_CurrentShadowMap)
		{
		case 0:
			Effects::TiledLightningFX->SetShadowMap0(m_ShadowMapSRV0);
			break;
		case 1:
			Effects::TiledLightningFX->SetShadowMap0(m_ShadowMapSRV0);
			Effects::TiledLightningFX->SetShadowMap1(m_ShadowMapSRV1);
			break;
		case 2:
			Effects::TiledLightningFX->SetShadowMap0(m_ShadowMapSRV0);
			Effects::TiledLightningFX->SetShadowMap1(m_ShadowMapSRV1);
			Effects::TiledLightningFX->SetShadowMap2(m_ShadowMapSRV2);
			break;
		case 3:
			Effects::TiledLightningFX->SetShadowMap0(m_ShadowMapSRV0);
			Effects::TiledLightningFX->SetShadowMap1(m_ShadowMapSRV1);
			Effects::TiledLightningFX->SetShadowMap2(m_ShadowMapSRV2);
			Effects::TiledLightningFX->SetShadowMap3(m_ShadowMapSRV3);
			break;
		}


		ID3D11ShaderResourceView* dirLightMap = m_DirLightBuffer == NULL ? NULL : m_DirLightBuffer->GetShaderResource();
		ID3D11ShaderResourceView* pointLightMap = m_PointLightBuffer == NULL ? NULL : m_PointLightBuffer->GetShaderResource();
		ID3D11ShaderResourceView* spotLightMap = m_SpotLightBuffer == NULL ? NULL : m_SpotLightBuffer->GetShaderResource();

		Effects::TiledLightningFX->SetDirLightMap(dirLightMap);
		Effects::TiledLightningFX->SetPointLightMap(pointLightMap);
		Effects::TiledLightningFX->SetSpotLightMap(spotLightMap);

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

	switch (m_CurrentShadowMap)
	{
	case 0:
		Effects::TiledLightningFX->SetShadowMap0(NULL);
		break;
	case 1:
		Effects::TiledLightningFX->SetShadowMap0(NULL);
		Effects::TiledLightningFX->SetShadowMap1(NULL);
		break;
	case 2:
		Effects::TiledLightningFX->SetShadowMap0(NULL);
		Effects::TiledLightningFX->SetShadowMap1(NULL);
		Effects::TiledLightningFX->SetShadowMap2(NULL);
		break;
	case 3:
		Effects::TiledLightningFX->SetShadowMap0(NULL);
		Effects::TiledLightningFX->SetShadowMap1(NULL);
		Effects::TiledLightningFX->SetShadowMap2(NULL);
		Effects::TiledLightningFX->SetShadowMap3(NULL);
		break;
	}

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
	m_DeviceContext->OMSetDepthStencilState(RenderStates::NoDSS, 0);

	//Effects::CombineFinalFX->SetTexture(m_FinalSRV);
	//Effects::CombineFinalFX->SetTexture(m_ShadowMapSRV0);

	
	D3D11_VIEWPORT shadowVP;
	shadowVP.Width	= 400;
	shadowVP.Height	= ((float)m_ShadowHeight / (float)m_ShadowWidth) * shadowVP.Width;
	shadowVP.TopLeftX = 20;
	shadowVP.TopLeftY = m_Height - (shadowVP.Height + 300);		
	shadowVP.MinDepth = 0.0f;
	shadowVP.MaxDepth = 1.0f;

	RenderQuad(m_ViewPort, m_FinalSRV, Effects::CombineFinalFX->ColorTech);
	//Effects::CombineFinalFX->SetOpacity(0.8f);
	//RenderQuad(shadowVP, m_ShadowMapSRV, Effects::CombineFinalFX->BlendMonoTech);
	
	if (InputManager::GetInstance()->GetController(0)->GetButtonState(LEFT_BUMPER) == InputState::DOWN)
	{
		RenderQuad(shadowVP, m_ShadowMapSRV0, Effects::CombineFinalFX->MonoTech);

		shadowVP.TopLeftX = 600;

		RenderQuad(shadowVP, m_ShadowMapSRV1, Effects::CombineFinalFX->MonoTech);

		shadowVP.TopLeftY = m_Height - (shadowVP.Height + 20);
		shadowVP.TopLeftX = 20;

		RenderQuad(shadowVP, m_ShadowMapSRV2, Effects::CombineFinalFX->MonoTech);

		shadowVP.TopLeftX = 600;

		RenderQuad(shadowVP, m_ShadowMapSRV3, Effects::CombineFinalFX->MonoTech);
	}

	m_DeviceContext->RSSetViewports( 1, &m_ViewPort );
}

void GraphicsManager::RenderQuad(D3D11_VIEWPORT &vp, ID3D11ShaderResourceView* SRV, ID3DX11EffectTechnique* tech)
{
	m_DeviceContext->RSSetViewports( 1, &vp );
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc( &techDesc );

	Effects::CombineFinalFX->SetTexture(SRV);

	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		tech->GetPassByIndex(p)->Apply(0, m_DeviceContext);

		UINT offset = 0;
		UINT vertexStride = sizeof(Vertex::Quad);
		m_DeviceContext->IASetVertexBuffers(0, 1, &m_FullSceenQuad, &vertexStride, &offset);
		m_DeviceContext->Draw(4, 0);
	}

	Effects::CombineFinalFX->SetTexture(NULL);
	tech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
}

void GraphicsManager::Render(vector<Camera*>& Cameras)
{
	m_DeviceContext->OMSetBlendState(RenderStates::OpaqueBS, NULL, 0xffffffff);	
	ClearBuffers();

	RenderShadowMaps(Cameras);
	UpdateLights();
	FillGBuffer(Cameras);
	ComputeLight(Cameras);
	CombineFinal();
}

/*
void GraphicsManager::FillGBuffer(vector<Player*>& players)
{
	m_DeviceContext->OMSetRenderTargets( 2, GBuffer, m_DepthStencilView );

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_DeviceContext->OMSetDepthStencilState(RenderStates::LessDSS, 0);

	m_DeviceContext->RSSetState(RenderStates::NoCullRS);
	//m_DeviceContext->RSSetState(RenderStates::WireframeRS);
	
	for (int i = 0; i < (int)players.size(); ++i)
	{
		if (m_Terrain != NULL)
			RenderTerrain(players[i]);

		if (g_QuadTree)
			RenderModels(players[i]);
	}
	cout << endl;

	m_DeviceContext->OMSetRenderTargets( 0, 0, 0 );
}
*/

/*
void GraphicsManager::RenderModels(Player* player)
{
	Camera* camera = player->GetCamera();
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

	
	vector<GameObject*> instances;
	BoundingFrustum frustum = camera->GetFrustum();	
	g_QuadTree->GetIntersectingInstances(frustum, instances);
	sort( instances.begin(), instances.end() );
	instances.erase( unique( instances.begin(), instances.end() ), instances.end() );

	cout << instances.size() << "\t";

	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		//for each (ModelInstance* instance in m_modelInstances)
		for each (GameObject* instance in instances)
		{
			ModelInstance* modelInstance = instance->GetModelInstance();
			if (modelInstance != NULL)
				RenderModel(*modelInstance, view, proj, tech, p);
		}
	}
}
*/

/*
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
*/

/*
void GraphicsManager::Render(vector<Player*>& players)
{
	m_DeviceContext->OMSetBlendState(RenderStates::OpaqueBS, NULL, 0xffffffff);
	UpdateLights();
	ClearBuffers();
	FillGBuffer(players);
	ComputeLight(players);
	CombineFinal();
}
*/

/*
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
		Effects::TiledLightningFX->SetShadowMap(m_DepthSRV);
		Effects::TiledLightningFX->SetOutputMap(m_FinalUAV);


		ID3D11ShaderResourceView* dirLightMap = m_DirLightBuffer == NULL ? NULL : m_DirLightBuffer->GetShaderResource();
		ID3D11ShaderResourceView* pointLightMap = m_PointLightBuffer == NULL ? NULL : m_PointLightBuffer->GetShaderResource();
		ID3D11ShaderResourceView* spotLightMap = m_SpotLightBuffer == NULL ? NULL : m_SpotLightBuffer->GetShaderResource();

		Effects::TiledLightningFX->SetDirLightMap(dirLightMap);
		Effects::TiledLightningFX->SetPointLightMap(pointLightMap);
		Effects::TiledLightningFX->SetSpotLightMap(spotLightMap);

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
	Effects::TiledLightningFX->SetShadowMap(NULL);
	Effects::TiledLightningFX->SetOutputMap(NULL);

	tech->GetPassByIndex(0)->Apply(0, m_DeviceContext);
	m_DeviceContext->CSSetShader(0, 0, 0);
}
*/
