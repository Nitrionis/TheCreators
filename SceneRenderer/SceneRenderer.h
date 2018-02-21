#pragma once
#include "StandardBehavior.h"

class SceneRenderer : public BaseCode
{
public:
    SceneRenderer();
    ~SceneRenderer();

private:
	void DrawScene();
	void CreateCommandBuffers();

public:
	class VulkanObjectsSettings {
	public:
		void Initialize();

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
	private:
		void InitExtensions();

	} settings;

	class VulkanSharedDate : Noncopyable {
	public:
		vk::Instance instance;
		vk::Debug debug;

		vk::Device device;
		vk::SwapChain swapChain;

		vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;

		vk::CommandQueue commandQueue = VK_NULL_HANDLE;
		vk::UniqueCommandPool commandPool{device, vkDestroyCommandPool};
		std::vector<vk::CommandBuffer> commandBuffers;

		std::vector<vk::UniqueFramebuffer> framebuffers;

		vk::RenderPass renderPass{device};

		vk::UniqueSemaphore imageAvailableSemaphore{device, vkDestroySemaphore};
		vk::UniqueSemaphore renderFinishedSemaphore{device, vkDestroySemaphore};

		vk::Material material;
		vk::Image image;

		void Initialize(VulkanObjectsSettings& settings);
	private:
		void PickPhysicalDevice();

		void CreateSemaphores();
		void CreateRenderPass();
		void CreateFrameBuffers();

	} vulkan;

	class Chunks : Noncopyable {
	private:
		VulkanSharedDate& vulkan;
		VulkanObjectsSettings& settings;
	public:
		vk::UniqueHandle<VkSampler> sampler{vulkan.device, vkDestroySampler};
		vk::UniqueHandle<VkDescriptorSetLayout> descriptorSetLayout{vulkan.device, vkDestroyDescriptorSetLayout};
		vk::UniqueHandle<VkDescriptorPool> descriptorPool{vulkan.device, vkDestroyDescriptorPool};
		VkDescriptorSet descriptorSet;

		Chunks(VulkanSharedDate& vko, VulkanObjectsSettings& settings)
			: vulkan(vko), settings(settings) {}

		void Initialize();
	private:
		void CreateMaterials();
		void CreateImage();
		void CreateTextureImageView();
		void CreateSamplers();
		void CreateDescriptors();

	} chunks{vulkan, settings};
};