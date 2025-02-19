#include "vulkan/vulkan_core.h"
#include "vulkanrender.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
// #include <vk_initializers.h>
#include "VkBootstrap.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <thread>

constexpr bool mUseValidationLayers = false;

VulkanRenderer *currentRenderer = nullptr;

VulkanRenderer::VulkanRenderer() {}
VulkanRenderer::~VulkanRenderer() {};

VulkanRenderer &VulkanRenderer::Get() { return *currentRenderer; };

bool VulkanRenderer::init() {

  assert(currentRenderer == nullptr);
  currentRenderer = this;

  fmt::println("Started Vulkan init");
  int sdlResult = SDL_Init(SDL_INIT_VIDEO);

  if (!sdlResult) {
    // fmt::print("Unable to initialize SDL");
    return false;
  }

  mWindow = SDL_CreateWindow("Worlds apart", mWindowExtent.width,
                             mWindowExtent.height, SDL_WINDOW_VULKAN);

  if (!mWindow) {
    // fmt::print("Unable to open window");
  }

  InitVulkan();
  InitSwapchain();
  InitCommands();
  InitSyncStructures();

  mIsRunning = true;

  return true;
}

void VulkanRenderer::InitVulkan() {
  vkb::InstanceBuilder builder;

  auto inst_ret = builder.set_app_name("Hello World")
                      .request_validation_layers(mUseValidationLayers)
                      .request_validation_layers()
                      .use_default_debug_messenger()
                      .require_api_version(1, 3, 0)
                      .build();

  vkb::Instance vkb_inst = inst_ret.value();

  mInstance = vkb_inst.instance;
  mDebugMessenger = vkb_inst.debug_messenger;

  SDL_Vulkan_CreateSurface(mWindow, mInstance, NULL, &mSurface);

  VkPhysicalDeviceVulkan13Features features{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
  features.dynamicRendering = true;
  features.synchronization2 = true;

  VkPhysicalDeviceVulkan12Features features12{
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  features12.bufferDeviceAddress = true;
  features12.descriptorIndexing = true;

  vkb::PhysicalDeviceSelector selector{vkb_inst};
  vkb::PhysicalDevice physicalDevice = selector.set_minimum_version(1, 3)
                                           .set_required_features_13(features)
                                           .set_required_features_12(features12)
                                           .set_surface(mSurface)
                                           .select()
                                           .value();

  vkb::DeviceBuilder deviceBuilder{physicalDevice};
  vkb::Device vkbDevice = deviceBuilder.build().value();

  mDevice = vkbDevice.device;
  mChosenGPU = physicalDevice.physical_device;
};

void VulkanRenderer::CreateSwapchain(uint32_t width, uint32_t height) {
  vkb::SwapchainBuilder swapChainBuilder{mChosenGPU, mDevice, mSurface};
  mSwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

  vkb::Swapchain vkbSwapchain =
      swapChainBuilder //.use_default_format_selection()
          .set_desired_format(VkSurfaceFormatKHR{
              .format = mSwapchainImageFormat,
              .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
          .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
          .set_desired_extent(width, height)
          .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
          .build()
          .value();

  mSwapchainExtent = vkbSwapchain.extent;
  mSwapnhain = vkbSwapchain.swapchain;
  mSwapChainImages = vkbSwapchain.get_images().value();
  mSwapChainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanRenderer::DestroySwapchain() {
  vkDestroySwapchainKHR(mDevice, mSwapnhain, nullptr);

  for (int i = 0; i < mSwapChainImageViews.size(); i++) {
    vkDestroyImageView(mDevice, mSwapChainImageViews[i], nullptr);
  }
}

void VulkanRenderer::InitSwapchain() {
  CreateSwapchain(mWindowExtent.width, mWindowExtent.height);
};

void VulkanRenderer::InitCommands() {

};

void VulkanRenderer::InitSyncStructures() {

};
bool VulkanRenderer::shutdown() {

  fmt::println("Started Vulkan shutdown");
  // SDL_GL_DestroyContext(mContext);
  DestroySwapchain();
  vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  vkDestroyDevice(mDevice, nullptr);

  vkb::destroy_debug_utils_messenger(mInstance,mDebugMessenger);

  vkDestroyInstance(mInstance, nullptr);
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
  currentRenderer = nullptr;
  return true;
}

void VulkanRenderer::loop() {

  while (mIsRunning) {

    ProcessInput();
  }
}

void VulkanRenderer::ProcessInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      mIsRunning = false;
      break;
    }
  }

  const bool *state = SDL_GetKeyboardState(NULL);

  if (state[SDL_SCANCODE_ESCAPE]) {
    mIsRunning = false;
  }
}
