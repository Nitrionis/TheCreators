#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 uv_0;

layout(location = 0) out vec4 out_color_0;

const vec2 pixelSize = vec2(1.0f / 1920.0f, 0.0f);

void main() {
    out_color_0 =
          texture(texSampler, uv_0 - pixelSize * 3) * 0.00598
        + texture(texSampler, uv_0 - pixelSize * 2) * 0.060626
        + texture(texSampler, uv_0 - pixelSize) * 0.241843
        + texture(texSampler, uv_0) * 0.383103
        + texture(texSampler, uv_0 + pixelSize) * 0.241843
        + texture(texSampler, uv_0 + pixelSize * 2) * 0.060626
        + texture(texSampler, uv_0 + pixelSize * 3) * 0.00598;
}