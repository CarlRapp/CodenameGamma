#pragma once
#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include "Model.h"
#include "..\..\..\stdafx.h"
#include "QuadTree.h"
#include "..\Camera.h"
#include "..\Player.h"
#include "..\Terrain\Terrain.h"
#include "Buffer.h"
#include "..\GameObject.h"

using namespace std;
using namespace DirectX;
class GraphicsManager
{
	D3D11_VIEWPORT			m_ViewPort;

	ID3D11Device			*m_Device; 
	ID3D11DeviceContext		*m_DeviceContext;
	ID3D11RenderTargetView	*m_RenderTargetView;

	int						m_Width, m_Height;

	ID3D11Buffer			*m_FullSceenQuad;

	ID3D11DepthStencilView		*m_DepthStencilView, *m_ShadowMapDSV;
	ID3D11RenderTargetView		*m_AlbedoRTV, *m_NormalSpecRTV;
	ID3D11ShaderResourceView	*m_AlbedoSRV, *m_NormalSpecSRV, *m_DepthSRV, *m_FinalSRV, *m_ShadowMapSRV;

	ID3D11UnorderedAccessView	*m_FinalUAV;

	ID3D11RenderTargetView* GBuffer[2];

	QuadTree				*g_QuadTree;
	Terrain					*m_Terrain;

	vector<DirectionalLight*>			*m_DirLights;
	vector<PointLight*>					*m_PointLights;
	vector<SpotLight*>					*m_SpotLights;

	StructuredBuffer<DirectionalLight>	*m_DirLightBuffer;
	StructuredBuffer<PointLight>		*m_PointLightBuffer;
	StructuredBuffer<SpotLight>			*m_SpotLightBuffer;

private:

	void InitFullScreenQuad();
	void InitBuffers();
	void InitShadowMap(int width, int height);

	void UpdateLights();
	void ClearBuffers();
	void FillGBuffer(vector<Player*>& players);
	void ComputeLight(vector<Player*>& players);

	void FillGBuffer(vector<Camera*>& Cameras);
	void ComputeLight(vector<Camera*>& Cameras);
	void CombineFinal();

public:
	GraphicsManager(ID3D11Device *device, ID3D11DeviceContext *deviceContext, ID3D11RenderTargetView *renderTargetView, int width, int height);
	~GraphicsManager(void);

	void SetTerrain(Terrain *terrain) { m_Terrain = terrain; }
	void SetQuadTree(QuadTree *quadTree) { g_QuadTree = quadTree; }
	void SetLights(	vector<DirectionalLight*>			*DirLights,
					vector<PointLight*>					*PointLights,
					vector<SpotLight*>					*SpotLights)
	{
		m_DirLights		= DirLights;
		m_PointLights	= PointLights;
		m_SpotLights	= SpotLights;
	}

	void Render(vector<Player*>& players);
	void RenderModels(Player* player);
	void RenderModel(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass);
	void RenderTerrain(Player* player);

	void RenderTerrain(Camera* tCamera);
	void RenderModels(Camera* tCamera);
	void Render(vector<Camera*>& Cameras);
};

#endif