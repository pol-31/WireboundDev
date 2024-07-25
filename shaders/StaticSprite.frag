/*
#version 460 core

in vec2 texcoord;
uniform usampler2D tex;

uniform float transparency;
uniform float brightness;
uniform vec3 color;

out vec4 FragColor;

void main() {
    vec4 tex_color = texture(tex, texcoord); // GL_RED texture for bitmaps
//    vec3 texel = vec3(tex_color.x);
//    texel *= color;
//    texel *= brightness;
    //    FragColor = vec4(texel, transparency);
    FragColor = vec4(0.0f, 1.0f, tex_color.r, 1.0f);
}*/

#version 460 core

in vec2 texcoord;
uniform usampler2D tex;
out vec4 FragColor;

void main() {
    uvec4 tex_color = texture(tex, texcoord);
    float value = float(tex_color.r);
    FragColor = vec4(value, value, value, 1.0f);
}
