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

			std::vector<XMFLOAT3> bigBoxPoints;
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
					{
						points.push_back(point);
						bigBoxPoints.push_back(point);
					}
				}
				BoundingBox AABB;
				BoundingBox::CreateFromPoints(AABB, points.size(), &points[0], sizeof(XMFLOAT3));

				XMFLOAT3 Extents = AABB.Extents;
				m_SmallestRadiusInBox = min(m_SmallestRadiusInBox, min(min(Extents.x, Extents.z), Extents.y));
			}

			BoundingBox AABB;
			BoundingBox::CreateFromPoints(AABB, bigBoxPoints.size(), &bigBoxPoints[0], sizeof(XMFLOAT3));

			BoundingOrientedBox::CreateFromBoundingBox(m_BoundingOrientedBox, AABB);
			BoundingSphere::CreateFromBoundingBox(m_BoundingSphere, m_BoundingOrientedBox);
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

void ModelInstance::SortAnimations()
{
	vector<Animation*>	tempVector;

	while (!ActiveAnimations.empty())
	{
		Animation* tempAnimation = ActiveAnimations[0];
		for (int i = 1; i < ActiveAnimations.size(); ++i)
		{
			Animation* animation = ActiveAnimations[i];
			if (m_Model->SkinnedData.GetAnimationFirstBone(animation->ClipName) < m_Model->SkinnedData.GetAnimationFirstBone(tempAnimation->ClipName))
				tempAnimation = animation;
		}

		tempVector.push_back(tempAnimation);
		ActiveAnimations.erase(remove(ActiveAnimations.begin(), ActiveAnimations.end(), tempAnimation), ActiveAnimations.end());
	}

	ActiveAnimations = tempVector;

	for (int i = 0; i < ActiveAnimations.size(); ++i)
	{
		cout << "Animation " << i << ": " << ActiveAnimations[i]->ClipName << endl;
	}
	cout << endl;
}

bool ModelInstance::PlayingAnimation(string clipName)
{
	for each (Animation* animation in ActiveAnimations)
	{
		if (animation->ClipName == clipName)
		{
			return true;
		}
	}
	return false;
}

void ModelInstance::PlayAnimation(Animation* animation)
{
	int bone = m_Model->SkinnedData.GetAnimationFirstBone(animation->ClipName);

	if (bone == -1)
		return;

	for (int i = ActiveAnimations.size() - 1; i >= 0; --i)
	{
		Animation* temp = ActiveAnimations[i];
		if (bone == (m_Model->SkinnedData.GetAnimationFirstBone(temp->ClipName)))
		{
			StopAnimation(temp->ClipName);
		}
	}
	cout << "Starting animation " << animation->ClipName << "\t Number of animations: " << ActiveAnimations.size() + 1 << endl;
	ActiveAnimations.push_back(animation);
	SortAnimations();
}

bool ModelInstance::PlayAnimation(string clipName, bool loop)
{
	if (!m_Model)
		return false;

	if (m_Model->SkinnedData.HasAnimation(clipName))
	{
		Animation* animation	= new Animation();
		animation->ClipName		= clipName;
		animation->Loop			= loop;
		animation->numKeyFrames = m_Model->SkinnedData.GetNumberOfKeyFrames(clipName);
		
		PlayAnimation(animation);

		return true;
	}	
	return false;
}

bool ModelInstance::PlayAnimationAfter(string current, string next, bool loop)
{
	if (!m_Model)
		return false;
	if (!m_Model->SkinnedData.HasAnimation(next))
		return false;

	for each (Animation* animation in ActiveAnimations)
	{

		while (animation)
		{
			if (animation->ClipName == current)
			{
				if (!animation->Loop)
				{
					Animation* nextAnimation	= new Animation();
					nextAnimation->ClipName		= next;
					nextAnimation->Loop			= loop;
					nextAnimation->numKeyFrames = m_Model->SkinnedData.GetNumberOfKeyFrames(next);

					animation->nextAnimation	= nextAnimation;

					return true;
				}
				return false;
			}

			animation = animation->nextAnimation;
		}
	}
	return false;
}

void ModelInstance::StopAnimation(string clipName)
{
	Animation* temp;
	for each (Animation* animation in ActiveAnimations)
	{
		if (animation->ClipName == clipName)
		{
			temp = animation;
			cout << "Stopping animation " << clipName << "\t Number of animations: " << ActiveAnimations.size() - 1 << endl;
			break;
		}
	}
	ActiveAnimations.erase(remove(ActiveAnimations.begin(), ActiveAnimations.end(), temp), ActiveAnimations.end());
}

void ModelInstance::StopAllAnimations()
{
	for each (Animation* animation in ActiveAnimations)
	{
		delete animation;
	}

	ActiveAnimations.clear();
}

bool ModelInstance::SetAnimationSpeed(string clipName, float speed)
{
	for each (Animation* animation in ActiveAnimations)
	{
		if (animation->ClipName == clipName)
		{
			animation->AnimationSpeed = speed;
			return true;
		}
	}
	return false;
}

bool ModelInstance::SetAnimationProgress(string clipName, float progress)
{
	if (!m_Model)
		return false;

	for each (Animation* animation in ActiveAnimations)
	{
		if (animation->ClipName == clipName)
		{
			float endTime	= m_Model->SkinnedData.GetClipEndTime(clipName);

			animation->TimePos = progress * endTime;
			return true;
		}
	}
	return false;
}

float ModelInstance::GetAnimationSpeed(string clipName)
{
	for each (Animation* animation in ActiveAnimations)
	{
		if (animation->ClipName == clipName)
		{
			return animation->AnimationSpeed;
		}
	}
	return 0.0f;
}

float ModelInstance::GetAnimationProgress(string clipName)
{
	if (!m_Model)
		return 1.0f;

	for each (Animation* animation in ActiveAnimations)
	{
		if (animation->ClipName == clipName)
		{
			float endTime	= m_Model->SkinnedData.GetClipEndTime(clipName);
			return animation->TimePos / endTime;
		}
	}
	return 1.0f;
}

float ModelInstance::GetAnimationTime(string clipName)
{
	if (!m_Model)
		return 1.0f;

	if (m_Model->SkinnedData.HasAnimation(clipName))
		return m_Model->SkinnedData.GetClipEndTime(clipName);

	return 1.0f;
}

void ModelInstance::UpdateBoxes()
{
	if (m_Model && BoxesNeedsUpdate)
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
			BoxesNeedsUpdate = false;
		}
		
	}
}

void ModelInstance::Update(float dt, float AnimationSpeed, bool UpdateAnimation)
{
	BoxesNeedsUpdate = true;	

	if (HasActiveAnimations())
	{
		for each (Animation* animation in ActiveAnimations)
		{
			if (animation->numKeyFrames > 1)
				animation->TimePos += dt * animation->AnimationSpeed;
			//cout << "Updating timer: " << animation->ClipName << " - " << animation->TimePos << " - " << dt * animation->AnimationSpeed << endl;
		}

		if (UpdateTimer >= UPDATE_INTERVAL)
		{
			UpdateTimer = 0;
			m_Model->SkinnedData.GetFinalTransforms(ActiveAnimations, FinalTransforms);

			for (int i = ActiveAnimations.size() - 1; i >= 0; --i)
			{
				Animation* animation = ActiveAnimations[i];
				if(animation->TimePos > m_Model->SkinnedData.GetClipEndTime(animation->ClipName))
				{
					if (animation->Loop)
					{
						//cout << "Restarting animation: " << animation->ClipName << endl;
						animation->TimePos = 0.0f;
					}
					else
					{
						Animation* next = animation->nextAnimation;
						StopAnimation(animation->ClipName);

						if (next)
							PlayAnimation(next);						
					}
				}
			}
		}
		UpdateTimer += dt;
	}


			/*
		TimePos		+= dt * AnimationSpeed;
		

		if (UpdateTimer >= UPDATE_INTERVAL)
		{
			UpdateTimer = 0;
			m_Model->SkinnedData.GetFinalTransforms(ClipName, TimePos, FinalTransforms);

			// Loop animation
			if(TimePos > m_Model->SkinnedData.GetClipEndTime(ClipName))
				TimePos = 0.0f;	
		}		
	}*/
	//UpdateBoxes();	
}
/*
void ModelInstance::UpdatePose()
{
	if (UsingAnimationOrPose() && !Animating)
	{
		m_Model->SkinnedData.GetFinalTransforms(ClipName, FinalTransforms);
	}
}
*/