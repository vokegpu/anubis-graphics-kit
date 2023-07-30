#version 450

layout (location = 0) out vec4 vFragColor;

in vec3 vPos;
in vec2 vTexCoord;
in vec3 vNormal;
in float vStarsLuminance;
in flat int vStarsRendering;

uniform vec3 uColor;
uniform vec3 uDirectionLightMoon;
uniform vec3 uSkyColor;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 hash4(vec4 n) {
    return fract(sin(n) * 1399763.5453123);
}

vec3 hash3(vec3 n) {
    return fract(sin(n) * 1399763.5453123);
}

vec3 hpos(vec3 n) {
    return hash3(vec3(dot(n, vec3(157.0, 113.0, 271.0)), dot(n, vec3(311.0, 337.0, 179.0)), dot(n, vec3(271.0, 557.0, 431.0))));
}

vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
    return mod(((x * 34.0) + 1.0) * x, 289.0);
}

vec4 taylorInvSqrt(vec4 r) {
    return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float noise4D(vec2 p) {
    vec4 Pi = floor(p.xyxy) + vec4(0.0, 0.0, 0.0, 1.0);
    vec4 Pf = fract(p.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);

    Pi = mod289(Pi);

    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;

    vec4 i = permute(permute(ix) + iy);
    vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0;
    vec4 gy = abs(gx) - 0.5;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;

    vec2 g00 = vec2(gx.x, gy.x);
    vec2 g10 = vec2(gx.y, gy.y);
    vec2 g01 = vec2(gx.z, gy.z);
    vec2 g11 = vec2(gx.w, gy.w);
    vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.z;

    float n00 = dot(g00, vec2(fx.x, fy.y));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));

    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    float a = rand(i);
    float b = rand(i + vec2(1.0f, 0.0f));
    float c = rand(i + vec2(0.0f, 1.0f));
    float d = rand(i + vec2(1.0f, 1.0f));

    vec2 u = f * f * (3.0f - 2.0f * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0f - u.x) + (d - b) * u.x * u.y;
}

float snoise(vec2 v)  {
    vec2 i = floor(v + dot(v, vec2(0.898, 0.2357)));
    vec2 x = v - i + dot(i, vec2(0.3678, 0.153));

    vec2 p;
    p = vec2(0.5) + 0.5*vec2(dot(x, x), dot(x, x + vec2(0.153, 0.9135)));

    vec2 lo = floor(x);
    vec2 hi = lo + vec2(1.0);
    vec2 f = smoothstep(vec2(0.0), vec2(1.0), fract(p));

    float n = (mix(dot(lo, vec2(0.153, 0.9135)), dot(hi, vec2(0.153, 0.9135)), f.x));
    n += (mix(dot(lo, vec2(0.898, 0.2357)), dot(hi, vec2(0.898, 0.2357)), f.y));
    return n;
}
vec3 albedoBlinnPhong() {
    //return mix(uColor, uSkyColor, dot(vNormal, uDirectionLightMoon));
    vec3 albedoColor = max(uColor * max(dot(vNormal, uDirectionLightMoon), 0.0f), uSkyColor);
    return albedoColor;
}

#define LIGHT_ASTROO_PROBABILITY 0.7
#define SUN_LIGHT_INTERPOLATION_FACTOR 2.0

void main() {
    if (vStarsRendering == 0) {
        vFragColor = vec4(albedoBlinnPhong(), 1.0f);
    } else {
        float f = noise(vec2(length((rand(vec2(-22.0f, 5000.0f))) * 0.1 - (-vPos.zz))));

        float interpFactor = -1.0f;
        if (vStarsLuminance < SUN_LIGHT_INTERPOLATION_FACTOR) {
            interpFactor = vStarsLuminance / SUN_LIGHT_INTERPOLATION_FACTOR;
        }

        if (interpFactor >= 0.0f && interpFactor < 0.2f && f < LIGHT_ASTROO_PROBABILITY) {
            discard;
        } else if (interpFactor > 0.2f && rand(vPos.xz) > 0.5) {
            discard;
        }

        if (f < LIGHT_ASTROO_PROBABILITY) {
            vFragColor = vec4(0.5f - (noise(vec2(length((rand(vec2(-22.0f, 666.0f))) * 0.2 - (-vPos.zy))))), f, noise(vec2(length((rand(vec2(-22.0f, 40.0f))) * 0.1 - (-vPos.zy)))), 1.0f);
        } else {
            vFragColor = vec4(1.0f);
        }

        if (interpFactor >= 0.0f) {
            vFragColor = mix(vFragColor, vec4(1.0f), 1.0f - interpFactor);
            vFragColor.a = max(interpFactor, 0.09f);
        }
    }
}