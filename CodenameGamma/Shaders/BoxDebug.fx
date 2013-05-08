cbuffer cbPerObject
{
	float4x4 gViewProj;
}; 

struct VertexIn
{
	float3 PosW			: POSITION;
};

struct VertexOut
{
	float4 PosH			: SV_POSITION;
};

struct PsOut
{
	float4 Albedo		: COLOR0;
	float4 NormalSpec	: COLOR1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosW, 1.0f), gViewProj);

	return vout;
}

PsOut PS(VertexOut pin) : SV_Target
{
	PsOut pout;

    // Default to multiplicative identity.
    pout.Albedo = float4(1, 1, 1, 1);

	//
	// Normal mapping
	//
	float3 NormalW = float3(0, 0, -1);
	

	pout.NormalSpec.xyz = (NormalW + float3(1.0f, 1.0f, 1.0f)) * 0.5f;
	pout.NormalSpec.w  	= 1000.0f / 1000.0f;
	pout.Albedo.w  		= 0.0f;

	return pout;
}

technique11 BasicTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}