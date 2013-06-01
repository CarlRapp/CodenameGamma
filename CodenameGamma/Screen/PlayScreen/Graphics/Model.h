#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "..\..\..\stdafx.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "Vertex.h"
#include "ModelLoader.h"

using namespace std;

class Model
{
	int TPM;
	vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	vector<ID3D11ShaderResourceView*> NormalMapSRV;
public:
	Model(ID3D11Device* device, TextureManager& texMgr, const std::string& modelFilename, const std::string& texturePath);
	~Model(void);

	UINT SubsetCount;

	vector<Material> Mat;
	
	bool HasDiffuseMaps() { return !DiffuseMapSRV.empty(); }
	bool HasNormalMaps() { return !NormalMapSRV.empty(); }

	int  GetTPM() { return TPM; }

	ID3D11ShaderResourceView* GetDiffuseMap(int Subset, int textureIndex)
	{
		int index = TPM * Subset + textureIndex;
		return DiffuseMapSRV[index];
	}

	ID3D11ShaderResourceView* GetNormalMap(int Subset, int textureIndex)
	{
		int index = TPM * Subset + textureIndex;
		return NormalMapSRV[index];
	}
	
	// Keep CPU copies of the mesh data to read from.  
	vector<Vertex::PosNormalTexTanSkinned> Vertices;
	vector<UINT> Indices;
	vector<Mesh::Subset> Subsets;
	
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

	float							m_SmallestRadiusInBox;

	vector<DirectX::BoundingOrientedBox>	m_BoneBoxes;

};

#define UPDATE_INTERVAL 1.0f / 50.0f
class ModelInstance
{
private:
	Model			*m_Model;
	XMFLOAT4X4		m_World;
	float			m_Scale;

	bool			BoxesNeedsUpdate;
	float			UpdateTimer;

	int				m_TextureIndex;

	void			SortAnimations();

public:

	bool  Animating;
	vector<XMFLOAT4X4>	FinalTransforms;
	vector<Animation*>	ActiveAnimations;

	

	void UpdateBoxes();
	void Update(float dt, float AnimationSpeed, bool UpdateAnimation);


	
	XMFLOAT4						m_Rotation;
	XMFLOAT3						m_Translation;
	DirectX::XMFLOAT4X4				m_WorldInverseTranspose;
	vector<BoundingOrientedBox>		m_BoneBoxes;
	void							*m_Node;

	bool PlayingAnimation(string clipName);
	void PlayAnimation(Animation* animation);
	bool PlayAnimation(string clipName, bool loop);
	bool PlayAnimationAfter(string current, string next, bool loop);
	void StopAnimation(string clipName);
	void StopAllAnimations();
	
	bool SetAnimationSpeed(string clipName, float speed);
	bool SetAnimationProgress(string clipName, float progress);

	float GetAnimationSpeed(string clipName);
	float GetAnimationProgress(string clipName);

	float GetAnimationTime(string clipName);

	bool HasActiveAnimations() { return !ActiveAnimations.empty(); }

	void SetModel(Model* Model)
	{
		m_Model = Model;
		FinalTransforms.clear();
		if (m_Model)
		{
			XMFLOAT4X4 dummyTransform;
			XMStoreFloat4x4(&dummyTransform, XMMatrixTranslation(0,0,0));
			FinalTransforms.resize(m_Model->SkinnedData.BoneCount(), dummyTransform);
			//FinalTransforms.resize(m_Model->SkinnedData.BoneCount());
			m_BoneBoxes.resize(m_Model->SkinnedData.BoneCount());
		}
	}

	Model* GetModel() { return m_Model; }

	void SetWorld(XMFLOAT4X4& World, XMFLOAT4X4& WorldInvTrans, float& Scale)
	{
		m_World = World;
		m_WorldInverseTranspose = WorldInvTrans;
		m_Scale = Scale;
		//UpdateBoxes();
	}

	void SetTextureIndex(int index) 
	{ 
		m_TextureIndex = MathHelper::Clamp<int>(index, 0, m_Model->GetTPM() - 1); 
	}

	int GetTextureIndex() { return m_TextureIndex; }

	XMFLOAT4X4 GetWorld() { return m_World; }

	//void UpdatePose();

	ModelInstance()
	{
		m_Model					=	NULL;
		m_World					=	XMFLOAT4X4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
		m_Scale					=	1;
		m_TextureIndex			=	0;
		m_WorldInverseTranspose	=	XMFLOAT4X4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

		UpdateTimer = MathHelper::RandF(0.0f, UPDATE_INTERVAL);
		Animating = false;
	}

	BoundingSphere GetBoundingSphere()
	{
		BoundingSphere out;
		if (m_Model)
		{
			//XMMATRIX world = XMLoadFloat4x4(&m_World);
			//m_Model->m_BoundingSphere.Transform(out, world);
				
			//if (m_Model->m_BoneBoxes.empty())
			//{
				XMMATRIX world = XMLoadFloat4x4(&m_World);
				m_Model->m_BoundingSphere.Transform(out, world);
			//}
				/*
			else
			{
				out.CreateFromBoundingBox(out, GetBoundingOrientedBox());

			}*/
		
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
			//if (m_Model->m_BoneBoxes.empty())
			//{
				out	= m_Model->m_BoundingOrientedBox;				
				out.Transform(out, m_Scale, rot, trans);

				//XMMATRIX World	= XMLoadFloat4x4(&m_World);
				//out.Transform(out, World);
			//}
			/*
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
			}*/
			
			//out.Transform(out, world);
		}
		return out;
	}

	float GetSmallestRaduisInBox()
	{
		return m_Model->m_SmallestRadiusInBox * m_Scale;
	}

	bool GetJointPosition(string name, XMFLOAT3& pos)
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
		//return false;
		return bone >= 0;
	}

	bool GetJointDirection(string name, XMFLOAT3& dir)
	{
		int bone = -1;
		XMFLOAT3 jointPos = XMFLOAT3(0, 0, 0);
		m_Model->SkinnedData.GetJointData(name, bone, jointPos);
		dir = XMFLOAT3(0,1,0);

		if (bone != -1)
		{
			XMFLOAT4X4 boneTransf = FinalTransforms[bone];
			float yaw	=	-atan2(boneTransf._31, boneTransf._32);
			float pitch	=	-acos(boneTransf._33);
			float roll	=	atan2(boneTransf._13, boneTransf._23);

			XMMATRIX boneRotM = XMMatrixRotationRollPitchYaw(yaw, pitch, roll);

			XMMATRIX worldRotM = XMMatrixRotationQuaternion( XMLoadFloat4(&m_Rotation) );

			XMMATRIX finalTransfM = boneRotM * worldRotM;

			XMVECTOR temp  = XMLoadFloat3(&dir);
			temp = XMVector3TransformCoord(temp, finalTransfM);
			/*
			XMVECTOR jointPosV = XMLoadFloat3(&jointPos);
			XMVECTOR temp  = jointPosV + XMLoadFloat3(&dir);
			
			XMMATRIX transf = XMLoadFloat4x4(&FinalTransforms[bone]);

			temp = XMVector3TransformCoord(temp, transf);

			XMMATRIX world = XMLoadFloat4x4(&m_World);
			temp = XMVector3TransformCoord(temp, world);

			XMFLOAT3 pos = XMFLOAT3(0, 0, 0);
			GetJointPosition(name, pos);

			XMVECTOR posJ = XMLoadFloat3(&pos);

			temp = temp - posJ;
			*/
			XMStoreFloat3(&dir, temp);
		}

		return bone >= 0;
	}

	bool GetJointRotation(string name, XMFLOAT4& rot)
	{
		int bone = m_Model->SkinnedData.GetBoneIndex(name);		

		XMStoreFloat4(&rot, XMQuaternionIdentity());

		if (bone != -1)
		{
			XMMATRIX boneTransf = XMLoadFloat4x4(&FinalTransforms[bone]);

			XMVECTOR Offset = XMQuaternionRotationRollPitchYaw(-PI/2, 0, 0);
			XMVECTOR Offset2 = XMQuaternionRotationRollPitchYaw(0, 0, -PI/2);

			Offset = XMQuaternionMultiply(Offset, Offset2);

			XMVECTOR Quat = XMQuaternionRotationMatrix(boneTransf);

			Quat = XMQuaternionMultiply(Offset, Quat);

			Quat = XMQuaternionMultiply(Quat, XMLoadFloat4(&m_Rotation));

			XMStoreFloat4(&rot, Quat);
		}

		return bone >= 0;
	}

	string GetBoneName(UINT index)
	{
		return m_Model->SkinnedData.GetBoneName(index);
		/*
		std::string name = m_Model->SkinnedData.GetBoneName(index);
		if (name != "")
			return name;
		else
			return "body";
			*/
	}

	bool HasJoint(std::string name)
	{
		return m_Model->SkinnedData.HasJoint(name);
	}
};


#endif