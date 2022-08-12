#version 330 core

out vec4 final_frag_color;
in vec2 varying_attrib_tex_coords;
in vec4 varying_attrib_light_pos;

uniform float u_float_viewport_height;
uniform vec4 u_vec4_color;
uniform vec4 u_vec4_ambient_color;
uniform vec4 u_vec4_light_color;
uniform sampler2D u_sampler2d_texture_active;
uniform bool u_bool_texture_enabled;

float clamp(float a, float b, float c) {
    return a < b ? b : (a > c ? c : 0);
}

void main() {
    vec4 fragcolor = u_vec4_color * u_vec4_ambient_color;

    if (u_bool_texture_enabled) {
        fragcolor = texture2D(u_sampler2d_texture_active, varying_attrib_tex_coords) * fragcolor;
    }

    vec4 fragcoord = vec4(0, 1, 0, 1.0f);
    vec3 diff = vec3(fragcoord.x - varying_attrib_light_pos.x, fragcoord.y - varying_attrib_light_pos.y, fragcoord.z - varying_attrib_light_pos.z);

    float dist = (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    fragcolor *= vec4(1.0f - clamp(dist, 0, 1.0f), 1.0f - clamp(dist, 0, 1.0f), 1.0f - clamp(dist, 0, 1.0f), u_vec4_light_color.w);
    final_frag_color = fragcolor * u_vec4_light_color;
}