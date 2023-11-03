#ifndef VKUTIL_H
#define VKUTIL_H

#include <Volk/volk.h>
#include <glm/glm.hpp>

#include <vector>

VkApplicationInfo GetApplicationInfo();
VkInstanceCreateInfo GetInstanceCreateInfo(VkApplicationInfo *appInfo, const std::vector<const char *> &layers, const std::vector<const char *> &extensions);
VkDeviceCreateInfo GetDeviceCreateInfo(const std::vector<VkDeviceQueueCreateInfo> &queueInfos, const std::vector<const char *> &extensions);
VkDeviceQueueCreateInfo GetDeviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t count);
VkSwapchainCreateInfoKHR GetSwapchainCreateInfo();
VkImageViewCreateInfo GetImageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
VkFenceCreateInfo GetFenceCreateInfo(VkFenceCreateFlags flags);
VkSemaphoreCreateInfo GetSemaphoreCreateInfo();
VkCommandPoolCreateInfo GetCommandPoolCreateInfo(VkCommandPoolCreateFlags flags, uint32_t familyIndex);
VkCommandBufferAllocateInfo GetCommandBufferAllocateInfo(VkCommandPool pool, uint32_t count);
VkCommandBufferBeginInfo GetCommandBufferBeginInfo(VkCommandBufferUsageFlags flags);
VkSubmitInfo GetSubmitInfo(VkCommandBuffer *cmd, const std::vector<VkSemaphore> &waitSemaphores, const std::vector<VkPipelineStageFlags> &waitStages, const std::vector<VkSemaphore> &signalSemaphores);
VkPresentInfoKHR GetPresentInfo(const std::vector<VkSemaphore> &waitSemaphores, VkSwapchainKHR *swapchain, uint32_t *imageIndex);
VkRenderPassCreateInfo GetRenderPassCreateInfo(const std::vector<VkAttachmentDescription> &attachments, const std::vector<VkSubpassDescription> &subpasses, const std::vector<VkSubpassDependency> &dependencies);
VkRenderPassBeginInfo GetRenderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent, const glm::vec4 color);
VkFramebufferCreateInfo GetFramebufferCreateInfo(VkRenderPass renderPass, const std::vector<VkImageView> &attachments, VkExtent2D extent);
VkShaderModuleCreateInfo GetShaderModuleCreateInfo(const std::vector<uint32_t> &code);

VkPipelineShaderStageCreateInfo GetPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule module);
VkPipelineVertexInputStateCreateInfo GetPipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription> &bindings, const std::vector<VkVertexInputAttributeDescription> &attributes);
VkPipelineInputAssemblyStateCreateInfo GetPipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology);
VkPipelineTessellationStateCreateInfo GetPipelineTessellationStateCreateInfo();
VkPipelineViewportStateCreateInfo GetPipelineViewportStateCreateInfo(VkViewport viewport, VkRect2D scissor);

#endif // VK_UTIL