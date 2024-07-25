#version 460

layout(binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

uniform float current_time;
uniform float delta_time;

uniform float wind_magnitude;
uniform float wind_wave_length;
uniform float wind_wave_period;

struct Blade {
    vec4 v0;
    vec4 v1;
    vec4 v2;
    vec4 up;
};

//TODO: is ubo enough (64kb)?
//TODO: we only read it, generate new data based on time and physic, so its readonly
layout(binding = 1, std140) buffer inputBuffer {
    Blade inputBlades[];
};

layout(binding = 2, std140) buffer outputBuffer {
    Blade outputBlades[];
};

out VS_OUT {
    vec4 v1;
    vec4 v2;
    vec4 up;
    vec4 dir;
    vec2 tc; // TODO: maybe we need to reduce it for "by chunks"
} vs_out;


bool inBounds(float value, float bounds) {
    return (value >= -bounds) && (value <= bounds);
}

float rand(float seed) {
    return fract(sin(seed)*100000.0);
}

//TODO: fix amount; prev invocation was
// glDispatchCompute(static_cast<GLuint>(blades_count_), 1, 1);
// layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

void main() {
    const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
    vec4(0.5, 0.0, -0.5, 1.0),
    vec4(-0.5, 0.0, 0.5, 1.0),
    vec4(0.5, 0.0, 0.5, 1.0));
    int x = gl_InstanceID & 63;
    int y = gl_InstanceID >> 6;
    vec2 offs = vec2(x, y);
    vs_out.tc = (vertices[gl_VertexID].xz + offs + vec2(0.5)) / 64.0;


    uint index = gl_InstanceID;
//    uint index = gl_InstanceID;

    vec3 v0 = inputBlades[index].v0.xyz;
    vec3 v1 = inputBlades[index].v1.xyz;
    vec3 v2 = inputBlades[index].v2.xyz;
    vec3 up = normalize(inputBlades[index].up.xyz);
    float orientation = inputBlades[index].v0.w;
    float height = inputBlades[index].v1.w;
    float width = inputBlades[index].v2.w;
    float stiffness = inputBlades[index].up.w;

    // Frustum culling
    /*vec4 v0ClipSpace = camera.proj * camera.view * vec4(v0, 1);
    vec4 v1ClipSpace = camera.proj * camera.view * vec4(v1, 1);
    v0ClipSpace /= v0ClipSpace.w;
    v1ClipSpace /= v1ClipSpace.w;

    bool v0OutFrustum =
    v0ClipSpace.x < -1 || v0ClipSpace.x > 1
    || v0ClipSpace.y < -1 || v0ClipSpace.y > 1;

    bool v1OutFrustum =
    v1ClipSpace.x < -1 || v1ClipSpace.x > 1
    || v1ClipSpace.y < -1 || v1ClipSpace.y > 1;
    if (v0OutFrustum && v1OutFrustum) return;

    // Distance culling
    const float far1 = 0.95;
    if (v0ClipSpace.z > far1 && v1ClipSpace.z > far1 && rand(index) > 0.5) {
        return;
    }
    const float far2 = 0.98;
    if (v0ClipSpace.z > far2 && v1ClipSpace.z > far2 && rand(index) > 0.2) {
        return;
    }
    const float far3 = 0.99;
    if (v0ClipSpace.z > far3 && v1ClipSpace.z > far3 && rand(index) > 0.1) {
        return;
    }*/

    // Apply forces {
    //  Gravities
    vec3 gE = vec3(0, -0.98, 0);
    vec3 widthDir = vec3(cos(orientation), 0, sin(orientation));
    vec3 bladeFace = normalize(cross(up, widthDir));
    vec3 gF = 0.25*length(gE)*bladeFace;
    vec3 g = gE + gF;

    //  Recovery
    vec3 r = (v0 + up * height - v2) * stiffness;

    //  Wind
    vec3 windForce = 0.25 * wind_magnitude *
    vec3(
    sin(current_time * 3. / wind_wave_period + v0.x * 0.1 * 11 / wind_wave_length),
    0,
    sin(current_time * 3. / wind_wave_period + v0.z * 0.2 * 11 / wind_wave_length) * 0.1
    );
    float fd = 1 - abs(dot(normalize(windForce), normalize(v2 - v0)));
    float fr = dot((v2 - v0), up) / height;
    vec3 w = windForce * fd * fr;

    //TODO: looks like that's because we "reset" by using start data from inputBlades
//    v2 = vec3(0.0f);
//    v2 += w * delta_time * 200;
//    v2 += vec3(4.0f, 4.0f, 4.0f) * delta_time;
    v2 += (0.1 * g + r + w) * delta_time * 200;

    float lproj = length(v2 - v0 - up * dot((v2-v0), up));
    v1 = v0 + height*up*max(1-lproj/height, 0.05*max(lproj/height, 1));


    // _--- _---_ ----
    outputBlades[index] = inputBlades[index];
    outputBlades[index].v1.xyz = v1;
    outputBlades[index].v2.xyz = v2;

    vs_out.v1 = outputBlades[index].v1;
    vs_out.v2 = outputBlades[index].v2;
    vs_out.up = vec4((outputBlades[index].up.xyz), outputBlades[index].up.w);

    float angle = outputBlades[index].v0.w;

    vec3 dir = normalize(cross(vs_out.up.xyz,
    vec3(sin(angle),
    0,
    cos(angle))));

    vs_out.dir = vec4(dir, 0.0);

    gl_Position = outputBlades[index].v0;
}
