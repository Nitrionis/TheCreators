#pragma once

#include "Vulkan/Window/Window.h"
#include "Vulkan/Other/VulkanSharedHaders.h"

namespace vk
{
    class Device;

    class SwapChain
    {
        friend class Device;

    private:
	    VkSwapchainKHR   swapChain      = VK_NULL_HANDLE;
	    VkSurfaceKHR     surface        = VK_NULL_HANDLE;

	    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	    VkDevice         logicalDevice  = VK_NULL_HANDLE;
	    VkInstance       instance       = VK_NULL_HANDLE;

    public:
	    vk::Window       window;

	    std::vector<VkImage>     images;
	    std::vector<VkImageView> views;

	    VkExtent2D       extent;
	    VkPresentModeKHR presentMode;
	    VkColorSpaceKHR  colorSpace;
	    VkFormat         colorFormat;
	    uint32_t         imageCount;

	    uint32_t currImageIndex;

	    template <typename T>
	    T& GetComponent();

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
