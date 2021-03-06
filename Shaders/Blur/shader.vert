#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec2 uv_0;
layout(location = 1) out float blurDir;

const vec2 positions[6] = vec2[](
    vec2(-1.0,-1.0),
    vec2(-1.0, 1.0),
    vec2( 1.0, 1.0),
    vec2(-1.0,-1.0),
    vec2( 1.0, 1.0),
    vec2( 1.0,-1.0)
);
const vec2 coords[6] = vec2[](
    vec2(0.0000, 0.0000),
    vec2(0.0000, 0.0625),
    vec2(0.0625, 0.0625),
    vec2(0.0000, 0.0000),
    vec2(0.0625, 0.0625),
    vec2(0.0625, 0.0000)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    uv_0 = coords[gl_VertexIndex];
    blurDir = gl_InstanceIndex;
}