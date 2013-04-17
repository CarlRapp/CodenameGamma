#define COMPUTE_SHADER_TILE_GROUP_DIM 16
#define COMPUTE_SHADER_TILE_GROUP_SIZE (COMPUTE_SHADER_TILE_GROUP_DIM*COMPUTE_SHADER_TILE_GROUP_DIM)

#include "LightHelper.fx"
 
 cbuffer Frame
{
	float4x4 gCameraProj;
}
 
StructuredBuffer<DirectionalLight> 	gDirLight;
StructuredBuffer<PointLight> 		gPointLight;
StructuredBuffer<SpotLight> 		gSpotLight;
 
 [numthreads(COMPUTE_SHADER_TILE_GROUP_DIM, COMPUTE_SHADER_TILE_GROUP_DIM, 1)]	//Antal trådar per grupp x,y,z
void ComputeShaderTileCS(uint3 groupId          : SV_GroupID,
                         uint3 dispatchThreadId : SV_DispatchThreadID,
                         uint3 groupThreadId    : SV_GroupThreadID
                         )
{
// NOTE: This is currently necessary rather than just using SV_GroupIndex to work
    // around a compiler bug on Fermi.
    uint groupIndex = groupThreadId.y * COMPUTE_SHADER_TILE_GROUP_DIM + groupThreadId.x;	//Lokalt trådindex
    
    // How many total lights?
    uint totalDirLights, totalPointLights, totalSpotLights, dummy;
    gDirLight.GetDimensions(totalDirLights, dummy);	//Hämtar längden på lightbuffern i totalLights
	gPointLight.GetDimensions(totalPointLights, dummy);
	gSpotLight.GetDimensions(totalSpotLights, dummy);
	
    uint2 globalCoords = dispatchThreadId.xy;  //globalt trådindex

    SurfaceData surfaceSamples[MSAA_SAMPLES]; //MSAA?
    ComputeSurfaceDataFromGBufferAllSamples(globalCoords, surfaceSamples); //MSAA?
        
    // Work out Z bounds for our samples
	float minZSample = mCameraNearFar.y; //minz = max
	float maxZSample = mCameraNearFar.x; //maxz = min
	
	float viewSpaceZ = g_Depth.x;
	
	bool validPixel = viewSpaceZ >= mCameraNearFar.x && viewSpaceZ <  mCameraNearFar.y;
	
	if (validPixel)
	{
		minZSample = min(minZSample, viewSpaceZ);	//om nya min är mindre än förra min byter vi den.
        maxZSample = max(maxZSample, viewSpaceZ); 	//om nya max är större än förra max byter vi den.
	}
    
    // Initialize shared memory light list and Z bounds
    if (groupIndex == 0) {	//Om vi är i första tråden för gruppen:
        sTileNumLights = 0;	//Säter standardvärden.
        sNumPerSamplePixels = 0;
        sMinZ = 0x7F7FFFFF;      // Max float
        sMaxZ = 0;
    }

    GroupMemoryBarrierWithGroupSync(); //Väntar på alla trådarna i gruppen
    
    if (maxZSample >= minZSample) {	//om lokalt max är större/lika med lokalt min
        InterlockedMin(sMinZ, asuint(minZSample)); //Ändrar delad sMinZ/sMaxZ till lokala variablen om den är mindre/större.
        InterlockedMax(sMaxZ, asuint(maxZSample));
    }

    GroupMemoryBarrierWithGroupSync(); //Väntar på alla trådarna i gruppen

    float minTileZ = asfloat(sMinZ); //Hämtar minsta z-värdet för tilen
    float maxTileZ = asfloat(sMaxZ); //Hämtar största z-värdet för tilen
    

	//Skapar ett frustum för tilen som vi kan Culla ljus med.

    // Work out scale/bias from [0, 1]
    float2 tileScale = float2(mFramebufferDimensions.xy) * rcp(float(2 * COMPUTE_SHADER_TILE_GROUP_DIM)); //Skala för kamerans projection till tilens projektion
    float2 tileBias = tileScale - float2(groupId.xy); //offset för prjektionen

    //Skapar en prjektionsmatris för tilen
    float4 c1 = float4(gCameraProj._11 * tileScale.x, 0.0f, tileBias.x, 0.0f);
    float4 c2 = float4(0.0f, -gCameraProj._22 * tileScale.y, tileBias.y, 0.0f);
    float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);

    // Derive frustum planes
    float4 frustumPlanes[6];
    // Sides
    frustumPlanes[0] = c4 - c1;
    frustumPlanes[1] = c4 + c1;
    frustumPlanes[2] = c4 - c2;
    frustumPlanes[3] = c4 + c2;
    // Near/far
    frustumPlanes[4] = float4(0.0f, 0.0f,  1.0f, -minTileZ);
    frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f,  maxTileZ);
    
    // Normalize frustum planes (near/far already normalized)
    [unroll] for (uint i = 0; i < 4; ++i) {
        frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));
    }
    
	//Låter varje tråd gå i genom varsit ljus. tråd 0 börjar med ljus 0, tråd 1 med ljus 1 osv. de fortsätter sedan med att addera indexet med antal trådar.

    // Cull lights for this tile
    for (uint lightIndex = groupIndex; lightIndex < totalLights; lightIndex += COMPUTE_SHADER_TILE_GROUP_SIZE) {
        PointLight light = gLight[lightIndex];
                
        // Cull: point light sphere vs tile frustum
        bool inFrustum = true;
        [unroll] for (uint i = 0; i < 6; ++i) {	//Kollar så att spheren för ljuset är inne i frusumet för alla planen
            float d = dot(frustumPlanes[i], float4(light.positionView, 1.0f));
            inFrustum = inFrustum && (d >= -light.attenuationEnd);
        }

        [branch] if (inFrustum) {	//om ljuset är inne
            // Append light to list
            // Compaction might be better if we expect a lot of lights
            uint listIndex;
            InterlockedAdd(sTileNumLights, 1, listIndex);	//Adderar 1 till den gemensamma "antal ljus"-variablen. listIndex får nu värdet på storleken "antal ljus"-variablen innan operationen.
            sTileLightIndices[listIndex] = lightIndex; //Lägger till indexet på ljuset till den gemensamma "ljus som kan påverka denna tilen"-listan på positionen listIndex.
        }
    }

    GroupMemoryBarrierWithGroupSync(); //Väntar på alla trådarna i gruppen
    
    uint numLights = sTileNumLights; //Antal ljus som påverkar tilen.

    // Only process onscreen pixels (tiles can span screen edges)
    if (all(globalCoords < mFramebufferDimensions.xy)) {	//Om alla pixlar finns på bilden.
        [branch] if (mUI.visualizeLightCount) {	//Om man ska visa hur många ljus som påverkar tilen.
            [unroll] for (uint sample = 0; sample < MSAA_SAMPLES; ++sample) {
                WriteSample(globalCoords, sample, (float(sTileNumLights) / 255.0f).xxxx);
            }
        } else if (numLights > 0) { //Om man har minst ett ljus som påverkar tilen.
            bool perSampleShading = RequiresPerSampleShading(surfaceSamples);
            [branch] if (mUI.visualizePerSampleShading && perSampleShading) {
                [unroll] for (uint sample = 0; sample < MSAA_SAMPLES; ++sample) {
                    WriteSample(globalCoords, sample, float4(1, 0, 0, 1));
                }
            } else {	//Om man ska måla ut ljusen.
                float3 lit = float3(0.0f, 0.0f, 0.0f); //Ljus för pixeln
                for (uint tileLightIndex = 0; tileLightIndex < numLights; ++tileLightIndex) { //För varje ljus
                    PointLight light = gLight[sTileLightIndices[tileLightIndex]]; //hämtar pointlight
                    AccumulateBRDF(surfaceSamples[0], light, lit);	//Utför ljustberäkning och adderar ljuset till pixeln
                }

                // Write sample 0 result
                WriteSample(globalCoords, 0, float4(lit, 1.0f));
                        
                [branch] if (perSampleShading) {
                    #if DEFER_PER_SAMPLE
                        // Create a list of pixels that need per-sample shading
                        uint listIndex;
                        InterlockedAdd(sNumPerSamplePixels, 1, listIndex);
                        sPerSamplePixels[listIndex] = PackCoords(globalCoords);
                    #else
                        // Shade the other samples for this pixel
                        for (uint sample = 1; sample < MSAA_SAMPLES; ++sample) {
                            float3 litSample = float3(0.0f, 0.0f, 0.0f);
                            for (uint tileLightIndex = 0; tileLightIndex < numLights; ++tileLightIndex) {
                                PointLight light = gLight[sTileLightIndices[tileLightIndex]];
                                AccumulateBRDF(surfaceSamples[sample], light, litSample);
                            }                        
                            WriteSample(globalCoords, sample, float4(litSample, 1.0f));
                        }
                    #endif
                } else {
                    // Otherwise per-pixel shading, so splat the result to all samples
                    [unroll] for (uint sample = 1; sample < MSAA_SAMPLES; ++sample) {
                        WriteSample(globalCoords, sample, float4(lit, 1.0f));
                    }
                }
            }
        } else {
            // Otherwise no lights affect here so clear all samples
            [unroll] for (uint sample = 0; sample < MSAA_SAMPLES; ++sample) {
                WriteSample(globalCoords, sample, float4(0.0f, 0.0f, 0.0f, 0.0f));
            }
        }
    }

    #if DEFER_PER_SAMPLE && MSAA_SAMPLES > 1
        // NOTE: We were careful to write only sample 0 above if we are going to do sample
        // frequency shading below, so we don't need a device memory barrier here.
        GroupMemoryBarrierWithGroupSync();

        // Now handle any pixels that require per-sample shading
        // NOTE: Each pixel requires MSAA_SAMPLES - 1 additional shading passes
        const uint shadingPassesPerPixel = MSAA_SAMPLES - 1;
        uint globalSamples = sNumPerSamplePixels * shadingPassesPerPixel;

        for (uint globalSample = groupIndex; globalSample < globalSamples; globalSample += COMPUTE_SHADER_TILE_GROUP_SIZE) {
            uint listIndex = globalSample / shadingPassesPerPixel;
            uint sampleIndex = globalSample % shadingPassesPerPixel + 1;        // sample 0 has been handled earlier

            uint2 sampleCoords = UnpackCoords(sPerSamplePixels[listIndex]);
            SurfaceData surface = ComputeSurfaceDataFromGBufferSample(sampleCoords, sampleIndex);

            float3 lit = float3(0.0f, 0.0f, 0.0f);
            for (uint tileLightIndex = 0; tileLightIndex < numLights; ++tileLightIndex) {
                PointLight light = gLight[sTileLightIndices[tileLightIndex]];
                AccumulateBRDF(surface, light, lit);
            }
            WriteSample(sampleCoords, sampleIndex, float4(lit, 1.0f));
        }
    #endif
}