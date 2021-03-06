#pragma once

#include "Vulkan/Other/VulkanSharedHaders.h"
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
		VkImage                obj      = VK_NULL_HANDLE;
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
			if (obj)
				vkDestroyImage(device, obj, nullptr);
			if (memory)
				vkFreeMemory(device, memory, nullptr);
		}

		Image(const Image& obj) = delete;

		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
			return vkMapMemory(device, memory, offset, size, 0, &mapped);
		}
		void Unmap() {
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
        VkBuffer               obj    = VK_NULL_HANDLE;
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
            if (obj)
                vkDestroyBuffer(device, obj, nullptr);
            if (memory)
                vkFreeMemory(device, memory, nullptr);
        }

	    Buffer(const Buffer& obj) = delete;

        VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
            return vkMapMemory(device, memory, offset, size, 0, &mapped);
        }

        void Unmap() {
            if (mapped) {
                vkUnmapMemory(device, memory);
                mapped = nullptr;
            }
        }
        VkResult Bind(VkDeviceSize offset = 0) {
            return vkBindBufferMemory(device, obj, memory, offset);
        }
        void SetupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
	        descriptor.buffer = obj;
            descriptor.offset = offset;
            descriptor.range  = size;
        }
        void CopyFrom(void* data, VkDeviceSize size) {
            if (data != nullptr) {
	            memcpy(mapped, data, size);
            }
	        else
	            throw std::runtime_error("Runtime in CopyFrom method!");
        }

        VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
            VkMappedMemoryRange mappedRange = {};
            mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory  = memory;
            mappedRange.offset  = offset;
            mappedRange.size    = size;
            return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
        }

        VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
            VkMappedMemoryRange mappedRange = {};
            mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory  = memory;
            mappedRange.offset  = offset;
            mappedRange.size    = size;
            return vkInvalidateMappedMemoryRanges(device, 1, &mappedRange);
        }
    };
}