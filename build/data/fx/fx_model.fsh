#version 330 core

in vec2 varying_attrib_tex_coords;

uniform float u_float_viewport_height;
uniform vec4 u_vec4_color;
uniform sampler2D u_sampler2d_texture_active;
uniform bool u_bool_texture_enabled;

void main() {
    vec4 fragcolor = u_vec4_color;

    if (u_bool_texture_enabled) {
        fragcolor = texture2D(u_sampler2d_texture_active, varying_attrib_tex_coords) * fragcolor;
    }

    gl_FragColor = fragcolor;
}