#include "Instance.h"

vk::Instance::Instance() {

}

vk::Instance::~Instance() {
    Destroy();
}

VkResult vk::Instance::Setup(std::vector<const char*>& requestedExtensions,
                         std::vector<const char*>& requestedLayers)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Unknown Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Unknown Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requestedExtensions.size();
    createInfo.ppEnabledExtensionNames = requestedExtensions.size() > 0 ? requestedExtensions.data() : NULL;
    createInfo.enabledLayerCount = requestedLayers.size();
    createInfo.ppEnabledLayerNames = requestedLayers.size() > 0 ? requestedLayers.data() : NULL;

    return vkCreateInstance(&createInfo, nullptr, &instance);
}

void vk::Instance::Destroy() {
    if (instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}
