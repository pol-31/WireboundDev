#version 460 core
layout (location = 0) in vec2 in_position;
layout (location = 1) in vec2 in_texcoord;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(in_position, 0.0f, 1.0);
    texcoord = in_texcoord;
}
