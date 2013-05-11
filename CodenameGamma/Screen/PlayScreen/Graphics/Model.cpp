#include "Model.h"

Model::Model(ID3D11Device* device, TextureManager& texMgr, const std::string& modelFilename, const std::string& texturePath)
{
	//std::vector<M3dMaterial> mats;
	std::vector<MaterialLoader> mats;

	//M3DLoader m3dLoader;
	//m3dLoader.LoadM3d(modelFilename, Vertices, Indices, Subsets, mats);

	ModelLoader loader;
	loader.Load(modelFilename, Vertices, Indices, Subsets, mats, SkinnedData);



	ModelMesh.SetVertices(device, &Vertices[0], Vertices.size());
	ModelMesh.SetIndices(device, &Indices[0], Indices.size());
	ModelMesh.SetSubsetTable(Subsets);

	SubsetCount = mats.size();

	for(UINT i = 0; i < SubsetCount; ++i)
	{
		Mat.push_back(mats[i].Mat);

		if (mats[i].DiffuseMapName != "")
		{
			ID3D11ShaderResourceView* diffuseMapSRV = texMgr.CreateTexture(texturePath + mats[i].DiffuseMapName);
			if (diffuseMapSRV)
				DiffuseMapSRV.push_back(diffuseMapSRV);
		}

		if (mats[i].NormalMapName != "")
		{
			ID3D11ShaderResourceView* normalMapSRV = texMgr.CreateTexture(texturePath + mats[i].NormalMapName);
			if (normalMapSRV)
				NormalMapSRV.push_back(normalMapSRV);
		}
	}
	
	m_BoneBoxes = SkinnedData.CreateBoneBoxes(Vertices);

	if (m_BoneBoxes.size() == 0)
	{
		//BoundingSphere::CreateFromPoints(m_BoundingSphere, Vertices.size(), &Vertices[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));

		BoundingBox AABB;
		BoundingBox::CreateFromPoints(AABB, Vertices.size(), &Vertices[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));

		BoundingOrientedBox::CreateFromBoundingBox(m_BoundingOrientedBox, AABB);

		XMFLOAT3 Extents = m_BoundingOrientedBox.Extents;
		m_SmallestRadiusInBox =	( Extents.x > Extents.z ) ? Extents.z : Extents.x;

		BoundingSphere::CreateFromBoundingBox(m_BoundingSphere, m_BoundingOrientedBox);
	}

	else
	{
		AnimationClip* AC = SkinnedData.GetAnimation("ALL");

		if (AC)
		{
			UINT numKeyFrames = AC->BoneAnimations[0].Keyframes.size();
			m_SmallestRadiusInBox = 99999;
			for (int i = 0; i < numKeyFrames; ++i)
			{
				float time = AC->BoneAnimations[0].Keyframes[i].TimePos;

				std::vector<XMFLOAT4X4> FinalTransforms(AC->BoneAnimations.size());
				SkinnedData.GetFinalTransforms("ALL", time, FinalTransforms);

				std::vector<DirectX::BoundingOrientedBox>	tempboxes(m_BoneBoxes.size());
				for (int i = 0; i < m_BoneBoxes.size(); ++i)
				{
					m_BoneBoxes[i].Transform(tempboxes[i], XMLoadFloat4x4(&FinalTransforms[i]));
				}

				std::vector<XMFLOAT3> points;
				for (int i = 0; i < tempboxes.size(); ++i)
				{
					XMFLOAT3 corners[BoundingOrientedBox::CORNER_COUNT];
					tempboxes[i].GetCorners(&corners[0]);

					for each (XMFLOAT3 point in corners)
						points.push_back(point);
				}
				BoundingBox AABB;
				BoundingBox::CreateFromPoints(AABB, points.size(), &points[0], sizeof(XMFLOAT3));

				XMFLOAT3 Extents = AABB.Extents;
				m_SmallestRadiusInBox = min(m_SmallestRadiusInBox, min(min(Extents.x, Extents.z), Extents.y));
			}

		}

		
		//m_SmallestRadiusInBox = 10;
		/*
		BoundingSphere::CreateFromBoundingBox(m_BoundingSphere, m_BoundingOrientedBox);
		BoundingOrientedBox::CreateFromBoundingBox(m_BoundingOrientedBox, AABB);
		BoundingSphere::CreateFromBoundingBox(m_BoundingSphere, m_BoundingOrientedBox);
		*/
	}

	//BoundingOrientedBox::CreateFromPoints(m_BoundingOrientedBox, Vertices.size(), &Vertices[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));
}

Model::~Model(void)
{
/*
	KOMMENTERADE BORT DETTA DÅ DESSA
	TEXTURER REDAN FANNS I TEXTURE MANAGER
	OCH DET BLEV PROBLEM NÄR TEXTURE MANAGERNS
	DESTRUKTOR KÖRS OCH SEDAN DENNAS. (EFTERSOM
	TEXTUREN DÅ REDAN ÄR BORTTAGEN!)

	if ( DiffuseMapSRV.size() > 0 )
	{
		for ( int i = 0; i < DiffuseMapSRV.size(); ++i )
		{
			if ( DiffuseMapSRV.at(i) )
			{
				DiffuseMapSRV.at(i)->Release();
				DiffuseMapSRV.at(i)	=	0;
			}
		}
		DiffuseMapSRV.clear();
	}

	if ( NormalMapSRV.size() > 0 )
	{
		for ( int i = 0; i < NormalMapSRV.size(); ++i )
		{
			if ( NormalMapSRV.at(i) )
			{
				NormalMapSRV.at(i)->Release();
				NormalMapSRV.at(i)	=	0;
			}
		}
		NormalMapSRV.clear();
	}
	*/
}

void ModelInstance::UpdateBoxes()
{
	if (m_Model)
	{	
		std::vector<XMFLOAT3> points;
		for (int i = 0; i < m_Model->m_BoneBoxes.size(); ++i)
		{
			XMVECTOR rot	= XMLoadFloat4(&m_Rotation);
			XMVECTOR trans	= XMLoadFloat3(&m_Translation);

			//m_BoneBoxes[i] = m_Model->m_BoneBoxes[i];
			m_Model->m_BoneBoxes[i].Transform(m_BoneBoxes[i], XMLoadFloat4x4(&FinalTransforms[i]));
			m_BoneBoxes[i].Transform(m_BoneBoxes[i], m_Scale, rot, trans);

			//XMMATRIX World	= XMLoadFloat4x4(&m_World);
			//m_BoneBoxes[i].Transform(m_BoneBoxes[i], World);
		}
		
	}
}

void ModelInstance::Update(float dt)
{
	if (UsingAnimationOrPose() && Animating)
	//if (ClipName != "")
	{
		TimePos += dt;
		m_Model->SkinnedData.GetFinalTransforms(ClipName, TimePos, FinalTransforms);

		// Loop animation
		if(TimePos > m_Model->SkinnedData.GetClipEndTime(ClipName))
			TimePos = 0.0f;	
	}
	//UpdateBoxes();	
}

void ModelInstance::UpdatePose()
{
	if (UsingAnimationOrPose() && !Animating)
	{
		m_Model->SkinnedData.GetFinalTransforms(ClipName, FinalTransforms);
	}
}