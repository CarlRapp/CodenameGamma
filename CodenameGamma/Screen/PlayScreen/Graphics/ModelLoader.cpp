#include "ModelLoader.h"

bool ModelLoader::Load(const std::string& filename, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices,
		std::vector<UINT>& indices,
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats,
		SkinnedData& skinInfo)
{
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile( filename, 
	aiProcess_CalcTangentSpace       |
	aiProcess_Triangulate            |
	aiProcess_JoinIdenticalVertices  |
	aiProcess_SortByPType			 |
	aiProcess_FlipUVs				 |
	aiProcess_FixInfacingNormals	 |
	aiProcess_MakeLeftHanded		 |
	aiProcess_LimitBoneWeights		/* |
	aiProcess_PreTransformVertices*/);
	//aiProcess_

	std::cout << "Starting to load " << filename << std::endl;

	// If the import failed, report it
	if(!scene)
	  return false;

	std::vector<XMFLOAT4X4> boneOffsets;
	std::vector<int> boneIndexToParentIndex;
	std::map<std::string, AnimationClip> animations;


	LoadBones(scene, scene->mRootNode, boneOffsets);

	UINT numBones = m_BoneToIndex.size();
	boneIndexToParentIndex.resize(numBones);

	XMFLOAT4X4 toParrent;
	XMStoreFloat4x4(&toParrent, XMMatrixTranslation(0,0,0));

	LoadBoneHierarchy(scene, scene->mRootNode, boneIndexToParentIndex, -1, boneOffsets);

	typedef std::map<std::string, int>::iterator it_type;
	for(it_type iterator1 = m_BoneToIndex.begin(); iterator1 != m_BoneToIndex.end(); iterator1++) 
	{

		int parent = boneIndexToParentIndex[iterator1->second];
		std::string parentName;
		for(it_type iterator2 = m_BoneToIndex.begin(); iterator2 != m_BoneToIndex.end(); iterator2++) 
		{
			if (iterator2->second == parent)
			{
				parentName = iterator2->first;
				break;
			}
		}

		std::cout << iterator1->first << " has parent: " << parentName << std::endl;
		// iterator->first = key
		// iterator->second = value
		// Repeat if you also want to iterate through the second map.
	}

	AnimationClip animationClip;
	LoadAnimation(scene, animationClip);

	if (!animationClip.BoneAnimations.empty())
		animations["ALL"] = animationClip;

	//LoadBones(scene, scene->mRootNode);	


	//LoadAnimationNodes(scene, scene->mRootNode);	
	//LoadAnimations(scene);


	std::vector<Vertex::PosNormalTexTanSkinnedTemp> tempVerts;
	GetVerts(scene, scene->mRootNode, tempVerts, indices, subsets, mats);


	for each (Vertex::PosNormalTexTanSkinnedTemp vertex in tempVerts)
	{		
		float sum = vertex.Weight.x + vertex.Weight.y + vertex.Weight.z + vertex.Weight.w;
		if (sum != 0.0f)
		{			
			XMVECTOR w = XMLoadFloat4(&vertex.Weight);
			w /= sum;
			XMStoreFloat4(&vertex.Weight, w);	
		}

		Vertex::PosNormalTexTanSkinned v;
		v.Pos			= vertex.Pos;
		v.Normal		= vertex.Normal;
		v.Tex			= vertex.Tex;
		v.TangentU		= vertex.TangentU;
		v.BoneIndex[0]	= vertex.BoneIndex[0];
		v.BoneIndex[1]	= vertex.BoneIndex[1];
		v.BoneIndex[2]	= vertex.BoneIndex[2];
		v.BoneIndex[3]	= vertex.BoneIndex[3];
		v.Weight.x		= vertex.Weight.x;
		v.Weight.y		= vertex.Weight.y;
		v.Weight.z		= vertex.Weight.z;

		vertices.push_back(v);
	}

	USHORT numBoneAnimation = animationClip.BoneAnimations.size();

	std::cout << "Number of BoneAnimations " << numBoneAnimation << std::endl;
	std::cout << "Number of bones " << m_BoneToIndex.size() << std::endl;

	skinInfo.Set(boneIndexToParentIndex, boneOffsets, animations, m_BoneToIndex, m_IndexToBone, m_Joints);

	LoadAnimationClipsAndPoses(filename, skinInfo);
	
	
	XMFLOAT4X4 globalInverseTransform = aiMatrixToXMFloat4x4(scene->mRootNode->mTransformation);
	XMMATRIX temp = XMLoadFloat4x4(&globalInverseTransform);
	XMVECTOR detV = XMMatrixDeterminant(temp);
	temp = XMMatrixInverse(&detV, temp);
	XMStoreFloat4x4(&globalInverseTransform, temp);
	skinInfo.mGlobalInverseTransform = globalInverseTransform;

	std::cout << std::endl << std::endl << std::endl;
	return true;
}

void ModelLoader::LoadBones(const aiScene* scene, aiNode* node, std::vector<XMFLOAT4X4>& boneOffsets)
{
	//for each mesh in this node.
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		//for each bone in the mesh.
		for (UINT j = 0; j < mesh->mNumBones; ++j)
		{
			//add the bone to the bone-map
			std::string name	= mesh->mBones[j]->mName.data;
			int boneIndex		= m_BoneToIndex.size();
			m_BoneToIndex.insert(std::pair<std::string, int>(name, boneIndex));
			m_IndexToBone.insert(std::pair<int, std::string>(boneIndex, name));

			std::cout << name << " - " << boneIndex << std::endl;

			XMFLOAT4X4 offsetM = aiMatrixToXMFloat4x4(mesh->mBones[j]->mOffsetMatrix);

			boneOffsets.push_back(offsetM);

		}
	}	

	for (UINT i = 0; i < node->mNumChildren; ++i)
		LoadBones(scene, node->mChildren[i], boneOffsets);
}

void ModelLoader::LoadBoneHierarchy(const aiScene* scene, aiNode* node, std::vector<int>& boneIndexToParentIndex, int parrentIndex, std::vector<XMFLOAT4X4> boneOffsets)
{
	if (m_BoneToIndex.empty())
		return;

	int boneIndex = -1;
	if (m_BoneToIndex.find(node->mName.data) != m_BoneToIndex.end()) 
	{
		boneIndex = m_BoneToIndex[node->mName.data];
		boneIndexToParentIndex[boneIndex] = parrentIndex;
    }

	XMMATRIX transformation;
	XMVECTOR det;
	XMMATRIX offsetM;
	if (parrentIndex >= 0)	
	{
		offsetM = XMLoadFloat4x4(&boneOffsets[parrentIndex]);
		transformation = XMLoadFloat4x4(&aiMatrixToXMFloat4x4(node->mTransformation));	
		det = XMMatrixDeterminant(transformation);
		transformation = offsetM * XMMatrixInverse(&det, transformation);
	}
	else
	{
		transformation = XMLoadFloat4x4(&boneOffsets[0]);	
	}		

	det = XMMatrixDeterminant(transformation);
	transformation = XMMatrixInverse(&det, transformation);

	Joint joint;
	joint.parent = parrentIndex;
	joint.position = XMFLOAT3(0,0,0);
	XMVECTOR posV = XMLoadFloat3(&joint.position);
	posV = XMVector3TransformCoord(posV, transformation);
	XMStoreFloat3(&joint.position, posV);

	std::string name	= node->mName.data;
	m_Joints.insert(std::pair<std::string, Joint>(name, joint));	

	for (UINT i = 0; i < node->mNumChildren; ++i)
		LoadBoneHierarchy(scene, node->mChildren[i], boneIndexToParentIndex, boneIndex, boneOffsets);
}

void ModelLoader::LoadAnimation(const aiScene* scene, AnimationClip& animationClip)
{

	int numAnimations = min(scene->mNumAnimations, 1);

	for (UINT i = 0; i < numAnimations; ++i)
	{
		//Create a vector for all boneanimations in this animation
		std::vector<BoneAnimation> BoneAnimations(m_BoneToIndex.size());

		std::cout << "Animation: " << scene->mAnimations[i]->mName.data << std::endl;

		//for each node affected by the animation
		for (UINT j = 0; j < scene->mAnimations[i]->mNumChannels; ++j)
		{
			std::string nodeName = scene->mAnimations[i]->mChannels[j]->mNodeName.data;

			if (m_BoneToIndex.find(nodeName) != m_BoneToIndex.end())
			{
				std::cout << "Channel: " << nodeName << std::endl;

				//creates the boneanimation for this "channel".
				BoneAnimation boneAnimation;					

				std::vector<Keyframe>			Keyframes; 

				//number of pos, slace, rotation keyframes.
				int numPosKeys = scene->mAnimations[i]->mChannels[j]->mNumPositionKeys;
				int numScaleKeys = scene->mAnimations[i]->mChannels[j]->mNumScalingKeys;
				int numRotKeys = scene->mAnimations[i]->mChannels[j]->mNumRotationKeys;


				std::cout << "numPosKeys: " << numPosKeys << std::endl;
				std::cout << "numScaleKeys: " << numScaleKeys << std::endl;
				std::cout << "numRotKeys: " << numRotKeys << std::endl;


				//Adding all the Keyframes.
				assert(numPosKeys == numScaleKeys && numPosKeys == numRotKeys);
				for (int key = 0; key < numPosKeys; ++key)
				{
					Keyframe keyFrame;

					keyFrame.Translation	= aiVector3DToXMFLOAT3(scene->mAnimations[i]->mChannels[j]->mPositionKeys[key].mValue);
					keyFrame.Scale			= aiVector3DToXMFLOAT3(scene->mAnimations[i]->mChannels[j]->mScalingKeys[key].mValue);
					keyFrame.RotationQuat	= aiQuaternionToXMFLOAT4(scene->mAnimations[i]->mChannels[j]->mRotationKeys[key].mValue);

					float posTime = (float)scene->mAnimations[i]->mChannels[j]->mPositionKeys[key].mTime;
					float scaleTime = (float)scene->mAnimations[i]->mChannels[j]->mScalingKeys[key].mTime;
					float rotTime = (float)scene->mAnimations[i]->mChannels[j]->mRotationKeys[key].mTime;

					assert(posTime == scaleTime && posTime == rotTime);
					keyFrame.TimePos		= posTime;

					Keyframes.push_back(keyFrame);
				}

			
				int boneIndex = m_BoneToIndex[nodeName];

				BoneAnimations[boneIndex].Keyframes	= Keyframes;
			}
		}
		animationClip.BoneAnimations = BoneAnimations;
		animationClip.numKeyFrames   = animationClip.BoneAnimations[0].Keyframes.size();
	}	
}

void ModelLoader::LoadAnimationClipsAndPoses(const std::string& filename, SkinnedData& skinInfo)
{
	skinInfo.CreatePose("StartPose", 0, 0);
	//read file
	UINT lastIndex = filename.find_last_of('.');
	std::string path = filename.substr(0, lastIndex) + ".txt";

	std::string token;

	std::ifstream file;
	file.open(path);

	if(!file)
		return;

	while(!file.eof())
	{
		file >> token;

		if (token == "AnimationClip")
		{
			std::string name;
			int firstFrame;
			int lastFrame;
			float TimeScale;
			std::string FirstBone;

			file >> name;
			file >> firstFrame;
			file >> lastFrame;
			file >> TimeScale;
			file >> FirstBone;

			skinInfo.CreateClip(name, firstFrame, lastFrame, TimeScale, m_BoneToIndex[FirstBone]);
		}
		else if (token == "CombineAnimation")
		{
			std::string name1;
			std::string name2;
			std::string name3;
			float TimeScale;
			std::string FirstBone;

			file >> name1;
			file >> name2;
			file >> name3;
			file >> TimeScale;
			file >> FirstBone;

			skinInfo.CreateClip(name1, name2, name3, TimeScale, m_BoneToIndex[FirstBone]);
		}
		else if (token == "Pose")
		{
			std::string name;
			int frame;
			std::string FirstBone;

			file >> name;
			file >> frame;
			file >> FirstBone;
			skinInfo.CreatePose(name, frame, m_BoneToIndex[FirstBone]);
		}
		else if (token == "//")
		{
			string str;
			getline(file, str);
		}
		else
			return;

	}
}


void ModelLoader::GetVerts(const aiScene* scene, aiNode *node, 
		std::vector<Vertex::PosNormalTexTanSkinnedTemp>& vertices, 
		std::vector<UINT>& indices, 
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats)
{
	if (!node)
		return;

	const char* name  = node->mName.C_Str();

	

	XMFLOAT4X4 transformation = aiMatrixToXMFloat4x4(node->mTransformation);

	int numverts = 0;

	int nodNumMesh = node->mNumMeshes;

	std::cout << "visit: " << name << " - meshses: " << nodNumMesh << std::endl;

	for (int i = 0; i < nodNumMesh; i++)
	{
		GetVerts(scene, scene->mMeshes[node->mMeshes[i]], vertices, indices, subsets, mats);
	}

	int numChildren = node->mNumChildren;

	for (int i = 0; i < numChildren; i++)
	{
		GetVerts(scene, node->mChildren[i], vertices, indices, subsets, mats);
	}
}

void  ModelLoader::GetVerts(const aiScene* scene, 
		aiMesh *mesh, 
		std::vector<Vertex::PosNormalTexTanSkinnedTemp>& vertices, 
		std::vector<UINT>& indices, 
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats)
{
	if (!mesh)
		return;

	aiColor3D amb, diff, spec;
	float shininess;
	scene->mMaterials[mesh->mMaterialIndex]->Get<aiColor3D>(AI_MATKEY_COLOR_AMBIENT, amb);
	scene->mMaterials[mesh->mMaterialIndex]->Get<aiColor3D>(AI_MATKEY_COLOR_DIFFUSE, diff);
	scene->mMaterials[mesh->mMaterialIndex]->Get<aiColor3D>(AI_MATKEY_COLOR_SPECULAR, spec);
	scene->mMaterials[mesh->mMaterialIndex]->Get<float>(AI_MATKEY_SHININESS, shininess);

	MaterialLoader material;
	material.Mat.SpecIntensity	= (aiColor3DToXMFLOAT4(spec).x + aiColor3DToXMFLOAT4(spec).y + aiColor3DToXMFLOAT4(spec).z) / 3;
	material.Mat.SpecPower		= shininess;
	/*
	material.Mat.Ambient = D3DXVECTOR4(1,1,1,0);
	material.Mat.Diffuse = D3DXVECTOR4(1,1,1,0);
	material.Mat.Specular = D3DXVECTOR4(1,1,1,3);

	*/
	material.AlphaClip = 0;
	material.EffectTypeName = "Basic";
	

	int numbones = mesh->mNumBones;

	std::cout << "bones: " << numbones << std::endl;

	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		const char* a = mesh->mBones[i]->mName.C_Str();
		int hejrasda = 113246;
	}

	if (scene->mMaterials[mesh->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString path;
		scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL);
		material.DiffuseMapName = path.C_Str();
	}

	if (scene->mMaterials[mesh->mMaterialIndex]->GetTextureCount(aiTextureType_HEIGHT) > 0)
	{
		aiString path;
		material.NormalMapName = scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_HEIGHT, 0, &path, NULL, NULL, NULL, NULL, NULL);
		material.NormalMapName = path.C_Str();
	}

	if (scene->mMaterials[mesh->mMaterialIndex]->GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		aiString path;
		material.NormalMapName = scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL);
		material.NormalMapName = path.C_Str();
	}	
	mats.push_back(material);

	Mesh::Subset subset;

	subset.Id			= subsets.size();
	subset.VertexStart	= indices.size();
	subset.FaceStart	= indices.size() / 3;


	UINT offset = vertices.size();
	int numVerts = mesh->mNumVertices;

	std::cout << "numVerts: " << numVerts << std::endl;

	for (int i = 0; i < numVerts; i++)
	{
		
		Vertex::PosNormalTexTanSkinnedTemp vertex;
		vertex.Pos		= aiVector3DToXMFLOAT3(mesh->mVertices[i]);
		
		
		vertex.Normal	= aiVector3DToXMFLOAT3(mesh->mNormals[i]);
		
		vertex.Tex		= convertTexCoord(mesh->mTextureCoords[0][i]);
		
		//vertex.Tex		= D3DXVECTOR2(0,0);
		vertex.TangentU	= convertTangent(mesh->mTangents[i]);
		

		vertices.push_back(vertex);
		
	}
	int numFaces = mesh->mNumFaces;

    for(UINT i = 0; i < numFaces; ++i)
    {
		int numIndices = mesh->mFaces[i].mNumIndices;

		if (numIndices == 3)
		{
			for (int j = 0; j < numIndices; j++)
			{
				indices.push_back(offset + mesh->mFaces[i].mIndices[j]);
			}
		}
		else
		{
			int ingatrianglar = 707;
		}
    }

	
	int numBones = mesh->mNumBones;

    for(UINT i = 0; i < numBones; ++i)
    {
		std::string boneName = mesh->mBones[i]->mName.C_Str();

		int boneIndex = m_BoneToIndex[boneName];

		int numWeights = mesh->mBones[i]->mNumWeights;
		for (int j = 0; j < numWeights; j++)
		{
			int index		= mesh->mBones[i]->mWeights[j].mVertexId;
			float weight	= mesh->mBones[i]->mWeights[j].mWeight;
			
			
			Vertex::PosNormalTexTanSkinnedTemp *v = &vertices[offset + index];
			//v.Normal.x = 1;
			//vertices[offset + index] = v;			
			
			if (weight != 1.0f)
				std::cout << "NoN 1.0 Weight: " << weight << std::endl;

			if (v->Weight.x == 0)
			{
				v->BoneIndex[0] = boneIndex;
				v->Weight.x	= weight;
			}
			else if (v->Weight.y == 0)
			{
				v->BoneIndex[1] = boneIndex;
				v->Weight.y	= weight;
			}
			else if (vertices[offset + index].Weight.z == 0)
			{
				v->BoneIndex[2] = boneIndex;
				v->Weight.z	= weight;
			}
			else if (vertices[offset + index].Weight.w == 0)
			{
				v->BoneIndex[3] = boneIndex;
				v->Weight.w	= weight;
			}

			/*
			for (int k = 0; k < Vertex::NUMBER_OF_BONES_PER_VERTEX; ++k)
			{
				if (vertices[offset + index].boneInfos[k].Weight == 0)
				{

					vertices[offset + index].boneInfos[k].BoneIndex = m_Bones[boneName];
					vertices[offset + index].boneInfos[k].Weight	= mesh->mBones[i]->mWeights[j].mWeight;
					break;
				}	
			}
			*/
		}
    }
	

	subset.VertexCount	= indices.size()		- subset.VertexStart;
	subset.FaceCount	= (indices.size() / 3)	- subset.FaceStart;

	subsets.push_back(subset);
}

XMFLOAT4 ModelLoader::aiColor3DToXMFLOAT4(aiColor3D in)
{
	XMFLOAT4 out;
	out.x = in.r;
	out.y = in.g;
	out.z = in.b;
	out.w = 0;
	return out;
}

XMFLOAT3 ModelLoader::aiVector3DToXMFLOAT3(aiVector3D in)
{
	XMFLOAT3 out;
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
	return out;
}

XMFLOAT4 ModelLoader::aiQuaternionToXMFLOAT4(aiQuaternion in)
{
	XMFLOAT4 out;
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
	out.w = in.w;
	return out;
}

XMFLOAT2 ModelLoader::convertTexCoord(aiVector3D in)
{
	XMFLOAT2 out;
	out.x = in.x;
	out.y = in.y;
	return out;
}

XMFLOAT4 ModelLoader::convertTangent(aiVector3D in)
{
	XMFLOAT4 out;
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
	out.w = -1;
	return out;
}

XMFLOAT4X4 ModelLoader::aiMatrixToXMFloat4x4(aiMatrix4x4 in)
{
	XMFLOAT4X4 matrix;
	/*
	matrix._11 = in.a1;
	matrix._12 = in.a2;
	matrix._13 = in.a3;
	matrix._14 = in.a4;
	matrix._21 = in.b1;
	matrix._22 = in.b2;
	matrix._23 = in.b3;
	matrix._24 = in.b4;
	matrix._31 = in.c1;
	matrix._32 = in.c2;
	matrix._33 = in.c3;
	matrix._34 = in.c4;
	matrix._41 = in.d1;
	matrix._42 = in.d2;
	matrix._43 = in.d3;
	matrix._44 = in.d4;
	*/


	matrix._11 = in.a1;
	matrix._12 = in.b1;
	matrix._13 = in.c1;
	matrix._14 = in.d1;
	matrix._21 = in.a2;
	matrix._22 = in.b2;
	matrix._23 = in.c2;
	matrix._24 = in.d2;
	matrix._31 = in.a3;
	matrix._32 = in.b3;
	matrix._33 = in.c3;
	matrix._34 = in.d3;
	matrix._41 = in.a4;
	matrix._42 = in.b4;
	matrix._43 = in.c4;
	matrix._44 = in.d4;



	return matrix;
}

/*
D3DXVECTOR2 ModelLoader::convertTexCoord(aiVector3D in)
{
	D3DXVECTOR2 out;
	out.x = in.x;
	out.y = in.y;
	return out;
}

D3DXVECTOR4 ModelLoader::convertTangent(aiVector3D in)
{
	D3DXVECTOR4 out;
	out.x = in.x;
	out.y = in.y;
	out.z = in.z;
	out.w = -1;
	return out;
}

*/


/*
void ModelLoader::LoadAnimationNodes(const aiScene* scene, aiNode* node)
{
	AnimationNode aniNode;
	std::string nodeName = node->mName.data;
	//for each mesh in tht node.
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		//for each bone in the mesh.
		for (UINT j = 0; j < mesh->mNumBones; ++j)
		{
			//add the name of the bone to the list.
			aiBone *bone = mesh->mBones[j];
			aniNode.bones.push_back(bone->mName.data);
		}
	}

	//inserting the AnimationNode to the map.
	m_AnimationNodes.insert(std::pair<std::string, AnimationNode>(nodeName, aniNode));

	//Repeat for all the childerens.
	for (UINT i = 0; i < node->mNumChildren; ++i)
		LoadAnimationNodes(scene, node->mChildren[i]);
}



void ModelLoader::LoadAnimations(const aiScene* scene)
{

	std::cout << "ANIMATIONLOAD" << std::endl;
	//for each animation in the aiScene
	for (UINT i = 0; i < scene->mNumAnimations; ++i)
	{
		AnimationClip animation;
		//Create a vector for all boneanimations in this animation
		std::vector<BoneAnimation> BoneAnimations(m_Bones.size());

		std::cout << "Animation: " << scene->mAnimations[i]->mName.data << std::endl;


		//for each node affected by the animation
		for (UINT j = 0; j < scene->mAnimations[i]->mNumChannels; ++j)
		{

			std::cout << "Channel: " << scene->mAnimations[i]->mChannels[j]->mNodeName.data << std::endl;

			//creates the boneanimation for this "channel".
			BoneAnimation boneAnimation;					

			std::vector<PosKeyframe>		PosKeyframes; 
			std::vector<ScaleKeyframe>		ScaleKeyframes; 
			std::vector<RotationKeyframe>	RoatationKeyframes; 

			//number of pos, slace, rotation keyframes.
			int numPosKeys = scene->mAnimations[i]->mChannels[j]->mNumPositionKeys;
			int numScaleKeys = scene->mAnimations[i]->mChannels[j]->mNumScalingKeys;
			int numRotKeys = scene->mAnimations[i]->mChannels[j]->mNumRotationKeys;


			std::cout << "numPosKeys: " << numPosKeys << std::endl;
			std::cout << "numScaleKeys: " << numScaleKeys << std::endl;
			std::cout << "numRotKeys: " << numRotKeys << std::endl;

			//Adding all the Posision keyframes.
			for (int key = 0; key < numPosKeys; ++key)
			{
				PosKeyframe poskeyframe;
				poskeyframe.Translation = aiVector3DToXMFLOAT3(scene->mAnimations[i]->mChannels[j]->mPositionKeys[key].mValue);
				poskeyframe.TimePos		= (float)scene->mAnimations[i]->mChannels[j]->mPositionKeys[key].mTime;

				PosKeyframes.push_back(poskeyframe);
			}

			//Adding all the Scaling keyframes.
			for (int key = 0; key < numScaleKeys; ++key)
			{
				ScaleKeyframe scalekeyframe;
				scalekeyframe.Scale			= aiVector3DToXMFLOAT3(scene->mAnimations[i]->mChannels[j]->mScalingKeys[key].mValue);
				scalekeyframe.TimePos		= (float)scene->mAnimations[i]->mChannels[j]->mScalingKeys[key].mTime;

				ScaleKeyframes.push_back(scalekeyframe);
			}

			//Adding all the Rotation keyframes.
			for (int key = 0; key < numRotKeys; ++key)
			{
				
				RotationKeyframe rotationkeyframe;
				rotationkeyframe.RotationQuat	= aiQuaternionToXMFLOAT4(scene->mAnimations[i]->mChannels[j]->mRotationKeys[key].mValue);
				rotationkeyframe.TimePos		= (float)scene->mAnimations[i]->mChannels[j]->mRotationKeys[key].mTime;

				RoatationKeyframes.push_back(rotationkeyframe);				
			}

			//name of the node

			std::string nodeName = scene->mAnimations[i]->mChannels[j]->mNodeName.data;
			AnimationNode aniNode = m_AnimationNodes[nodeName]; //hämtar listan på ben som tillhör noden

			std::cout << "Bones: " << aniNode.bones.size() << std::endl;
			for (UINT k = 0; k < aniNode.bones.size(); ++k)
			{
				std::string boneName = aniNode.bones[k];	
				int boneIndex = m_Bones[boneName];			//hämtar benet

				//Adding the boneanimation to the list
				BoneAnimations[boneIndex].PosKeyframes			= PosKeyframes;
				BoneAnimations[boneIndex].ScaleKeyframes		= ScaleKeyframes;
				BoneAnimations[boneIndex].RoatationKeyframes	= RoatationKeyframes;
			}
			//Adding the boneanimation to the animation-map.
			//BoneAnimations.insert(std::pair<std::string, BoneAnimation>(nodeName, boneAnimation));
		}
		std::string animationName = scene->mAnimations[i]->mName.data;
		animation.BoneAnimations = BoneAnimations;

		//Adding animation
		m_Animations.insert(std::pair<std::string, AnimationClip>(animationName, animation));
	}	
}
*/
/*
void ModelLoader::GetBonesFromNode(const aiScene*			scene,
								   aiNode*					node,
								   int						parent,
								   std::vector<int>&		boneHierarchy,
								   std::vector<XMFLOAT4X4>&	mBoneOffsets)
{
	if (!node)
		return;

	for (int i = 0; i < node->mNumMeshes; ++i)
	{
		GetBonesFromMesh(scene,
						 scene->mMeshes[i], 
						 parent, 
						 boneHierarchy, 
						 mBoneOffsets);
	}

	int newParent;
	
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		GetBonesFromNode(scene,	
						 node->mChildren[i], 
						 newParent, 
						 boneHierarchy, 
						 mBoneOffsets);
	}

}

void ModelLoader::GetBonesFromMesh(const aiScene*			scene,
								   aiMesh*					mesh,
								   int						parent,
								   std::vector<int>&		boneHierarchy,
								   std::vector<XMFLOAT4X4>&	mBoneOffsets)
{
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		//push back parent for this bone.
		boneHierarchy.push_back(parent);

		//push back the offsetmatrix for this bone.
		XMFLOAT4X4 offset = aiMatrixToXMFloat4x4(mesh->mBones[i]->mOffsetMatrix);
		mBoneOffsets.push_back(offset);
	}
}

int ModelLoader::NumberOfBones(const aiScene* scene, aiNode *node)
{
	if (!node)
		return 0;

	int numbones = 0;

	int nodNumMesh = node->mNumMeshes;

	for (int i = 0; i < nodNumMesh; i++)
	{
		numbones += scene->mMeshes[i]->mNumBones;
	}

	int numChildren = node->mNumChildren;

	for (int i = 0; i < numChildren; i++)
	{
		numbones += NumberOfBones(scene, node->mChildren[i]);
	}
	return numbones;
}
*/