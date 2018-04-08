#pragma once

#include "VulkanReference.h"

class InterfaceRenderer : virtual public VulkanReference {
private:

	const uint32_t sourceImage = 0;

	struct {
		vk::RenderPass final{vulkan.device};
	}renderPass;

	struct {
		vk::Material final;
	}material;

	struct {
		std::vector<vk::UniqueFramebuffer> final;
	}framebuffer;

	struct {
		VkDescriptorSet final = VK_NULL_HANDLE;
	}descSet;

	struct {
		vk::UniqueHandle<VkDescriptorSetLayout> final{vulkan.device, vkDestroyDescriptorSetLayout};
	}descSetLayout;

	vk::UniqueHandle<VkSampler> sampler{vulkan.device, vkDestroySampler};

public:
	void AddToCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t bufferIndex);
	void Initialize();

private:
	void CreateRenderPasses();
	void CreateFramebuffers();
	void CreateSamplers();
	void CreateDescriptorSet();
	void CreateMaterials();
};


