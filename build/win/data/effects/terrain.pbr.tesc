#version 450 core

layout (vertices = 4) out;

uniform vec4 uMVP_TescStage;
in vec2 vTexCoord_VertStage[];
out vec2 vTexCoord_TescStage[];

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    vTexCoord_TescStage[gl_InvocationID] = vTexCoord_VertStage[gl_InvocationID];

    if (gl_InvocationID == 0) {
//        const int MIN_TESS_LEVEL = 4;
//        const int MAX_TESS_LEVEL = 4;
//
//        const float MIN_DIST = 20;
//        const float MAX_DIST = 800;
//
//        vec4 eyeSpacePos00 = uMVP_TescStage * gl_in[0].gl_Position;
//        vec4 eyeSpacePos01 = uMVP_TescStage * gl_in[1].gl_Position;
//        vec4 eyeSpacePos10 = uMVP_TescStage * gl_in[2].gl_Position;
//        vec4 eyeSpacePos11 = uMVP_TescStage * gl_in[3].gl_Position;
//
//        float dist00 = clamp((abs(eyeSpacePos00.z) - MIN_DIST) / (MAX_DIST - MIN_DIST), 0.0, 1.0f);
//        float dist01 = clamp((abs(eyeSpacePos01.z) - MIN_DIST) / (MAX_DIST - MIN_DIST), 0.0, 1.0f);
//        float dist10 = clamp((abs(eyeSpacePos10.z) - MIN_DIST) / (MAX_DIST - MIN_DIST), 0.0, 1.0f);
//        float dist11 = clamp((abs(eyeSpacePos11.z) - MIN_DIST) / (MAX_DIST - MIN_DIST), 0.0, 1.0f);
//
//        float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(dist10, dist00));
//        float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(dist00, dist01));
//        float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(dist01, dist11));
//        float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(dist11, dist10));
//
//        gl_TessLevelOuter[0] = tessLevel0;
//        gl_TessLevelOuter[1] = tessLevel1;
//        gl_TessLevelOuter[2] = tessLevel2;
//        gl_TessLevelOuter[3] = tessLevel3;
//
//        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
//        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);

        gl_TessLevelOuter[0] = 4.0f;
        gl_TessLevelOuter[1] = 4.0f;
        gl_TessLevelOuter[2] = 4.0f;
        gl_TessLevelOuter[3] = 4.0f;

        gl_TessLevelInner[0] = 4.0f;
        gl_TessLevelInner[1] = 4.0f;
    }
}