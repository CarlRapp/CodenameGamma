#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "..\..\..\stdafx.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "Vertex.h"
#include "ModelLoader.h"

#include <string>


class Model
{
public:
	Model(ID3D11Device* device, TextureManager& texMgr, const std::string& modelFilename, const std::string& texturePath);
	~Model(void);



	UINT SubsetCount;

	std::vector<Material> Mat;
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;

	
	// Keep CPU copies of the mesh data to read from.  
	std::vector<Vertex::PosNormalTexTanSkinned> Vertices;
	std::vector<UINT> Indices;
	std::vector<Mesh::Subset> Subsets;
	
	/*
	// Keep CPU copies of the mesh data to read from.  
	std::vector<Vertex::PosNormalTexTan>* Vertices;
	std::vector<USHORT>* Indices;
	std::vector<Mesh::Subset>* Subsets;
	*/
	Mesh ModelMesh;

	DirectX::BoundingSphere m_BoundingSphere;
	DirectX::BoundingBox	m_BoundingBox;
};

struct ModelInstance
{
	Model*							m_Model;
	DirectX::XMFLOAT4X4				m_World;
	DirectX::XMFLOAT4X4				m_WorldInverseTranspose;
	DirectX::BoundingSphere			m_OldBoundingSphere;

	void							*m_Node;

	ModelInstance()
	{
		m_Model					=	NULL;
		m_World					=	XMFLOAT4X4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
		m_WorldInverseTranspose	=	XMFLOAT4X4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	}

	BoundingSphere GetBoundingSphere()
	{
		BoundingSphere out;
		if (m_Model)
		{
			XMMATRIX world = XMLoadFloat4x4(&m_World);
			m_Model->m_BoundingSphere.Transform(out, world);
		}
		return out;
	}

	BoundingBox GetBoundingBox()
	{
		BoundingBox out;
		if (m_Model)
		{
			XMMATRIX world = XMLoadFloat4x4(&m_World);
			m_Model->m_BoundingBox.Transform(out, world);
		}
		return out;
	}
};


#endif