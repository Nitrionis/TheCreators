#include "Debug.h"

vk::Debug::Debug() {

}

vk::Debug::~Debug() {
    DestroyCallback();
}

VkBool32 vk::Debug::MessageCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
                                    size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg, void *pUserData)
{
    //static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        //SetConsoleTextAttribute(hConsole, 12);
        std::cout << "ERROR:";
    };
    if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        //SetConsoleTextAttribute(hConsole, 14);
        std::cout << "WARNING:";
    };
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        //SetConsoleTextAttribute(hConsole, 14);
        std::cout << "PERFORMANCE:";
    };
    if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
    {
        //SetConsoleTextAttribute(hConsole, 10);
        std::cout << "INFO:";
    }
    if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
    {
        //SetConsoleTextAttribute(hConsole, 12);
        std::cout << "DEBUG:";
    }
    std::cout << " Code " << msgCode << " Message: " << pMsg << std::endl;

    return VK_FALSE;
}

VkResult vk::Debug::Setup(VkInstance instance, VkDebugReportFlagsEXT flags)
{
    this->instance = instance;

    CreateDebugReportCallbackEXT =
        (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (!CreateDebugReportCallbackEXT)
        throw std::runtime_error("Runtime: failed get vkCreateDebugReportCallbackEXT");

    DestroyDebugReportCallbackEXT =
        (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (!DestroyDebugReportCallbackEXT)
        throw std::runtime_error("Runtime: failed get vkDestroyDebugReportCallbackEXT");

    VkDebugReportCallbackCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    debugCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)MessageCallback;
    debugCreateInfo.flags = flags;

    return CreateDebugReportCallbackEXT(
        instance,
        &debugCreateInfo,
        nullptr,
        &callback
    );
}

void vk::Debug::DestroyCallback() {
    if (callback != VK_NULL_HANDLE)
    {
        DestroyDebugReportCallbackEXT(instance, callback, nullptr);
    }
}
