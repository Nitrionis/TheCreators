#include "Vulkan/Device/Device.h"

vk::Device::Device() {

}

vk::Device::~Device() {
	defaultPool = VK_NULL_HANDLE;
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

	    PrintMemoryInfo();
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
		defaultQueue = GetQueue(indices.graphicsFamily);
		defaultPool = CreateCommandPool(indices.graphicsFamily, 0);
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

    // If requested, also start recording for the new command obj
    if (begin)
    {
        VkCommandBufferBeginInfo cmdBufInfo = {};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	    cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
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

    // If a pointer to the obj data has been passed, map the obj and copy over the data
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

	if (vkCreateBuffer(logicalDevice, &bufferCreateInfo, nullptr, &buffer->obj) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create obj!");
	}

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(logicalDevice, buffer->obj, &memReqs);

    VkMemoryAllocateInfo memAlloc = {};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.allocationSize  = memReqs.size;
    memAlloc.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, memoryFlags);

    if (vkAllocateMemory(logicalDevice, &memAlloc, nullptr, &buffer->memory) != VK_SUCCESS) {
	    throw std::runtime_error("Failed to allocate obj memory!");
    }

    buffer->alignment = memReqs.alignment;
    buffer->size = memAlloc.allocationSize;
    buffer->usageFlags = usageFlags;
    buffer->memoryPropertyFlags = memoryFlags;

    if (data != nullptr)
    {
        VK_THROW_IF_FAILED(buffer->Map());
        memcpy(buffer->mapped, data, size);
	    if ((memoryFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
	    {
		    buffer->Flush();
	    }
        buffer->Unmap();
    }
    // Initialize a default descriptor that covers the whole obj size
    buffer->SetupDescriptor();

    return buffer->Bind();
}

void vk::Device::ExecuteCommandBuffer(VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue, bool free) {
    if (commandBuffer == VK_NULL_HANDLE) {
        return;
    }

    VK_THROW_IF_FAILED(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // Create fence to ensure that the command obj has finished executing
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FLAGS_NONE;
    VkFence fence;
    VK_THROW_IF_FAILED(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &fence));

    // Submit to the queue
    VK_THROW_IF_FAILED(vkQueueSubmit(queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command obj has finished executing
    VK_THROW_IF_FAILED(vkWaitForFences(logicalDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

    vkDestroyFence(logicalDevice, fence, nullptr);

    if (free) {
        vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
    }
}

void vk::Device::CopyBuffer(vk::Buffer *src, vk::Buffer *dst, VkBufferCopy *copyRegion, VkCommandPool commandPool, VkQueue commandQueue) {

	if (copyRegion == nullptr) {
		assert(dst->size > src->size);
		assert(src->obj && src->obj);
	}

    VkCommandBuffer copyCmdBuffer = CreateCommandBuffer(
	    commandPool == VK_NULL_HANDLE ? this->defaultPool : commandPool,
	    VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	    true
    );

    VkBufferCopy bufferCopy{};

    if (copyRegion == nullptr) {
        bufferCopy.size = src->size;
    }
    else {
        bufferCopy = *copyRegion;
    }

    vkCmdCopyBuffer(copyCmdBuffer, src->obj, dst->obj, 1, &bufferCopy);

    ExecuteCommandBuffer(
	    copyCmdBuffer,
	    commandPool == VK_NULL_HANDLE ? this->defaultPool : commandPool,
	    commandQueue == VK_NULL_HANDLE ? this->defaultQueue : commandQueue
    );
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

void vk::Device::CreateImage(
	vk::Image            *image,
	VkExtent3D            extent,
	VkDeviceSize          size,
	void*                 data,
	VkImageUsageFlags     usage,
	VkFormat              format,
	VkSampleCountFlagBits samples,
	VkImageType           imageType,
	uint32_t              mipLevels,
	uint32_t              arrayLayers)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.pNext = nullptr;
	imageInfo.flags = VK_FLAGS_NONE;
	imageInfo.imageType             = imageType;
	imageInfo.format                = format;
	imageInfo.extent                = extent;
	imageInfo.mipLevels             = mipLevels;
	imageInfo.arrayLayers           = arrayLayers;
	imageInfo.samples               = samples;
	imageInfo.tiling                = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage                 = usage;
	imageInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 1;
	imageInfo.pQueueFamilyIndices   = &indices.graphicsFamily;
	imageInfo.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &image->obj) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create textureImage!");
	}

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(logicalDevice, image->obj, &memReqs);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &image->memory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate textureImage memory!");
	}

	vkBindImageMemory(logicalDevice, image->obj, image->memory, 0);

	image->memoryPropertyFlags  = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	image->device               = logicalDevice;
	image->usageFlags           = usage;
	image->extent               = extent;
	image->alignment            = memReqs.alignment;
	image->size                 = allocInfo.allocationSize;

	if (data == nullptr)
		return;

	vk::Buffer stagingBuffer;

	CreateBuffer(
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer,
		size,
		data
	);

	SetImageBarrier(image->obj,
		VK_IMAGE_LAYOUT_UNDEFINED,         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0,                                 VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	CopyImage(&stagingBuffer, image);

	SetImageBarrier(image->obj,
	    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	    VK_ACCESS_TRANSFER_WRITE_BIT,         VK_ACCESS_SHADER_READ_BIT,
	    VK_PIPELINE_STAGE_TRANSFER_BIT,       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
	);
}

void vk::Device::CopyImage(vk::Buffer *src, vk::Image *dst, VkBufferImageCopy *copyRegion, VkCommandPool commandPool, VkQueue commandQueue) {

	if (copyRegion == nullptr) {
		assert(dst->size >= src->size);
		assert(dst->obj && src->obj);
	}
	VkCommandBuffer copyCmdBuffer = CreateCommandBuffer(
		commandPool == VK_NULL_HANDLE ? this->defaultPool : commandPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		true
	);
	VkBufferImageCopy region = {};
	if (copyRegion == nullptr) {
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = {0, 0, 0};
		region.imageExtent = dst->extent;
	}
	else {
		region = *copyRegion;
	}
	vkCmdCopyBufferToImage(
		copyCmdBuffer,
		src->obj,
		dst->obj,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);
	ExecuteCommandBuffer(
		copyCmdBuffer,
		commandPool == VK_NULL_HANDLE ? this->defaultPool : commandPool,
		commandQueue == VK_NULL_HANDLE ? this->defaultQueue : commandQueue
	);
}

void vk::Device::SetImageBarrier(
	VkImage                  image,
	VkImageLayout            oldLayout,
	VkImageLayout            newLayout,
	VkAccessFlags            srcAccessMask,
	VkAccessFlags            dstAccessMask,
	VkPipelineStageFlagBits  srcStage,
	VkPipelineStageFlagBits  dstStage,
	uint32_t                 srcQueueFamilyIndex,
	uint32_t                 dstQueueFamilyIndex,
	VkImageSubresourceRange* subresourceRange,
	VkCommandBuffer          commandBuffer)
{
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;

	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;

	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;

	barrier.srcQueueFamilyIndex = srcQueueFamilyIndex;
	barrier.dstQueueFamilyIndex = dstQueueFamilyIndex;

	if (subresourceRange != nullptr) {
		barrier.subresourceRange = *subresourceRange;
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
	}

	if (commandBuffer == VK_NULL_HANDLE) {
		commandBuffer = CreateCommandBuffer(defaultPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		vkCmdPipelineBarrier(
			commandBuffer,
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
		ExecuteCommandBuffer(commandBuffer, defaultPool, defaultQueue);
	}
	else {
		vkCmdPipelineBarrier(
			commandBuffer,
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}
}

void vk::Device::CopyImage(vk::Image *src, vk::Image *dst, VkImageCopy *copyRegion, VkCommandPool commandPool, VkQueue commandQueue) {
	assert(dst->size >= src->size);
	assert(dst->obj && src->obj);

	VkCommandBuffer copyCmdBuffer = CreateCommandBuffer(
		commandPool == VK_NULL_HANDLE ? this->defaultPool : commandPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		true
	);
	VkImageCopy region = {};
	if (copyRegion == nullptr) {
		region.extent = src->extent;
		region.dstOffset = {0, 0, 0};
		region.srcOffset = {0, 0, 0};
		region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.dstSubresource.baseArrayLayer = 0;
		region.dstSubresource.layerCount     = 1;
		region.dstSubresource.mipLevel       = 0;
		region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.srcSubresource.baseArrayLayer = 0;
		region.srcSubresource.layerCount     = 1;
		region.srcSubresource.mipLevel       = 0;
	}
	else {
		region = *copyRegion;
	}
	vkCmdCopyImage(
		copyCmdBuffer,
		src->obj,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		dst->obj,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);
	ExecuteCommandBuffer(
		copyCmdBuffer,
		commandPool == VK_NULL_HANDLE ? this->defaultPool : commandPool,
		commandQueue == VK_NULL_HANDLE ? this->defaultQueue : commandQueue
	);
}

void vk::Device::CopyImage(VkImage src, VkImage dst, VkImageCopy *copyRegion, VkCommandPool commandPool, VkQueue commandQueue) {
	assert(dst && src);

	VkCommandBuffer copyCmdBuffer = CreateCommandBuffer(
		commandPool == VK_NULL_HANDLE ? this->defaultPool : commandPool,
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		true
	);

	vkCmdCopyImage(
		copyCmdBuffer,
		src,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		dst,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		copyRegion
	);
	ExecuteCommandBuffer(
		copyCmdBuffer,
		commandPool == VK_NULL_HANDLE ? this->defaultPool : commandPool,
		commandQueue == VK_NULL_HANDLE ? this->defaultQueue : commandQueue
	);
}

void vk::Device::PrintMemoryInfo() {
	std::cout << "\n---------------Memory-Info-------------------\n";
	std::cout << "Heaps:\n";
	for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++) {
		std::cout << "MemoryHeap index: " << i << " size: " << memoryProperties.memoryHeaps[i].size << std::endl;
	}
	std::cout << "Types:\n";
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		std::cout << "MemoryHeap index: " << memoryProperties.memoryTypes[i].heapIndex << " flags: "
		          << memoryProperties.memoryTypes[i].propertyFlags << std::endl;
	}
	std::cout << "\n-----------END-Memory-Info-------------------\n";
}
