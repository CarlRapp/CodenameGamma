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

	ID3D11DepthStencilView		*m_DepthStencilView;
	ID3D11RenderTargetView		*m_AlbedoRTV, *m_NormalSpecRTV;
	ID3D11ShaderResourceView	*m_AlbedoSRV, *m_NormalSpecSRV, *m_DepthSRV, *m_FinalSRV;

	ID3D11UnorderedAccessView	*m_FinalUAV;

	ID3D11RenderTargetView* GBuffer[2];

	QuadTree				*m_InstanceTree;
	vector<ModelInstance*>	m_modelInstances;

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

	void UpdateLights();
	void ClearBuffers();
	void FillGBuffer(vector<Player*>& players);
	void ComputeLight(vector<Player*>& players);
	void CombineFinal();

public:
	GraphicsManager(ID3D11Device *device, ID3D11DeviceContext *deviceContext, ID3D11RenderTargetView *renderTargetView, int width, int height);
	~GraphicsManager(void);

	void AddModelInstance(ModelInstance* instance) 
	{ 
		m_modelInstances.push_back(instance); 
		sort( m_modelInstances.begin(), m_modelInstances.end() );
		m_InstanceTree->Insert(instance); 
	}
	void RemoveModelInstance(ModelInstance* instance) 
	{ 
		if (!m_modelInstances.empty()) 
		{ 
			m_InstanceTree->Delete(instance); 
			//ModelInstance* instance = m_modelInstances.back();
			m_modelInstances.erase(remove(m_modelInstances.begin(), m_modelInstances.end(), instance), m_modelInstances.end());
			//return instance;
		} 
	}
	void SetQuadTree(QuadTree *instanceTree);

	void SetTerrain(Terrain *terrain) { m_Terrain = terrain; }
	void SetLights(	vector<DirectionalLight*>			*DirLights,
					vector<PointLight*>					*PointLights,
					vector<SpotLight*>					*SpotLights)
	{
		m_DirLights		= DirLights;
		m_PointLights	= PointLights;
		m_SpotLights	= SpotLights;
	}

	void Update(vector<ModelInstance*>	&modelInstances)
	{
		/*
		vector<ModelInstance*> updatedInstances(modelInstances.size());
		vector<ModelInstance*>::iterator it;

		sort( modelInstances.begin(), modelInstances.end() );
		//sort( m_modelInstances.begin(), m_modelInstances.end() );
		it = set_intersection(modelInstances.begin(), modelInstances.end(), m_modelInstances.begin(), m_modelInstances.end(), updatedInstances.begin());
		updatedInstances.resize(it - updatedInstances.begin());
		*/
		for each (ModelInstance *instance in modelInstances)
		{
			m_InstanceTree->Update(instance);
		}
	}
	void Render(vector<Player*>& players);
	void RenderModel(ModelInstance& instance, CXMMATRIX view, CXMMATRIX proj, ID3DX11EffectTechnique* tech, UINT pass);
	void RenderTerrain(Player* player);
};

#endif