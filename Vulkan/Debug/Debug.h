#pragma once

#include "Vulkan/Other/VulkanSharedHaders.h"

namespace vk
{
    class Debug {
    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
        PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallbackEXT;
        PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallbackEXT;

    public:
        Debug();

        ~Debug();

        static VKAPI_ATTR VkBool32 VKAPI_CALL MessageCallback(
            VkDebugReportFlagsEXT      flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t                   srcObject,
            size_t                     location,
            int32_t                    msgCode,
            const char*                pLayerPrefix,
            const char*                pMsg,
            void*                      pUserData
        );

        VkResult Setup(
            VkInstance instance,
            VkDebugReportFlagsEXT flags = VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT
        );

        void DestroyCallback();
    };
}