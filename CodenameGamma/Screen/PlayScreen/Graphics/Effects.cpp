#include "Effects.h"


#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename.c_str(), std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HRESULT(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	Light1ReflectTech    = mFX->GetTechniqueByName("Light1Reflect");
	Light2ReflectTech    = mFX->GetTechniqueByName("Light2Reflect");
	Light3ReflectTech    = mFX->GetTechniqueByName("Light3Reflect");

	Light0TexReflectTech = mFX->GetTechniqueByName("Light0TexReflect");
	Light1TexReflectTech = mFX->GetTechniqueByName("Light1TexReflect");
	Light2TexReflectTech = mFX->GetTechniqueByName("Light2TexReflect");
	Light3TexReflectTech = mFX->GetTechniqueByName("Light3TexReflect");

	Light0TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflect");
	Light1TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflect");
	Light2TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflect");
	Light3TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflect");

	Light1FogReflectTech    = mFX->GetTechniqueByName("Light1FogReflect");
	Light2FogReflectTech    = mFX->GetTechniqueByName("Light2FogReflect");
	Light3FogReflectTech    = mFX->GetTechniqueByName("Light3FogReflect");

	Light0TexFogReflectTech = mFX->GetTechniqueByName("Light0TexFogReflect");
	Light1TexFogReflectTech = mFX->GetTechniqueByName("Light1TexFogReflect");
	Light2TexFogReflectTech = mFX->GetTechniqueByName("Light2TexFogReflect");
	Light3TexFogReflectTech = mFX->GetTechniqueByName("Light3TexFogReflect");

	Light0TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	Light1TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	Light2TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	Light3TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex  = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	ShadowTransform   = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap           = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	ShadowMap         = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
	SsaoMap           = mFX->GetVariableByName("gSsaoMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

#pragma region BoxDebugEffect
BoxDebugEffect::BoxDebugEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BasicTech			= mFX->GetTechniqueByName("BasicTech");
	ViewProj			= mFX->GetVariableByName("gViewProj")->AsMatrix();
}

BoxDebugEffect::~BoxDebugEffect()
{
}
#pragma endregion

#pragma region ObjectDeferredEffect
ObjectDeferredEffect::ObjectDeferredEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BasicTech				= mFX->GetTechniqueByName("BasicTech");
	TexTech					= mFX->GetTechniqueByName("TexTech");
	TexNormalTech			= mFX->GetTechniqueByName("TexNormalTech");
	TexAlphaClipTech		= mFX->GetTechniqueByName("TexAlphaClipTech");
	TexNormalAlphaClipTech	= mFX->GetTechniqueByName("TexNormalAlphaClipTech");
	NormalTech				= mFX->GetTechniqueByName("NormalTech");

	BasicSkinnedTech				= mFX->GetTechniqueByName("BasicSkinnedTech");
	TexSkinnedTech					= mFX->GetTechniqueByName("TexSkinnedTech");
	TexNormalSkinnedTech			= mFX->GetTechniqueByName("TexNormalSkinnedTech");
	TexAlphaClipSkinnedTech			= mFX->GetTechniqueByName("TexAlphaClipSkinnedTech");
	TexNormalAlphaClipSkinnedTech	= mFX->GetTechniqueByName("TexNormalAlphaClipSkinnedTech");
	NormalSkinnedTech				= mFX->GetTechniqueByName("NormalSkinnedTech");

	WorldViewProj			= mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World					= mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose		= mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform			= mFX->GetVariableByName("gTexTransform")->AsMatrix();
	BoneTransforms			= mFX->GetVariableByName("gBoneTransforms")->AsMatrix();

	Mat						= mFX->GetVariableByName("gMaterial");

	DiffuseMap				= mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap				= mFX->GetVariableByName("gNormalMap")->AsShaderResource();
}

ObjectDeferredEffect::~ObjectDeferredEffect()
{
}
#pragma endregion

#pragma region TerrainDeferredEffect
TerrainDeferredEffect::TerrainDeferredEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BasicTech				= mFX->GetTechniqueByName("BasicTech");
	TexTech					= mFX->GetTechniqueByName("TexTech");
	TexNormalTech			= mFX->GetTechniqueByName("TexNormalTech");
	NormalTech				= mFX->GetTechniqueByName("NormalTech");

	WorldViewProj			= mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World					= mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose		= mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform			= mFX->GetVariableByName("gTexTransform")->AsMatrix();

	Mat						= mFX->GetVariableByName("gMaterial");

	BlendMap				= mFX->GetVariableByName("gBlendMap")->AsShaderResource();

	Ground1Map				= mFX->GetVariableByName("gGround1Map")->AsShaderResource();
	Ground2Map				= mFX->GetVariableByName("gGround2Map")->AsShaderResource();
	Ground3Map				= mFX->GetVariableByName("gGround3Map")->AsShaderResource();
	Ground4Map				= mFX->GetVariableByName("gGround4Map")->AsShaderResource();

	Normal1Map				= mFX->GetVariableByName("gNormal1Map")->AsShaderResource();
	Normal2Map				= mFX->GetVariableByName("gNormal2Map")->AsShaderResource();
	Normal3Map				= mFX->GetVariableByName("gNormal3Map")->AsShaderResource();
	Normal4Map				= mFX->GetVariableByName("gNormal4Map")->AsShaderResource();
}

TerrainDeferredEffect::~TerrainDeferredEffect()
{
}
#pragma endregion

#pragma region TiledLightningEffect
TiledLightningEffect::TiledLightningEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Viewport1		= mFX->GetTechniqueByName("Viewport1");
	Viewport2		= mFX->GetTechniqueByName("Viewport2");
	Viewport3		= mFX->GetTechniqueByName("Viewport3");
	Viewport4		= mFX->GetTechniqueByName("Viewport4");

	ViewProjTexs		= mFX->GetVariableByName("gLightViewProjTex")->AsMatrix();
	ViewProj			= mFX->GetVariableByName("gLightViewProj")->AsMatrix();
	Texs				= mFX->GetVariableByName("gLightTex")->AsMatrix();
	InvViewProjs		= mFX->GetVariableByName("gInvViewProjs")->AsMatrix();
	CamPositions		= mFX->GetVariableByName("gCamPositions")->AsVector();
	Resolution			= mFX->GetVariableByName("gResolution")->AsVector();
	ShadowMapSwitches	= mFX->GetVariableByName("gShadowMapSwitches")->AsVector();
	ShadowMapResolution	= mFX->GetVariableByName("gShadowMapResolution")->AsVector();
	GlobalLight			= mFX->GetVariableByName("gGlobalLight")->AsVector();

	AlbedoMap		= mFX->GetVariableByName("gAlbedoMap")->AsShaderResource();
	NormalSpecMap   = mFX->GetVariableByName("gNormalSpecMap")->AsShaderResource();
	DepthMap		= mFX->GetVariableByName("gDepthMap")->AsShaderResource();
	ShadowMap0		= mFX->GetVariableByName("gShadowMap0")->AsShaderResource();
	ShadowMap1		= mFX->GetVariableByName("gShadowMap1")->AsShaderResource();
	ShadowMap2		= mFX->GetVariableByName("gShadowMap2")->AsShaderResource();
	ShadowMap3		= mFX->GetVariableByName("gShadowMap3")->AsShaderResource();
	OutputMap		= mFX->GetVariableByName("gOutput")->AsUnorderedAccessView();

	DirLightMap     = mFX->GetVariableByName("gDirLightBuffer")->AsShaderResource();
	PointLightMap   = mFX->GetVariableByName("gPointLightBuffer")->AsShaderResource();
	SpotLightMap    = mFX->GetVariableByName("gSpotLightBuffer")->AsShaderResource();
}

TiledLightningEffect::~TiledLightningEffect()
{
}
#pragma endregion

#pragma region NormalMapEffect
NormalMapEffect::NormalMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	Light1ReflectTech    = mFX->GetTechniqueByName("Light1Reflect");
	Light2ReflectTech    = mFX->GetTechniqueByName("Light2Reflect");
	Light3ReflectTech    = mFX->GetTechniqueByName("Light3Reflect");

	Light0TexReflectTech = mFX->GetTechniqueByName("Light0TexReflect");
	Light1TexReflectTech = mFX->GetTechniqueByName("Light1TexReflect");
	Light2TexReflectTech = mFX->GetTechniqueByName("Light2TexReflect");
	Light3TexReflectTech = mFX->GetTechniqueByName("Light3TexReflect");

	Light0TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflect");
	Light1TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflect");
	Light2TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflect");
	Light3TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflect");

	Light1FogReflectTech    = mFX->GetTechniqueByName("Light1FogReflect");
	Light2FogReflectTech    = mFX->GetTechniqueByName("Light2FogReflect");
	Light3FogReflectTech    = mFX->GetTechniqueByName("Light3FogReflect");

	Light0TexFogReflectTech = mFX->GetTechniqueByName("Light0TexFogReflect");
	Light1TexFogReflectTech = mFX->GetTechniqueByName("Light1TexFogReflect");
	Light2TexFogReflectTech = mFX->GetTechniqueByName("Light2TexFogReflect");
	Light3TexFogReflectTech = mFX->GetTechniqueByName("Light3TexFogReflect");

	Light0TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	Light1TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	Light2TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	Light3TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	Light1SkinnedTech    = mFX->GetTechniqueByName("Light1Skinned");
	Light2SkinnedTech    = mFX->GetTechniqueByName("Light2Skinned");
	Light3SkinnedTech    = mFX->GetTechniqueByName("Light3Skinned");

	Light0TexSkinnedTech = mFX->GetTechniqueByName("Light0TexSkinned");
	Light1TexSkinnedTech = mFX->GetTechniqueByName("Light1TexSkinned");
	Light2TexSkinnedTech = mFX->GetTechniqueByName("Light2TexSkinned");
	Light3TexSkinnedTech = mFX->GetTechniqueByName("Light3TexSkinned");

	Light0TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("Light0TexAlphaClipSkinned");
	Light1TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("Light1TexAlphaClipSkinned");
	Light2TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("Light2TexAlphaClipSkinned");
	Light3TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("Light3TexAlphaClipSkinned");

	Light1FogSkinnedTech    = mFX->GetTechniqueByName("Light1FogSkinned");
	Light2FogSkinnedTech    = mFX->GetTechniqueByName("Light2FogSkinned");
	Light3FogSkinnedTech    = mFX->GetTechniqueByName("Light3FogSkinned");

	Light0TexFogSkinnedTech = mFX->GetTechniqueByName("Light0TexFogSkinned");
	Light1TexFogSkinnedTech = mFX->GetTechniqueByName("Light1TexFogSkinned");
	Light2TexFogSkinnedTech = mFX->GetTechniqueByName("Light2TexFogSkinned");
	Light3TexFogSkinnedTech = mFX->GetTechniqueByName("Light3TexFogSkinned");

	Light0TexAlphaClipFogSkinnedTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogSkinned");
	Light1TexAlphaClipFogSkinnedTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogSkinned");
	Light2TexAlphaClipFogSkinnedTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogSkinned");
	Light3TexAlphaClipFogSkinnedTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogSkinned");

	Light1ReflectSkinnedTech    = mFX->GetTechniqueByName("Light1ReflectSkinned");
	Light2ReflectSkinnedTech    = mFX->GetTechniqueByName("Light2ReflectSkinned");
	Light3ReflectSkinnedTech    = mFX->GetTechniqueByName("Light3ReflectSkinned");

	Light0TexReflectSkinnedTech = mFX->GetTechniqueByName("Light0TexReflectSkinned");
	Light1TexReflectSkinnedTech = mFX->GetTechniqueByName("Light1TexReflectSkinned");
	Light2TexReflectSkinnedTech = mFX->GetTechniqueByName("Light2TexReflectSkinned");
	Light3TexReflectSkinnedTech = mFX->GetTechniqueByName("Light3TexReflectSkinned");

	Light0TexAlphaClipReflectSkinnedTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflectSkinned");
	Light1TexAlphaClipReflectSkinnedTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflectSkinned");
	Light2TexAlphaClipReflectSkinnedTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflectSkinned");
	Light3TexAlphaClipReflectSkinnedTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflectSkinned");

	Light1FogReflectSkinnedTech    = mFX->GetTechniqueByName("Light1FogReflectSkinned");
	Light2FogReflectSkinnedTech    = mFX->GetTechniqueByName("Light2FogReflectSkinned");
	Light3FogReflectSkinnedTech    = mFX->GetTechniqueByName("Light3FogReflectSkinned");

	Light0TexFogReflectSkinnedTech = mFX->GetTechniqueByName("Light0TexFogReflectSkinned");
	Light1TexFogReflectSkinnedTech = mFX->GetTechniqueByName("Light1TexFogReflectSkinned");
	Light2TexFogReflectSkinnedTech = mFX->GetTechniqueByName("Light2TexFogReflectSkinned");
	Light3TexFogReflectSkinnedTech = mFX->GetTechniqueByName("Light3TexFogReflectSkinned");

	Light0TexAlphaClipFogReflectSkinnedTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflectSkinned");
	Light1TexAlphaClipFogReflectSkinnedTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflectSkinned");
	Light2TexAlphaClipFogReflectSkinnedTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflectSkinned");
	Light3TexAlphaClipFogReflectSkinnedTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflectSkinned");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex  = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	BoneTransforms    = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
	ShadowTransform   = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap           = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	NormalMap         = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
	ShadowMap         = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
	SsaoMap           = mFX->GetVariableByName("gSsaoMap")->AsShaderResource();
	DirLightMap       = mFX->GetVariableByName("gDirLightBuffer")->AsShaderResource();
}

NormalMapEffect::~NormalMapEffect()
{
}
#pragma endregion

#pragma region BuildShadowMapEffect
BuildShadowMapEffect::BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BuildShadowMapTech           = mFX->GetTechniqueByName("BuildShadowMapTech");
	BuildShadowMapAlphaClipTech  = mFX->GetTechniqueByName("BuildShadowMapAlphaClipTech");

	BuildShadowMapSkinnedTech          = mFX->GetTechniqueByName("BuildShadowMapSkinnedTech");
	BuildShadowMapAlphaClipSkinnedTech = mFX->GetTechniqueByName("BuildShadowMapAlphaClipSkinnedTech");

	TessBuildShadowMapTech           = mFX->GetTechniqueByName("TessBuildShadowMapTech");
	TessBuildShadowMapAlphaClipTech  = mFX->GetTechniqueByName("TessBuildShadowMapAlphaClipTech");
	
	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	BoneTransforms    = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	HeightScale       = mFX->GetVariableByName("gHeightScale")->AsScalar();
	MaxTessDistance   = mFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	MinTessDistance   = mFX->GetVariableByName("gMinTessDistance")->AsScalar();
	MinTessFactor     = mFX->GetVariableByName("gMinTessFactor")->AsScalar();
	MaxTessFactor     = mFX->GetVariableByName("gMaxTessFactor")->AsScalar();
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap         = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
}

BuildShadowMapEffect::~BuildShadowMapEffect()
{
}
#pragma endregion

#pragma region SsaoNormalDepthEffect
SsaoNormalDepthEffect::SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	NormalDepthTech          = mFX->GetTechniqueByName("NormalDepth");
	NormalDepthAlphaClipTech = mFX->GetTechniqueByName("NormalDepthAlphaClip");

	NormalDepthSkinnedTech          = mFX->GetTechniqueByName("NormalDepthSkinned");
	NormalDepthAlphaClipSkinnedTech = mFX->GetTechniqueByName("NormalDepthAlphaClipSkinned");

	WorldView             = mFX->GetVariableByName("gWorldView")->AsMatrix();
	WorldInvTransposeView = mFX->GetVariableByName("gWorldInvTransposeView")->AsMatrix();
	BoneTransforms        = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
	WorldViewProj         = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	TexTransform          = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap            = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

SsaoNormalDepthEffect::~SsaoNormalDepthEffect()
{
}
#pragma endregion

#pragma region SsaoEffect
SsaoEffect::SsaoEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SsaoTech           = mFX->GetTechniqueByName("Ssao");

	ViewToTexSpace     = mFX->GetVariableByName("gViewToTexSpace")->AsMatrix();
	OffsetVectors      = mFX->GetVariableByName("gOffsetVectors")->AsVector();
	FrustumCorners     = mFX->GetVariableByName("gFrustumCorners")->AsVector();
	OcclusionRadius    = mFX->GetVariableByName("gOcclusionRadius")->AsScalar();
	OcclusionFadeStart = mFX->GetVariableByName("gOcclusionFadeStart")->AsScalar();
	OcclusionFadeEnd   = mFX->GetVariableByName("gOcclusionFadeEnd")->AsScalar();
	SurfaceEpsilon     = mFX->GetVariableByName("gSurfaceEpsilon")->AsScalar();

	NormalDepthMap     = mFX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	RandomVecMap       = mFX->GetVariableByName("gRandomVecMap")->AsShaderResource();
}

SsaoEffect::~SsaoEffect()
{
}
#pragma endregion

#pragma region SsaoBlurEffect
SsaoBlurEffect::SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	HorzBlurTech    = mFX->GetTechniqueByName("HorzBlur");
	VertBlurTech    = mFX->GetTechniqueByName("VertBlur");

	TexelWidth      = mFX->GetVariableByName("gTexelWidth")->AsScalar();
	TexelHeight     = mFX->GetVariableByName("gTexelHeight")->AsScalar();

	NormalDepthMap  = mFX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	InputImage      = mFX->GetVariableByName("gInputImage")->AsShaderResource();
}

SsaoBlurEffect::~SsaoBlurEffect()
{
}
#pragma endregion

#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SkyTech       = mFX->GetTechniqueByName("SkyTech");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap       = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect()
{
}
#pragma endregion

#pragma region DebugTexEffect
DebugTexEffect::DebugTexEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	ViewArgbTech  = mFX->GetTechniqueByName("ViewArgbTech");
	ViewRedTech   = mFX->GetTechniqueByName("ViewRedTech");
	ViewGreenTech = mFX->GetTechniqueByName("ViewGreenTech");
	ViewBlueTech  = mFX->GetTechniqueByName("ViewBlueTech");
	ViewAlphaTech = mFX->GetTechniqueByName("ViewAlphaTech");

	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	Texture       = mFX->GetVariableByName("gTexture")->AsShaderResource();
}

DebugTexEffect::~DebugTexEffect()
{

}

#pragma endregion

#pragma region ClearGBufferEffect
ClearGBufferEffect::ClearGBufferEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	ClearTech  = mFX->GetTechniqueByName("ClearTech");
}

ClearGBufferEffect::~ClearGBufferEffect()
{
}
#pragma endregion

#pragma region ShadowMapEffect
ShadowMapEffect::ShadowMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BasicShadowDirTech		= mFX->GetTechniqueByName("BasicShadowDir");
	BasicShadowPointTech	= mFX->GetTechniqueByName("BasicShadowPoint");
	BasicShadowSpotTech		= mFX->GetTechniqueByName("BasicShadowSpot");
	AlphaClipShadowDirTech	= mFX->GetTechniqueByName("AlphaClipShadowDir");
	AlphaClipShadowPointTech= mFX->GetTechniqueByName("AlphaClipShadowPoint");
	AlphaClipShadowSpotTech	= mFX->GetTechniqueByName("AlphaClipShadowSpot");

	SkinnedBasicShadowDirTech		= mFX->GetTechniqueByName("SkinnedBasicShadowDir");
	SkinnedBasicShadowPointTech		= mFX->GetTechniqueByName("SkinnedBasicShadowPoint");
	SkinnedBasicShadowSpotTech		= mFX->GetTechniqueByName("SkinnedBasicShadowSpot");
	SkinnedAlphaClipShadowDirTech	= mFX->GetTechniqueByName("SkinnedAlphaClipShadowDir");
	SkinnedAlphaClipShadowPointTech	= mFX->GetTechniqueByName("SkinnedAlphaClipShadowPoint");
	SkinnedAlphaClipShadowSpotTech	= mFX->GetTechniqueByName("SkinnedAlphaClipShadowSpot");

	DiffuseMap  = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();

	TexTransform	= mFX->GetVariableByName("gTexTransform")->AsMatrix();
	WorldViewProj	= mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	BoneTransforms	= mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
}

ShadowMapEffect::~ShadowMapEffect()
{
}
#pragma endregion

#pragma region CombineFinalEffect
CombineFinalEffect::CombineFinalEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	MonoTech  = mFX->GetTechniqueByName("Mono");
	ColorTech  = mFX->GetTechniqueByName("Color");
	TransparencyColorTech  = mFX->GetTechniqueByName("TransparencyColor");
	AlphaTransparencyColorTech  = mFX->GetTechniqueByName("AlphaTransparencyColor");
	AlphaClipColorTech  = mFX->GetTechniqueByName("AlphaClipColor");

	Opacity = mFX->GetVariableByName("g_Opacity")->AsScalar();

	Texture = mFX->GetVariableByName("g_Texture")->AsShaderResource();
}

CombineFinalEffect::~CombineFinalEffect()
{
}
#pragma endregion

#pragma region Effects

BasicEffect*           Effects::BasicFX           = 0;
NormalMapEffect*       Effects::NormalMapFX       = 0;
BuildShadowMapEffect*  Effects::BuildShadowMapFX  = 0;
SsaoNormalDepthEffect* Effects::SsaoNormalDepthFX = 0;
SsaoEffect*            Effects::SsaoFX            = 0;
SsaoBlurEffect*        Effects::SsaoBlurFX        = 0;
SkyEffect*             Effects::SkyFX             = 0;
DebugTexEffect*        Effects::DebugTexFX        = 0;
ClearGBufferEffect*    Effects::ClearGBufferFX    = 0;
CombineFinalEffect*	   Effects::CombineFinalFX    = 0;
ObjectDeferredEffect*  Effects::ObjectDeferredFX  = 0;
TerrainDeferredEffect* Effects::TerrainDeferredFX = 0;
TiledLightningEffect*  Effects::TiledLightningFX  = 0;
ShadowMapEffect*	   Effects::ShadowMapFX		  = 0;
BoxDebugEffect*		   Effects::BoxDebugFX		  = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX           = new BasicEffect(device, L"Shaders/Basic.fxo");	
	NormalMapFX       = new NormalMapEffect(device, L"Shaders/NormalMap.fxo");
	BuildShadowMapFX  = new BuildShadowMapEffect(device, L"Shaders/BuildShadowMap.fxo");
	SsaoNormalDepthFX = new SsaoNormalDepthEffect(device, L"Shaders/SsaoNormalDepth.fxo");
	SsaoFX            = new SsaoEffect(device, L"Shaders/Ssao.fxo");
	SsaoBlurFX        = new SsaoBlurEffect(device, L"Shaders/SsaoBlur.fxo");
	SkyFX             = new SkyEffect(device, L"Shaders/Sky.fxo");
	DebugTexFX        = new DebugTexEffect(device, L"Shaders/DebugTexture.fxo");
	ClearGBufferFX    = new ClearGBufferEffect(device, L"Shaders/ClearGBuffer.fxo");
	CombineFinalFX    = new CombineFinalEffect(device, L"Shaders/CombineFinal.fxo");
	ObjectDeferredFX  = new ObjectDeferredEffect(device, L"Shaders/ObjectDeferred.fxo");
	TerrainDeferredFX = new TerrainDeferredEffect(device, L"Shaders/TerrainDeferred.fxo");
	TiledLightningFX  = new TiledLightningEffect(device, L"Shaders/TiledLightning.fxo");
	ShadowMapFX		  = new ShadowMapEffect(device, L"Shaders/ShadowMap.fxo");
	BoxDebugFX		  = new BoxDebugEffect(device, L"Shaders/BoxDebug.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);	
	SafeDelete(NormalMapFX);
	SafeDelete(BuildShadowMapFX);
	SafeDelete(SsaoNormalDepthFX);
	SafeDelete(SsaoFX);
	SafeDelete(SsaoBlurFX);
	SafeDelete(SkyFX);
	SafeDelete(DebugTexFX);
	SafeDelete(ClearGBufferFX);
	SafeDelete(CombineFinalFX);
	SafeDelete(ObjectDeferredFX);
	SafeDelete(TerrainDeferredFX);
	SafeDelete(TiledLightningFX);
	SafeDelete(ShadowMapFX);
	SafeDelete(BoxDebugFX);
}

#pragma endregion