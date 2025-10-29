#ifndef IMAGE_H
#define IMAGE_H

#include <Volk/volk.h>

#include <vma/vk_mem_alloc.h>

struct Image {
    VkImage image;
    VkImageView view;
    VmaAllocation alloc;

    VkFormat format;
    uint32_t width, height;
};

Image CreateImage(VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height);

void SetImageLayout(VkCommandBuffer cmd, Image image, VkImageLayout srcLayout, VkImageLayout dstLayout);

#endif // IMAGE_H