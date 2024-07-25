#version 460 core

out vec4 color;

//uniform sampler2D tex_color;
layout (binding = 1) uniform sampler2D tex_color;
uniform sampler2D tex_occlusion;

in TES_OUT {
    vec2 tc;
} fs_in;

void main(void) {
    color = texture(tex_color, fs_in.tc);
    color *= texture(tex_occlusion, fs_in.tc).r;
    color.a = 1.0f;
}