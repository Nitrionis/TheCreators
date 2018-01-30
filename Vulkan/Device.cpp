#include "Device.h"

vk::Device::Device() {

}

vk::Device::~Device() {
    vkDeviceWaitIdle(logicalDevice);
    if (logicalDevice)
        vkDestroyDevice(logicalDevice, nullptr);
}

VkResult vk::Device::Setup(
        VkPhysicalDevice          physicalDevice,
        VkPhysicalDeviceFeatures  enabledFeatures,
        std::vector<const char*>  enabledExtensions,
        VkQueueFlags              requestedQueueTypes,
        VkInstance                instance,
        vk::SwapChain*            swapChain)
{
	assert(physicalDevice != VK_NULL_HANDLE);

	if ((instance != VK_NULL_HANDLE) && (swapChain != nullptr))
	{
		swapChain->CreateSurface(instance, physicalDevice);
	}

    if ((this->physicalDevice == VK_NULL_HANDLE) || (this->physicalDevice != physicalDevice))
    {
        this->physicalDevice = physicalDevice;

        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        uint32_t extCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
        if (extCount > 0)
        {
            std::vector<VkExtensionProperties> extensions(extCount);
            if (VK_SUCCESS == vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, &extensions.front()))
            {
                for (auto ext : extensions)
                {
                    supportedExtensions.push_back(ext.extensionName);
                }
            }
        }
        for (auto ext : enabledExtensions)
        {
            if (!ExtensionSupported(ext))
            {
                throw std::runtime_error("Not all device extensions are supported!");
            }
        }

        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        assert(queueFamilyCount > 0);
        queueFamilyProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
    }
    // if "surface != nullptr" will search queue, that support present
    if (swapChain != nullptr)
    {
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        queueSupportsPresent.resize(queueFamilyCount);

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, swapChain->surface, &queueSupportsPresent[i]);
        }
    }

    if ((swapChain != nullptr)
        && (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
        && (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT))
    {
        uint32_t index;
        if (UINT32_MAX != (index = GetQueueFamilyIndex(
            static_cast<VkQueueFlagBits>(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT), true)))
        {
            indices.graphicsFamily = index;
            indices.computeFamily = index;
            indices.presentFamily = index;
        }
    }
    if ((swapChain != nullptr)
        && (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
        && (indices.graphicsFamily == UINT32_MAX))
    {
        uint32_t index;
        if (UINT32_MAX != (index = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT, true)))
        {
            indices.graphicsFamily = index;
            indices.presentFamily = index;
        }
    }
    if ((swapChain != nullptr)
        && (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
        && (indices.computeFamily == UINT32_MAX))
    {
        uint32_t index;
        if (UINT32_MAX != (index = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT, true)))
        {
            indices.computeFamily = index;
            indices.presentFamily = index;
        }
    }
    if ((requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
         && (indices.graphicsFamily == UINT32_MAX))
    {
        indices.graphicsFamily = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT, false);
    }
    if ((requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
        && (indices.computeFamily == UINT32_MAX))
    {
        indices.computeFamily = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT, false);
    }
    if (indices.presentFamily == UINT32_MAX)
    {
        indices.presentFamily = GetQueueFamilyIndex(VK_QUEUE_FLAG_BITS_MAX_ENUM, true);
    }

    if ((requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT) && (indices.graphicsFamily == UINT32_MAX))
    {
        throw std::runtime_error("Graphics queue family index not found!");
    }
    if ((requestedQueueTypes & VK_QUEUE_COMPUTE_BIT) && (indices.computeFamily == UINT32_MAX))
    {
        throw std::runtime_error("Compute queue family index not found!");
    }
    if ((swapChain != nullptr) && (indices.presentFamily == UINT32_MAX))
    {
        throw std::runtime_error("Present queue family index not found!");
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

    const float defaultQueuePriority(0.0f);
    // Graphics queue
    if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
    {
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = indices.graphicsFamily;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
    }
    // Compute queue
    if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
    {
        if (indices.computeFamily != indices.graphicsFamily)
        {   // If compute family index differs, we need an additional queue create info for the compute queue
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = indices.computeFamily;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    }
    // Present queue
    if (swapChain != nullptr)
    {
        if ((indices.presentFamily != indices.graphicsFamily) && (indices.presentFamily != indices.computeFamily))
        {
	        // If present family index differs, we need an additional queue create info for the present queue
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = indices.presentFamily;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
    deviceCreateInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();;
	VkResult res = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);

	if ((res == VK_SUCCESS) && (swapChain != nullptr))
	{
		swapChain->logicalDevice = logicalDevice;
	}
    return res;
}

uint32_t vk::Device::GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags flags, VkBool32 *memTypeFound) {
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            if ((memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
            {
                if (memTypeFound)
                {
                    *memTypeFound = true;
                }
                return i;
            }
        }
        typeBits >>= 1;
    }

    if (memTypeFound)
    {
        *memTypeFound = false;
        return 0;
    }
    else
    {
        throw std::runtime_error("Could not find a matching memoryProperties type");
    }
}



VkCommandPool vk::Device::CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags) {
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
    cmdPoolInfo.flags = createFlags;
    VkCommandPool cmdPool;
    VK_THROW_IF_FAILED(vkCreateCommandPool(logicalDevice, &cmdPoolInfo, nullptr, &cmdPool));
    return cmdPool;
}

VkCommandBuffer vk::Device::CreateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level, bool begin) {
    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = commandPool;
    cmdBufAllocateInfo.level = level;
    cmdBufAllocateInfo.commandBufferCount = 1;
    cmdBufAllocateInfo.pNext = nullptr;

    VkCommandBuffer cmdBuffer;
    VK_THROW_IF_FAILED(vkAllocateCommandBuffers(logicalDevice, &cmdBufAllocateInfo, &cmdBuffer));

    // If requested, also start recording for the new command buffer
    if (begin)
    {
        VkCommandBufferBeginInfo cmdBufInfo = {};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_THROW_IF_FAILED(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
    }

    return cmdBuffer;
}

VkResult vk::Device::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags, VkDeviceSize size,
                                  VkBuffer *buffer, VkDeviceMemory *memory, void *data)
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.usage = usageFlags;
    bufferCreateInfo.size = size;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VK_THROW_IF_FAILED(vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, buffer));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(logicalDevice, *buffer, &memReqs);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryFlags);
    VK_THROW_IF_FAILED(vkAllocateMemory(logicalDevice, &memAlloc, nullptr, memory));

    // If a pointer to the buffer data has been passed, map the buffer and copy over the data
    if (data != nullptr)
    {
        void *mapped;
        VK_THROW_IF_FAILED(vkMapMemory(logicalDevice, *memory, 0, size, 0, &mapped));
        memcpy(mapped, data, size);
        // If host coherency hasn't been requested, do a manual flush to make writes visible
        if ((memoryFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
        {
            VkMappedMemoryRange mappedRange = {};
            mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory = *memory;
            mappedRange.offset = 0;
            mappedRange.size = size;
            vkFlushMappedMemoryRanges(logicalDevice, 1, &mappedRange);
        }
        vkUnmapMemory(logicalDevice, *memory);
    }

    VK_THROW_IF_FAILED(vkBindBufferMemory(logicalDevice, *buffer, *memory, 0));

    return VK_SUCCESS;
}

VkResult vk::Device::CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags, vk::Buffer *buffer,
                                  VkDeviceSize size, void *data)
{
    buffer->device = logicalDevice;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.usage = usageFlags;
    bufferCreateInfo.size  = size;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VK_THROW_IF_FAILED(vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer->buffer));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(logicalDevice, buffer->buffer, &memReqs);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.allocationSize  = memReqs.size;
    memAlloc.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryFlags);
    VK_THROW_IF_FAILED(vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &buffer->memory));

    buffer->alignment = memReqs.alignment;
    buffer->size = memAlloc.allocationSize;
    buffer->usageFlags = usageFlags;
    buffer->memoryPropertyFlags = memoryFlags;

    // If a pointer to the buffer data has been passed, map the buffer and copy over the data
    if (data != nullptr)
    {
        VK_THROW_IF_FAILED(buffer->Map());
        memcpy(buffer->mapped, data, size);
        buffer->Unmap();
    }

    // Initialize a default descriptor that covers the whole buffer size
    buffer->SetupDescriptor();

    return buffer->Bind();
}

void vk::Device::ExecuteCommandBuffer(VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue, bool free) {
    if (commandBuffer == VK_NULL_HANDLE)
    {
        return;
    }

    VK_THROW_IF_FAILED(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FLAGS_NONE;
    VkFence fence;
    VK_THROW_IF_FAILED(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence));

    // Submit to the queue
    VK_THROW_IF_FAILED(vkQueueSubmit(queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_THROW_IF_FAILED(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

    vkDestroyFence(logicalDevice, fence, nullptr);

    if (free)
    {
        vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
    }
}

void vk::Device::CopyBuffer(vk::Buffer *src, vk::Buffer *dst, VkCommandPool commandPool,  VkQueue queue, VkBufferCopy *copyRegion) {
    assert(dst->size <= src->size);
    assert(src->buffer && src->buffer);
    VkCommandBuffer copyCmdBuffer = CreateCommandBuffer(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
    VkBufferCopy bufferCopy{};
    if (copyRegion == nullptr)
    {
        bufferCopy.size = src->size;
    }
    else
    {
        bufferCopy = *copyRegion;
    }

    vkCmdCopyBuffer(copyCmdBuffer, src->buffer, dst->buffer, 1, &bufferCopy);

    ExecuteCommandBuffer(copyCmdBuffer, commandPool, queue);
}

bool vk::Device::ExtensionSupported(std::string extension) {
    return true;//(std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end());
}

VkQueue vk::Device::GetQueue(uint32_t queueFamilyIndex) {
	VkQueue queue;
	vkGetDeviceQueue(logicalDevice, queueFamilyIndex, 0, &queue);
	return queue;
}

uint32_t vk::Device::GetQueueFamilyIndex(VkQueueFlagBits flags, bool supportPresent)
{
	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
	{
		if (queueFamilyProperties[i].queueFlags & flags)
		{
			if (supportPresent)
			{
				if (queueSupportsPresent[i] == VK_TRUE)
					return i;
			}
			else return i;
		}
	}
	return UINT32_MAX;
}
