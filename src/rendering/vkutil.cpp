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
    info.codeSize = 4 * (uint32_t)code.size();
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

    return info;
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

VkPipelineRasterizationStateCreateInfo GetPipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, float lineWidth) {
    VkPipelineRasterizationStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.depthClampEnable = VK_FALSE;
    info.rasterizerDiscardEnable = VK_FALSE;
    info.polygonMode = polygonMode;
    info.cullMode = VK_CULL_MODE_NONE;
    info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    info.depthBiasEnable = VK_FALSE;
    info.depthBiasConstantFactor = 0.0f;
    info.depthBiasClamp = 0.0f;
    info.depthBiasSlopeFactor = 0.0f;
    info.lineWidth = lineWidth;

    return info;
}

VkPipelineMultisampleStateCreateInfo GetPipelineMultisampleStateCreateInfo(VkSampleCountFlagBits samples) {
    static VkSampleMask mask = 0xFFFFFFFF;

    VkPipelineMultisampleStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.rasterizationSamples = samples;
    info.sampleShadingEnable = VK_FALSE;
    info.minSampleShading = 0.0f;
    info.pSampleMask = &mask;
    info.alphaToCoverageEnable = VK_FALSE;
    info.alphaToOneEnable = VK_FALSE;

    return info;
}

VkPipelineDepthStencilStateCreateInfo GetPipelineDepthStencilStateCreateInfo() {
    VkPipelineDepthStencilStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.depthTestEnable = VK_FALSE;
    info.depthWriteEnable = VK_FALSE;
    info.depthCompareOp = VK_COMPARE_OP_NEVER;
    info.depthBoundsTestEnable = VK_FALSE;
    info.stencilTestEnable = VK_FALSE;
    info.front = {};
    info.back = {};
    info.minDepthBounds = 0.0f;
    info.maxDepthBounds = 0.0f;

    return info;
}

VkPipelineColorBlendStateCreateInfo GetPipelineColorBlendstateCreateInfo() {
    static VkPipelineColorBlendAttachmentState attachment = {};
    attachment.blendEnable = VK_FALSE;
    attachment.srcColorBlendFactor = VK_BLEND_FACTOR_CONSTANT_ALPHA;
    attachment.dstColorBlendFactor = VK_BLEND_FACTOR_CONSTANT_ALPHA;
    attachment.colorBlendOp = VK_BLEND_OP_ADD;
    attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_CONSTANT_ALPHA;
    attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_CONSTANT_ALPHA;
    attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    
    VkPipelineColorBlendStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.logicOpEnable = VK_FALSE;
    info.logicOp = VK_LOGIC_OP_AND;
    info.attachmentCount = 1;
    info.pAttachments = &attachment;

    return info;
}

VkPipelineDynamicStateCreateInfo GetPipelineDynamicStateCreateInfo(const std::vector<VkDynamicState> &states) {
    VkPipelineDynamicStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.dynamicStateCount = (uint32_t)states.size();
    info.pDynamicStates = states.data();

    return info;
}

VkPipelineLayoutCreateInfo GetPipelineLayoutCreateInfo(const std::vector<VkDescriptorSetLayout> &setLayouts, const std::vector<VkPushConstantRange> &pushConstants) {
    VkPipelineLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.setLayoutCount = (uint32_t)setLayouts.size();
    info.pSetLayouts = setLayouts.data();
    info.pushConstantRangeCount = (uint32_t)pushConstants.size();
    info.pPushConstantRanges = pushConstants.data();

    return info;
}

VkDescriptorPoolCreateInfo GetDescriptorPoolCreateInfo(uint32_t maxSets, const std::vector<VkDescriptorPoolSize> &poolSizes) {
    VkDescriptorPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.maxSets = maxSets;
    info.poolSizeCount = (uint32_t)poolSizes.size();
    info.pPoolSizes = poolSizes.data();

    return info;
}

VkDescriptorSetLayoutCreateInfo GetDescriptorsetLayoutCreatInfo(const std::vector<VkDescriptorSetLayoutBinding> &bindings) {
    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.bindingCount = (uint32_t)bindings.size();
    info.pBindings = bindings.data();

    return info;
}

VkDescriptorSetAllocateInfo GetDescriptorSetAllocateInfo(VkDescriptorPool pool, const std::vector<VkDescriptorSetLayout> &layouts) {
    VkDescriptorSetAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.descriptorPool = pool;
    info.descriptorSetCount = (uint32_t)layouts.size();
    info.pSetLayouts = layouts.data();

    return info;
}

VkSamplerCreateInfo GetSamplerCreateInfo() {
    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.magFilter = VK_FILTER_NEAREST;
    info.minFilter = VK_FILTER_NEAREST;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    info.mipLodBias = 1.0f;
    info.anisotropyEnable = VK_FALSE;
    info.maxAnisotropy = 0.0f;
    info.compareEnable = VK_FALSE;
    info.compareOp = VK_COMPARE_OP_NEVER;
    info.minLod = 0.0f;
    info.maxLod = 1.0f;
    info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    info.unnormalizedCoordinates = VK_FALSE;
    
    return info;
}

VkSemaphore CreateSemaphore() {
    VkSemaphoreCreateInfo info = GetSemaphoreCreateInfo();
    VkSemaphore semaphore;
    vkCreateSemaphore(context.device, &info, nullptr, &semaphore);
    return semaphore;
}

VkFence CreateFence(VkFenceCreateFlags flags) {
    VkFenceCreateInfo info = GetFenceCreateInfo(flags);
    VkFence fence;
    vkCreateFence(context.device, &info, nullptr, &fence);
    return fence;
}

VkCommandBuffer AllocateCommandBuffer() {
    VkCommandBufferAllocateInfo info = GetCommandBufferAllocateInfo(context.commandPool, 1);
    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(context.device, &info, &cmd);
    return cmd;
}

VkCommandBuffer BeginSingleUseCmd() {
    VkCommandBuffer cmd = AllocateCommandBuffer();
    VkCommandBufferBeginInfo info = GetCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    vkBeginCommandBuffer(cmd, &info);

    return cmd;
}

void EndSingleUseCmd(VkCommandBuffer cmd) {
    vkEndCommandBuffer(cmd);

    VkSubmitInfo info = GetSubmitInfo(&cmd, {}, {}, {});
    vkQueueSubmit(context.queue, 1, &info, VK_NULL_HANDLE);
    vkQueueWaitIdle(context.queue);
}