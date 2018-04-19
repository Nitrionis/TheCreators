cmake_minimum_required(VERSION 3.8)
project(TheCreators)

add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(C:/Developer/Libs/glm)

set(SOURCE_FILES
        main.cpp
        Vulkan/Other/VulkanSharedHaders.h
        Scene/Scene.cpp
        Scene/Scene.h
        Scene/SceneRenderer/SceneRenderer.cpp
        Scene/SceneRenderer/SceneRenderer.h
        Scene/SceneRenderer/VulkanSharedData/VulkanSharedData.cpp
        Scene/SceneRenderer/RendererSettings/RendererSettings.cpp
        Vulkan/Device/Device.cpp
        Vulkan/Device/Device.h
        Vulkan/Buffer/Buffer.h
        Vulkan/Debug/Debug.cpp
        Vulkan/Debug/Debug.h
        Vulkan/Instance/Instance.cpp
        Vulkan/Instance/Instance.h
        Vulkan/Other/CustomVulkanTypes.hpp
        Vulkan/Swapchain/SwapChain.cpp
        Vulkan/Swapchain/SwapChain.h
        Vulkan/Window/Window.cpp
        Vulkan/Window/Window.h
        ScriptsManager/StandardBehavior.h
        BaseCode.h
        Vulkan/Material/Material.cpp
        Vulkan/Material/Material.h
        Vulkan/Other/UniqueHandle.h
        ImageLoader.cpp
        ImageLoader.h
        stb_image.h
        Vulkan/Other/VectorTypes.h
        Vulkan/Mesh/Mesh.cpp
        Vulkan/Mesh/Mesh.h
        Vulkan/Buffer/Buffer.cpp
        Scene/SceneRenderer/VulkanSharedData/VulkanSharedData.h
        Scene/SceneRenderer/VulkanSharedData/VulkanReference.h
        Scene/SceneRenderer/VulkanSharedData/VulkanReference.cpp
        Scene/Chunks/ChunksRenderer.cpp
        Scene/Chunks/ChunksRenderer.h
        Scene/SceneRenderer/RendererSettings/RendererSettings.h
        Scene/UserInterface/InterfaceRenderer.cpp
        Scene/UserInterface/InterfaceRenderer.h
        Scene/PostProcessing/BlurRenderer.cpp
        Scene/PostProcessing/BlurRenderer.h
        Scene/SceneRenderer/SubpassIndex.h
        Scene/Character/Camera/Camera.cpp
        Scene/Character/Camera/Camera.h
        Scene/Chunks/ChunksBuilder.cpp
        Scene/Chunks/ChunksBuilder.h
        Scene/UserInterface/InterfaceBuilder.cpp
        Scene/UserInterface/InterfaceBuilder.h
        Scene/Character/Character.cpp
        Scene/Character/Character.h
        Scene/PostProcessing/BlurBuilder.cpp
        Scene/PostProcessing/BlurBuilder.h
        Scene/ThreadController/ThreadController.cpp
        Scene/ThreadController/ThreadController.h
        Vulkan/Input/InputControl.cpp
        Vulkan/Input/InputControl.h
        ScriptsManager/ScriptsManager.cpp
        ScriptsManager/ScriptsManager.h)

add_executable(TheCreators ${SOURCE_FILES})

find_package (Vulkan)
if (Vulkan_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries (TheCreators ${Vulkan_LIBRARIES})
    message(STATUS "Vulkan Include dirs: ${Vulkan_INCLUDE_DIRS} and libs: ${Vulkan_LIBRARIES}")
endif (Vulkan_FOUND)

