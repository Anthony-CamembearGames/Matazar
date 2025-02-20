#include "externals/glm/detail/qualifier.hpp"
#include "vk_types.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>

struct FrameData {
        VkCommandPool mCommandPool;
        VkCommandBuffer mMainCommandBuffer;
        VkSemaphore mSwapChainSemaphore, mRenderSemaphore;
        VkFence mRenderFence;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanRenderer {
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        bool init();
        bool shutdown();
        void loop();

        static VulkanRenderer& Get();

        VkInstance mInstance;
        VkDebugUtilsMessengerEXT mDebugMessenger;
        VkPhysicalDevice mChosenGPU;
        VkDevice mDevice;
        VkSurfaceKHR mSurface;

        VkSwapchainKHR mSwapnhain;
        VkFormat mSwapchainImageFormat;

        std::vector<VkImage> mSwapChainImages;
        std::vector<VkImageView> mSwapChainImageViews;

        VkExtent2D mSwapchainExtent;

        FrameData mFrames[FRAME_OVERLAP];
        FrameData& GetCurrentFrame() {
            return mFrames[mFrameNumber % FRAME_OVERLAP];
        };
        VkQueue mGraphicsQueue;
        uint32_t mGraphicsQueueFamily;

    private:
        void ProcessInput();

        void Draw();

        void InitVulkan();
        void InitSwapchain();
        void InitCommands();
        void InitSyncStructures();

        void CreateSwapchain(uint32_t width, uint32_t height);
        void DestroySwapchain();

        struct SDL_Window* mWindow{nullptr};
        bool mIsRunning;
        VkExtent2D mWindowExtent{1280, 720};
        int mFrameNumber{0};
};
