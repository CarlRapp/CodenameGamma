#pragma once
#ifndef EFFECTS_H
#define EFFECTS_H

#include "..\..\..\stdafx.h"
#include "LightHelper.h"
using namespace DirectX;

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M)                 { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)              { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                         { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)             { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetShadowTransform(CXMMATRIX M)               { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                  { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)               { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const XMFLOAT4 v)               { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex); }
	void SetSsaoMap(ID3D11ShaderResourceView* tex)      { SsaoMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectShaderResourceVariable* SsaoMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
};
#pragma endregion

#pragma region ObjectDeferredEffect
class ObjectDeferredEffect : public Effect
{
public:
	ObjectDeferredEffect(ID3D11Device* device, const std::wstring& filename);
	~ObjectDeferredEffect();

	void SetWorldViewProj(CXMMATRIX M)					{ WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)							{ World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)				{ WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)					{ TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }

	ID3DX11EffectTechnique* BasicTech;
	ID3DX11EffectTechnique* TexTech;
	ID3DX11EffectTechnique* TexNormalTech;
	ID3DX11EffectTechnique* TexAlphaClipTech;
	ID3DX11EffectTechnique* TexNormalAlphaClipTech;
	ID3DX11EffectTechnique* NormalTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};
#pragma endregion

#pragma region TerrainDeferredEffect
class TerrainDeferredEffect : public Effect
{
public:
	TerrainDeferredEffect(ID3D11Device* device, const std::wstring& filename);
	~TerrainDeferredEffect();

	void SetWorldViewProj(CXMMATRIX M)					{ WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)							{ World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)				{ WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)					{ TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetBlendMap(ID3D11ShaderResourceView* tex)		{ if (tex) BlendMap->SetResource(tex); }

	void SetGround1Map(ID3D11ShaderResourceView* tex)   { if (tex) Ground1Map->SetResource(tex); }
	void SetGround2Map(ID3D11ShaderResourceView* tex)   { if (tex) Ground2Map->SetResource(tex); }
	void SetGround3Map(ID3D11ShaderResourceView* tex)   { if (tex) Ground3Map->SetResource(tex); }
	void SetGround4Map(ID3D11ShaderResourceView* tex)   { if (tex) Ground4Map->SetResource(tex); }

	void SetNormal1Map(ID3D11ShaderResourceView* tex)    { if (tex) Normal1Map->SetResource(tex); }
	void SetNormal2Map(ID3D11ShaderResourceView* tex)    { if (tex) Normal2Map->SetResource(tex); }
	void SetNormal3Map(ID3D11ShaderResourceView* tex)    { if (tex) Normal3Map->SetResource(tex); }
	void SetNormal4Map(ID3D11ShaderResourceView* tex)    { if (tex) Normal4Map->SetResource(tex); }

	ID3DX11EffectTechnique* BasicTech;
	ID3DX11EffectTechnique* TexTech;
	ID3DX11EffectTechnique* TexNormalTech;
	ID3DX11EffectTechnique* NormalTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* BlendMap;

	ID3DX11EffectShaderResourceVariable* Ground1Map;
	ID3DX11EffectShaderResourceVariable* Ground2Map;
	ID3DX11EffectShaderResourceVariable* Ground3Map;
	ID3DX11EffectShaderResourceVariable* Ground4Map;

	ID3DX11EffectShaderResourceVariable* Normal1Map;
	ID3DX11EffectShaderResourceVariable* Normal2Map;
	ID3DX11EffectShaderResourceVariable* Normal3Map;
	ID3DX11EffectShaderResourceVariable* Normal4Map;
};
#pragma endregion

#pragma region TiledLightningEffect
class TiledLightningEffect : public Effect
{
public:
	TiledLightningEffect(ID3D11Device* device, const std::wstring& filename);
	~TiledLightningEffect();

	void SetViewProjTexs(const XMFLOAT4X4* M, int cnt)		{ ViewProjTexs->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetViewProj(const XMFLOAT4X4* M, int cnt)			{ ViewProj->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetTexs(const XMFLOAT4X4* M, int cnt)				{ Texs->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetInvViewProjs(const XMFLOAT4X4* M, int cnt)		{ InvViewProjs->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetCamPositions(const XMFLOAT4* V, int cnt)		{ CamPositions->SetFloatVectorArray(reinterpret_cast<const float*>(V), 0, cnt); }
	void SetResolution(const XMFLOAT2 v)					{ Resolution->SetRawValue(&v, 0, sizeof(XMFLOAT2)); }
	void SetShadowMapSwitches(const XMFLOAT3 v)				{ ShadowMapSwitches->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }	
	void SetShadowMapResolution(const XMFLOAT2 v)			{ ShadowMapResolution->SetRawValue(&v, 0, sizeof(XMFLOAT2)); }

	void SetAlbedoMap(ID3D11ShaderResourceView* tex)		{ AlbedoMap->SetResource(tex); }
	void SetNormalSpecMap(ID3D11ShaderResourceView* tex)    { NormalSpecMap->SetResource(tex); }
	void SetDepthMap(ID3D11ShaderResourceView* tex)			{ DepthMap->SetResource(tex); }
	void SetShadowMap0(ID3D11ShaderResourceView* tex)		{ ShadowMap0->SetResource(tex); }
	void SetShadowMap1(ID3D11ShaderResourceView* tex)		{ ShadowMap1->SetResource(tex); }
	void SetShadowMap2(ID3D11ShaderResourceView* tex)		{ ShadowMap2->SetResource(tex); }
	void SetShadowMap3(ID3D11ShaderResourceView* tex)		{ ShadowMap3->SetResource(tex); }
	void SetOutputMap(ID3D11UnorderedAccessView* tex)		{ OutputMap->SetUnorderedAccessView(tex); }
	void SetDirLightMap(ID3D11ShaderResourceView* tex)		{ DirLightMap->SetResource(tex); }
	void SetPointLightMap(ID3D11ShaderResourceView* tex)	{ PointLightMap->SetResource(tex); }
	void SetSpotLightMap(ID3D11ShaderResourceView* tex)		{ SpotLightMap->SetResource(tex); }

	ID3DX11EffectTechnique* Viewport1;
	ID3DX11EffectTechnique* Viewport2;
	ID3DX11EffectTechnique* Viewport3;
	ID3DX11EffectTechnique* Viewport4;

	ID3DX11EffectMatrixVariable* ViewProjTexs;
	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* Texs;
	ID3DX11EffectMatrixVariable* InvViewProjs;	
	ID3DX11EffectVectorVariable* CamPositions;
	ID3DX11EffectVectorVariable* Resolution;
	ID3DX11EffectVectorVariable* ShadowMapSwitches;
	ID3DX11EffectVectorVariable* ShadowMapResolution;

	ID3DX11EffectShaderResourceVariable* AlbedoMap;
	ID3DX11EffectShaderResourceVariable* NormalSpecMap;
	ID3DX11EffectShaderResourceVariable* DepthMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap0;
	ID3DX11EffectShaderResourceVariable* ShadowMap1;
	ID3DX11EffectShaderResourceVariable* ShadowMap2;
	ID3DX11EffectShaderResourceVariable* ShadowMap3;
	ID3DX11EffectUnorderedAccessViewVariable* OutputMap;

	ID3DX11EffectShaderResourceVariable* DirLightMap;
	ID3DX11EffectShaderResourceVariable* PointLightMap;
	ID3DX11EffectShaderResourceVariable* SpotLightMap;
};
#pragma endregion

#pragma region NormalMapEffect
class NormalMapEffect : public Effect
{
public:
	NormalMapEffect(ID3D11Device* device, const std::wstring& filename);
	~NormalMapEffect();

	void SetWorldViewProj(CXMMATRIX M)                   { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)							 { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)               { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt) { BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)					 { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3 v)					 { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const XMFLOAT4 v)                { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                            { FogStart->SetFloat(f); }
	void SetFogRange(float f)                            { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)    { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)                { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)    { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)       { CubeMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)     { NormalMap->SetResource(tex); }
	void SetSsaoMap(ID3D11ShaderResourceView* tex)       { SsaoMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)     { ShadowMap->SetResource(tex); }
	void SetDirLightMap(ID3D11ShaderResourceView* tex)   { DirLightMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectTechnique* Light1SkinnedTech;
	ID3DX11EffectTechnique* Light2SkinnedTech;
	ID3DX11EffectTechnique* Light3SkinnedTech;

	ID3DX11EffectTechnique* Light0TexSkinnedTech;
	ID3DX11EffectTechnique* Light1TexSkinnedTech;
	ID3DX11EffectTechnique* Light2TexSkinnedTech;
	ID3DX11EffectTechnique* Light3TexSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipSkinnedTech;

	ID3DX11EffectTechnique* Light1FogSkinnedTech;
	ID3DX11EffectTechnique* Light2FogSkinnedTech;
	ID3DX11EffectTechnique* Light3FogSkinnedTech;

	ID3DX11EffectTechnique* Light0TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light1TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light2TexFogSkinnedTech;
	ID3DX11EffectTechnique* Light3TexFogSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogSkinnedTech;

	ID3DX11EffectTechnique* Light1ReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2ReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3ReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectSkinnedTech;

	ID3DX11EffectTechnique* Light1FogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2FogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3FogReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexFogReflectSkinnedTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectSkinnedTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectSkinnedTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* BoneTransforms;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectShaderResourceVariable* SsaoMap;
	ID3DX11EffectShaderResourceVariable* DirLightMap;


};
#pragma endregion

#pragma region BuildShadowMapEffect
class BuildShadowMapEffect : public Effect
{
public:
	BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename);
	~BuildShadowMapEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt){ BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	
	void SetHeightScale(float f)                        { HeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { MaxTessFactor->SetFloat(f); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }

	ID3DX11EffectTechnique* BuildShadowMapTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique* BuildShadowMapSkinnedTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipSkinnedTech;
	ID3DX11EffectTechnique* TessBuildShadowMapTech;
	ID3DX11EffectTechnique* TessBuildShadowMapAlphaClipTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* BoneTransforms;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectScalarVariable* HeightScale;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;
 
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};
#pragma endregion

#pragma region SsaoNormalDepthEffect
class SsaoNormalDepthEffect : public Effect
{
public:
	SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoNormalDepthEffect();

	void SetWorldView(CXMMATRIX M)                      { WorldView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTransposeView(CXMMATRIX M)          { WorldInvTransposeView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt){ BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* NormalDepthTech;
	ID3DX11EffectTechnique* NormalDepthAlphaClipTech;

	ID3DX11EffectTechnique* NormalDepthSkinnedTech;
	ID3DX11EffectTechnique* NormalDepthAlphaClipSkinnedTech;

	ID3DX11EffectMatrixVariable* WorldView;
	ID3DX11EffectMatrixVariable* WorldInvTransposeView;
	ID3DX11EffectMatrixVariable* BoneTransforms;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};
#pragma endregion

#pragma region SsaoEffect
class SsaoEffect : public Effect
{
public:
	SsaoEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoEffect();

	void SetViewToTexSpace(CXMMATRIX M)                    { ViewToTexSpace->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetOffsetVectors(const XMFLOAT4 v[14])            { OffsetVectors->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 14); }
	void SetFrustumCorners(const XMFLOAT4 v[4])            { FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4); }
	void SetOcclusionRadius(float f)                       { OcclusionRadius->SetFloat(f); }
	void SetOcclusionFadeStart(float f)                    { OcclusionFadeStart->SetFloat(f); }
	void SetOcclusionFadeEnd(float f)                      { OcclusionFadeEnd->SetFloat(f); }
	void SetSurfaceEpsilon(float f)                        { SurfaceEpsilon->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetRandomVecMap(ID3D11ShaderResourceView* srv)    { RandomVecMap->SetResource(srv); }

	ID3DX11EffectTechnique* SsaoTech;

	ID3DX11EffectMatrixVariable* ViewToTexSpace;
	ID3DX11EffectVectorVariable* OffsetVectors;
	ID3DX11EffectVectorVariable* FrustumCorners;
	ID3DX11EffectScalarVariable* OcclusionRadius;
	ID3DX11EffectScalarVariable* OcclusionFadeStart;
	ID3DX11EffectScalarVariable* OcclusionFadeEnd;
	ID3DX11EffectScalarVariable* SurfaceEpsilon;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* RandomVecMap;
};
#pragma endregion

#pragma region SsaoBlurEffect
class SsaoBlurEffect : public Effect
{
public:
	SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoBlurEffect();

	void SetTexelWidth(float f)                            { TexelWidth->SetFloat(f); }
	void SetTexelHeight(float f)                           { TexelHeight->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetInputImage(ID3D11ShaderResourceView* srv)      { InputImage->SetResource(srv); }

	ID3DX11EffectTechnique* HorzBlurTech;
	ID3DX11EffectTechnique* VertBlurTech;

	ID3DX11EffectScalarVariable* TexelWidth;
	ID3DX11EffectScalarVariable* TexelHeight;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* InputImage;
};
#pragma endregion

#pragma region SkyEffect
class SkyEffect : public Effect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetCubeMap(ID3D11ShaderResourceView* cubemap)  { CubeMap->SetResource(cubemap); }

	ID3DX11EffectTechnique* SkyTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;

	ID3DX11EffectShaderResourceVariable* CubeMap;
};
#pragma endregion

#pragma region DebugTexEffect
class DebugTexEffect : public Effect
{
public:
	DebugTexEffect(ID3D11Device* device, const std::wstring& filename);
	~DebugTexEffect();

	void SetWorldViewProj(CXMMATRIX M)              { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexture(ID3D11ShaderResourceView* tex)  { Texture->SetResource(tex); }

	ID3DX11EffectTechnique* ViewArgbTech;
	ID3DX11EffectTechnique* ViewRedTech;
	ID3DX11EffectTechnique* ViewGreenTech;
	ID3DX11EffectTechnique* ViewBlueTech;
	ID3DX11EffectTechnique* ViewAlphaTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectShaderResourceVariable* Texture;
};
#pragma endregion

#pragma region ClearGBufferEffect
class ClearGBufferEffect : public Effect
{
public:
	ClearGBufferEffect(ID3D11Device* device, const std::wstring& filename);
	~ClearGBufferEffect();

	ID3DX11EffectTechnique* ClearTech;
};
#pragma endregion

#pragma region ShadowMapEffect
class ShadowMapEffect : public Effect
{
public:
	ShadowMapEffect(ID3D11Device* device, const std::wstring& filename);
	~ShadowMapEffect();

	void SetWorld(CXMMATRIX M)							{ World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetViewProj(CXMMATRIX M)						{ ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* BasicShadowDirTech;
	ID3DX11EffectTechnique* BasicShadowPointTech;
	ID3DX11EffectTechnique* BasicShadowSpotTech;

	ID3DX11EffectTechnique* AlphaClipShadowDirTech;
	ID3DX11EffectTechnique* AlphaClipShadowPointTech;
	ID3DX11EffectTechnique* AlphaClipShadowSpotTech;

	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* ViewProj;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};
#pragma endregion

#pragma region CombineFinalEffect
class CombineFinalEffect : public Effect
{
public:
	CombineFinalEffect(ID3D11Device* device, const std::wstring& filename);
	~CombineFinalEffect();

	void SetOpacity(float value)					{ Opacity->SetFloat(value); }
	void SetTexture(ID3D11ShaderResourceView* tex)  { Texture->SetResource(tex); }

	ID3DX11EffectTechnique* MonoTech;
	ID3DX11EffectTechnique* BlendMonoTech;
	ID3DX11EffectTechnique* ColorTech;
	ID3DX11EffectTechnique* BlendColorTech;
		
	ID3DX11EffectScalarVariable*		Opacity;

	ID3DX11EffectShaderResourceVariable* Texture;
};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;	
	static NormalMapEffect* NormalMapFX;
	static BuildShadowMapEffect* BuildShadowMapFX;
	static SsaoNormalDepthEffect* SsaoNormalDepthFX;
	static SsaoEffect* SsaoFX;
	static SsaoBlurEffect* SsaoBlurFX;
	static SkyEffect* SkyFX;
	static DebugTexEffect* DebugTexFX;
	static ClearGBufferEffect* ClearGBufferFX;
	static CombineFinalEffect* CombineFinalFX;
	static ObjectDeferredEffect* ObjectDeferredFX;
	static TerrainDeferredEffect* TerrainDeferredFX;
	static TiledLightningEffect* TiledLightningFX;
	static ShadowMapEffect*	ShadowMapFX;
};
#pragma endregion



#endif

