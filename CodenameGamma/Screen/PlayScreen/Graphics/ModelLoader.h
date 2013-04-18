#pragma once
#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <vector>
#include <iostream>
#include <string>


#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "LightHelper.h"
#include "Mesh.h"
#include "Vertex.h"
#include "MySkinnedData.h"

using namespace DirectX;

struct MaterialLoader
{
	Material Mat;
	bool AlphaClip;
	std::string EffectTypeName;
	std::string DiffuseMapName;
	std::string NormalMapName;
};

class ModelLoader
{
private:

	std::map<std::string, int> m_Bones;
	std::map<std::string, AnimationNode> m_AnimationNodes;
	std::map<std::string, AnimationClip> m_Animations;

	int NumberOfBones(const aiScene* scene, aiNode *node);

	void GetVerts(const aiScene* scene, aiNode *node, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices, 
		std::vector<UINT>& indices, 
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats,
		std::vector<int> boneHierarchy,
		int parent);
	
	void GetVerts(const aiScene* scene, 
		aiMesh *mesh, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices, 
		std::vector<UINT>& indices, 
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats);
	
	void LoadBones(const aiScene* scene, aiNode* node);

	void ModelLoader::LoadAnimationNodes(const aiScene*	scene, aiNode* node);

	void LoadAnimations(const aiScene* scene);

	/*
	void GetBonesFromNode(const aiScene*			scene,
						  aiNode*					node,
						  int						parent,
						  std::vector<int>&			boneHierarchy,
						  std::vector<XMFLOAT4X4>&	mBoneOffsets);

	void GetBonesFromMesh(const aiScene*			scene,
						  aiMesh*					mesh,
						  int						parent,
						  std::vector<int>&			boneHierarchy,
						  std::vector<XMFLOAT4X4>&	mBoneOffsets);
	*/

	//D3DXVECTOR2 convertTexCoord(aiVector3D in);
	//D3DXVECTOR4 convertTangent(aiVector3D in);

	XMFLOAT4 aiColor3DToXMFLOAT4(aiColor3D in);
	XMFLOAT3 aiVector3DToXMFLOAT3(aiVector3D in);
	XMFLOAT4 aiQuaternionToXMFLOAT4(aiQuaternion in);
	XMFLOAT2 convertTexCoord(aiVector3D in);
	XMFLOAT4 convertTangent(aiVector3D in);
	XMFLOAT4X4 aiMatrixToXMFloat4x4(aiMatrix4x4 in);

public:
	//Utan animation
	bool Load(const std::string& filename, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices,
		std::vector<UINT>& indices,
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats);

	//Animation
	/*
	bool Load(const std::string& filename, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices,
		std::vector<UINT>& indices,
		std::vector<Mesh::Subset>& subsets,
		std::vector<MaterialLoader>& mats,
		SkinnedData& skinInfo);
		*/
};

#endif