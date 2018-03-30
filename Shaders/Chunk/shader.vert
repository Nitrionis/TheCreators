#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform UniformBufferObject {
    mat4 mvp;
} ubo;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in uint my_vertex_index;
layout(location = 0) out vec2 uv_0;

const vec2 positions[6] = vec2[](
    vec2(-1.0,-1.0),
    vec2(-1.0, 1.0),
    vec2( 1.0, 1.0),
    vec2(-1.0,-1.0),
    vec2( 1.0, 1.0),
    vec2( 1.0,-1.0)
);
const vec2 coords[6] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
);

void main() {
    gl_Position = vec4(positions[my_vertex_index] * ubo.mvp[0][0], 0.0, 1.0);
    uv_0 = coords[gl_VertexIndex];
}