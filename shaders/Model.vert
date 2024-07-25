#version 460 core
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout (location = 6) in mat4 in_transform;

layout(binding = 0) uniform CameraBufferObject {
	mat4 view;
	mat4 proj;
} camera;

out vec3 frag_normal;
out vec3 frag_position;
out vec2 frag_texcoord;

void main() {
	mat4 MVP = camera.proj * camera.view * in_transform;
	gl_Position = MVP * vec4(in_vertex, 1.0);
	frag_position = vec3(in_transform * vec4(in_vertex, 1.0));
	// transformed in fragment shader
//	frag_normal = mat3(transpose(inverse(in_transform))) * in_normal;
	frag_normal = mat3(in_transform) * in_normal;
	frag_texcoord = in_texcoord;
}
