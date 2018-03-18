#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 uv_0;

layout(location = 0) out vec4 out_color_0;

void main() {
    out_color_0 = texture(texSampler, uv_0);
}