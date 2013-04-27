#define COMPUTE_SHADER_TILE_GROUP_DIM 16
#define COMPUTE_SHADER_TILE_GROUP_SIZE (COMPUTE_SHADER_TILE_GROUP_DIM*COMPUTE_SHADER_TILE_GROUP_DIM)

#include "LightHelper.fx"
#define MAX_LIGHTS 1024
#define MAX_SHADOWMAPS 10

cbuffer perFrame
{
	float4x4    gLightViewProjTex[MAX_SHADOWMAPS];
	float4x4	gInvViewProjs[4];
	float4		gCamPositions[4];
	float2		gResolution;
};

//Lights
StructuredBuffer<DirectionalLight>	gDirLightBuffer;
StructuredBuffer<PointLight>		gPointLightBuffer;
StructuredBuffer<SpotLight>			gSpotLightBuffer;

//GBuffer
Texture2D gAlbedoMap;
Texture2D gNormalSpecMap;
Texture2D gDepthMap;

//Shadowmap
Texture2D gShadowMap;

//Output
RWTexture2D<float4> gOutput;

//Group Variables
groupshared int sMinX;
groupshared int sMaxX;
groupshared int sMinY;
groupshared int sMaxY;
groupshared int sMinZ;
groupshared int sMaxZ;

groupshared uint sTilePointLightIndices[MAX_LIGHTS];
groupshared uint sTileSpotLightIndices[MAX_LIGHTS];

groupshared uint sTileNumPointLights;
groupshared uint sTileNumSpotLights;

void ConeToAABB(float3 pos, float3 dir, float angle, out float3 min, out float3 max)
{

}

bool AABBvsSphere(float3 min, float3 max, float3 c, float r)
{
	float d = 0;
	
	for (int i = 0; i < 3; ++i)
	{
		float e1 = c[i] - min[i];
		float e2 = c[i] - max[i];
		if (e1 < 0)
		{
			if (e1 < -r)
			{
				return false;
			}
			d = d + pow(e1, 2);			
		}
	
		else if (e2 > 0)
		{
			if (e2 > r)
			{
				return false;
			}
			d = d + pow(e2, 2);			
		}	
	}

	if (d <= pow(r, 2))
	{
		return true;
	}
	return false;
}



float CalcualteShadowFactor(DirectionalLight light, float4 posW)
{
	if (light.HasShadow)
		{
			float4x4 VPT = gLightViewProjTex[light.ShadowIndex];
			float4 posT = mul(posW, VPT);
			float shadowDepth = gShadowMap[posT.xy * gResolution].x;
			
			if (posT.z > shadowDepth)
				return 0;

			return 1;
		}
	else
		return 1;
}

float CalcualteShadowFactor(SpotLight light, float4 posW)
{
	if (light.HasShadow)
		{
			float4x4 VPT = gLightViewProjTex[light.ShadowIndex];
			float4 posT = mul(posW, VPT);
			float shadowDepth = gShadowMap[posT.xy * gResolution].x;
			
			if (posT.z > shadowDepth)
				return 0;

			return 1;
		}
	else
		return 1;
}

[numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]	//Antal trådar per grupp x,y,z
void TiledLightningCS(	uniform int gViewportCount,
						uint3 dispatchThreadID : SV_DispatchThreadID,
						uint3 groupThreadID    : SV_GroupThreadID
                         )
{
	uint groupIndex = groupThreadID.y * COMPUTE_SHADER_TILE_GROUP_DIM + groupThreadID.x;	//Lokalt trådindex

	//Bästämmer vilken viewport vi är i.
	uint viewportIndex = 0; //player 1
	uint viewportX = 0;
	uint viewportY = 0;
	uint viewportWidth = gResolution.x;
	uint viewportHeight = gResolution.y;
	switch (gViewportCount)
	{
	case 2:
		viewportWidth = gResolution.x / 2;
		if (dispatchThreadID.x >= gResolution.x / 2)	//Högra halvan
		{
			viewportX = gResolution.x / 2;
			viewportIndex = 1;	//player 2
		}
		break;

	case 3:
		viewportHeight	= gResolution.y / 2;
		if (dispatchThreadID.y >= gResolution.y / 2)	//Undre halvan
		{
			viewportY = gResolution.y / 2;
			viewportWidth	= gResolution.x / 2;
			if (dispatchThreadID.x < gResolution.x / 2)	//Vänstra halvan
				viewportIndex = 1; //player 2
			else                                        //Högra halvan
			{
				viewportX = gResolution.x / 2;
				viewportIndex = 2; //player 3
			}
		}
		break;

	case 4:
		viewportWidth	= gResolution.x / 2;
		viewportHeight	= gResolution.y / 2;
		if (dispatchThreadID.y < gResolution.y / 2)		//Övre halvan
		{
			if (dispatchThreadID.x >= gResolution.x / 2)//Högra halvan
			{
				viewportX = gResolution.x / 2;
				viewportIndex = 1; //player 2
			}
		}
		else											//Undre halvan
		{
			viewportY = gResolution.y / 2;
			if (dispatchThreadID.x < gResolution.x / 2)	//Vänstra halvan
				viewportIndex = 2; //player 3
			else                                        //Högra halvan.
			{
				viewportX = gResolution.x / 2;
				viewportIndex = 3; //player 4
			}
		}
		break;
	}

	float2 viewportStart = float2(viewportX, viewportY);
	float2 viewportDim   = float2(viewportWidth, viewportHeight);

	float4 viewportColor;

	if (viewportIndex == 0)
		viewportColor = float4(1,0,0,0);
	else if (viewportIndex == 1)
		viewportColor = float4(0,1,0,0);
	else if (viewportIndex == 2)
		viewportColor = float4(0,0,1,0);
	else if (viewportIndex == 3)
		viewportColor = float4(1,1,1,0);

	//Extracting albedo, normal(world), pos(proj), material(specpower+specintesity)
	float4 albedo		= gAlbedoMap[dispatchThreadID.xy];
	float4 normalspec	= gNormalSpecMap[dispatchThreadID.xy];

	float4 normalW		= float4(normalspec.x, normalspec.y, normalspec.z, 0);	//Get X, Y-components of the Normal
	normalW				= (normalW * 2.0f) - float4(1, 1, 1, 0);	//[0, 1] to [-1, 1]

	//normalW 			= (normalW + float4(1,1,1,1)) * 0.5f;		//[-1, 1] to [0, 1] for testing purpose



	float depth			= gDepthMap[dispatchThreadID.xy].x;

	float4 posW;
	posW.xy = (dispatchThreadID.xy - viewportStart) / viewportDim;
	posW.x  = posW.x * 2.0f		- 1.0f;
	posW.y	= posW.y * -2.0f	+ 1.0f;
	//posW.y	= posW.y * -1.0f	+ 1.0f;
	posW.z = depth;
	//posW.z = 0;
	posW.w = 1.0f;
	posW = mul(posW, gInvViewProjs[viewportIndex]);
	posW /= posW.w;

	Material mat;
	mat.SpecIntensity	= albedo.w;
	mat.SpecPower		= normalspec.w * 1000.0f;


	float3 toEye = normalize(gCamPositions[viewportIndex].xyz - posW.xyz);

	
	// Initialize shared memory light list and Z bounds
    if (groupIndex == 0) {	//Om vi är i första tråden för gruppen:
        sMinX = 0x7F7FFFFF;      // Max float
        sMaxX = 0x00000000;
		sMinY = 0x7F7FFFFF;      // Max float
        sMaxY = 0x00000000;
		sMinZ = 0x7F7FFFFF;      // Max float
        sMaxZ = 0x00000000;
		sTileNumPointLights = 0;
		sTileNumSpotLights = 0;
    }

	GroupMemoryBarrierWithGroupSync(); //Väntar på alla trådarna i gruppen
    /*
	sMinX
		*/

	if (depth != 1.0f)
	{
		InterlockedMin(sMinX, asint(posW.x)); //Ändrar delad sMinZ/sMaxZ till lokala variablen om den är mindre/större.
		InterlockedMax(sMaxX, asint(posW.x));

		InterlockedMin(sMinY, asint(posW.y)); //Ändrar delad sMinZ/sMaxZ till lokala variablen om den är mindre/större.
		InterlockedMax(sMaxY, asint(posW.y));

		InterlockedMin(sMinZ, asint(posW.z)); //Ändrar delad sMinZ/sMaxZ till lokala variablen om den är mindre/större.
		InterlockedMax(sMaxZ, asint(posW.z));
	}
    

	GroupMemoryBarrierWithGroupSync(); //Väntar på alla trådarna i gruppen

    float minTileX = asfloat(sMinX); //Hämtar minsta x-värdet för tilen
    float maxTileX = asfloat(sMaxX); //Hämtar största x-värdet för tilen
	float minTileY = asfloat(sMinY); //Hämtar minsta y-värdet för tilen
    float maxTileY = asfloat(sMaxY); //Hämtar största y-värdet för tilen
	float minTileZ = asfloat(sMinZ); //Hämtar minsta z-värdet för tilen
    float maxTileZ = asfloat(sMaxZ); //Hämtar största z-värdet för tilen

	float3 minTile = float3(minTileX, minTileY, minTileZ);
	float3 maxTile = float3(maxTileX, maxTileY, maxTileZ);

	uint totalDirLights = 0, totalPointLights = 0, totalSpotLights = 0, dummy;
	gDirLightBuffer.GetDimensions(totalDirLights, dummy);
    gPointLightBuffer.GetDimensions(totalPointLights, dummy);	
    gSpotLightBuffer.GetDimensions(totalSpotLights, dummy);

	//Kollar vilka ljus som berör tilen. 

	//Alla ytor berörs av alla directionallights.

	//PointLights
	for (uint lightIndex = groupIndex; lightIndex < totalPointLights; lightIndex += COMPUTE_SHADER_TILE_GROUP_SIZE) 
	{
		PointLight light = gPointLightBuffer[lightIndex];        
		if (AABBvsSphere(minTile, maxTile, light.Position, light.Range))
		{
			uint listIndex;
			InterlockedAdd(sTileNumPointLights, 1, listIndex);
			sTilePointLightIndices[listIndex] = lightIndex;
		}
    }


	//SpotLights
	for (uint lightIndex = groupIndex; lightIndex < totalSpotLights; lightIndex += COMPUTE_SHADER_TILE_GROUP_SIZE) 
	{
		SpotLight light = gSpotLightBuffer[lightIndex];        
		if (AABBvsSphere(minTile, maxTile, light.Position, light.Range))
		{
			uint listIndex;
			InterlockedAdd(sTileNumSpotLights, 1, listIndex);
			sTileSpotLightIndices[listIndex] = lightIndex;
		}
    }

	GroupMemoryBarrierWithGroupSync(); //Väntar på alla trådarna i gruppen

	float4 ambient	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec		= float4(0.0f, 0.0f, 0.0f, 0.0f);

	//Calculate DirectionLights
	if (depth != 1.0f)
	{
		for(int i = 0; i < totalDirLights; ++i)
		{
			DirectionalLight light = gDirLightBuffer[i];			
			float shadowFactor = CalcualteShadowFactor(light, posW);

			float4 A, D, S;
			ComputeDirectionalLight(mat, light, normalW, toEye, A, D, S);

			ambient += A;    
			diffuse += D * shadowFactor;
			spec    += S * shadowFactor;
		}
	}
	
	//Calculate PointLights	
	
	for (uint tileLightIndex = 0; tileLightIndex < sTileNumPointLights; ++tileLightIndex) 
	{
		PointLight light = gPointLightBuffer[sTilePointLightIndices[tileLightIndex]]; //hämtar pointlight

		//float shadowFactor = CalcualteShadowFactor(light, posW);

		float4 A, D, S;
		ComputePointLight(mat, light, posW, normalW, toEye, A, D, S);

		ambient += A;    
		diffuse += D;
		spec    += S;
	}

	/*
	for(int i = 0; i < totalPointLights; ++i)
	{

		float4 A, D, S;
		ComputePointLight(mat, gPointLightBuffer[i], posW, normalW, toEye, A, D, S);

		ambient += A;    
		diffuse += D;
		spec    += S;
	}


	*/
	//Calculate SpotLights
	
	for (uint tileLightIndex = 0; tileLightIndex < sTileNumSpotLights; ++tileLightIndex) 
	{
		
		SpotLight light = gSpotLightBuffer[sTileSpotLightIndices[tileLightIndex]]; //hämtar pointlight
		
		
		float shadowFactor = CalcualteShadowFactor(light, posW);

		float4 A, D, S;
		ComputeSpotLight(mat, light, posW, normalW, toEye, A, D, S);

		ambient += A;    
		diffuse += shadowFactor * D;
		spec    += shadowFactor * S;
		
	}
	
	/*
	for(int i = 0; i < totalSpotLights; ++i)
	{
		float4 A, D, S;
		ComputeSpotLight(mat, gSpotLightBuffer[i], posW, normalW, toEye, A, D, S);

		ambient += A;    
		diffuse += D;
		spec    += S;
	}
	*/

	//Final result.
	//gOutput[dispatchThreadID.xy] = sTileNumSpotLights / 100.0f;
	//gOutput[dispatchThreadID.xy] = sTileNumPointLights / 100.0f;
	//gOutput[dispatchThreadID.xy] = posW.x / 4000;
	//gOutput[dispatchThreadID.xy] = (ambient + diffuse) + spec;
	gOutput[dispatchThreadID.xy] = albedo * (ambient + diffuse) + spec;
	//gOutput[dispatchThreadID.xy] = viewportColor * (albedo * (ambient + diffuse) + spec);
}


technique11 Viewport1
{
    pass P0
    {
		SetVertexShader( NULL );
        SetPixelShader( NULL );
		SetComputeShader( CompileShader( cs_5_0, TiledLightningCS(1) ) );
    }
}

technique11 Viewport2
{
    pass P0
    {
		SetVertexShader( NULL );
        SetPixelShader( NULL );
		SetComputeShader( CompileShader( cs_5_0, TiledLightningCS(2) ) );
    }
}

technique11 Viewport3
{
    pass P0
    {
		SetVertexShader( NULL );
        SetPixelShader( NULL );
		SetComputeShader( CompileShader( cs_5_0, TiledLightningCS(3) ) );
    }
}

technique11 Viewport4
{
    pass P0
    {
		SetVertexShader( NULL );
        SetPixelShader( NULL );
		SetComputeShader( CompileShader( cs_5_0, TiledLightningCS(4) ) );
    }
}