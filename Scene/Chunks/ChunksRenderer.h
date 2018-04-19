#pragma once

#include "Scene/SceneRenderer/VulkanSharedData/VulkanReference.h"

class ChunksRenderer : virtual public VulkanReference {
private:
	struct {
		vk::Material ground;
	}material;

	struct {
		vk::Image atlas;
	}image;

	vk::RenderPass renderPass{vulkan.device};

	vk::UniqueFramebuffer framebuffer{vulkan.device, vkDestroyFramebuffer};

	vk::UniqueHandle<VkSampler> sampler{vulkan.device, vkDestroySampler};

	vk::UniqueHandle<VkDescriptorSetLayout> descSetLayout{vulkan.device, vkDestroyDescriptorSetLayout};

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	struct {
		vk::Buffer indices;
		vk::Buffer vertices;
		vk::Buffer uniform;
		vk::Buffer staging;
	}buffer;

	vk::shared_array<uint32_t> vertices = nullptr;
	vk::shared_array<uint16_t> indices = nullptr;

public:
	void AddToCommandBuffer(vk::CommandBuffer commandBuffer);
	void Initialize();

	void UpdateUniformBuffer(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void UpdateVerticesBuffer(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void UpdateIndicesBuffer(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

private:
	void CreateBuffers();
	void CreateDate();
	void CreateRenderPasses();
	void CreateFramebuffers();
	void CreateMaterialGround();
	void CreateAtlasImage();
	void CreateAtlasImageView();
	void CreateSamplers();
	void CreateDescriptorSet();
};