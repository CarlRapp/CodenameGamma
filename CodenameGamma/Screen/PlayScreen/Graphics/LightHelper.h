#pragma once
#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include "..\..\..\stdafx.h"


// Note: Make sure structure alignment agrees with HLSL structure padding rules. 
//   Elements are packed into 4D vectors with the restriction that an element
//   cannot straddle a 4D vector boundary.

struct DirectionalLight
{
	//DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Color;
	XMFLOAT4 Direction;

	//shadow info
	XMFLOAT2 Resolution;
	UINT	 ShadowIndex;
	bool	 HasShadow;
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Color;

	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	//shadow info
	XMFLOAT2 Resolution;
	UINT	 ShadowIndex;
	bool	 HasShadow;
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Color;

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
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	float SpecIntensity;
	float SpecPower;
	XMFLOAT2 pad;
};


#endif