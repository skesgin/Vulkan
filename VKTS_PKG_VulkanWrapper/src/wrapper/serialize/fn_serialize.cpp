/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <vkts/vulkan/wrapper/vkts_wrapper.hpp>

namespace vkts
{

uint32_t VKTS_APIENTRY serializeGetStructureTypeSize(const void* ptr)
{
    if (!ptr)
    {
        return 0;
    }

    const VkTsStructureTypeHeader* structureTypeHeader = (const VkTsStructureTypeHeader*) ptr;

    switch (structureTypeHeader->sType)
    {
        case VK_STRUCTURE_TYPE_APPLICATION_INFO:
            return sizeof(VkApplicationInfo);
        case VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO:
            return sizeof(VkInstanceCreateInfo);
        case VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO:
            return sizeof(VkDeviceQueueCreateInfo);
        case VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO:
            return sizeof(VkDeviceCreateInfo);
        case VK_STRUCTURE_TYPE_SUBMIT_INFO:
            return sizeof(VkSubmitInfo);
        case VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO:
            return sizeof(VkMemoryAllocateInfo);
        case VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE:
            return sizeof(VkMappedMemoryRange);
        case VK_STRUCTURE_TYPE_BIND_SPARSE_INFO:
            return sizeof(VkBindSparseInfo);
        case VK_STRUCTURE_TYPE_FENCE_CREATE_INFO:
            return sizeof(VkFenceCreateInfo);
        case VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO:
            return sizeof(VkSemaphoreCreateInfo);
        case VK_STRUCTURE_TYPE_EVENT_CREATE_INFO:
            return sizeof(VkEventCreateInfo);
        case VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO:
            return sizeof(VkQueryPoolCreateInfo);
        case VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO:
            return sizeof(VkBufferCreateInfo);
        case VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO:
            return sizeof(VkBufferViewCreateInfo);
        case VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO:
            return sizeof(VkImageCreateInfo);
        case VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO:
            return sizeof(VkImageViewCreateInfo);
        case VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO:
            return sizeof(VkShaderModuleCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO:
            return sizeof(VkPipelineCacheCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO:
            return sizeof(VkPipelineShaderStageCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO:
            return sizeof(VkPipelineVertexInputStateCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO:
            return sizeof(VkPipelineInputAssemblyStateCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO:
            return sizeof(VkPipelineTessellationStateCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO:
            return sizeof(VkPipelineViewportStateCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO:
            return sizeof(VkPipelineRasterizationStateCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO:
            return sizeof(VkPipelineMultisampleStateCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO:
            return sizeof(VkPipelineDepthStencilStateCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO:
            return sizeof(VkPipelineColorBlendStateCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO:
            return sizeof(VkPipelineDynamicStateCreateInfo);
        case VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO:
            return sizeof(VkGraphicsPipelineCreateInfo);
        case VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO:
            return sizeof(VkComputePipelineCreateInfo);
        case VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO:
            return sizeof(VkPipelineLayoutCreateInfo);
        case VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO:
            return sizeof(VkSamplerCreateInfo);
        case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO:
            return sizeof(VkDescriptorSetLayoutCreateInfo);
        case VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO:
            return sizeof(VkDescriptorPoolCreateInfo);
        case VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO:
            return sizeof(VkDescriptorSetAllocateInfo);
        case VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET:
            return sizeof(VkWriteDescriptorSet);
        case VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET:
            return sizeof(VkCopyDescriptorSet);
        case VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO:
            return sizeof(VkFramebufferCreateInfo);
        case VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO:
            return sizeof(VkRenderPassCreateInfo);
        case VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO:
            return sizeof(VkCommandPoolCreateInfo);
        case VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO:
            return sizeof(VkCommandBufferAllocateInfo);
        case VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO:
            return sizeof(VkCommandBufferBeginInfo);
        case VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO:
            return sizeof(VkRenderPassBeginInfo);
        case VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER:
            return sizeof(VkBufferMemoryBarrier);
        case VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER:
            return sizeof(VkImageMemoryBarrier);
        case VK_STRUCTURE_TYPE_MEMORY_BARRIER:
            return sizeof(VkMemoryBarrier);

        default:
            logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Structure type not found 0x%x", structureTypeHeader->sType);
    }

    return 0;
}

IBinaryBufferSP VKTS_APIENTRY serializeStructureType(const void* ptr)
{
    if (!ptr)
    {
        return IBinaryBufferSP();
    }

    uint32_t totalSize = 0;

    const VkTsStructureTypeHeader* sourceStructureTypeHeader = (const VkTsStructureTypeHeader*) ptr;

    while (sourceStructureTypeHeader)
    {
        totalSize += serializeGetStructureTypeSize(sourceStructureTypeHeader);

        sourceStructureTypeHeader = (const VkTsStructureTypeHeader*) sourceStructureTypeHeader->pNext;
    }

    //

    auto binaryBuffer = binaryBufferCreate(totalSize);

    if (!binaryBuffer.get())
    {
    	return IBinaryBufferSP();
    }

    binaryBuffer->seek(0, VKTS_SEARCH_ABSOLUTE);

    //

    sourceStructureTypeHeader = (const VkTsStructureTypeHeader*)ptr;

    VkTsStructureTypeHeader* targetStructureTypeHeader = nullptr;

    while (sourceStructureTypeHeader)
    {
        targetStructureTypeHeader = (VkTsStructureTypeHeader*)binaryBuffer->getCurrentData();

        uint32_t currentSize = serializeGetStructureTypeSize(sourceStructureTypeHeader);

        binaryBuffer->write((void*)sourceStructureTypeHeader, 1, currentSize);

        if (sourceStructureTypeHeader->pNext)
        {
            targetStructureTypeHeader->pNext = binaryBuffer->getCurrentData();
        }

        sourceStructureTypeHeader = (const VkTsStructureTypeHeader*)sourceStructureTypeHeader->pNext;
    }

    return binaryBuffer;
}

}
