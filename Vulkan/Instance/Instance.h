#pragma once

#include "Vulkan/Other/VulkanSharedHaders.h"

namespace vk
{
    class Instance {
    private:
        VkInstance instance = VK_NULL_HANDLE;
    public:
        operator VkInstance*() {
            return &instance;
        }

        operator const VkInstance* () const {
            return &instance;
        }

        Instance();

        ~Instance();

        void Destroy();

        operator VkInstance()const { return instance; }

        VkResult Setup(
            std::vector<const char*>& requestedExtensions,
            std::vector<const char*>& requestedLayers
        );
    };
}