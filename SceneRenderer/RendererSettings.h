#pragma once

#include "CustomVulkanTypes.hpp"

class RendererSettings {
private:
	RendererSettings() { InitExtensions(); }

public:
	~RendererSettings(){}

	RendererSettings(RendererSettings const&) = delete;
	RendererSettings& operator= (RendererSettings const&) = delete;

	static RendererSettings& Instance() {
		static RendererSettings s;
		return s;
	}

	std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};
	std::vector<const char*> instanceExtensions = {
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_KHR_SURFACE_EXTENSION_NAME
	};
	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkPhysicalDeviceFeatures deviceFeatures = {
		//.geometryShader = VK_TRUE,
		//.multiDrawIndirect = VK_TRUE
	};
	std::vector<const char*> chunkShaderNames = {
		"Shaders\\Chunk\\vert.spv",
		"Shaders\\Chunk\\frag.spv"
	};
	std::vector<VkShaderStageFlagBits> chunkShaderUsage = {
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};
	/*std::vector<const char*> blurHorShaderNames = {
		"Shaders\\BlurHorizontal\\vert.spv",
		"Shaders\\BlurHorizontal\\frag.spv"
	};
	std::vector<VkShaderStageFlagBits> blurHorShaderUsage = {
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};
	std::vector<const char*> blurVerShaderNames = {
		"Shaders\\BlurVertical\\vert.spv",
		"Shaders\\BlurVertical\\frag.spv"
	};
	std::vector<VkShaderStageFlagBits> blurVerShaderUsage = {
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};*/
	std::vector<const char*> blurComShaderNames = {
		"Shaders\\BlurCompression\\vert.spv",
		"Shaders\\BlurCompression\\frag.spv"
	};
	std::vector<VkShaderStageFlagBits> blurComShaderUsage = {
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};
	std::vector<const char*> blurShaderNames = {
		"Shaders\\Blur\\vert.spv",
		"Shaders\\Blur\\frag.spv"
	};
	std::vector<VkShaderStageFlagBits> blurShaderUsage = {
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};
	std::vector<const char*> blurDecomShaderNames = {
		"Shaders\\BlurDecompression\\vert.spv",
		"Shaders\\BlurDecompression\\frag.spv"
	};
	std::vector<VkShaderStageFlagBits> blurDecomShaderUsage = {
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};
	std::vector<const char*> uiShaderNames = {
		"Shaders\\UserInterface\\vert.spv",
		"Shaders\\UserInterface\\frag.spv"
	};
	std::vector<VkShaderStageFlagBits> uiShaderUsage = {
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};
private:
	void InitExtensions();

};