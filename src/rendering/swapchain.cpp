#include "swapchain.h"

#include "context.h"
#include "vkutil.h"

static VkSurfaceFormatKHR GetSwapchainSurfaceFormat() {
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &formatCount, formats.data());

    for (const auto &f : formats) {
        if (f.format == VK_FORMAT_R8G8B8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return f;
    }

    return formats[0];
}

static VkPresentModeKHR GetSwapchainPresentMode(bool vsync) {
    uint32_t modeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &modeCount, nullptr);
    std::vector<VkPresentModeKHR> modes(modeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &modeCount, modes.data());

    for (const auto &m : modes) {
        if (!vsync && m == VK_PRESENT_MODE_IMMEDIATE_KHR)
            return m;
        if (vsync && m == VK_PRESENT_MODE_MAILBOX_KHR)
            return m;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

Result CreateSwapchain(Swapchain *swapchain, bool vsync) {
    swapchain->vsync = vsync;

    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.physicalDevice, context.surface, &caps);

    swapchain->imageCount = caps.minImageCount + 1;
    swapchain->extent = caps.currentExtent;

    swapchain->surfaceFormat = GetSwapchainSurfaceFormat();
    swapchain->presentMode = GetSwapchainPresentMode(vsync);

    VkSwapchainCreateInfoKHR swapchainInfo = GetSwapchainCreateInfo();

    swapchainInfo.minImageCount = swapchain->imageCount;
    swapchainInfo.imageFormat = swapchain->surfaceFormat.format;
    swapchainInfo.imageColorSpace = swapchain->surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = swapchain->extent;
    swapchainInfo.presentMode = swapchain->presentMode;

    VkCheck(vkCreateSwapchainKHR(context.device, &swapchainInfo, nullptr, &swapchain->swapchain));

    uint32_t swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(context.device, swapchain->swapchain, &swapchainImageCount, nullptr);
    swapchain->imageCount = swapchainImageCount;
    swapchain->images.resize(swapchain->imageCount);
    vkGetSwapchainImagesKHR(context.device, swapchain->swapchain, &swapchainImageCount, swapchain->images.data());

    swapchain->imageViews.resize(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        VkImageViewCreateInfo viewInfo = GetImageViewCreateInfo(swapchain->images[i], swapchain->surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

        VkCheck(vkCreateImageView(context.device, &viewInfo, nullptr, &swapchain->imageViews[i]));
    }

    return Success;
}

void SwapchainDestroy(Swapchain *swapchain) {

}
