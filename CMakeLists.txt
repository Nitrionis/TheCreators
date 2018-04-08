cmake_minimum_required(VERSION 3.8)
project(TheCreators)

add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        main.cpp
        Vulkan/VulkanSharedHaders.h
        Scene/Scene.cpp
        Scene/Scene.h
        SceneRenderer/SceneRenderer.cpp
        SceneRenderer/SceneRenderer.h
        SceneRenderer/VulkanSharedDate.cpp
        SceneRenderer/RendererSettings.cpp
        Vulkan/Device.cpp
        Vulkan/Device.h
        Vulkan/Buffer.h
        Vulkan/Debug.cpp
        Vulkan/Debug.h
        Vulkan/Instance.cpp
        Vulkan/Instance.h
        Vulkan/CustomVulkanTypes.hpp
        Vulkan/SwapChain.cpp
        Vulkan/SwapChain.h
        Vulkan/Window.cpp
        Vulkan/Window.h
        StandardBehavior.h
        BaseCode.h
        Vulkan/Material.cpp
        Vulkan/Material.h
        Vulkan/UniqueHandle.h
        ImageLoader.cpp
        ImageLoader.h
        stb_image.h
        Vulkan/VectorTypes.h
        Vulkan/Mesh.cpp
        Vulkan/Mesh.h
        Vulkan/Buffer.cpp
        SceneRenderer/VulkanSharedDate.h
        SceneRenderer/VulkanReference.h
        SceneRenderer/VulkanReference.cpp
        Scene/Chunks/ChunksRenderer.cpp
        Scene/Chunks/ChunksRenderer.h
        SceneRenderer/RendererSettings.h
        Scene/UserInterface/InterfaceRenderer.cpp
        Scene/UserInterface/InterfaceRenderer.h
        Scene/PostProcessing/BlurRenderer.cpp
        Scene/PostProcessing/BlurRenderer.h
        SceneRenderer/SubpassIndex.h
        Scene/Character/Character.cpp
        Scene/Character/Character.h Scene/Chunks/ChunksController.cpp Scene/Chunks/ChunksController.h Scene/UserInterface/InterfaceController.cpp Scene/UserInterface/InterfaceController.h Scene/Chunks/ChunksBuilder.cpp Scene/Chunks/ChunksBuilder.h Scene/UserInterface/InterfaceBuilder.cpp Scene/UserInterface/InterfaceBuilder.h Scene/Character/CharacterController.cpp Scene/Character/CharacterController.h Scene/PostProcessing/BlurController.cpp Scene/PostProcessing/BlurController.h Scene/PostProcessing/BlurBuilder.cpp Scene/PostProcessing/BlurBuilder.h Scene/ThreadController.cpp Scene/ThreadController.h Vulkan/InputControl.cpp Vulkan/InputControl.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SceneRenderer/)
include_directories(C:/Developer/Libs/glm)

add_executable(TheCreators ${SOURCE_FILES})

find_package (Vulkan)
if (Vulkan_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries (TheCreators ${Vulkan_LIBRARIES})
    message(STATUS "Vulkan Include dirs: ${Vulkan_INCLUDE_DIRS} and libs: ${Vulkan_LIBRARIES}")
endif (Vulkan_FOUND)

