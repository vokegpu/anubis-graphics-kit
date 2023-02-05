#version 450

layout (quads, fractional_odd_spacing, ccw) in;
layout (binding = 0) uniform sampler2D uTextureHeightMap;

uniform mat4 uMVP_TeseStage;
uniform mat3 uNormalMatrix;
uniform mat4 uMatrixModel;

in vec2 vTexCoord_TescStage[];
out float vHeight;
out vec3 vPos;
out vec3 vPosModel;
out vec2 vTessCoord;
out vec3 vNormal;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 t00 = vTexCoord_TescStage[0];
    vec2 t01 = vTexCoord_TescStage[1];
    vec2 t10 = vTexCoord_TescStage[2];
    vec2 t11 = vTexCoord_TescStage[3];

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 texCoord = (t1 - t0) * v + t0;

    vHeight = texture(uTextureHeightMap, texCoord).r * 255.0f;
    vTessCoord = gl_TessCoord.xy;

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vNormal = normalize(cross(vVec.xyz, uVec.xyz));

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    p += vec4(vNormal, 0.0f) * vHeight;
    gl_Position = uMVP_TeseStage * p;
    vec4 modelPos = uMatrixModel * p;

    vPos = gl_Position.xyz;
    vPosModel = modelPos.xyz;
    vNormal = normalize(uNormalMatrix * vNormal);
}