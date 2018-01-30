#pragma once

#include "Window.h"
#include "VulkanSharedHaders.h"

namespace vk
{
    class Device;

    class SwapChain {
        friend class Device;

    private:
	    vk::Window       window;

	    VkSwapchainKHR   swapChain      = VK_NULL_HANDLE;
	    VkSurfaceKHR     surface        = VK_NULL_HANDLE;

	    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	    VkDevice         logicalDevice  = VK_NULL_HANDLE;
	    VkInstance       instance       = VK_NULL_HANDLE;

    public:
	    std::vector<VkImage>     images;
	    std::vector<VkImageView> views;

	    VkExtent2D       extent;
	    VkPresentModeKHR presentMode;
	    VkColorSpaceKHR  colorSpace;
	    VkFormat         colorFormat;
	    uint32_t         imageCount;

	    uint32_t currImageIndex;

        SwapChain();

        ~SwapChain();

	    void Destroy();

        void Setup();

	    VkResult AcquireNext(VkSemaphore waitSemaphore);

	    VkResult Present(VkQueue queue, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

    private:
        // Create the os-specific surface
        VkResult CreateSurface(
	        VkInstance       instance,
	        VkPhysicalDevice physicalDevice
        );
    };
}
