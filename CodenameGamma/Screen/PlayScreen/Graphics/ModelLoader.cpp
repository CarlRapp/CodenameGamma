#include "ModelLoader.h"

bool ModelLoader::Load(const std::string& filename, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices,
		std::vector<UINT>& indices,
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats)
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
	aiProcess_MakeLeftHanded	/*	 |
	aiProcess_PreTransformVertices*/);
	//aiProcess_

	// If the import failed, report it
	if(!scene)
	  return false;

	std::vector<int> boneHierarchy;

	
	LoadBones(scene, scene->mRootNode);	
	LoadAnimationNodes(scene, scene->mRootNode);	
	LoadAnimations(scene);


	if (m_Animations.size() > 1)
		return true;

	GetVerts(scene, scene->mRootNode, vertices, indices, subsets, mats, boneHierarchy, -1);

	std::cout << m_Animations.size() << std::endl;
	std::cout << m_Bones.size() << std::endl;

	std::cout << "animationnodes" << std::endl;

	typedef std::map<std::string, AnimationNode>::iterator it_an;

	for (it_an iterator = m_AnimationNodes.begin(); iterator != m_AnimationNodes.end(); iterator++)
	{
		std::cout << iterator->first << std::endl;
	}

	std::cout << "bones" << std::endl;

	typedef std::map<std::string, int>::iterator it_b;

	for (it_b iterator = m_Bones.begin(); iterator != m_Bones.end(); iterator++)
	{
		std::cout << iterator->first << std::endl;
	}

	//Ladda ben		map a
	//Ladda noder	map b

	//convert from node to bones using b
	//convert from bonename to boneindex using a
	//convert from node to boneindices using b followed by a


	//load animations							 nodename


	//save keyframes for each node		std::map<string , vector<poskf>>
	//									std::map<string , vector<scalekf>>
	//									std::map<string , vector<rotkf>>

	//extract keyframes and insert the for each bone somehow -.-

	//extract keyframes and insert the for each node instead usning map a,b

	//load positions, normals, texcoords, tangents
	//load boneweights, boneindicies



	return true;
}


void ModelLoader::LoadBones(const aiScene* scene, aiNode* node)
{
	//for each mesh in this node.
	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		//for each bone in the mesh.
		for (UINT j = 0; j < mesh->mNumBones; ++j)
		{
			//add the bone to the bone-map
			std::string name = mesh->mBones[j]->mName.data;
			m_Bones.insert(std::pair<std::string, int>(name, m_Bones.size()));
		}
	}

	for (UINT i = 0; i < node->mNumChildren; ++i)
		LoadBones(scene, node->mChildren[i]);
}


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
	//for each animation in the aiScene
	for (UINT i = 0; i < scene->mNumAnimations; ++i)
	{
		AnimationClip animation;

		//Create a vector for all boneanimations in this animation
		std::vector<BoneAnimation> BoneAnimations(m_Bones.size());

		//for each node affected by the animation
		for (UINT j = 0; j < scene->mAnimations[i]->mNumChannels; ++j)
		{
			//creates the boneanimation for this "channel".
			BoneAnimation boneAnimation;					

			std::vector<PosKeyframe>		PosKeyframes; 
			std::vector<ScaleKeyframe>		ScaleKeyframes; 
			std::vector<RotationKeyframe>	RoatationKeyframes; 

			//number of pos, slace, rotation keyframes.
			int numPosKeys = scene->mAnimations[i]->mChannels[j]->mNumPositionKeys;
			int numScaleKeys = scene->mAnimations[i]->mChannels[j]->mNumScalingKeys;
			int numRotKeys = scene->mAnimations[i]->mChannels[j]->mNumRotationKeys;

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
*/
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

void ModelLoader::GetVerts(const aiScene* scene, aiNode *node, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices, 
		std::vector<UINT>& indices, 
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats,
		std::vector<int> boneHierarchy,
		int parent)
{
	if (!node)
		return;

	const char* name  = node->mName.C_Str();

	XMFLOAT4X4 transformation = aiMatrixToXMFloat4x4(node->mTransformation);

	int numverts = 0;

	int nodNumMesh = node->mNumMeshes;

	for (int i = 0; i < nodNumMesh; i++)
	{
		GetVerts(scene, scene->mMeshes[node->mMeshes[i]], vertices, indices, subsets, mats);
	}

	int numChildren = node->mNumChildren;

	for (int i = 0; i < numChildren; i++)
	{
		GetVerts(scene, node->mChildren[i], vertices, indices, subsets, mats, boneHierarchy, 1);
	}
}

void  ModelLoader::GetVerts(const aiScene* scene, 
		aiMesh *mesh, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices, 
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
	for (int i = 0; i < numVerts; i++)
	{
		
		Vertex::PosNormalTexTanSkinned vertex;
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

		int numWeights = mesh->mBones[i]->mNumWeights;
		for (int j = 0; j < numWeights; j++)
		{
			int index		= mesh->mBones[i]->mWeights[j].mVertexId;
			float weight	= mesh->mBones[i]->mWeights[j].mWeight;
			
			Vertex::PosNormalTexTanSkinned v = vertices[offset + index];
			v.Normal.x = 1;
			vertices[offset + index] = v;

			for (int k = 0; k < Vertex::NUMBER_OF_BONES_PER_VERTEX; ++k)
			{
				if (vertices[offset + index].boneInfos[k].Weight == 0)
				{
					vertices[offset + index].boneInfos[k].BoneIndex = m_Bones[boneName];
					vertices[offset + index].boneInfos[k].Weight	= mesh->mBones[i]->mWeights[j].mWeight;
					break;
				}	
			}
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