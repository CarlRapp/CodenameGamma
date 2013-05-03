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
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	float SpecIntensity;
	float SpecPower;
	XMFLOAT2 pad;
};


#endif