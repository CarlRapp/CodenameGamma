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

	bool HasDiffuseMaps() { return !DiffuseMapSRV.empty(); }
	bool HasNormalMaps() { return !NormalMapSRV.empty(); }
	
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
	SkinnedData SkinnedData;

	BoundingSphere					m_BoundingSphere;
	BoundingOrientedBox				m_BoundingOrientedBox;

	std::vector<DirectX::BoundingOrientedBox>	m_BoneBoxes;

};

struct ModelInstance
{
	Model							*m_Model;
	DirectX::XMFLOAT4X4				m_World;

	float TimePos;
	bool  Animating;
	std::string ClipName;
	std::vector<XMFLOAT4X4> FinalTransforms;
	void Update(float dt);


	float							m_Scale;
	XMFLOAT4						m_Rotation;
	XMFLOAT3						m_Translation;


	DirectX::XMFLOAT4X4				m_WorldInverseTranspose;
	DirectX::BoundingSphere			m_OldBoundingSphere;

	std::vector<DirectX::BoundingOrientedBox>	m_BoneBoxes;

	void							*m_Node;

	bool UsingAnimationOrPose() { return ClipName != ""; }

	void SetModel(Model* Model)
	{
		m_Model = Model;
		FinalTransforms.clear();
		if (m_Model)
		{
			FinalTransforms.resize(m_Model->SkinnedData.BoneCount());
			m_BoneBoxes.resize(m_Model->SkinnedData.BoneCount());
		}
	}

	void UpdatePose();

	ModelInstance()
	{
		m_Model					=	NULL;
		m_World					=	XMFLOAT4X4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
		m_Scale					=	1;
		m_Rotation				=	XMFLOAT4(0,0,0,0);
		m_Translation			=	XMFLOAT3(1,1,1);
		m_WorldInverseTranspose	=	XMFLOAT4X4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

		TimePos = 0;
		ClipName = "";
		Animating = false;
	}

	BoundingSphere GetBoundingSphere()
	{
		BoundingSphere out;
		if (m_Model)
		{
			//XMMATRIX world = XMLoadFloat4x4(&m_World);
			//m_Model->m_BoundingSphere.Transform(out, world);
				
			if (m_Model->m_BoneBoxes.empty())
			{
				XMMATRIX world = XMLoadFloat4x4(&m_World);
				m_Model->m_BoundingSphere.Transform(out, world);
			}

			else
			{
				out.CreateFromBoundingBox(out, GetBoundingOrientedBox());

			}
		
		}
		return out;
	}

	BoundingOrientedBox GetBoundingOrientedBox()
	{
		BoundingOrientedBox out;
		if (m_Model)
		{			
			XMVECTOR rot	= XMLoadFloat4(&m_Rotation);
			XMVECTOR trans	= XMLoadFloat3(&m_Translation);
			if (m_Model->m_BoneBoxes.empty())
			{
				out	= m_Model->m_BoundingOrientedBox;				
				out.Transform(out, m_Scale, rot, trans);
			}
			
			else
			{
				std::vector<XMFLOAT3> points;

				for (int i = 0; i < m_BoneBoxes.size(); ++i)
				{
					XMFLOAT3 corners[BoundingOrientedBox::CORNER_COUNT];
					m_BoneBoxes[i].GetCorners(&corners[0]);

					for each (XMFLOAT3 point in corners)
						points.push_back(point);
				}
				BoundingBox AABB;
				out.CreateFromPoints(out, points.size(), &points[0], sizeof(XMFLOAT3));
				//out.CreateFromBoundingBox(out, AABB);
			}
			
			//out.Transform(out, world);
		}
		return out;
	}

	bool GetJointPosition(std::string name, XMFLOAT3& pos)
	{
		int bone = -1;
		pos = XMFLOAT3(0,0,0);
		m_Model->SkinnedData.GetJointData(name, bone, pos);

		if (bone != -1)
		{
			XMVECTOR temp   = XMLoadFloat3(&pos);
			XMMATRIX transf = XMLoadFloat4x4(&FinalTransforms[bone]);

			temp = XMVector3TransformCoord(temp, transf);

			XMMATRIX world = XMLoadFloat4x4(&m_World);
			temp = XMVector3TransformCoord(temp, world);

			XMStoreFloat3(&pos, temp);
		}

		return bone >= 0;
	}
};


#endif