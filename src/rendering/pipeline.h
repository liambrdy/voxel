#ifndef PIPELINE_H
#define PIPELINE_H

#include <Volk/volk.h>

#include <vector>
#include <string>

struct Pipeline {
    VkPipeline pipeline;
    VkPipelineLayout layout;

    VkDescriptorSetLayout setLayout;
    VkDescriptorSet set;
};

Pipeline CreateGraphicsPipeline(const std::vector<std::string> &shaderPaths, VkRenderPass renderPass);
Pipeline CreateComputePipeline(const std::string &shaderPath);

#endif // PIPELINE_H