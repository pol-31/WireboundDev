#version 460

layout(quads, equal_spacing, ccw) in;

uniform sampler2D tex_displacement;
uniform float dmap_depth;

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

/*patch */in TESC_OUT {
  vec4 v1;
  vec4 v2;
  vec4 up;
  vec4 dir;
  vec2 tc;
} tese_in[];

out TESE_OUT {
  vec3 normal;
  vec2 uv;
} tese_out;

void main() {
  const float u = gl_TessCoord.x;
  const float v = gl_TessCoord.y;

  const vec3 v0 = gl_in[0].gl_Position.xyz;
  const vec3 v1 = tese_in[0].v1.xyz;
  const vec3 v2 = tese_in[0].v2.xyz;
  const vec3 dir = tese_in[0].dir.xyz;
  const float width = tese_in[0].v2.w;

  // TODO: use mix()
  const vec3 a = v0 + v * (v1 - v0);
  const vec3 b = v1 + v * (v2 - v1);
  const vec3 c = a + v * (b - a);
  const vec3 t1 = dir;
  
  const vec3 c0 = c - t1 * width * 0.5;
  const vec3 c1 = c + t1 * width * 0.5;
  const vec3 t0 = normalize(b - a);

  const float t = u + 0.5 * v - u * v;
  vec3 p = (1.0 - t) * c0 + t * c1;

  // TODO: we also need to take into accound height of grass itself (0.5 by now)?
  // TODO: do we need dmap_depth?
  p.y += 0.5f;//texture(tex_displacement, p.xz).r * dmap_depth / 2.0f/* / 64.0f + 0.5f * dmap_depth*/;

  // some radius from border to not to "place grass in fly";
  // left some offset for grass part below the blade;

  // OR use height map for each blade

//  p.y += 0.5f;

  tese_out.uv = vec2(u, v);
  tese_out.normal = normalize(cross(t0, t1));
  gl_Position = camera.proj * camera.view * vec4(p, 1.0);
}
