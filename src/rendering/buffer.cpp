#include "buffer.h"

#include "context.h"
#include "vkutil.h"

Buffer CreateBuffer(uint32_t size, VkBufferUsageFlags usage, VmaMemoryUsage memUsage) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 0;
    bufferInfo.pQueueFamilyIndices = nullptr;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = memUsage;

    Buffer buffer = {};
    buffer.size = size;
    vmaCreateBuffer(context.allocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.alloc, nullptr);

    return buffer;
}

void CopyToBuffer(Buffer *buffer, uint8_t *data, uint32_t dataCount) {
    Buffer staging = CreateBuffer(dataCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    
    uint8_t *stagingData;
    vmaMapMemory(context.allocator, staging.alloc, (void **)&stagingData);
    std::memcpy(stagingData, data, dataCount);
    vmaUnmapMemory(context.allocator, staging.alloc);
    vmaFlushAllocation(context.allocator, staging.alloc, 0, VK_WHOLE_SIZE);

    VkCommandBuffer cmd = BeginSingleUseCmd();
    VkBufferCopy copy = {};
    copy.srcOffset = 0;
    copy.dstOffset = 0;
    copy.size = dataCount;

    vkCmdCopyBuffer(cmd, staging.buffer, buffer->buffer, 1, &copy);

    EndSingleUseCmd(cmd);
}