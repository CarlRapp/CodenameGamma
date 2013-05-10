#pragma once
#ifndef VERTEX_H
#define VERTEX_H

#include "..\..\..\stdafx.h"
#include "Effects.h"
using namespace DirectX;

namespace Vertex
{
	const int NUMBER_OF_BONES_PER_VERTEX = 4;

	struct Quad
	{
		XMFLOAT2 Pos;
		XMFLOAT2 Tex;
	};

	struct PosTex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
	};

	// Basic 32-byte vertex structure.
	struct Basic32
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	struct PosNormalTexTan
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 TangentU;
	};

	struct BoneInfo
	{
		float Weight;
		BYTE BoneIndex;

		BoneInfo()
		{
			Weight = 0.0f;
			BoneIndex = 0;
		}

	};

	struct PosNormalTexTanSkinned
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 TangentU;

		XMFLOAT4 Weight;
		UINT	 BoneIndex[4];

		PosNormalTexTanSkinned()
		{
			Weight = XMFLOAT4(0,0,0,0);
		}
	};

	struct Terrain
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 TangentU;
	};
}

class InputLayoutDesc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC Quad[2];
	static const D3D11_INPUT_ELEMENT_DESC PosTex[2];
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTan[4];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTanSkinned[6];
	static const D3D11_INPUT_ELEMENT_DESC Terrain[4];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Pos;
	static ID3D11InputLayout* Quad;
	static ID3D11InputLayout* PosTex;
	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* PosNormalTexTan;
	static ID3D11InputLayout* PosNormalTexTanSkinned;
	static ID3D11InputLayout* Terrain;
};

#endif