#pragma once

#include "VulkanReference.h"

class BlurRenderer : virtual public VulkanReference {
public:
	uint32_t width, height;

	struct {
		vk::Material compression;
		vk::Material horizontal;
		vk::Material vertical;
		vk::Material decompression;
	}material;

	vk::RenderPass renderPass{vulkan.device};

	struct {
		vk::UniqueFramebuffer firstTarget{vulkan.device, vkDestroyFramebuffer};
		vk::UniqueFramebuffer secondTarget{vulkan.device, vkDestroyFramebuffer};
	}framebuffer;

	vk::UniqueHandle<VkSampler> sampler{vulkan.device, vkDestroySampler};

	vk::UniqueHandle<VkDescriptorSetLayout> descSetLayout{vulkan.device, vkDestroyDescriptorSetLayout};

	struct {
		VkDescriptorSet compression = VK_NULL_HANDLE; // from 0 to 1
		VkDescriptorSet horizontal = VK_NULL_HANDLE; // from 1 to 2 | also using for decompression pass
		VkDescriptorSet vertical = VK_NULL_HANDLE; // from 2 to 1
	}descSet;

	void AddToCommandBuffer(vk::CommandBuffer commandBuffer);

	void Initialize();
private:
	void CreateRenderPasses();
	void CreateFramebuffers();
	void CreateSamplers();
	void CreateDescriptorSet();
	void CreateMaterials();

};


