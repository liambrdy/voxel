#ifndef BUFFER_H
#define BUFFER_H

#include <Volk/volk.h>
#include <vma/vk_mem_alloc.h>

struct Buffer {
    VkBuffer buffer;
    VmaAllocation alloc;
    uint32_t size;
};

Buffer CreateBuffer(uint32_t size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);
void CopyToBuffer(Buffer *buffer, uint8_t *data, uint32_t dataCount);

#endif // BUFFER_H