
inline float PCFShadowMap(float3 shadowCoord, uint cascadeIndex, float bias) {
    float texelSize = 1.0f / g_ShadowMapSize;
    float kernelSize = 3;
    int index = floor(kernelSize / 2);
    float shadowValue = 0;
    float compareValue = shadowCoord.z - bias;
    [unroll]
    for (int i = -index; i <= index; ++i) {
        [unroll]
        for (int j = -index; j <= index; ++j) {
            int2 offset = int2(i, j);
            float sampleCmp = g_ShadowMapTexture.SampleCmpLevelZero(g_ShadowMapSampler, float3(shadowCoord.xy, cascadeIndex), compareValue, offset);
            shadowValue += sampleCmp;
        }
    }
    shadowValue /= (kernelSize * kernelSize);
    return shadowValue;
}

inline float CalculateShadowValue(float3 worldSpacePos, out uint hitCascadeIndex) {
    float shadowFactor = 1;
    float3 visualizeColor = float3(1.0f, 1.0f, 1.0f);
    
    for (uint cascadeIndex = 0; cascadeIndex < g_ShadowCascadeCount; ++cascadeIndex) {
        // ShadowCoordinates
        float4 worldPosLightSpace = mul(g_DirectionLightProjMatrices[cascadeIndex], mul(g_DirectionLightViewMatrix, float4(worldSpacePos, 1.0f)));
        float3 shadowCoord = worldPosLightSpace.xyz / worldPosLightSpace.w;
        shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
        shadowCoord.y = shadowCoord.y * -0.5f + 0.5f;

        [branch]
        float bias = 0.0015f;
        if (all(saturate(shadowCoord) == shadowCoord)) {
            bias += (0.0015f * cascadeIndex * cascadeIndex);
            float shadowValue = PCFShadowMap(shadowCoord, cascadeIndex, bias);
            shadowFactor = shadowValue;
            hitCascadeIndex = cascadeIndex;

            // TODO: Blend cascade edges for smooth cascade transition
            break;
        }
    }

    return shadowFactor;
}

