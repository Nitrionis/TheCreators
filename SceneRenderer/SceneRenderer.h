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
		std::vector<const char*> blurHorShaderNames = {
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
		};
		std::vector<const char*> blurComShaderNames = {
			"Shaders\\BlurCompression\\vert.spv",
			"Shaders\\BlurCompression\\frag.spv"
		};
		std::vector<VkShaderStageFlagBits> blurComShaderUsage = {
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
			vk::Image intermediate[3];
		}image;

		void Initialize(VulkanObjectsSettings& settings);
		void ShowIntermediateImage(uint32_t index);
	private:
		void PickPhysicalDevice();

		void CreateDescriptorPool();
		void CreateSemaphores();
		void CreateIntermediateImages();

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

		class Mesh {
		public:
			struct {
				vk::Buffer indices;
				vk::Buffer vertices;
				struct {
					vk::Buffer indices;
					vk::Buffer vertices;
				}staging;
			}buffer;

			vk::shared_array<uint32_t> vertices = nullptr;
			vk::shared_array<uint16_t> indices = nullptr;

			void UpdateBlock(Vector3<> pos, uint32_t value);

			void UpdateAll();

			void Initialize();
		private:
			void CreateBuffers();
			void CreateDate();
		}mesh;

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

	} chunks;

	class Blur {
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

	} blur;

	class UserInterface {
	public:
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

		void AddToCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t bufferIndex);

		void Initialize();
	private:
		void CreateRenderPasses();
		void CreateFramebuffers();
		void CreateSamplers();
		void CreateDescriptorSet();
		void CreateMaterials();
	} ui;
};