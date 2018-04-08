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

        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        std::vector<VkBool32>                queueSupportsPresent;
        std::vector<std::string>             supportedExtensions;

    public:
	    VkPhysicalDeviceProperties       properties;
	    VkPhysicalDeviceFeatures         features;
	    VkPhysicalDeviceMemoryProperties memoryProperties;
	    // Default queue for copy operations
	    vk::CommandQueue defaultQueue = VK_NULL_HANDLE;
	    // Default command pool for copy operations
	    vk::UniqueCommandPool defaultPool{&logicalDevice, vkDestroyCommandPool};

        struct {
            uint32_t graphicsFamily = UINT32_MAX;
            uint32_t computeFamily  = UINT32_MAX;
            uint32_t presentFamily  = UINT32_MAX;
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
            bool            free        = true
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
            VkBufferCopy* copyRegion = nullptr,
            VkCommandPool commandPool = VK_NULL_HANDLE,
            VkQueue       commandQueue = VK_NULL_HANDLE
        );

	    void CopyImage(
		    vk::Buffer*        src,
		    vk::Image*         dst,
		    VkBufferImageCopy* copyRegion = nullptr,
		    VkCommandPool      commandPool = VK_NULL_HANDLE,
		    VkQueue            commandQueue = VK_NULL_HANDLE
	    );

	    void CopyImage(
		    vk::Image*    src,
		    vk::Image*    dst,
		    VkImageCopy*  copyRegion = nullptr,
		    VkCommandPool commandPool = VK_NULL_HANDLE,
		    VkQueue       commandQueue = VK_NULL_HANDLE
	    );

	    void CopyImage(
		    VkImage       src,
		    VkImage       dst,
		    VkImageCopy*  copyRegion = nullptr,
		    VkCommandPool commandPool = VK_NULL_HANDLE,
		    VkQueue       commandQueue = VK_NULL_HANDLE
	    );

	    void CreateImage(
		    vk::Image*            image,
		    VkExtent3D            extent,
		    VkDeviceSize          size,
		    void*                 data        = nullptr,
		    VkImageUsageFlags     usage       = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		    VkFormat              format      = VK_FORMAT_R8G8B8A8_UNORM,
		    VkSampleCountFlagBits samples     = VK_SAMPLE_COUNT_1_BIT,
		    VkImageType           imageType   = VK_IMAGE_TYPE_2D,
		    uint32_t              mipLevels   = 1,
		    uint32_t              arrayLayers = 1
	    );

	    void SetImageBarrier(
		    VkImage                  image,
		    VkImageLayout            oldLayout,
		    VkImageLayout            newLayout,
		    VkAccessFlags            srcAccessMask,
	        VkAccessFlags            dstAccessMask,
		    VkPipelineStageFlagBits  srcStage,
		    VkPipelineStageFlagBits  dstStage,
		    uint32_t                 srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
	        uint32_t                 dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		    VkImageSubresourceRange* subresourceRange    = nullptr,
		    VkCommandBuffer          commandBuffer       = VK_NULL_HANDLE
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