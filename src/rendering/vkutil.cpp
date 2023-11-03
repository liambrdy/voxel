#include "vkutil.h"

#include "context.h"

VkApplicationInfo GetApplicationInfo() {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Voxel";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    return appInfo;
}

VkInstanceCreateInfo GetInstanceCreateInfo(VkApplicationInfo *appInfo, const std::vector<const char *> &layers, const std::vector<const char *> &extensions) {
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = appInfo;
    instanceInfo.enabledLayerCount = (uint32_t)layers.size();
    instanceInfo.ppEnabledLayerNames = layers.data();
    instanceInfo.enabledExtensionCount = (uint32_t)extensions.size();;
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    return instanceInfo;
}

VkDeviceCreateInfo GetDeviceCreateInfo(const std::vector<VkDeviceQueueCreateInfo> &queueInfos, const std::vector<const char *> &extensions) {
    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = nullptr;
    deviceInfo.flags = 0;
    deviceInfo.queueCreateInfoCount = (uint32_t)queueInfos.size();
    deviceInfo.pQueueCreateInfos = queueInfos.data();
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;
    deviceInfo.enabledExtensionCount = (uint32_t)extensions.size();
    deviceInfo.ppEnabledExtensionNames = extensions.data();
    deviceInfo.pEnabledFeatures = nullptr;

    return deviceInfo;
}

VkDeviceQueueCreateInfo GetDeviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t count) {
    static float priorities[] = {1.0f};
    
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = nullptr;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = queueFamilyIndex;
    queueInfo.queueCount = count;
    queueInfo.pQueuePriorities = priorities;

    return queueInfo;
}

VkSwapchainCreateInfoKHR GetSwapchainCreateInfo() {
    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = nullptr;
    info.flags = 0;
    info.surface = context.surface;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.clipped = VK_TRUE;
    info.oldSwapchain = VK_NULL_HANDLE;

    return info;
}

VkImageViewCreateInfo GetImageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspectMask) {
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.image = image;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = format;
    info.components.r = VK_COMPONENT_SWIZZLE_R;
    info.components.g = VK_COMPONENT_SWIZZLE_G;
    info.components.b = VK_COMPONENT_SWIZZLE_B;
    info.components.a = VK_COMPONENT_SWIZZLE_A;
    info.subresourceRange.aspectMask = aspectMask;
    info.subresourceRange.baseMipLevel = 0;
    info.subresourceRange.levelCount = 1;
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;

    return info;
}

VkFenceCreateInfo GetFenceCreateInfo(VkFenceCreateFlags flags) {
    VkFenceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;

    return info;
}

VkSemaphoreCreateInfo GetSemaphoreCreateInfo() {
    VkSemaphoreCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;

    return info;
}

VkCommandPoolCreateInfo GetCommandPoolCreateInfo(VkCommandPoolCreateFlags flags, uint32_t familyIndex) {
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    info.queueFamilyIndex = familyIndex;

    return info;
}

VkCommandBufferAllocateInfo GetCommandBufferAllocateInfo(VkCommandPool pool, uint32_t count) {
    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = pool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = count;

    return info;
}

VkCommandBufferBeginInfo GetCommandBufferBeginInfo(VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.flags = flags;
    info.pInheritanceInfo = nullptr;

    return info;
}

VkSubmitInfo GetSubmitInfo(VkCommandBuffer *cmd, const std::vector<VkSemaphore> &waitSemaphores, const std::vector<VkPipelineStageFlags> &waitStages, const std::vector<VkSemaphore> &signalSemaphores) {
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.pNext = nullptr;
    info.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
    info.pWaitSemaphores = waitSemaphores.data();
    info.pWaitDstStageMask = waitStages.data();
    info.commandBufferCount = 1;
    info.pCommandBuffers = cmd;
    info.signalSemaphoreCount = (uint32_t)signalSemaphores.size();
    info.pSignalSemaphores = signalSemaphores.data();
    
    return info;
}

VkPresentInfoKHR GetPresentInfo(const std::vector<VkSemaphore> &waitSemaphores, VkSwapchainKHR *swapchain, uint32_t *imageIndex) {
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.pNext = nullptr;
    info.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
    info.pWaitSemaphores = waitSemaphores.data();
    info.swapchainCount = 1;
    info.pSwapchains = swapchain;
    info.pImageIndices = imageIndex;
    info.pResults = nullptr;

    return info;
}

VkRenderPassCreateInfo GetRenderPassCreateInfo(const std::vector<VkAttachmentDescription> &attachments, const std::vector<VkSubpassDescription> &subpasses, const std::vector<VkSubpassDependency> &dependencies) {
    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.attachmentCount = (uint32_t)attachments.size();
    info.pAttachments = attachments.data();
    info.subpassCount = (uint32_t)subpasses.size();
    info.pSubpasses = subpasses.data();
    info.dependencyCount = (uint32_t)dependencies.size();
    info.pDependencies = dependencies.data();

    return info;
}

VkRenderPassBeginInfo GetRenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent, const glm::vec4 color) {
    static VkClearValue clear = {};
    clear.color = {color.r, color.g, color.b, color.a};
    
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.pNext = nullptr;
    info.renderPass = renderPass;
    info.framebuffer = framebuffer;
    info.renderArea.extent = extent;
    info.renderArea.offset = {0, 0};
    info.clearValueCount = 1;
    info.pClearValues = &clear;

    return info;
}

VkFramebufferCreateInfo GetFramebufferCreateInfo(VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D extent) {
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.renderPass = renderPass;
    info.attachmentCount = (uint32_t)attachments.size();
    info.pAttachments = attachments.data();
    info.width = extent.width;
    info.height = extent.height;
    info.layers = 1;

    return info;
}

VkShaderModuleCreateInfo GetShaderModuleCreateInfo(const std::vector<uint32_t> &code) {
    VkShaderModuleCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.codeSize = (uint32_t)code.size();
    info.pCode = code.data();

    return info;
}

VkPipelineShaderStageCreateInfo GetPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module) {
    VkPipelineShaderStageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.stage = stage;
    info.module = module;
    info.pName = "main";
    info.pSpecializationInfo = nullptr;

    return info;
}

VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription> &bindings, const std::vector<VkVertexInputAttributeDescription> &attributes) {
    VkPipelineVertexInputStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.vertexBindingDescriptionCount = (uint32_t)bindings.size();
    info.pVertexBindingDescriptions = bindings.data();
    info.vertexAttributeDescriptionCount = (uint32_t)attributes.size();
    info.pVertexAttributeDescriptions = attributes.data();

    return info;
}

VkPipelineInputAssemblyStateCreateInfo GetPipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology) {
    VkPipelineInputAssemblyStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.topology = topology;
    info.primitiveRestartEnable = VK_FALSE;
}

VkPipelineTessellationStateCreateInfo GetPipelineTessellationStateCreateInfo() {
    VkPipelineTessellationStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.patchControlPoints = 0;

    return info;
}

VkPipelineViewportStateCreateInfo GetPipelineViewportStateCreateInfo(VkViewport view, VkRect2D sciss) {
    static VkViewport viewport = view;
    static VkRect2D scissor = sciss;
    
    VkPipelineViewportStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.viewportCount = 1;
    info.pViewports = &viewport;
    info.scissorCount = 1;
    info.pScissors = &scissor;

    return info;
}