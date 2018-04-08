#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 uv_0;
layout(location = 1) in float blurDir_f;

layout(location = 0) out vec4 out_color_0;

const vec2 pixelSize[2] = {vec2(1.0f / 1920.0f, 0.0f), vec2(0.0f, 1.0f / 1080.0f)};

const float coeffs[6] = {0.001654,0.010128,0.041428,0.113248,0.207,0.253085};

void main() {
    uint blurDir = uint(blurDir_f);
    out_color_0 =
          texture(texSampler, uv_0 - pixelSize[blurDir] * 5) * coeffs[0]
        + texture(texSampler, uv_0 - pixelSize[blurDir] * 4) * coeffs[1]
        + texture(texSampler, uv_0 - pixelSize[blurDir] * 3) * coeffs[2]
        + texture(texSampler, uv_0 - pixelSize[blurDir] * 2) * coeffs[3]
        + texture(texSampler, uv_0 - pixelSize[blurDir]) * coeffs[4]
        + texture(texSampler, uv_0) * coeffs[5]
        + texture(texSampler, uv_0 + pixelSize[blurDir]) * coeffs[4]
        + texture(texSampler, uv_0 + pixelSize[blurDir] * 2) * coeffs[3]
        + texture(texSampler, uv_0 + pixelSize[blurDir] * 3) * coeffs[2]
        + texture(texSampler, uv_0 + pixelSize[blurDir] * 4) * coeffs[1]
        + texture(texSampler, uv_0 + pixelSize[blurDir] * 5) * coeffs[0];
}