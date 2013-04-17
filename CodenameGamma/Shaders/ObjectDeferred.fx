#include "LightHelper.fx"

cbuffer cbPerObject
{
	float4x4 gWorldView;
	float4x4 gWorldViewInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gNormalMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH       : SV_POSITION;
    float3 NormalV    : NORMAL;
	float4 TangentV   : TANGENT;
	float2 Tex        : TEXCOORD0;
};

struct PsOut
{
	float4 Albedo		: COLOR0;
	float4 NormalSpec	: COLOR1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.NormalV  = mul(vin.NormalL, (float3x3)gWorldViewInvTranspose);
	vout.TangentV = mul(vin.TangentL, gWorldView);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

PsOut PS(VertexOut pin, 
		  uniform bool gUseTexure, 
		  uniform bool gUseNormalMap,
		  uniform bool gAlphaClip) : SV_Target
{
	PsOut pout;
	
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalV = normalize(pin.NormalV);

    // Default to multiplicative identity.
    pout.Albedo = float4(1, 1, 1, 1);
    if(gUseTexure)
	{
		// Sample texture.
		pout.Albedo = gDiffuseMap.Sample( samLinear, pin.Tex );
		//return texColor;
		if(gAlphaClip)
		{
			clip(pout.Albedo.a - 0.1f);
		}
	}

	//
	// Normal mapping
	//
	float3 NormalV = pin.NormalV;
	if (gUseNormalMap)
	{
		float3 normalMapSample = gNormalMap.Sample(samLinear, pin.Tex).rgb;
		NormalV = NormalSampleToWorldSpace(normalMapSample, pin.NormalV, pin.TangentV);
	}
	pout.NormalSpec.x 	= (NormalV.x + 1.0f) * 0.5f;
	pout.NormalSpec.y 	= (NormalV.y + 1.0f) * 0.5f;
	pout.NormalSpec.z  	= gMaterial.Specular.x;
	pout.NormalSpec.w  	= gMaterial.Specular.w;
	
	//pout.NormalSpec = float4(1,0,0,1);
	
	return pout;
}

technique11 BasicTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false, false, false) ) );
    }
}

technique11 TexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, false, false) ) );
    }
}

technique11 TexNormalTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, true, false) ) );
    }
}

technique11 TexAlphaClipTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, false, true) ) );
    }
}

technique11 TexNormalAlphaClipTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, true, true) ) );
    }
}

technique11 NormalTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false, true, false) ) );
    }
}
