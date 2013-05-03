#pragma once
#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include "..\..\..\stdafx.h"


// Note: Make sure structure alignment agrees with HLSL structure padding rules. 
//   Elements are packed into 4D vectors with the restriction that an element
//   cannot straddle a 4D vector boundary.

static const XMFLOAT2 SHADOWMAP_512  = XMFLOAT2(512, 512);
static const XMFLOAT2 SHADOWMAP_1024 = XMFLOAT2(1024, 1024);
static const XMFLOAT2 SHADOWMAP_2048 = XMFLOAT2(2048, 2048);
static const XMFLOAT2 SHADOWMAP_4096 = XMFLOAT2(4096, 4096);

static int ShadowTileSize(XMFLOAT2 resolution)
{
	if (resolution.x == SHADOWMAP_512.x && resolution.y == SHADOWMAP_512.y)
		return 1;

	else if (resolution.x == SHADOWMAP_1024.x && resolution.y == SHADOWMAP_1024.y)
		return 4;

	else if (resolution.x == SHADOWMAP_2048.x && resolution.y == SHADOWMAP_2048.y)
		return 16;

	else if (resolution.x == SHADOWMAP_4096.x && resolution.y == SHADOWMAP_4096.y)
		return 64;

	else return 0;
}

static XMFLOAT2 CalculateShadowCoord(int ShadowIndex)
{
	int numHuge   = ShadowIndex / 64;
	int numBig    = (ShadowIndex % 64) / 16;
	int numMedium = ((ShadowIndex % 64) % 16) / 4;
	int numSmall  = ((ShadowIndex % 64) % 16) % 4;



	int x = numHuge * 4096;

	if (numBig == 1 || numBig == 3)
		x += 2048;

	if (numMedium == 1 || numMedium == 3)
		x += 1024;

	if (numSmall == 1 || numSmall == 3)
		x += 512;

	int y = 0;
	
	if (numBig == 2 || numBig == 3)
		y += 2048;

	if (numMedium == 2 || numMedium == 3)
		y += 1024;

	if (numSmall == 2 || numSmall == 3)
		y += 512;

	return XMFLOAT2(x, y);
}

struct Light
{
	XMFLOAT4 Color;
};

struct DirectionalLight : public Light
{
	//DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Direction;

	//shadow info
	XMFLOAT2 Resolution;
	UINT	 ShadowIndex;
	bool	 HasShadow;
};

struct PointLight : public Light
{
	PointLight() { ZeroMemory(this, sizeof(this)); }


	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	//shadow info
	XMFLOAT2 Resolution;
	UINT	 ShadowIndex;
	bool	 HasShadow;

	//XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix(float nearZ, float farZ);

	BoundingSphere GetBoundingSphere()
	{
		return BoundingSphere(Position, Range);
	}
};

struct SpotLight : public Light
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }


	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	// Packed into 4D vector: (Direction, Spot)
	XMFLOAT3 Direction;
	float angle;

	//shadow info
	XMFLOAT2 Resolution;
	UINT	 ShadowIndex;
	bool	 HasShadow;

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix(float nearZ, float farZ);

	BoundingFrustum GetBoundingFrustum()
	{
		return MathHelper::GenerateBoundingFrustum(GetViewMatrix(), GetProjectionMatrix(0.0f, Range));
		//return BoundingFrustum(Position, Range);
	}
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	float SpecIntensity;
	float SpecPower;
	XMFLOAT2 pad;
};


#endif