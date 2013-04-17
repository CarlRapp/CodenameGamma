Texture2D g_Albedo;
Texture2D g_Diffuse;
Texture2D g_Specular;

cbuffer Frame
{
	matrix InverseProjection;
	bool showColor;
}

SamplerState g_Sampler 
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState depthSampler 
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

struct VSIn
{
	float2 Pos 	: POSITION;
	float2 Tex	: TEXTURECOORD;
};

struct PSSceneIn
{
	float4 Pos	: SV_POSITION;
	float2 Tex	: TEXTURECOORD;
};


//-----------------------------------------------------------------------------------------
// State Structures
//-----------------------------------------------------------------------------------------
RasterizerState NoCulling
{
	CullMode = NONE;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
PSSceneIn VSScene(VSIn input)
{
	PSSceneIn output 	= (PSSceneIn)0;
	output.Pos			= float4(input.Pos, 0, 1);
	output.Tex			= input.Tex;
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
float4 PSScene(PSSceneIn input) : SV_Target
{	
	//if (showColor)
	return g_Albedo.Sample(g_Sampler, input.Tex);
		
	float3 albedo 		= g_Albedo.Sample(g_Sampler, input.Tex).rgb;	
	float3 diffuse		= g_Diffuse.Sample(g_Sampler, input.Tex).rgb;
	float3 specular		= g_Specular.Sample(g_Sampler, input.Tex).rgb;	

	return float4((albedo * diffuse + specular),1);
}

DepthStencilState DepthStencil
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

BlendState NoBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
};

//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 CombineTech
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    SetDepthStencilState( DepthStencil, 0 );
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	    //SetRasterizerState( NoCulling );
    }  
}

