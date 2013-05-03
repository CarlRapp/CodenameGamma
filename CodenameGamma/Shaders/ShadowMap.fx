cbuffer Camera
{
	matrix 				ViewProjection;
};

cbuffer Object
{
	matrix 				World;
};

struct VSIn
{
	float3 Pos 			: POSITION;
};

struct PSSceneIn
{
	float4 Pos			: SV_POSITION;
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
	output.Pos			= mul(float4(input.Pos, 1), World);
	output.Pos			= mul(output.Pos, ViewProjection);
	//output.Pos.z		= 1.0f;
	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader: PSSceneMain
//-----------------------------------------------------------------------------------------
void PSScene(PSSceneIn input)// : SV_Target
{	
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

RasterizerState Depth2048
{
	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// You need to experiment with these values for your scene.
	DepthBias = 500;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
	CullMode = NONE;
};

RasterizerState Depth1024
{
	DepthBias = 1500;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
	CullMode = NONE;
};

RasterizerState Depth512
{
	DepthBias = 2000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
	CullMode = NONE;
};

RasterizerState Depth256
{
	DepthBias = 4000;
    DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
	CullMode = NONE;
};


//-----------------------------------------------------------------------------------------
// Technique: RenderTextured  
//-----------------------------------------------------------------------------------------
technique11 BasicShadow
{
    pass p0
    {
		// Set VS, GS, and PS
        SetVertexShader( CompileShader( vs_4_0, VSScene() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSScene() ) );
	    SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState( DepthStencil, 0 );
	    SetRasterizerState( Depth1024 );
    }  
}

