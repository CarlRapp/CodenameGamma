#include "Model.h"

Model::Model(ID3D11Device* device, TextureManager& texMgr, const std::string& modelFilename, const std::string& texturePath)
{
	//std::vector<M3dMaterial> mats;
	std::vector<MaterialLoader> mats;

	//M3DLoader m3dLoader;
	//m3dLoader.LoadM3d(modelFilename, Vertices, Indices, Subsets, mats);

	ModelLoader loader;
	loader.Load(modelFilename, Vertices, Indices, Subsets, mats);

	ModelMesh.SetVertices(device, &Vertices[0], Vertices.size());
	ModelMesh.SetIndices(device, &Indices[0], Indices.size());
	ModelMesh.SetSubsetTable(Subsets);

	SubsetCount = mats.size();

	for(UINT i = 0; i < SubsetCount; ++i)
	{
		Mat.push_back(mats[i].Mat);

		ID3D11ShaderResourceView* diffuseMapSRV = texMgr.CreateTexture(texturePath + mats[i].DiffuseMapName);
		DiffuseMapSRV.push_back(diffuseMapSRV);

		ID3D11ShaderResourceView* normalMapSRV = texMgr.CreateTexture(texturePath + mats[i].NormalMapName);
		NormalMapSRV.push_back(normalMapSRV);
	}

	BoundingSphere::CreateFromPoints(m_BoundingSphere, Vertices.size(), &Vertices[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));
	BoundingBox::CreateFromPoints(m_BoundingBox, Vertices.size(), &Vertices[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));
}


Model::~Model(void)
{
}
