#version 460 core
layout (location = 0) in vec3 in_vertex;

out vec3 frag_texcoord;

/*layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;*/

uniform mat4 view;
uniform mat4 proj;

void main()
{
    frag_texcoord = in_vertex;
    vec4 pos = proj * view * vec4(in_vertex, 1.0);
//    vec4 pos = camera.proj * camera.view * vec4(in_vertex, 1.0);
    gl_Position = pos.xyww;
}  

