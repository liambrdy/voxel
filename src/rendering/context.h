#ifndef CONTEXT_H
#define CONTEXT_H

#include <Volk/volk.h>
#include <SDL2/SDL.h>
#include <vulkan/vk_enum_string_helper.h>

#include <array>

#include "swapchain.h"

#ifdef VOXEL_DEBUG
#define VkCheck(res) {\
    VkResult r = res; \
    if (r != VK_SUCCESS) { \
        printf("Vulan Error: (file '%s' line %d): %s, with code %s\n", __FILE__, __LINE__, #res, string_VkResult(r)); \
        return GetResultFromVkResult(r); \
    } \
}
#else
#define VkCheck(res) GetResultFromVkResult(res)
#endif

#define ResCheck(res) {\
    Result r = res; \
    if (r != Success) { \
        return r; \
    } \
}

struct FrameData {
    VkCommandBuffer computeCmd;
    VkCommandBuffer graphicsCmd;

    VkFence computeFence;
    VkSemaphore computeFinishedSemaphore;

    VkFence renderFence;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
};

constexpr uint32_t MaxFramesInFlight = 2;

struct RenderContext {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue queue;
    VkSurfaceKHR surface;
    VkCommandPool commandPool;
    VkRenderPass renderPass;

    Swapchain swapchain;
    std::vector<VkFramebuffer> framebuffers;

    std::array<FrameData, MaxFramesInFlight> frames;
    uint32_t frameCount;

    uint32_t queueFamily;
};

enum Result {
    Success,
    ExtensionNotPresent,
    LayerNotPresent,
    UnsupportedPhysicalDevice,
    ErrorCreatingSurface,
    UnsupportedQueueFamily,
    Unknown
};

Result InitializeRenderContext(SDL_Window *window);
Result RenderFrame();

Result GetResultFromVkResult(VkResult res);

extern RenderContext context;

#endif // CONTEX_H