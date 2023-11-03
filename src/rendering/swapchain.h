#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <Volk/volk.h>

#include <vector>

struct Swapchain {
    VkSwapchainKHR swapchain;

    VkExtent2D extent;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    uint32_t imageCount;

    bool vsync;

    std::vector<VkImageView> imageViews;
    std::vector<VkImage> images;
};

enum Result;

Result CreateSwapchain(Swapchain *swapchain, bool vsync);

#endif // SWAPCHAIN_H