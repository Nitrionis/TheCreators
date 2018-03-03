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
		std::vector<const char*> bloorHorShaderNames = {
			"Shaders\\BloorHorizontal\\vert.spv",
			"Shaders\\BloorHorizontal\\frag.spv"
		};
		std::vector<VkShaderStageFlagBits> bloorHorShaderUsage = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT
		};
	private:
		void InitExtensions();

	} settings;

	class VulkanSharedDate {
	public:
		vk::Instance    instance;
		vk::Debug       debug;

		vk::Device      device;
		vk::SwapChain   swapChain;

		vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;

		vk::CommandQueue                commandQueue    = VK_NULL_HANDLE;
		vk::UniqueCommandPool           commandPool     {device, vkDestroyCommandPool};
		std::vector<vk::CommandBuffer>  commandBuffers;

		std::vector<vk::UniqueFramebuffer> framebuffers;

		vk::RenderPass renderPass{device};

		vk::UniqueSemaphore imageAvailableSemaphore{device, vkDestroySemaphore};
		vk::UniqueSemaphore renderFinishedSemaphore{device, vkDestroySemaphore};

		vk::UniqueHandle<VkDescriptorPool> descriptorPool{device, vkDestroyDescriptorPool};

		struct {
			vk::Image intermediate[2];
		}image;

		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

		void Initialize(VulkanObjectsSettings& settings);
		void ShowIntermediateImage();
	private:
		void PickPhysicalDevice();

		void CreateDescriptorPool(); // TODO
		void CreateSemaphores();
		void CreateRenderPass();
		void CreateIntermediateImages();
		void CreateFrameBuffers();
	} vulkan;

	class Chunks {
	private:
		VulkanSharedDate& vulkan;
		VulkanObjectsSettings& settings;
	public:
		Chunks(VulkanSharedDate& vko, VulkanObjectsSettings& settings)
			: vulkan(vko), settings(settings) {}

		struct {
			vk::Material ground;
		}material;

		struct {
			vk::Image atlas;
		}image;

		vk::UniqueHandle<VkSampler> sampler{vulkan.device, vkDestroySampler};

		vk::UniqueHandle<VkDescriptorSetLayout> descriptorSetLayout{vulkan.device, vkDestroyDescriptorSetLayout};

		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

		/*struct {
			vk::Buffer indices;
			vk::Buffer vertices;
		}buffer;*/

		void Initialize();
	private:
		void CreateMaterialGround();
		void CreateAtlasImage();
		void CreateAtlasImageView();
		void CreateSamplers();
		void CreateDescriptors();

	} chunks{vulkan, settings};

	class Bloor {
	private:
		VulkanSharedDate& vulkan;
		VulkanObjectsSettings& settings;
	public:
		Bloor(VulkanSharedDate& vko, VulkanObjectsSettings& settings)
			: vulkan(vko), settings(settings) {}

		struct {
			vk::Material horizontal;
			vk::Material vertical;
		}material;

		vk::UniqueHandle<VkSampler> samplerHor{vulkan.device, vkDestroySampler};
		vk::UniqueHandle<VkSampler> samplerVer{vulkan.device, vkDestroySampler};

		vk::UniqueHandle<VkDescriptorSetLayout> descSetLayoutHor{vulkan.device, vkDestroyDescriptorSetLayout};
		vk::UniqueHandle<VkDescriptorSetLayout> descSetLayoutVer{vulkan.device, vkDestroyDescriptorSetLayout};

		VkDescriptorSet descSetHor = VK_NULL_HANDLE;
		VkDescriptorSet descSetVer = VK_NULL_HANDLE;

		void Initialize();
	private:
		void CreateDescriptorSet();
		void CreateMaterials();

	} bloor{vulkan, settings};

	class UserInterface {
	private:
		VulkanSharedDate& vulkan;
		VulkanObjectsSettings& settings;
	public:


		UserInterface(VulkanSharedDate& vko, VulkanObjectsSettings& settings)
			: vulkan(vko), settings(settings) {}

	private:

	} ui{vulkan, settings};
};