#include "SwapChain.h"

vk::SwapChain::SwapChain() {

}

vk::SwapChain::~SwapChain() {
    Destroy();
}

void vk::SwapChain::Destroy() {
	if (swapChain != VK_NULL_HANDLE) {
		for (uint32_t i = 0; i < imageCount; i++) {
			if (views[i] != VK_NULL_HANDLE)
				vkDestroyImageView(logicalDevice, views[i], nullptr);
		}
	}
	if (swapChain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	}
	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}
}

void vk::SwapChain::Setup() {
	// Get list of supported surface formats
	uint32_t formatCount;
	VK_THROW_IF_FAILED(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL));
	assert(formatCount > 0);

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
	VK_THROW_IF_FAILED(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data()));

	// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
	// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
	if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
		colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		colorSpace = surfaceFormats[0].colorSpace;
	}
	else {
		// iterate over the list of available surface format and
		// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
		bool found_B8G8R8A8_UNORM = false;
		for (auto&& surfaceFormat : surfaceFormats) {
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
				colorFormat = surfaceFormat.format;
				colorSpace = surfaceFormat.colorSpace;
				found_B8G8R8A8_UNORM = true;
				break;
			}
		}
		// in case VK_FORMAT_B8G8R8A8_UNORM is not available
		// select the first available color format
		if (!found_B8G8R8A8_UNORM) {
			colorFormat = surfaceFormats[0].format;
			colorSpace = surfaceFormats[0].colorSpace;
		}
	}

	// Get physical device surface properties and formats
	VkSurfaceCapabilitiesKHR surfCaps;
	VK_THROW_IF_FAILED(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps));

	// Get available present modes
	uint32_t presentModeCount;
	VK_THROW_IF_FAILED(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL));
	assert(presentModeCount > 0);

	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
	VK_THROW_IF_FAILED(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data()));


	extent = {};
	// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
	if (surfCaps.currentExtent.width == (uint32_t)-1) {
		extent.width = window.width;
		extent.height = window.height;
	}
	else {
		extent = surfCaps.currentExtent;
		window.width = surfCaps.currentExtent.width;
		window.height = surfCaps.currentExtent.height;
	}


	presentMode = VK_PRESENT_MODE_FIFO_KHR;

	for (size_t i = 0; i < presentModeCount; i++) {
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((presentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
			presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}


	uint32_t desiredImagesCount = surfCaps.minImageCount + 1;
	if ((surfCaps.maxImageCount > 0) && (desiredImagesCount > surfCaps.maxImageCount)) {
		desiredImagesCount = surfCaps.maxImageCount;
	}

	// Find the transformation of the surface
	VkSurfaceTransformFlagsKHR preTransform;
	if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		// We prefer a non-rotated transform
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		preTransform = surfCaps.currentTransform;
	}
	// Find a supported composite alpha format (not all devices support alpha opaque)
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	// Simply select the first composite alpha format available
	std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
	for (auto& flag : compositeAlphaFlags) {
		if (surfCaps.supportedCompositeAlpha & flag) {
			compositeAlpha = flag;
			break;
		};
	}

	VkSwapchainCreateInfoKHR swapchainCI = {};
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.pNext = NULL;
	swapchainCI.surface = surface;
	swapchainCI.minImageCount = desiredImagesCount;
	swapchainCI.imageFormat = colorFormat;
	swapchainCI.imageColorSpace = colorSpace;
	swapchainCI.imageExtent = { extent.width, extent.height };
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.presentMode = presentMode;
	swapchainCI.compositeAlpha = compositeAlpha;
	swapchainCI.clipped = VK_TRUE;
	swapchainCI.oldSwapchain = VK_NULL_HANDLE;
	// WARNING ONLY EXCLUSIVE SHARING MODE
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.pQueueFamilyIndices = NULL;
	VK_THROW_IF_FAILED(vkCreateSwapchainKHR(logicalDevice, &swapchainCI, nullptr, &swapChain));
	std::cout << "SwapChain WARNING ONLY EXCLUSIVE SHARING MODE\n";

	VK_THROW_IF_FAILED(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr));
	images.resize(imageCount);
	VK_THROW_IF_FAILED(vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, images.data()));

	views.resize(imageCount);
	VkImageViewCreateInfo imageViewCI = {};
	imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCI.pNext = NULL;
	imageViewCI.flags = 0;
	imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCI.format = colorFormat;
	imageViewCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCI.subresourceRange.baseMipLevel = 0;
	imageViewCI.subresourceRange.levelCount = 1;
	imageViewCI.subresourceRange.baseArrayLayer = 0;
	imageViewCI.subresourceRange.layerCount = 1;
	for (uint32_t i = 0; i < imageCount; i++) {
		imageViewCI.image = images[i];
		VK_THROW_IF_FAILED(vkCreateImageView(logicalDevice, &imageViewCI, NULL, &views[i]));
	}

}

// Create the os-specific surface
VkResult vk::SwapChain::CreateSurface(
	VkInstance       instance,
	VkPhysicalDevice physicalDevice)
{
	this->instance = instance;
	this->physicalDevice = physicalDevice;

	window.Setup("TheCreators", true, 800, 600);

    VkResult res;

#if defined(TARGET_PLATFORM_WIN32)
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = window.hinstance;
    surfaceCreateInfo.hwnd = window.hwnd;
    res = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);

#elif defined(TARGET_PLATFORM_MACOS)
	VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
	surfaceCreateInfo.pNext = NULL;
	surfaceCreateInfo.flags = 0;
	surfaceCreateInfo.pView = view;
	res = vkCreateMacOSSurfaceMVK(instance, &surfaceCreateInfo, NULL, &surface);

#elif defined(TARGET_PLATFORM_DIRECT2DISPLAY)
	createDirect2DisplaySurface(width, height);

#elif defined(TARGET_PLATFORM_WAYLAND)
	VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.display = display;
	surfaceCreateInfo.surface = hwnd;
	res = vkCreateWaylandSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);

#elif defined(TARGET_PLATFORM_PLATFORM_XCB)
	VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.connection = connection;
	surfaceCreateInfo.hwnd = hwnd;
	res = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
#endif
    return res;
}

VkResult vk::SwapChain::AcquireNext(VkSemaphore waitSemaphore)
{
	return vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, waitSemaphore, (VkFence)nullptr, &currImageIndex);
}

VkResult vk::SwapChain::Present(VkQueue queue, VkSemaphore waitSemaphore)
{
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain;
	presentInfo.pImageIndices = &currImageIndex;
	// Check if a wait semaphore has been specified to wait for before presenting the textureImage
	if (waitSemaphore != VK_NULL_HANDLE)
	{
		presentInfo.pWaitSemaphores = &waitSemaphore;
		presentInfo.waitSemaphoreCount = 1;
	}
	return vkQueuePresentKHR(queue, &presentInfo);
}