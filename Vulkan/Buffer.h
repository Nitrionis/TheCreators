#pragma once

#include "VulkanSharedHaders.h"
#include <cassert>

namespace vk
{
    class Divice;

	class Image
	{
		friend class Device;
	private:
		VkDevice               device   = VK_NULL_HANDLE;
	public:
		VkImage                image    = VK_NULL_HANDLE;
		VkDeviceMemory         memory   = VK_NULL_HANDLE;
		VkImageView            view     = VK_NULL_HANDLE;

		VkDeviceSize           size     = 0;
		VkExtent3D             extent;
		VkDeviceSize           alignment = 0;

		void*                  mapped    = nullptr;

		VkBufferUsageFlags     usageFlags;
		VkMemoryPropertyFlags  memoryPropertyFlags;

		Image() {}
		~Image() { Destroy(); }

		void Destroy() {
			Unmap();
			if (view)
				vkDestroyImageView(device, view, nullptr);
			if (image)
				vkDestroyImage(device, image, nullptr);
			if (memory)
				vkFreeMemory(device, memory, nullptr);
		}

		Image(const Image& obj) = delete;

		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			return vkMapMemory(device, memory, offset, size, 0, &mapped);
		}

		void Unmap()
		{
			if (mapped) {
				vkUnmapMemory(device, memory);
				mapped = nullptr;
			}
		}
	};

    class Buffer
    {
        friend class Device;
    private:
        VkDevice               device = VK_NULL_HANDLE;
    public:
        VkBuffer               buffer = VK_NULL_HANDLE;
        VkDeviceMemory         memory = VK_NULL_HANDLE;

        VkDescriptorBufferInfo descriptor;

        VkDeviceSize           size      = 0;
        VkDeviceSize           alignment = 0;
        void*                  mapped    = nullptr;

        VkBufferUsageFlags     usageFlags;
        VkMemoryPropertyFlags  memoryPropertyFlags;

        Buffer(){}

        ~Buffer() { Destroy(); }

        void Destroy() {
            Unmap();
            if (buffer)
                vkDestroyBuffer(device, buffer, nullptr);
            if (memory)
                vkFreeMemory(device, memory, nullptr);
        }

	    Buffer(const Buffer& obj) = delete;

        VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
        {
            return vkMapMemory(device, memory, offset, size, 0, &mapped);
        }

        void Unmap()
        {
            if (mapped) {
                vkUnmapMemory(device, memory);
                mapped = nullptr;
            }
        }

        VkResult Bind(VkDeviceSize offset = 0)
        {
            return vkBindBufferMemory(device, buffer, memory, offset);
        }

        void SetupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
        {
            descriptor.offset = offset;
            descriptor.buffer = buffer;
            descriptor.range = size;
        }

        void CopyFrom(void* data, VkDeviceSize size)
        {
            assert(mapped);
            memcpy(mapped, data, size);
        }

        VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
        {
            VkMappedMemoryRange mappedRange = {};
            mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory = memory;
            mappedRange.offset = offset;
            mappedRange.size = size;
            return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
        }

        VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
        {
            VkMappedMemoryRange mappedRange = {};
            mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory = memory;
            mappedRange.offset = offset;
            mappedRange.size = size;
            return vkInvalidateMappedMemoryRanges(device, 1, &mappedRange);
        }
    };
}