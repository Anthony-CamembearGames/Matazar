#pragma once
#define FMT_HEADER_ONLY
#include "externals/fmt/base.h"

#include <array>
#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "externals/fmt/core.h"
#include "externals/vk_mem_alloc.h"
#include "vk_images.h"
#include "vulkan/vulkan_core.h"
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include "externals/glm/mat4x4.hpp"
#include "externals/glm/vec4.hpp"

#define VK_CHECK(x)                                                            \
  do {                                                                         \
    VkResult err = x;                                                          \
    if (err) {                                                                 \
      fmt::print("Detected Vulkan error: {}", string_VkResult(err));           \
      abort();                                                                 \
    }                                                                          \
  } while (0)
