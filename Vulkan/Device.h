#pragma once

#include "SwapChain.h"
#include "Buffer.h"

namespace vk
{
    class Device
    {
    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice         logicalDevice  = VK_NULL_HANDLE;

        VkPhysicalDeviceProperties       properties;
        VkPhysicalDeviceFeatures         features;
        VkPhysicalDeviceMemoryProperties memoryProperties;

        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        std::vector<VkBool32>                queueSupportsPresent;
        std::vector<std::string>             supportedExtensions;

    public:
        struct {
            uint32_t graphicsFamily = UINT32_MAX;
            uint32_t computeFamily = UINT32_MAX;
            uint32_t presentFamily = UINT32_MAX;
        } indices;

	    operator VkDevice() const {
		    return logicalDevice;
	    }

	    operator VkDevice*() {
		    return &logicalDevice;
	    }

	    operator const VkDevice* () const {
		    return &logicalDevice;
	    }

        Device();

        ~Device();

        VkResult Setup(
            VkPhysicalDevice          physicalDevice,
            VkPhysicalDeviceFeatures  enabledFeatures,
            std::vector<const char*>  enabledExtensions,
            VkQueueFlags              requestedQueueTypes,
            VkInstance                instance = VK_NULL_HANDLE,
            vk::SwapChain*            swapChain = nullptr
        );

		VkQueue GetQueue(uint32_t queueFamilyIndex);

        VkCommandPool CreateCommandPool(
            uint32_t queueFamilyIndex,
            VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        );

        VkCommandBuffer CreateCommandBuffer(
            VkCommandPool        commandPool,
            VkCommandBufferLevel level,
            bool                 begin = false
        );

        void ExecuteCommandBuffer(
	        VkCommandBuffer commandBuffer,
            VkCommandPool   commandPool,
            VkQueue         queue,
            bool            free = true
        );

        VkResult CreateBuffer(
            VkBufferUsageFlags    usageFlags,
            VkMemoryPropertyFlags memoryFlags,
            VkDeviceSize          size,
            VkBuffer*             buffer,
            VkDeviceMemory*       memory,
            void*                 data = nullptr
        );

        VkResult CreateBuffer(
            VkBufferUsageFlags    usageFlags,
            VkMemoryPropertyFlags memoryFlags,
            vk::Buffer*           buffer,
            VkDeviceSize          size,
            void*                 data = nullptr
        );

        void CopyBuffer(
            vk::Buffer*   src,
            vk::Buffer*   dst,
            VkCommandPool commandPool,
            VkQueue       queue,
            VkBufferCopy* copyRegion = nullptr
        );

        uint32_t GetMemoryType(
            uint32_t              typeBits,
            VkMemoryPropertyFlags flags,
            VkBool32*             memTypeFound = nullptr
        );

        uint32_t GetQueueFamilyIndex(VkQueueFlagBits flags, bool supportPresent = false);

        bool ExtensionSupported(std::string extension);
    };
}