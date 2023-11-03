#include "pipeline.h"

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <filesystem>

#include <shaderc/shaderc.hpp>

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
    if (ext == "vert") return shaderc_vertex_shader;
    else if (ext == "frag") return shaderc_fragment_shader;
    else if (ext == "comp") return shaderc_compute_shader;

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

    for (auto &shaderPath : shaderPaths) {
        std::filesystem::path p(shaderPath);
        auto ext = p.extension().string();
        shaderc_shader_kind kind = ExtToKind(ext);
        auto code = CompileShader(kind, shaderPath);
        
        VkShaderModule mod;
        VkShaderModuleCreateInfo moduleInfo = GetShaderModuleCreateInfo(code);
        vkCreateShaderModule(context.device, &moduleInfo, nullptr, &mod);

        VkShaderStageFlagBits stage = KindToStage(kind);
        VkPipelineShaderStageCreateInfo stageInfo = GetPipelineShaderStageCreateInfo(stage, mod);
        stages.push_back(stageInfo);
    }

    VkRect2D scissor = {};
    scissor.extent = context.swapchain.extent;
    scissor.offset = {0, 0};

    VkViewport viewport = {};
    viewport.width = scissor.extent.width;
    viewport.height = scissor.extent.height;
    viewport.x = 0;
    viewport.y = 0;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = GetPipelineVertexInputStateCreateInfo({}, {});
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = GetPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
    VkPipelineTessellationStateCreateInfo tessellationInfo = GetPipelineTessellationStateCreateInfo();
    VkPipelineViewportStateCreateInfo viewportInfo = GetPipelineViewportStateCreateInfo(viewport, scissor);
    VkPipelineRasterizationStateCreateInfo rasterizationInfo

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
    pipelineInfo.pRasterizationState;
    pipelineInfo.pMultisampleState;
    pipelineInfo.pDepthStencilState;
    pipelineInfo.pColorBlendState;
    pipelineInfo.pDynamicState;
    pipelineInfo.layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;
}