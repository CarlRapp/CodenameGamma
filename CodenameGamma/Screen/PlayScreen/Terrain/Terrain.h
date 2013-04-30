#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H	
#include "..\..\..\stdafx.h"
#include "HeigthMap.h"
#include "BlendMap.h"
#include "..\Graphics\Vertex.h"
#include "../Level/LevelParser.h"

class Terrain
{
	ID3D11Buffer			*m_VertexBuffer;
	ID3D11Buffer			*m_IndexBuffer;
	ID3D11DeviceContext		*m_DeviceContext;
	ID3D11Device			*m_Device;

	HeigthMap				*m_HeigthMap;
	BlendMap				*m_BlendMap;

	Material mat;

	ID3D11ShaderResourceView* m_GroundTextures[4];
	ID3D11ShaderResourceView* m_NormalTextures[4];

	int m_Width;
	int m_Height;
	float m_QuadWidth;
	float m_QuadHeight;
	
	std::vector<Vertex::Terrain>	Vertices;
	std::vector<UINT>				Indices;

	XMFLOAT4X4 m_TexTransform;

private:
	void LoadTexture(ID3D11ShaderResourceView** SRV, std::string path);
	void SetVertices();
	void SetIndices();

	void ParseTexture(std::ifstream& file, std::string texturePath, int index);
	void ParseNormal(std::ifstream& file, std::string normalPath, int index);
	void ParseBlendMap(std::ifstream& file, std::string path);
	void ParseHeightMap(std::ifstream& file, std::string path);
	void ParseTextureDim(std::ifstream& file);

public:
	Terrain(void);
	Terrain(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
	~Terrain(void);

	void LoadTerrain(LevelData LData);
	void LoadMap(std::string map);


	void CreateGrid(int Width, int Height, float QuadWidth, float QuadHeight);
	void LoadHeightMap(std::string path, int Width, int Height);
	void LoadBlendMap(std::string path, int Width, int Height);

	void SetMaterial(Material material) { mat = material; }

	void Draw(ID3D11DeviceContext* dc);


	Material	GetMaterial() { return mat; }
	float		GetHeight(float x, float z)		
	{ 
		if (m_HeigthMap)
		{
			float TotalWidth  = m_Width	 * m_QuadWidth;
			float TotalHeight = m_Height * m_QuadHeight;

			return m_HeigthMap->GetHeight(x / TotalWidth, z / TotalHeight);
		}
		else
			return 0;
	}
	ID3D11ShaderResourceView* GetGroundTexture(int index) { return m_GroundTextures[index]; }
	ID3D11ShaderResourceView* GetGroundNormal(int index) { return m_NormalTextures[index]; }

	XMMATRIX GetTexTransform() { return XMLoadFloat4x4(&m_TexTransform); }
};
#endif
