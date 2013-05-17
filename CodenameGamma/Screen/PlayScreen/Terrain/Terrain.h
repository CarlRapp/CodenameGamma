#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H	
#include "..\..\..\stdafx.h"
#include "HeigthMap.h"
#include "PathMap.h"
#include "..\Graphics\Vertex.h"
#include "../Level/LevelParser.h"

class Terrain
{
	ID3D11Buffer			*m_VertexBuffer;
	ID3D11Buffer			*m_IndexBuffer;
	ID3D11DeviceContext		*m_DeviceContext;
	ID3D11Device			*m_Device;

	HeigthMap				*m_HeigthMap;
	PathMap					*m_PathMap;

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

	bool IsWalkable(float x, float z)
	{
		if (m_PathMap)
			return m_PathMap->IsWalkable(x / m_Width, z / m_Height);
		return false;
	}

	void SetWalkable(bool walkable, float x, float z) { m_PathMap->SetWalkable(walkable, x / m_Width, z / m_Height); }

	bool FindPath(XMFLOAT3 A, XMFLOAT3 B, vector<XMFLOAT3>& path)
	{
		path.clear();

		vector<XMFLOAT2> temp;
		if (m_PathMap->FindPath(XMFLOAT2(A.x / m_Width, A.z / m_Height), XMFLOAT2(B.x / m_Width, B.z / m_Height), temp))
		{
			for (int i = 0; i < temp.size(); ++i)
			{
				XMFLOAT2 stepTemp = temp[i];
				stepTemp.x *= m_Width;
				stepTemp.y *= m_Height;

				XMFLOAT3 step = XMFLOAT3(stepTemp.x, 0, stepTemp.y);

				path.push_back(step);
			}
			return true;
		}

		return false;
	}
	
	void BlockPath(BoundingOrientedBox box)
	{
		XMMATRIX scale;
		scale = XMMatrixScaling(1.0f / m_Width, 1.0f, 1.0f / m_Height);

		//skala ner boxen
		box.Transform(box, scale);

		//Flyttar boxens centrum
		box.Center.y = 0;
		//box.Center.x /= m_Width;
		//box.Center.z /= m_Height;

		m_PathMap->BlockPath(box);
	}

	bool IsShortestPathFree(XMFLOAT3 A, XMFLOAT3 B)
	{
		return m_PathMap->IsShortestPathFree(XMFLOAT2(A.x / m_Width, A.z / m_Height), XMFLOAT2(B.x / m_Width, B.z / m_Height));
	}

	ID3D11ShaderResourceView* GetBlendMap() { return m_BlendMap; }
	ID3D11ShaderResourceView* GetGroundTexture(int index) { return m_GroundTextures[index]; }
	ID3D11ShaderResourceView* GetGroundNormal(int index) { return m_NormalTextures[index]; }

	XMMATRIX GetTexTransform() { return XMLoadFloat4x4(&m_TexTransform); }

	XMFLOAT2 GetDimensions() { return XMFLOAT2(m_Width, m_Height); }
};
#endif
