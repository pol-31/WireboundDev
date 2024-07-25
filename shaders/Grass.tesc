#version 460

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

in VS_OUT {
  vec4 v1;
  vec4 v2;
  vec4 up;
  vec4 dir;
  vec2 tc;
} tesc_in[];

/*patch */out TESC_OUT {
  vec4 v1;
  vec4 v2;
  vec4 up;
  vec4 dir;
  vec2 tc;
} tesc_out[];

//TODO: we need to tessellate in a way, so more distant blades "decay" into few blades
//TODO: we need to tessellate in a way, so more distant blades "decay" into few blades
//TODO: we need to tessellate in a way, so more distant blades "decay" into few blades
//TODO: we need to tessellate in a way, so more distant blades "decay" into few blades

layout(vertices = 1) out;

void main() {
  // we draw only one patch
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  tesc_out[gl_InvocationID].tc = tesc_in[gl_InvocationID].tc;

  tesc_out[gl_InvocationID].v1 = tesc_in[gl_InvocationID].v1;
  tesc_out[gl_InvocationID].v2 = tesc_in[gl_InvocationID].v2;
  tesc_out[gl_InvocationID].up = tesc_in[gl_InvocationID].up;
  tesc_out[gl_InvocationID].dir = tesc_in[gl_InvocationID].dir;

  float z1 = (camera.proj * camera.view * vec4(tesc_out[gl_InvocationID].v1.xyz, 1)).z;
  float z2 = (camera.proj * camera.view * vec4(tesc_out[gl_InvocationID].v2.xyz, 1)).z;

  if (z1 < 0.8 && z2 < 0.8) {
    gl_TessLevelInner[0] = 2.0;
    gl_TessLevelInner[1] = 7.0;
    gl_TessLevelOuter[0] = 7.0;
    gl_TessLevelOuter[1] = 2.0;
    gl_TessLevelOuter[2] = 7.0;
    gl_TessLevelOuter[3] = 2.0;
  } else {
    gl_TessLevelInner[0] = 1.0;
    gl_TessLevelInner[1] = 3.0;
    gl_TessLevelOuter[0] = 3.0;
    gl_TessLevelOuter[1] = 1.0;
    gl_TessLevelOuter[2] = 3.0;
    gl_TessLevelOuter[3] = 1.0;
  }
}
