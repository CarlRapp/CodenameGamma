#pragma once
#ifndef GRAPHICSMANAGER_H
#define GRAPHICSMANAGER_H

#include "Model.h"
#include "..\..\..\stdafx.h"
#include "QuadTree.h"
#include "..\Camera.h"
//#include "..\Player.h"
#include "..\Terrain\Terrain.h"
#include "Buffer.h"
#include "..\GameObject.h"

#define MAX_SHADOWMAPS 256

using namespace std;
using namespace DirectX;

class GraphicsManager
{
	D3D11_VIEWPORT			m_ViewPort;

	ID3D11Device			*m_Device; 
	ID3D11DeviceContext		*m_DeviceContext;
	ID3D11RenderTargetView	*m_RenderTargetView;

	int						m_Width, m_Height;
	int						m_ShadowWidth, m_ShadowHeight;

	UINT					m_CurrentShadowMap;
	XMFLOAT3				m_ShadowMapSwitches;


	ID3D11Buffer			*m_FullSceenQuad;

	ID3D11DepthStencilView		*m_DepthStencilView, *m_ShadowMapDSV;
	ID3D11RenderTargetView		*m_AlbedoRTV, *m_NormalSpecRTV;
	ID3D11ShaderResourceView	*m_AlbedoSRV, *m_NormalSpecSRV, *m_DepthSRV, *m_FinalSRV;

	ID3D11DepthStencilView		*m_ShadowMapDSV0, *m_ShadowMapDSV1, *m_ShadowMapDSV2, *m_ShadowMapDSV3;
	ID3D11ShaderResourceView	*m_ShadowMapSRV0, *m_ShadowMapSRV1, *m_ShadowMapSRV2, *m_ShadowMapSRV3;

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

	XMFLOAT4X4 m_ViewProjTexs[MAX_SHADOWMAPS];
	XMFLOAT4X4 m_ViewProj[MAX_SHADOWMAPS];
	XMFLOAT4X4 m_Texs[MAX_SHADOWMAPS];

private:

	static XMFLOAT2 CalculateShadowCoord(int ShadowIndex)
	{

		int rest = ShadowIndex;

		int row		  = rest / 128;
		rest		  %= 128;

		int numHuge   = rest / 64;
		rest		  %= 64;

		int numBig    = rest / 16;
		rest		  %= 16;

		int numMedium = rest / 4;
		rest		  %= 4;

		int numSmall  = rest;



		int x = numHuge * 4096;

		if (numBig == 1 || numBig == 3)
			x += 2048;

		if (numMedium == 1 || numMedium == 3)
			x += 1024;

		if (numSmall == 1 || numSmall == 3)
			x += 512;

		int y = 0;
	
		//y += row * 4096;

		if (numBig == 2 || numBig == 3)
			y += 2048;

		if (numMedium == 2 || numMedium == 3)
			y += 1024;

		if (numSmall == 2 || numSmall == 3)
			y += 512;

		return XMFLOAT2((float)x, (float)y);
	}

	D3D11_VIEWPORT ShadowViewPort(int x, int y, int width, int height)
	{
		D3D11_VIEWPORT vp;
		vp.TopLeftX = (float)x;
		vp.TopLeftY = (float)y;		
		vp.Width	= (float)width;
		vp.Height	= (float)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		return vp;
	}
	
	XMFLOAT4X4 ShadowViewProjTex(D3D11_VIEWPORT &vp, CXMMATRIX ViewProj)
	{
		//Skala om fr�n pixlar till intervallet [0, 1]
		
		float scaleX	= vp.Width		/ (float) m_ShadowWidth;
		float scaleY	= vp.Height		/ (float) m_ShadowHeight;
		float x			= (vp.TopLeftX	/ (float) m_ShadowWidth)  * scaleX;
		float y			= (vp.TopLeftY	/ (float) m_ShadowHeight) * scaleY;

		//R�kna ut Translation / Scala f�r tex-matrisen.
		XMMATRIX texTrans = XMMatrixTranslation(x, y, 0);
		XMMATRIX texScale = XMMatrixTranslation(scaleX, scaleY, 1.0f);

		//R�kna ut tex.
		XMMATRIX tex = texScale * texTrans;

		//R�kna ut viewprojtex.
		XMMATRIX ViewProjTex = ViewProj * tex;
		
		//Omvandla till XMFLOAT4X4
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, ViewProjTex);

		return result;
	}

	XMFLOAT4X4 ShadowTex(D3D11_VIEWPORT &vp)
	{
		//Skala om fr�n pixlar till intervallet [0, 1]
		
		float scaleX	= vp.Width		/ (float) m_ShadowWidth;
		float scaleY	= vp.Height		/ (float) m_ShadowHeight;
		float x			= (vp.TopLeftX	/ (float) m_ShadowWidth);
		float y			= (vp.TopLeftY	/ (float) m_ShadowHeight);

		//R�kna ut Translation / Scala f�r tex-matrisen.
		XMMATRIX texTrans = XMMatrixTranslation(x, y, 0);
		XMMATRIX texScale = XMMatrixScaling(scaleX, scaleY, 1.0f);

		//R�kna ut tex.
		XMMATRIX tex = texScale * texTrans;
		
		//Omvandla till XMFLOAT4X4
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, tex);

		return result;
	}

	XMFLOAT2 HighestRes(XMFLOAT2 res1, XMFLOAT2 res2)
	{
		float pix1 = res1.x * res1.y;
		float pix2 = res2.x * res2.y;

		return (pix1 >= pix2) ? res1 : res2;
	}

	float PosToLightDist(XMFLOAT3 pos1, XMFLOAT3 pos2, float lightRange)
	{
		XMVECTOR A = XMLoadFloat3(&pos1);
		XMVECTOR B = XMLoadFloat3(&pos2);
		XMVECTOR AB = B - A;
		XMVECTOR distVec = XMVector3Length(AB);
		float dist;
		XMStoreFloat(&dist, distVec);
		dist -= lightRange;
		return dist;
	}

	XMFLOAT2 ChooseResolution(PointLight* light, XMFLOAT3 cameraPos)
	{
		float dist = PosToLightDist(cameraPos, light->Position, light->Range);

		if (dist < -1200)
			return SHADOWMAP_4096;
		else if (dist < -700)
			return SHADOWMAP_2048;
		else if (dist < -100)
			return SHADOWMAP_1024;
		else
			return SHADOWMAP_512;	
	}

	XMFLOAT2 ChooseResolution(SpotLight* light, XMFLOAT3 cameraPos)
	{
		float dist = PosToLightDist(cameraPos, light->Position, light->Range);

		if (dist < -700)
			return SHADOWMAP_4096;
		else if (dist < -500)
			return SHADOWMAP_2048;
		else if (dist < -300)
			return SHADOWMAP_1024;
		else
			return SHADOWMAP_512;		
	}

	vector<GameObject*> GetIntersectingInstances(BoundingFrustum frustum)
	{
		vector<GameObject*> instances;	
		g_QuadTree->GetIntersectingInstances(frustum, instances);
		sort( instances.begin(), instances.end() );
		instances.erase( unique( instances.begin(), instances.end() ), instances.end() );
		return instances;
	}

	vector<GameObject*> GetIntersectingInstances(BoundingOrientedBox OBB)
	{
		vector<GameObject*> instances;	
		g_QuadTree->GetIntersectingInstances(OBB, instances);
		sort( instances.begin(), instances.end() );
		instances.erase( unique( instances.begin(), instances.end() ), instances.end() );
		return instances;
	}

	void SetCurrentShadowMap(UINT shadowTileIndex, UINT shadowIndex)
	{
		UINT NewShadowMap = shadowTileIndex / (UINT)((m_ShadowWidth * m_ShadowHeight) / (512 * 512));

		if (NewShadowMap != m_CurrentShadowMap)
		{
			switch (NewShadowMap)
			{
				case 1:
					m_ShadowMapSwitches.x = shadowIndex;
					break;
				case 2:
					m_ShadowMapSwitches.y = shadowIndex;
					break;
				case 3:
					m_ShadowMapSwitches.z = shadowIndex;
					break;
			}
		}
		m_CurrentShadowMap = NewShadowMap;
	}

	void SetShadowRTV()
	{
		switch (m_CurrentShadowMap)
		{
		case 0:
			m_DeviceContext->OMSetRenderTargets( 0, NULL, m_ShadowMapDSV0 );
			break;
		case 1:
			m_DeviceContext->OMSetRenderTargets( 0, NULL, m_ShadowMapDSV1 );
			break;
		case 2:
			m_DeviceContext->OMSetRenderTargets( 0, NULL, m_ShadowMapDSV2 );
			break;
		case 3:
			m_DeviceContext->OMSetRenderTargets( 0, NULL, m_ShadowMapDSV3 );
			break;
		}
	}

	void InitFullScreenQuad();
	void InitBuffers();

	void InitShadowMap(int width, int height);
	
	void RenderShadowMaps(vector<Camera*>& cameras);
	void RenderDirShadowMaps(vector<DirectionalLight*>& dirLights, vector<Camera*>& cameras, XMFLOAT2 Resolution, int& ShadowIndex, int& ShadowTileIndex);
	void RenderPointShadowMaps(vector<PointLight*>& pointLights, XMFLOAT2 Resolution, int& ShadowIndex, int& ShadowTileIndex);
	void RenderSpotShadowMaps(vector<SpotLight*>& spotLights, XMFLOAT2 Resolution, int& ShadowIndex, int& ShadowTileIndex);

	void RenderShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, D3D11_VIEWPORT vp, BoundingFrustum& frustum);
	void RenderShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, D3D11_VIEWPORT vp, BoundingOrientedBox& OBB);
	void RenderShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, D3D11_VIEWPORT vp)								{ RenderShadowMap(lightType, View, Proj, vp, MathHelper::GenerateBoundingFrustum(View, Proj)); }
	void RenderShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, D3D11_VIEWPORT vp, Camera& camera)				{ RenderShadowMap(lightType, View, Proj, vp, camera.GetFrustum()); }	

	void RenderTerrainShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj);
	void RenderModelsShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, vector<GameObject*>& instances);

	void RenderModelsShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, BoundingFrustum& frustum) { RenderModelsShadowMap(lightType, View, Proj, GetIntersectingInstances(frustum)); }
	void RenderModelsShadowMap(UINT lightType, CXMMATRIX View, CXMMATRIX Proj, BoundingOrientedBox& OBB)  { RenderModelsShadowMap(lightType, View, Proj, GetIntersectingInstances(OBB)); }

	void RenderModelShadowMap(ModelInstance& instance, ID3DX11EffectTechnique* tech, UINT pass);	



	//void RenderModels(Player* player);	
	//void RenderTerrain(Player* player);


	void RenderTerrain(Camera* tCamera);
	void RenderModel(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass);
	void RenderModels(Camera* tCamera);


	void UpdateLights();
	void ClearBuffers();
	//void FillGBuffer(vector<Player*>& players);
	//void ComputeLight(vector<Player*>& players);

	void FillGBuffer(vector<Camera*>& Cameras);
	void ComputeLight(vector<Camera*>& Cameras);
	void CombineFinal();

	void RenderQuad(D3D11_VIEWPORT &vp, ID3D11ShaderResourceView* SRV, ID3DX11EffectTechnique* tech);

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

	
	//void Render(vector<Player*>& players);


	void Render(vector<Camera*>& Cameras);
};

#endif