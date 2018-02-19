cmake_minimum_required(VERSION 3.8)
project(TheCreators)

add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        main.cpp
        Vulkan/VulkanSharedHaders.h
        Scene.cpp
        Scene.h
        SceneRenderer/SceneRenderer.cpp
        SceneRenderer/SceneRenderer.h
        SceneRenderer/SceneRendererChunks.cpp
        SceneRenderer/SceneRendererVulkanObjects.cpp
        SceneRenderer/SceneRendererSettings.cpp
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
        stb_image.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Vulkan/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SceneRenderer/)

add_executable(TheCreators ${SOURCE_FILES})

find_package (Vulkan)
if (Vulkan_FOUND)
    message(STATUS "Found Vulkan, Including and Linking now")
    include_directories(${Vulkan_INCLUDE_DIRS})
    target_link_libraries (TheCreators ${Vulkan_LIBRARIES})
    message(STATUS "Vulkan Include dirs: ${Vulkan_INCLUDE_DIRS} and libs: ${Vulkan_LIBRARIES}")
endif (Vulkan_FOUND)