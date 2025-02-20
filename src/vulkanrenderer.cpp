#include "VkBootstrap.h"
#include "vk_initializers.h"
#include "vk_types.h"
#include "vk_video/vulkan_video_codec_av1std.h"
#include "vulkan/vulkan_core.h"
#include "vulkanrender.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <thread>

constexpr bool mUseValidationLayers = false;
VulkanRenderer* currentRenderer = nullptr;

VulkanRenderer::VulkanRenderer() {}
VulkanRenderer::~VulkanRenderer() {};

VulkanRenderer& VulkanRenderer::Get() { return *currentRenderer; };

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
    vkb::PhysicalDevice physicalDevice =
        selector.set_minimum_version(1, 3)
            .set_required_features_13(features)
            .set_required_features_12(features12)
            .set_surface(mSurface)
            .select()
            .value();

    vkb::DeviceBuilder deviceBuilder{physicalDevice};
    vkb::Device vkbDevice = deviceBuilder.build().value();

    mDevice = vkbDevice.device;
    mChosenGPU = physicalDevice.physical_device;

    mGraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    mGraphicsQueueFamily =
        vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
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

    VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(
        mGraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (int i = 0; i < FRAME_OVERLAP; i++) {

        VK_CHECK(vkCreateCommandPool(mDevice, &commandPoolInfo, nullptr,
                                     &mFrames[i].mCommandPool));

        // allocate the default command buffer that we will use for
        // rendering
        VkCommandBufferAllocateInfo cmdAllocInfo =
            vkinit::command_buffer_allocate_info(mFrames[i].mCommandPool, 1);

        VK_CHECK(vkAllocateCommandBuffers(mDevice, &cmdAllocInfo,
                                          &mFrames[i].mMainCommandBuffer));
    }
};

void VulkanRenderer::InitSyncStructures() {
    VkFenceCreateInfo FenceCreateInfo =
        vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo SemaphoreCreateInfo = vkinit::semaphore_create_info();

    for (int i = 0; i < FRAME_OVERLAP; i++) {
        VK_CHECK(vkCreateFence(mDevice, &FenceCreateInfo, nullptr,
                               &mFrames[i].mRenderFence));

        VK_CHECK(vkCreateSemaphore(mDevice, &SemaphoreCreateInfo, nullptr,
                                   &mFrames[i].mSwapChainSemaphore));
        VK_CHECK(vkCreateSemaphore(mDevice, &SemaphoreCreateInfo, nullptr,
                                   &mFrames[i].mRenderSemaphore));
    }
};

void VulkanRenderer::Draw() {
    VK_CHECK(vkWaitForFences(mDevice, 1, &GetCurrentFrame().mRenderFence, true,
                             1000000000000));
    VK_CHECK(vkResetFences(mDevice, 1, &GetCurrentFrame().mRenderFence));

    uint32_t SwapChainImageIndex;
    VK_CHECK(vkAcquireNextImageKHR(mDevice, mSwapnhain, 1000000000000,
                                   GetCurrentFrame().mSwapChainSemaphore,
                                   nullptr, &SwapChainImageIndex));

    VkCommandBuffer cmd = GetCurrentFrame().mMainCommandBuffer;

    VK_CHECK(vkResetCommandBuffer(cmd, 0));

    VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    vkutil::TransitionImage(cmd, mSwapChainImages[SwapChainImageIndex],
                            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    // make a clear-color from frame number. This will flash with a 120 frame
    // period.
    VkClearColorValue clearValue;
    float flash = std::abs(std::sin(mFrameNumber / 120.f));
    clearValue = {{0.0f, 0.0f, flash, 1.0f}};

    VkImageSubresourceRange clearRange =
        vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

    // clear image
    vkCmdClearColorImage(cmd, mSwapChainImages[SwapChainImageIndex],
                         VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

    // make the swapchain image into presentable mode
    vkutil::TransitionImage(cmd, mSwapChainImages[SwapChainImageIndex],
                            VK_IMAGE_LAYOUT_GENERAL,
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // finalize the command buffer (we can no longer add commands, but it can
    // now be executed)
    VK_CHECK(vkEndCommandBuffer(cmd));

    VkCommandBufferSubmitInfo cmdinfo = vkinit::command_buffer_submit_info(cmd);

    VkSemaphoreSubmitInfo waitInfo = vkinit::semaphore_submit_info(
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        GetCurrentFrame().mSwapChainSemaphore);
    VkSemaphoreSubmitInfo signalInfo =
        vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
                                      GetCurrentFrame().mRenderSemaphore);

    VkSubmitInfo2 submit =
        vkinit::submit_info(&cmdinfo, &signalInfo, &waitInfo);

    // submit command buffer to the queue and execute it.
    //  _renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit2(mGraphicsQueue, 1, &submit,
                            GetCurrentFrame().mRenderFence));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.pSwapchains = &mSwapnhain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &GetCurrentFrame().mRenderSemaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &SwapChainImageIndex;

    VK_CHECK(vkQueuePresentKHR(mGraphicsQueue, &presentInfo));

    // increase the number of frames drawn
    mFrameNumber++;
}

bool VulkanRenderer::shutdown() {

    vkDeviceWaitIdle(mDevice);
    for (int i = 0; i < FRAME_OVERLAP; i++) {

        // already written from before
        vkDestroyCommandPool(mDevice, mFrames[i].mCommandPool, nullptr);

        // destroy sync objects
        vkDestroyFence(mDevice, mFrames[i].mRenderFence, nullptr);
        vkDestroySemaphore(mDevice, mFrames[i].mRenderSemaphore, nullptr);
        vkDestroySemaphore(mDevice, mFrames[i].mSwapChainSemaphore, nullptr);
    }
    fmt::println("Started Vulkan shutdown");
    // SDL_GL_DestroyContext(mContext);
    //

    DestroySwapchain();
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    vkDestroyDevice(mDevice, nullptr);

    vkb::destroy_debug_utils_messenger(mInstance, mDebugMessenger);

    vkDestroyInstance(mInstance, nullptr);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
    currentRenderer = nullptr;
    return true;
}

void VulkanRenderer::loop() {

    while (mIsRunning) {

        ProcessInput();
        Draw();
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

    const bool* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_ESCAPE]) {
        mIsRunning = false;
    }
}
