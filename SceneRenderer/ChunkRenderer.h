#pragma once

#include "VulkanReference.h"

class ChunkRenderer : virtual public VulkanReference {
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

	class Mesh {
	public:
		struct {
			vk::Buffer indices;
			vk::Buffer vertices;
			vk::Buffer uniform;
			vk::Buffer staging;
		}buffer;

		vk::shared_array<uint32_t> vertices = nullptr;
		vk::shared_array<uint16_t> indices = nullptr;

		//void UpdateBlock(Vector3<> pos, uint32_t value);

		//void UpdateAll();

		void Initialize();
	private:
		void CreateBuffers();
		void CreateDate();
	}mesh;

public:
	void AddToCommandBuffer(vk::CommandBuffer commandBuffer);
	void Initialize();

private:
	void CreateRenderPasses();
	void CreateFramebuffers();
	void CreateMaterialGround();
	void CreateAtlasImage();
	void CreateAtlasImageView();
	void CreateSamplers();
	void CreateDescriptorSet();
};