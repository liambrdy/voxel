#include "context.h"

#include "vkutil.h"

#include <SDL2/SDL_vulkan.h>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

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

    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
    };
    VkDescriptorPoolCreateInfo descriptorPoolInfo = GetDescriptorPoolCreateInfo(30, poolSizes);
    VkCheck(vkCreateDescriptorPool(context.device, &descriptorPoolInfo, nullptr, &context.descriptorPool));

    VmaVulkanFunctions functions = {};
    functions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    functions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    functions.vkAllocateMemory = vkAllocateMemory;
    functions.vkFreeMemory = vkFreeMemory;
    functions.vkMapMemory = vkMapMemory;
    functions.vkUnmapMemory = vkUnmapMemory;
    functions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    functions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    functions.vkBindBufferMemory = vkBindBufferMemory;
    functions.vkBindImageMemory = vkBindImageMemory;
    functions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    functions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    functions.vkCreateBuffer = vkCreateBuffer;
    functions.vkDestroyBuffer = vkDestroyBuffer;
    functions.vkCreateImage = vkCreateImage;
    functions.vkDestroyImage = vkDestroyImage;
    functions.vkCmdCopyBuffer = vkCmdCopyBuffer;
    functions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2;
    functions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2;
    functions.vkBindBufferMemory2KHR = vkBindBufferMemory2;
    functions.vkBindImageMemory2KHR = vkBindImageMemory2;
    functions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2;
    functions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
    functions.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.flags = 0;
    allocatorInfo.physicalDevice = context.physicalDevice;
    allocatorInfo.device = context.device;
    allocatorInfo.pVulkanFunctions = &functions;
    allocatorInfo.instance = context.instance;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;

    VkCheck(vmaCreateAllocator(&allocatorInfo, &context.allocator));

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
    
    context.computePipeline = CreateComputePipeline("../../res/shaders/voxel.comp");
    context.quadPipeline = CreateGraphicsPipeline({"../../res/shaders/screenquad.vert", "../../res/shaders/screenquad.frag"}, context.renderPass);
    context.renderImage = CreateImage(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, context.swapchain.extent.width, context.swapchain.extent.height);

    VkSamplerCreateInfo samplerInfo = GetSamplerCreateInfo();
    VkCheck(vkCreateSampler(context.device, &samplerInfo, nullptr, &context.renderImageSampler));

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = context.renderImage.view;
    
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstSet = context.computePipeline.set;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    write.pImageInfo = &imageInfo;
    write.pBufferInfo = nullptr;
    write.pTexelBufferView = nullptr;
    
    vkUpdateDescriptorSets(context.device, 1, &write, 0, nullptr);
    
    write.dstSet = context.quadPipeline.set;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageInfo.sampler = context.renderImageSampler;

    vkUpdateDescriptorSets(context.device, 1, &write, 0, nullptr);

    for (auto &frame : context.frames) {
        frame.graphicsCmd = AllocateCommandBuffer();
        frame.computeCmd = AllocateCommandBuffer();

        frame.renderFence = CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
        frame.computeFence = CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
        frame.imageAvailableSemaphore = CreateSemaphore();
        frame.computeDoneSemaphore = CreateSemaphore();
    }

    VkCommandBuffer cmd = BeginSingleUseCmd();

    SetImageLayout(cmd, context.renderImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    EndSingleUseCmd(cmd);

    return Success;
}

Result RenderFrame() {
    FrameData &frame = context.frames[context.frameCount % MaxFramesInFlight];

    vkWaitForFences(context.device, 1, &frame.computeFence, VK_TRUE, UINT64_MAX);
    vkResetFences(context.device, 1, &frame.computeFence);

    VkCommandBufferBeginInfo beginInfo = GetCommandBufferBeginInfo(0);
    vkBeginCommandBuffer(frame.computeCmd, &beginInfo);

    vkCmdBindPipeline(frame.computeCmd, VK_PIPELINE_BIND_POINT_COMPUTE, context.computePipeline.pipeline);
    vkCmdBindDescriptorSets(frame.computeCmd, VK_PIPELINE_BIND_POINT_COMPUTE, context.computePipeline.layout, 0, 1, &context.computePipeline.set, 0, nullptr);

    vkCmdDispatch(frame.computeCmd, context.renderImage.width / 16, context.renderImage.height / 16, 1);

    vkEndCommandBuffer(frame.computeCmd);

    std::vector<VkSemaphore> waitSemaphores = {};
    std::vector<VkSemaphore> signalSemaphores = {frame.computeDoneSemaphore};
    std::vector<VkPipelineStageFlags> waitFlags = {VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT};
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
        vkCmdBindPipeline(frame.graphicsCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, context.quadPipeline.pipeline);
        vkCmdBindDescriptorSets(frame.graphicsCmd, VK_PIPELINE_BIND_POINT_GRAPHICS, context.quadPipeline.layout, 0, 1, &context.quadPipeline.set, 0, nullptr);
        vkCmdDraw(frame.graphicsCmd, 4, 1, 0, 0);
        vkCmdEndRenderPass(frame.graphicsCmd);
    }
    vkEndCommandBuffer(frame.graphicsCmd);

    waitSemaphores = {frame.computeDoneSemaphore, frame.imageAvailableSemaphore};
    signalSemaphores = {context.swapchain.submitReadySemaphores[imageIndex]};
    waitFlags = {VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo = GetSubmitInfo(&frame.graphicsCmd, waitSemaphores, waitFlags, signalSemaphores);
    VkCheck(vkQueueSubmit(context.queue, 1, &submitInfo, frame.renderFence));

    waitSemaphores = {context.swapchain.submitReadySemaphores[imageIndex]};
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