#ifndef CONTEXT_H
#define CONTEXT_H

#include <Volk/volk.h>
#include <SDL2/SDL.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vma/vk_mem_alloc.h>

#include <array>

#include "swapchain.h"
#include "pipeline.h"
#include "image.h"
#include "buffer.h"

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
    VkCommandBuffer graphicsCmd;
    VkCommandBuffer computeCmd;

    VkFence renderFence;
    VkFence computeFence;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore computeDoneSemaphore;
};

constexpr uint32_t MaxFramesInFlight = 2;

struct RenderContext {
    VmaAllocator allocator;

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue queue;
    VkSurfaceKHR surface;
    VkCommandPool commandPool;
    VkRenderPass renderPass;
    VkDescriptorPool descriptorPool;

    Buffer voxelData;
    Pipeline quadPipeline;
    Pipeline computePipeline;
    Image renderImage;
    VkSampler renderImageSampler;

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

void UploadVoxelData(const std::vector<int> &data);

Result GetResultFromVkResult(VkResult res);

extern RenderContext context;

#endif // CONTEX_H