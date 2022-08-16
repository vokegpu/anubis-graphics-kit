#version 330 core

out vec4 final_frag_color;
in vec2 varying_attrib_tex_coords;
in vec3 varying_attrib_normals;
in vec3 varying_attrib_pos;

uniform vec4 u_vec_color;
uniform sampler2D u_sampler_texture_active;
uniform bool u_bool_texture_active;

void main() {
    float ka = 0.4f;
    vec4 ambient_light = vec4(ka);

    vec4 fragcolor = vec4(varying_attrib_tex_coords, 0, 1);

    if (u_bool_texture_active) {
        fragcolor = texture2D(u_sampler_texture_active, varying_attrib_tex_coords) * fragcolor;
    }

    vec3 light_color = vec3(1.0f);
    vec3 light_pos = vec3(0, 1, 0);
    vec3 diffuse_color = vec3(1, 1, 1);
    vec3 light_pos_normal = normalize(light_pos - varying_attrib_pos);

    vec3 diffuse = diffuse_color * light_color * max(dot(varying_attrib_normals, light_pos_normal), 0);
    final_frag_color.xyz = fragcolor.xyz + diffuse;
    final_frag_color = fragcolor;
    final_frag_color.w = 1.0;
}