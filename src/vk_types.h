#pragma once
#define FMT_HEADER_ONLY
#include "externals/fmt/base.h"


#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>

#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "externals/vk_mem_alloc.h"

#include "externals/fmt/core.h"

#include "externals/glm/mat4x4.hpp"
#include "externals/glm/vec4.hpp"


#define VK_CHECK(x)                                                     \
    do {                                                                \
        VkResult err = x;                                               \
        if (err) {                                                      \
             fmt::print("Detected Vulkan error: {}", string_VkResult(err)); \
            abort();                                                    \
        }                                                               \
    } while (0)
