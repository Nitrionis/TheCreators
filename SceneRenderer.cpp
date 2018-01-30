#include "SceneRenderer.h"
#include <chrono>
#include <fstream>

SceneRenderer::SceneRenderer() {
    try
    {
        InitVulkan();
    }
    catch (std::runtime_error e)
    {
        std::cout << std::endl << e.what() << std::endl;
    }
	DrawScene();

	const int drawCount = 10;

	auto startTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 10; i++)
		DrawScene();
	auto endTime = std::chrono::high_resolution_clock::now();

	auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << "Draw Count: " << drawCount << "Time: " << dtime.count() << "ms" << std::endl;
}

SceneRenderer::~SceneRenderer()
{
	vkDeviceWaitIdle(device);
}

void SceneRenderer::InitExtensions()
{
#ifdef _WIN32
	settings.instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_IOS_MVK)
    settings.instanceExtensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
    settings.instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    settings.instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#else
    settings.instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
}

void SceneRenderer::InitVulkan()
{
    InitExtensions();

    VK_THROW_IF_FAILED(instance.Setup(
        settings.instanceExtensions,
        settings.validationLayers
    ));

    VK_THROW_IF_FAILED(debug.Setup(
        instance,
        VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT
    ));

	if (VK_NULL_HANDLE == (physicalDevice = PickPhysicalDevice(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)))
	{
		throw std::runtime_error("Physical device not found!");
	}

	VK_THROW_IF_FAILED(device.Setup(
		physicalDevice,
		settings.deviceFeatures,
		settings.deviceExtensions,
		VK_QUEUE_GRAPHICS_BIT,
		instance,
		&swapChain
	));

	swapChain.Setup();

	CreateRenderPass();

	CreateMaterials();

	CreateFrameBuffers();

	CreateCommandBuffers();

	CreateSemaphores();
}

VkPhysicalDevice SceneRenderer::PickPhysicalDevice(VkPhysicalDeviceType deviceType) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (auto device : devices)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		if (deviceProperties.deviceType == deviceType)
		{
			std::cout << "\nSelected GPU: " << deviceProperties.deviceName << std::endl;
			return device;
		}
	}

	return VK_NULL_HANDLE;
}

void SceneRenderer::DrawScene() {
	auto startTime = std::chrono::high_resolution_clock::now();

	vkQueueWaitIdle(commandQueue);

	swapChain.AcquireNext(imageAvailableSemaphore);
	uint32_t i = swapChain.currImageIndex;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &imageAvailableSemaphore,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffers[i],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &renderFinishedSemaphore
	};

	if (vkQueueSubmit(commandQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}
	swapChain.Present(commandQueue, renderFinishedSemaphore);

	auto endTime = std::chrono::high_resolution_clock::now();
	auto dtime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
	std::cout << dtime.count() << std::endl;
}

void SceneRenderer::CreateMaterials() {
	vk::MaterialBehavior behavior = {};

	behavior.viewports[0] = vk::initialize::viewportDefault(swapChain.extent);
	behavior.scissors[0] = vk::initialize::scissorDefault(swapChain.extent);

	chunkMaterial.Setup(
		device,
		renderPass,
		behavior,
		settings.chunkShaderNames,
		settings.chunkShaderUsage,
		0
	);
}

std::vector<char> SceneRenderer::LoadFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

void SceneRenderer::CreateCommandBuffers() {
	commandQueue = device.GetQueue(device.indices.graphicsFamily);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = device.indices.graphicsFamily;
	poolInfo.flags = 0; // Optional

	if (vkCreateCommandPool(device, &poolInfo, nullptr, commandPool.replace()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}

	commandBuffers.resize(swapChain.imageCount);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChain.extent;

		VkClearValue clearColor = {0.1f, 0.1f, 0.1f * i, 1.0f};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, chunkMaterial);

		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
}

void SceneRenderer::CreateFrameBuffers() {
	swapChainFramebuffers.resize(swapChain.imageCount, vk::UniqueFramebuffer{device, vkDestroyFramebuffer});
	for (size_t i = 0; i < swapChain.imageCount; i++)
	{
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &swapChain.views[i];
		framebufferInfo.width = swapChain.extent.width;
		framebufferInfo.height = swapChain.extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, swapChainFramebuffers[i].replace()) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void SceneRenderer::CreateRenderPass() {
	renderPass.Setup(device);
	renderPass.colorAttachments[0].format = swapChain.colorFormat;
}

void SceneRenderer::CreateSemaphores() {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, imageAvailableSemaphore.replace()) != VK_SUCCESS ||
	    vkCreateSemaphore(device, &semaphoreInfo, nullptr, renderFinishedSemaphore.replace()) != VK_SUCCESS) {

		throw std::runtime_error("failed to create semaphores!");
	}
}
