#version 460 core

out VS_OUT {
    vec2 tc;
} vs_out;

void main(void) {
    const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
    vec4(0.5, 0.0, -0.5, 1.0),
    vec4(-0.5, 0.0, 0.5, 1.0),
    vec4(0.5, 0.0, 0.5, 1.0));
    int x = gl_InstanceID & 63;
    int y = gl_InstanceID >> 6;
    vec2 offs = vec2(x, y);
    vs_out.tc = (vertices[gl_VertexID].xz + offs + vec2(0.5)) / 64.0;
    gl_Position = vertices[gl_VertexID] + vec4(float(x - 32), 0.0,
    float(y - 32), 0.0);
//    gl_Position = mvp * vertices[gl_VertexID];
//    vs_out.tc = coords[gl_VertexID];
/*

        const vec4 vertices[] = vec4[](vec4(-1.0, 0.0, -1.0, 1.0),
        vec4(1.0, 0.0, -1.0, 1.0),
        vec4(-1.0, 0.0, 1.0, 1.0),
        vec4(1.0, 0.0, 1.0, 1.0));
        const vec2 coords[] = vec2[](
        vec2(0.0, 0.0), vec2(1.0, 0.0),
        vec2(0.0, 1.0), vec2(1.0, 1.0));

        int x = gl_InstanceID & 1023;
        int y = gl_InstanceID >> 10;
        vec2 offs = vec2(x, y);
        vs_out.tc = (coords[gl_VertexID] + offs) / 1024.0;
        gl_Position = mvp * vertices[gl_VertexID] + vec4(float(x - 512), 0.0,
        float(y - 512), 0.0);*/
}
