#pragma once
#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include "..\..\..\stdafx.h"

#define DIRECTION_MAXRANGE 10000.0f
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


struct GPULight
{
	GPULight(void) { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Color;
};

struct GPUDirectionalLight : GPULight
{
	GPUDirectionalLight(void) { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Direction;
	//shadow info	
	UINT	 ShadowIndex[4];
	XMFLOAT2 Resolution;
	bool	 HasShadow;
};

struct GPUPointLight : GPULight
{
	GPUPointLight(void) { ZeroMemory(this, sizeof(this)); }
	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;
	//shadow info	
	UINT	 ShadowIndex[6];
	XMFLOAT2 Resolution;
	bool	 HasShadow;
};

struct GPUSpotLight : GPULight
{
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


class Light
{
protected:
public:
	Light(void) { }
	virtual ~Light(){ }
	//GPULight* GetGPULight() { return gpuLight; }
};

class DirectionalLight : public Light
{
	GPUDirectionalLight* gpuLight;
public:
	DirectionalLight(void) { gpuLight = new GPUDirectionalLight(); }

	XMMATRIX GetViewMatrix(BoundingFrustum& frustum, float offset);
	XMMATRIX GetProjectionMatrix(/*float nearZ, float farZ*/);

	void GetViewProjOBB(BoundingFrustum& frustum, float offset, XMFLOAT4X4& View, XMFLOAT4X4& Proj, BoundingOrientedBox& OBB);

	GPUDirectionalLight* GetGPULight() { return gpuLight; }
	/*
	BoundingFrustum GetBoundingOrientedBox(BoundingFrustum& frustum, float offset)
	{
		return MathHelper::GenerateBoundingFrustum(GetViewMatrix(frustum, offset), GetProjectionMatrix(0.0f, DIRECTION_MAXRANGE));
		//return BoundingFrustum(Position, Range);
	}
	*/
	
};

class PointLight : public Light
{
	GPUPointLight* gpuLight;
public:
	PointLight(void) { gpuLight = new GPUPointLight(); }

	std::vector<XMFLOAT4X4> GetViewMatrixes();
	XMMATRIX GetProjectionMatrix(/*float nearZ, float farZ*/);
	
	std::vector<BoundingFrustum> GetBoundingFrustums()
	{
		std::vector<BoundingFrustum> frustums;
		
		std::vector<XMFLOAT4X4> Views = GetViewMatrixes();;
		XMMATRIX Proj = GetProjectionMatrix(/*0.0f, Range*/);
		
		for (int i = 0; i < Views.size(); ++i)
		{
			BoundingFrustum frustum = MathHelper::GenerateBoundingFrustum(XMLoadFloat4x4(&Views[i]), Proj);
			frustums.push_back(frustum);
		}		
		return frustums;
	}


	BoundingSphere GetBoundingSphere()
	{
		return BoundingSphere(gpuLight->Position, gpuLight->Range);
	}

	GPUPointLight* GetGPULight() { return gpuLight; }
};

class SpotLight : public Light
{
	GPUSpotLight* gpuLight;
public:
	SpotLight(void) { gpuLight = new GPUSpotLight(); }

	XMMATRIX GetViewMatrix();
	XMMATRIX GetProjectionMatrix(/*float nearZ, float farZ*/);

	BoundingFrustum GetBoundingFrustum()
	{
		return MathHelper::GenerateBoundingFrustum(GetViewMatrix(), GetProjectionMatrix(/*0.0f, Range*/));
		//return BoundingFrustum(Position, Range);
	}

	virtual GPUSpotLight* GetGPULight() { return gpuLight; }
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	float SpecIntensity;
	float SpecPower;
	XMFLOAT2 pad;
};


#endif