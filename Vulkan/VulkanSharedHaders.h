#pragma once

#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <functional>

#define VK_FLAGS_NONE 0
#define DEFAULT_FENCE_TIMEOUT 100000000000
#define TARGET_PLATFORM_WIN32 1
#define DEBUG_MSG(str) (std::string(str) + " in " + __FILE__ + " at line " + std::to_string(__LINE__))
#define VK_THROW_IF_FAILED(res) { if (res != VK_SUCCESS) throw std::runtime_error(DEBUG_MSG("Runtime error")); }

#include "UniqueHandle.h"