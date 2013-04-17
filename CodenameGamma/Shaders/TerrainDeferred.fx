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
Texture2D gGround1Map;
Texture2D gGround2Map;
Texture2D gGround3Map;
Texture2D gGround4Map;

Texture2D gNormal1Map;
Texture2D gNormal2Map;
Texture2D gNormal3Map;
Texture2D gNormal4Map;

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
	float4 BlendData: BLENDDATA;
};

struct VertexOut
{
	float4 PosH       : SV_POSITION;
    float3 NormalV    : NORMAL;
	float4 TangentV   : TANGENT;
	float2 Tex        : TEXCOORD0;
	float4 BlendData  : BLENDDATA;
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

	vout.BlendData = vin.BlendData;
	
	return vout;
}

PsOut PS(VertexOut pin, 
		  uniform bool gUseTexure, 
		  uniform bool gUseNormalMap) : SV_Target
{

	PsOut pout;
	/*
	pout.Albedo  	= float4(1, 0, 0, 1);
	pout.NormalSpec = float4(1, 1, 1, 1);
	return pout;
	*/
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalV = normalize(pin.NormalV);

    // Default to multiplicative identity.
    pout.Albedo = float4(1, 1, 1, 1);
    if(gUseTexure)
	{
		// Sample textures.
		
		float4 color1, color2, color3, color4;
		
		color1 = pin.BlendData.r * gGround1Map.Sample( samLinear, pin.Tex );
		color2 = pin.BlendData.g * gGround2Map.Sample( samLinear, pin.Tex );
		color3 = pin.BlendData.b * gGround3Map.Sample( samLinear, pin.Tex );
		color4 = pin.BlendData.a * gGround4Map.Sample( samLinear, pin.Tex );
		
		pout.Albedo = color1 + color2 + color3 + color4;		

	}

	//
	// Normal mapping
	//
	float3 NormalV = pin.NormalV;
	if (gUseNormalMap)
	{
		float3 normal1, normal2, normal3, normal4;
	
		normal1 = pin.BlendData.r * gNormal1Map.Sample( samLinear, pin.Tex ).rgb;
		normal2 = pin.BlendData.g * gNormal2Map.Sample( samLinear, pin.Tex ).rgb;
		normal3 = pin.BlendData.b * gNormal3Map.Sample( samLinear, pin.Tex ).rgb;
		normal4 = pin.BlendData.a * gNormal4Map.Sample( samLinear, pin.Tex ).rgb;
	
		
		float3 normalMapSample = normal1 + normal2 + normal3 + normal4;
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
        SetPixelShader( CompileShader( ps_5_0, PS(false, false) ) );
    }
}

technique11 TexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, false) ) );
    }
}

technique11 TexNormalTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, true) ) );
    }
}

technique11 NormalTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false, true) ) );
    }
}
