#pragma once

#include "StandardBehavior.h"

class SceneRenderer : public BaseCode
{
public:
    SceneRenderer();
    ~SceneRenderer();

private:
	void DrawScene();

	void CreateSemaphores();
	void CreateRenderPass();
	void CreateMaterials();
	void CreateCommandBuffers();
	void CreateFrameBuffers();
    void InitExtensions();
    void InitVulkan();

	std::vector<char> LoadFile(const std::string& filename);
	VkPhysicalDevice PickPhysicalDevice(VkPhysicalDeviceType deviceType);

public:
	struct {

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

	} settings;

    vk::Instance instance;
    vk::Debug debug;

    vk::Device device;
	vk::SwapChain swapChain;

	vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;


	vk::CommandQueue commandQueue = VK_NULL_HANDLE;
	vk::UniqueCommandPool commandPool{device, vkDestroyCommandPool };
	std::vector<vk::CommandBuffer> commandBuffers;

	std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;

	vk::RenderPass renderPass;
	vk::Material chunkMaterial;

	vk::UniqueHandle<VkSemaphore> imageAvailableSemaphore{device, vkDestroySemaphore};
	vk::UniqueHandle<VkSemaphore> renderFinishedSemaphore{device, vkDestroySemaphore};
};