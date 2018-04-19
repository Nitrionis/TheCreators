#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} ubo;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in uint position_data;
layout(location = 0) out vec2 uv_0;

const vec3 positions[4] = vec3[](
    vec3(-1.0,-1.0, 0.0),
    vec3(-1.0, 1.0, 0.0),
    vec3( 1.0, 1.0, 0.0),
    vec3( 1.0,-1.0, 0.0)
);
const vec2 coords[4] = vec2[](
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0)
);

void main() {
    gl_Position = ubo.mvp * vec4(positions[position_data], 1.0f);
    uv_0 = coords[position_data];
}