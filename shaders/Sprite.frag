#version 460 core

in vec2 texcoord;
uniform sampler2D tex;

uniform float transparency;
uniform float brightness;
uniform vec3 color;

out vec4 FragColor;

void main() {
    vec3 texel = texture(tex_bitmap_ascii, texcoord);
    texel = mix(texel, color);
    texel *= brightness;
    FragColor = vec4(texel, transparency);
}