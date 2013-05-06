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
	BoundingOrientedBox::CreateFromPoints(m_BoundingOrientedBox, Vertices.size(), &Vertices[0].Pos, sizeof(Vertex::PosNormalTexTanSkinned));
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
