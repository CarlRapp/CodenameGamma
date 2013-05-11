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

	Material mat;

	ID3D11ShaderResourceView* m_BlendMap;
	ID3D11ShaderResourceView* m_GroundTextures[4];
	ID3D11ShaderResourceView* m_NormalTextures[4];

	float m_Width;
	float m_Height;

	int m_ResolutionX;
	int m_ResolutionY;
	

	std::vector<Vertex::Terrain>	Vertices;
	std::vector<UINT>				Indices;

	XMFLOAT4X4 m_TexTransform;

private:
	void LoadTexture(ID3D11ShaderResourceView** SRV, std::string path);
	void SetVertices();
	void SetIndices();

public:
	Terrain(void);
	Terrain(ID3D11Device* Device, ID3D11DeviceContext* DeviceContext);
	~Terrain(void);

	void LoadTerrain(LevelData LData);

	void CreateGrid();
	void LoadHeightMap(std::string path, int Width, int Height);

	void SetMaterial(Material material) { mat = material; }

	void Draw(ID3D11DeviceContext* dc);

	bool HasDiffuseMaps() { return m_GroundTextures[0] != NULL; }
	bool HasNormalMaps() { return m_NormalTextures[0] != NULL; }

	Material	GetMaterial() { return mat; }
	float		GetHeight(float x, float z)		
	{ 
		if (m_HeigthMap)		
			return m_HeigthMap->GetHeight(x / m_Width, z / m_Height);		
		else
			return 0;
	}

	ID3D11ShaderResourceView* GetBlendMap() { return m_BlendMap; }
	ID3D11ShaderResourceView* GetGroundTexture(int index) { return m_GroundTextures[index]; }
	ID3D11ShaderResourceView* GetGroundNormal(int index) { return m_NormalTextures[index]; }

	XMMATRIX GetTexTransform() { return XMLoadFloat4x4(&m_TexTransform); }

	XMFLOAT2 GetDimensions() { return XMFLOAT2(m_Width, m_Height); }
};
#endif
