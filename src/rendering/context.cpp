#include "context.h"

#include "vkutil.h"

#include <SDL2/SDL_vulkan.h>

RenderContext context = {};

static Result CheckExtensions(const std::vector<const char *> &extensions, VkPhysicalDevice physicalDevice = VK_NULL_HANDLE) {
    uint32_t propCount = 0;
    std::vector<VkExtensionProperties> props;
    if (physicalDevice == VK_NULL_HANDLE) {
        vkEnumerateInstanceExtensionProperties(nullptr, &propCount, nullptr);
        props.resize(propCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &propCount, props.data());
    } else {
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &propCount, nullptr);
        props.resize(propCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &propCount, props.data());
    }

    uint32_t found = 0;
    for (uint32_t i = 0; i < extensions.size(); i++) {
        const char *name = extensions[i];
        for (uint32_t j = 0; j < propCount; j++) {
            if (strcmp(name, props[j].extensionName) == 0) {
                found++;
                break;
            }
        }
    }

    if (found == extensions.size()) {
        return Success;
    } else {
        return ExtensionNotPresent;
    }
}

static Result CheckLayers(const std::vector<const char *> &layers) {
    uint32_t propCount = 0;
    vkEnumerateInstanceLayerProperties(&propCount, nullptr);
    std::vector<VkLayerProperties> props(propCount);
    vkEnumerateInstanceLayerProperties(&propCount, props.data());

    uint32_t found = 0;
    for (uint32_t i = 0; i < layers.size(); i++) {
        const char *name = layers[i];
        for (uint32_t j = 0; j < propCount; j++) {
            if (strcmp(name, props[j].layerName) == 0) {
                found++;
                break;
            }
        }
    }

    if (found == layers.size()) {
        return Success;
    } else {
        return LayerNotPresent;
    }
}

static inline VkSemaphore CreateSemaphore() {
    VkSemaphoreCreateInfo info = GetSemaphoreCreateInfo();
    VkSemaphore semaphore;
    vkCreateSemaphore(context.device, &info, nullptr, &semaphore);
    return semaphore;
}

static inline VkFence CreateFence(VkFenceCreateFlags flags) {
    VkFenceCreateInfo info = GetFenceCreateInfo(flags);
    VkFence fence;
    vkCreateFence(context.device, &info, nullptr, &fence);
    return fence;
}

static inline VkCommandBuffer AllocateCommandBuffer() {
    VkCommandBufferAllocateInfo info = GetCommandBufferAllocateInfo(context.commandPool, 1);
    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(context.device, &info, &cmd);
    return cmd;
}

Result InitializeRenderContext(SDL_Window *window) {
    VkCheck(volkInitialize());

    uint32_t sdlExtensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr);
    std::vector<const char *> sdlExtensions(sdlExtensionCount);
    SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, sdlExtensions.data());

    ResCheck(CheckExtensions(sdlExtensions));

    std::vector<const char *> layers = {};
#ifdef VOXEL_DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    ResCheck(CheckLayers(layers));

    VkApplicationInfo appInfo = GetApplicationInfo();
    VkInstanceCreateInfo instanceInfo = GetInstanceCreateInfo(&appInfo, layers, sdlExtensions);

    VkCheck(vkCreateInstance(&instanceInfo, nullptr, &context.instance));

    volkLoadInstance(context.instance);

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCount, physicalDevices.data());

    for (const auto &device : physicalDevices) {
        VkPhysicalDeviceProperties props = {};
        vkGetPhysicalDeviceProperties(device, &props);
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            context.physicalDevice = device;
            break;
        }
    }

    if (context.physicalDevice == VK_NULL_HANDLE) {
        return UnsupportedPhysicalDevice;
    }

    if (!SDL_Vulkan_CreateSurface(window, context.instance, &context.surface)) {
        return ErrorCreatingSurface;
    }

    context.queueFamily = UINT32_MAX;

    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context.physicalDevice, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> familyProps(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(context.physicalDevice, &familyCount, familyProps.data());
    for (uint32_t i = 0; i < familyCount; i++) {
        VkBool32 supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(context.physicalDevice, i, context.surface, &supported);
        if (familyProps[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT) && supported) {
            context.queueFamily = i;
            break;
        }
    }

    if (context.queueFamily == UINT32_MAX) {
        return UnsupportedQueueFamily;
    }

    std::vector<const char *> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    ResCheck(CheckExtensions(deviceExtensions, context.physicalDevice));

    std::vector<VkDeviceQueueCreateInfo> queueInfos = { GetDeviceQueueCreateInfo(context.queueFamily, 1) };
    VkDeviceCreateInfo deviceInfo = GetDeviceCreateInfo(queueInfos, deviceExtensions);

    VkCheck(vkCreateDevice(context.physicalDevice, &deviceInfo, nullptr, &context.device));
    vkGetDeviceQueue(context.device, context.queueFamily, 0, &context.queue);

    ResCheck(CreateSwapchain(&context.swapchain, true));

    VkCommandPoolCreateInfo poolInfo = GetCommandPoolCreateInfo(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, context.queueFamily);
    VkCheck(vkCreateCommandPool(context.device, &poolInfo, nullptr, &context.commandPool));

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.flags = 0;
    colorAttachment.format = context.swapchain.surfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.flags = 0;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    std::vector<VkAttachmentDescription> attachments = {colorAttachment};
    std::vector<VkSubpassDescription> subpasses = {subpass};
    std::vector<VkSubpassDependency> dependencies = {};
    VkRenderPassCreateInfo renderPassInfo = GetRenderPassCreateInfo(attachments, subpasses, dependencies);
    VkCheck(vkCreateRenderPass(context.device, &renderPassInfo, nullptr, &context.renderPass));

    for (uint32_t i = 0; i < context.swapchain.imageCount; i++) {
        std::vector<VkImageView> attachments = {
            context.swapchain.imageViews[i]
        };

        VkFramebuffer framebuffer;
        VkFramebufferCreateInfo framebufferInfo = GetFramebufferCreateInfo(context.renderPass, attachments, context.swapchain.extent);
        VkCheck(vkCreateFramebuffer(context.device, &framebufferInfo, nullptr, &framebuffer));

        context.framebuffers.push_back(framebuffer);
    }

    for (auto &frame : context.frames) {
        frame.computeCmd = AllocateCommandBuffer();
        frame.computeFence = CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
        frame.computeFinishedSemaphore = CreateSemaphore();

        frame.graphicsCmd = AllocateCommandBuffer();
        frame.renderFence = CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
        frame.imageAvailableSemaphore = CreateSemaphore();
        frame.renderFinishedSemaphore = CreateSemaphore();
    }

    return Success;
}

Result RenderFrame() {
    FrameData &frame = context.frames[context.frameCount % MaxFramesInFlight];
    vkWaitForFences(context.device, 1, &frame.computeFence, VK_TRUE, UINT64_MAX);
    vkResetFences(context.device, 1, &frame.computeFence);

    vkResetCommandBuffer(frame.computeCmd, 0);

    VkCommandBufferBeginInfo beginInfo = GetCommandBufferBeginInfo(0);
    vkBeginCommandBuffer(frame.computeCmd, &beginInfo);
    vkEndCommandBuffer(frame.computeCmd);

    std::vector<VkSemaphore> waitSemaphores = {};
    std::vector<VkSemaphore> signalSemaphores = {frame.computeFinishedSemaphore};
    std::vector<VkPipelineStageFlags> waitFlags = {};

    VkSubmitInfo submitInfo = GetSubmitInfo(&frame.computeCmd, waitSemaphores, waitFlags, signalSemaphores);
    VkCheck(vkQueueSubmit(context.queue, 1, &submitInfo, frame.computeFence));

    vkWaitForFences(context.device, 1, &frame.renderFence, VK_TRUE, UINT64_MAX);
    vkResetFences(context.device, 1, &frame.renderFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(context.device, context.swapchain.swapchain, UINT64_MAX, frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(frame.graphicsCmd, 0);

    vkBeginCommandBuffer(frame.graphicsCmd, &beginInfo);
    {
        VkRenderPassBeginInfo passBeginInfo = GetRenderPassBeginInfo(context.renderPass, context.framebuffers[imageIndex], context.swapchain.extent, {1.0f, 0.0f, 0.0f, 1.0f});
        
        vkCmdBeginRenderPass(frame.graphicsCmd, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdEndRenderPass(frame.graphicsCmd);
    }
    vkEndCommandBuffer(frame.graphicsCmd);

    waitSemaphores = {frame.computeFinishedSemaphore, frame.imageAvailableSemaphore};
    signalSemaphores = {frame.renderFinishedSemaphore};
    waitFlags = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo = GetSubmitInfo(&frame.graphicsCmd, waitSemaphores, waitFlags, signalSemaphores);
    VkCheck(vkQueueSubmit(context.queue, 1, &submitInfo, frame.renderFence));

    waitSemaphores = {frame.renderFinishedSemaphore};
    VkPresentInfoKHR presentInfo = GetPresentInfo(waitSemaphores, &context.swapchain.swapchain, &imageIndex);
    VkCheck(vkQueuePresentKHR(context.queue, &presentInfo));

    return Success;
}

Result GetResultFromVkResult(VkResult res) {
    switch (res) {
        case VK_SUCCESS: return Success;
        case VK_ERROR_EXTENSION_NOT_PRESENT: return ExtensionNotPresent;
        default: return Unknown;
    }
}