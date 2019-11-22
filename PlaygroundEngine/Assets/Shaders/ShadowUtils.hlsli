
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
            float sampleDepth = g_ShadowMapTexture.SampleCmpLevelZero(g_ShadowMapSampler, float3(shadowCoord.xy + float2(i, j) * texelSize, cascadeIndex), compareValue);
            shadowValue += sampleDepth;
        }
    }
    shadowValue /= pow(kernelSize, 2);
    return shadowValue;
}

inline float CalculateShadowValue(float3 worldSpacePos, out uint hitCascadeIndex) {
    float shadowFactor = 1;
    float3 visualizeColor = float3(1.0f, 1.0f, 1.0f);
    for (uint cascadeIndex = 0; cascadeIndex < g_ShadowCascadeCount; ++cascadeIndex) {
        // ShadowCoordinates
        float4 worldPosLightSpace = mul(g_LightProjMatrices[cascadeIndex], mul(g_LightViewMatrix, float4(worldSpacePos, 1.0f)));
        float3 shadowCoord = worldPosLightSpace.xyz / worldPosLightSpace.w;
        shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
        shadowCoord.y = shadowCoord.y * -0.5f + 0.5f;

        [branch]
        if (all(saturate(shadowCoord) == shadowCoord)) {
            float bias = 0.004f;
            float shadowValue = PCFShadowMap(shadowCoord, cascadeIndex, bias);
            shadowFactor = shadowValue;
            hitCascadeIndex = cascadeIndex;

            // TODO: Blend cascade edges for smooth cascade transition
            break;
        }
    }

    return shadowFactor;
}

