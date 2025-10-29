#include "pipeline.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <filesystem>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_reflect.hpp>

#include "vkutil.h"
#include "context.h"

static std::vector<uint32_t> CompileShader(shaderc_shader_kind kind, const std::string &filename) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

#ifndef VOXEL_DEBUG
    options.SetOptimizationLevel(shaderc_optimization_level_size);
#endif

    std::ifstream file(filename);
    if (!file.is_open()) {
        printf("Failed to open shader file: %s\n", filename.c_str());
        assert(false);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    shaderc::SpvCompilationResult compResult = compiler.CompileGlslToSpv(buffer.str(), kind, filename.c_str());
    if (compResult.GetCompilationStatus() != shaderc_compilation_status_success) {
        printf("Failed to compile shader %s: %s\n", filename.c_str(), compResult.GetErrorMessage().c_str());
        assert(false);
    }

    return {compResult.begin(), compResult.end()};
}

static inline shaderc_shader_kind ExtToKind(const std::string &ext) {
    if (ext == ".vert") return shaderc_vertex_shader;
    else if (ext == ".frag") return shaderc_fragment_shader;
    else if (ext == ".comp") return shaderc_compute_shader;

    printf("Failed to detect shader kind: %s\n", ext.c_str());
    assert(false);

    return (shaderc_shader_kind)0;
}

static inline VkShaderStageFlagBits KindToStage(shaderc_shader_kind kind) {
    switch (kind) {
        case shaderc_vertex_shader: return VK_SHADER_STAGE_VERTEX_BIT;
        case shaderc_fragment_shader: return VK_SHADER_STAGE_FRAGMENT_BIT;
        case shaderc_compute_shader: return VK_SHADER_STAGE_COMPUTE_BIT;
        default: {
            printf("Unknown shaderc_shader_kind: %d\n", (int)kind);
            assert(false);
        }
    }

    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
}

Pipeline CreateGraphicsPipeline(const std::vector<std::string> &shaderPaths, VkRenderPass renderPass) {
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    std::vector<uint32_t> fragCode;

    for (auto &shaderPath : shaderPaths) {
        std::filesystem::path p(shaderPath);
        auto ext = p.extension().string();
        shaderc_shader_kind kind = ExtToKind(ext);
        auto code = CompileShader(kind, shaderPath);
        if (kind == shaderc_fragment_shader) fragCode = code;
        
        VkShaderModule mod;
        VkShaderModuleCreateInfo moduleInfo = GetShaderModuleCreateInfo(code);
        vkCreateShaderModule(context.device, &moduleInfo, nullptr, &mod);

        VkShaderStageFlagBits stage = KindToStage(kind);
        VkPipelineShaderStageCreateInfo stageInfo = GetPipelineShaderStageCreateInfo(stage, mod);
        stages.push_back(stageInfo);
    }
    
    Pipeline pipeline = {};

    spirv_cross::Compiler comp(std::move(fragCode));
    spirv_cross::ShaderResources resources = comp.get_shader_resources();

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto &image : resources.sampled_images) {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = comp.get_decoration(image.id, spv::DecorationBinding);
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo setLayoutInfo = GetDescriptorsetLayoutCreatInfo(bindings);
    vkCreateDescriptorSetLayout(context.device, &setLayoutInfo, nullptr, &pipeline.setLayout);

    std::vector<VkDescriptorSetLayout> layouts = {pipeline.setLayout};
    VkDescriptorSetAllocateInfo setAllocInfo = GetDescriptorSetAllocateInfo(context.descriptorPool, layouts);
    vkAllocateDescriptorSets(context.device, &setAllocInfo, &pipeline.set);

    VkPipelineLayoutCreateInfo layoutInfo = GetPipelineLayoutCreateInfo(layouts, {});
    vkCreatePipelineLayout(context.device, &layoutInfo, nullptr, &pipeline.layout);

    VkRect2D scissor = {};
    scissor.extent = context.swapchain.extent;
    scissor.offset = {0, 0};

    VkViewport viewport = {};
    viewport.width = (float)scissor.extent.width;
    viewport.height = (float)scissor.extent.height;
    viewport.x = 0;
    viewport.y = 0;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = GetPipelineVertexInputStateCreateInfo({}, {});
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = GetPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
    VkPipelineTessellationStateCreateInfo tessellationInfo = GetPipelineTessellationStateCreateInfo();
    VkPipelineViewportStateCreateInfo viewportInfo = GetPipelineViewportStateCreateInfo(viewport, scissor);
    VkPipelineRasterizationStateCreateInfo rasterizationInfo = GetPipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, 1.0f);
    VkPipelineMultisampleStateCreateInfo multisampleInfo = GetPipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = GetPipelineDepthStencilStateCreateInfo();
    VkPipelineColorBlendStateCreateInfo colorBlendInfo = GetPipelineColorBlendstateCreateInfo();
    VkPipelineDynamicStateCreateInfo dynamicInfo = GetPipelineDynamicStateCreateInfo({});

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.stageCount = (uint32_t)stages.size();
    pipelineInfo.pStages = stages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pTessellationState = &tessellationInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterizationInfo;
    pipelineInfo.pMultisampleState = &multisampleInfo;
    pipelineInfo.pDepthStencilState = &depthStencilInfo;
    pipelineInfo.pColorBlendState = &colorBlendInfo;
    pipelineInfo.pDynamicState = &dynamicInfo;
    pipelineInfo.layout = pipeline.layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;

    vkCreateGraphicsPipelines(context.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline);

    return pipeline;
}

Pipeline CreateComputePipeline(const std::string &shaderPath) {
    std::vector<uint32_t> code = CompileShader(shaderc_compute_shader, shaderPath);
    VkShaderModuleCreateInfo moduleInfo = GetShaderModuleCreateInfo(code);
    VkShaderModule mod;
    vkCreateShaderModule(context.device, &moduleInfo, nullptr, &mod);
    VkPipelineShaderStageCreateInfo stageInfo = GetPipelineShaderStageCreateInfo(VK_SHADER_STAGE_COMPUTE_BIT, mod);

    spirv_cross::Compiler comp(std::move(code));
    spirv_cross::ShaderResources resources = comp.get_shader_resources();

    Pipeline pipeline = {};

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (const auto &image : resources.storage_images) {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = comp.get_decoration(image.id, spv::DecorationBinding);
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }

    for (const auto &buffer : resources.storage_buffers) {
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = comp.get_decoration(buffer.id, spv::DecorationBinding);
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        binding.pImmutableSamplers = nullptr;
        bindings.push_back(binding);
    }


    std::vector<VkPushConstantRange> pushRanges;

    const auto &push = resources.push_constant_buffers;
    if (!push.empty()) {
        uint32_t id = push[0].id;
        for (auto &ranges : comp.get_active_buffer_ranges(id)) {
            VkPushConstantRange range = {};
            range.size = ranges.range;
            range.offset = ranges.offset;
            range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
            pushRanges.push_back(range);
        }
    }

    VkDescriptorSetLayoutCreateInfo setLayoutInfo = GetDescriptorsetLayoutCreatInfo(bindings);
    vkCreateDescriptorSetLayout(context.device, &setLayoutInfo, nullptr, &pipeline.setLayout);

    std::vector<VkDescriptorSetLayout> layouts = {pipeline.setLayout};
    VkDescriptorSetAllocateInfo setAllocInfo = GetDescriptorSetAllocateInfo(context.descriptorPool, layouts);
    vkAllocateDescriptorSets(context.device, &setAllocInfo, &pipeline.set);

    VkPipelineLayoutCreateInfo layoutInfo = GetPipelineLayoutCreateInfo(layouts, pushRanges);
    vkCreatePipelineLayout(context.device, &layoutInfo, nullptr, &pipeline.layout);

    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.flags = 0;
    pipelineInfo.stage = stageInfo;
    pipelineInfo.layout = pipeline.layout;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;

    vkCreateComputePipelines(context.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline);

    return pipeline;
}