#pragma once
#include "StandardBehavior.h"

enum class Stage : int {
	DrawChunks  = 0,
	BloorHor   = 1,
	BloorVer   = 2,
	Three = 3
};

class SceneRenderer : public BaseCode
{
public:
    SceneRenderer();
    ~SceneRenderer();

private:
	void DrawScene();
	void CreateCommandBuffers();

public:
	static class VulkanObjectsSettings {
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
		std::vector<const char*> bloorVerShaderNames = {
			"Shaders\\BloorVertical\\vert.spv",
			"Shaders\\BloorVertical\\frag.spv"
		};
		std::vector<VkShaderStageFlagBits> bloorVerShaderUsage = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT
		};
	private:
		void InitExtensions();

	} settings;

	static class VulkanSharedDate {
	public:
		vk::Instance    instance;
		vk::Debug       debug;

		vk::Device      device;
		vk::SwapChain   swapChain;

		vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;

		vk::CommandQueue                commandQueue    = VK_NULL_HANDLE;
		vk::UniqueCommandPool           commandPool     {device, vkDestroyCommandPool};
		std::vector<vk::CommandBuffer>  commandBuffers;

		vk::UniqueSemaphore imageAvailableSemaphore{device, vkDestroySemaphore};
		vk::UniqueSemaphore renderFinishedSemaphore{device, vkDestroySemaphore};

		vk::UniqueHandle<VkDescriptorPool> descriptorPool{device, vkDestroyDescriptorPool};

		struct {
			vk::Image intermediate[2];
		}image;

		//vk::RenderPass finalPass{vulkan.device};

		//std::vector<vk::UniqueFramebuffer> framebuffers;

		void Initialize(VulkanObjectsSettings& settings);
		void ShowIntermediateImage();
	private:
		void PickPhysicalDevice();

		void CreateDescriptorPool();
		void CreateSemaphores();
		void CreateIntermediateImages();
		// TODO is disable because
		//void CreateRenderPasses();
		//void CreateFramebuffers();

	} vulkan;

	class Chunks {
	public:
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

		void Initialize();
	private:
		void CreateRenderPasses();
		void CreateFramebuffers();
		void CreateMaterialGround();
		void CreateAtlasImage();
		void CreateAtlasImageView();
		void CreateSamplers();
		void CreateDescriptorSet();

	} chunks;

	class Bloor {
	public:
		struct {
			vk::Material horizontal;
			vk::Material vertical;
		}material;

		struct {
			vk::RenderPass horizontal{vulkan.device};
			vk::RenderPass vertical{vulkan.device};
		}renderPass;

		struct {
			vk::UniqueFramebuffer horizontal{vulkan.device, vkDestroyFramebuffer};
			vk::UniqueFramebuffer vertical{vulkan.device, vkDestroyFramebuffer};
			std::vector<vk::UniqueFramebuffer> final;
		}framebuffer;

		vk::UniqueHandle<VkSampler> sampler{vulkan.device, vkDestroySampler};

		vk::UniqueHandle<VkDescriptorSetLayout> descSetLayoutHor{vulkan.device, vkDestroyDescriptorSetLayout};
		vk::UniqueHandle<VkDescriptorSetLayout> descSetLayoutVer{vulkan.device, vkDestroyDescriptorSetLayout};

		VkDescriptorSet descSetHor = VK_NULL_HANDLE;
		VkDescriptorSet descSetVer = VK_NULL_HANDLE;

		void Initialize();
	private:
		void CreateRenderPasses();
		void CreateFramebuffers();
		void CreateSamplers();
		void CreateDescriptorSet();
		void CreateMaterials();

	} bloor;

	class UserInterface {
	private:

	} ui;
};