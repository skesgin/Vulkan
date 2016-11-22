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

#ifndef VKTS_VK_HPP_
#define VKTS_VK_HPP_

/**
 *
 * Depends on VKTS core.
 *
 */

#include <vkts/core.hpp>

/**
 *
 * VKTS Start.
 *
 */

/**
 * Defines.
 */

#define VKTS_ENGINE_NAME            "VKTS"
#define VKTS_ENGINE_MAJOR           1
#define VKTS_ENGINE_MINOR           0
#define VKTS_ENGINE_PATCH           30

/**
 * Types.
 */

typedef enum VkTsVertexBufferTypeBits_
{
    VKTS_VERTEX_BUFFER_TYPE_VERTEX = 0x00000001,
    VKTS_VERTEX_BUFFER_TYPE_NORMAL = 0x00000002,
    VKTS_VERTEX_BUFFER_TYPE_BITANGENT = 0x00000004,
    VKTS_VERTEX_BUFFER_TYPE_TANGENT = 0x00000008,
    VKTS_VERTEX_BUFFER_TYPE_TEXCOORD = 0x00000010,
    VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0 = 0x00000020,
    VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1 = 0x00000040,
    VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0 = 0x00000080,
    VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1 = 0x00000100,
    VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS = 0x00000200,

    VKTS_VERTEX_BUFFER_TYPE_TANGENTS = VKTS_VERTEX_BUFFER_TYPE_NORMAL | VKTS_VERTEX_BUFFER_TYPE_BITANGENT | VKTS_VERTEX_BUFFER_TYPE_TANGENT,

    VKTS_VERTEX_BUFFER_TYPE_BONES = VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0 | VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1 | VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0 | VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1 | VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS
} VkTsVertexBufferTypeBits;

typedef VkFlags VkTsVertexBufferType;

typedef struct VkTsStructureTypeHeader_
{
    VkStructureType sType;
    const void* pNext;
} VkTsStructureTypeHeader;

/**
 * Alignment.
 */

#include <vkts/vk/alignment/fn_alignment.hpp>

/**
 * Serialize.
 */

#include <vkts/vk/serialize/fn_serialize.hpp>

/**
 * Extensions and layers.
 */

#include <vkts/vk/extension/fn_extension.hpp>

#include <vkts/vk/extension/fn_layer.hpp>

#include <vkts/vk/extension/fn_debug.hpp>
#include <vkts/vk/extension/fn_validation.hpp>

/**
 * Instance.
 */

#include <vkts/vk/instance/IInstance.hpp>

#include <vkts/vk/instance/fn_instance.hpp>

/**
 * Physical device.
 */

#include <vkts/vk/physical_device/IPhysicalDevice.hpp>

#include <vkts/vk/physical_device/fn_physical_device.hpp>

/**
 * Device.
 */

#include <vkts/vk/device/IDevice.hpp>

#include <vkts/vk/device/fn_device.hpp>

/**
 * Queue.
 */

#include <vkts/vk/queue/IQueue.hpp>

#include <vkts/vk/queue/fn_queue.hpp>

/**
 * Command.
 */

#include <vkts/vk/command/ICommandBuffers.hpp>
#include <vkts/vk/command/ICommandPool.hpp>

#include <vkts/vk/command/fn_command.hpp>

/**
 * Fence.
 */

#include <vkts/vk/fence/IFence.hpp>

#include <vkts/vk/fence/fn_fence.hpp>

/**
 * Event.
 */

#include <vkts/vk/event/IEvent.hpp>

#include <vkts/vk/event/fn_event.hpp>

/**
 * Semaphore.
 */

#include <vkts/vk/semaphore/ISemaphore.hpp>

#include <vkts/vk/semaphore/fn_semaphore.hpp>

/**
 * Shader module.
 */

#include <vkts/vk/shader_module/IShaderModule.hpp>

#include <vkts/vk/shader_module/fn_shader_module.hpp>

/**
 * Buffer.
 */

#include <vkts/vk/buffer/IBuffer.hpp>
#include <vkts/vk/buffer/IBufferView.hpp>

#include <vkts/vk/buffer/fn_buffer.hpp>

/**
 * Image.
 */

#include <vkts/vk/image/IImage.hpp>

#include <vkts/vk/image/fn_image.hpp>

/**
 * Image view.
 */

#include <vkts/vk/image_view/IImageView.hpp>

#include <vkts/vk/image_view/fn_image_view.hpp>

/**
 * Sampler.
 */

#include <vkts/vk/sampler/ISampler.hpp>

#include <vkts/vk/sampler/fn_sampler.hpp>

/**
 * Device memory.
 */

#include <vkts/vk/device_memory/IDeviceMemory.hpp>

#include <vkts/vk/device_memory/fn_device_memory.hpp>

/**
 * Descriptor.
 */

#include <vkts/vk/descriptor/IDescriptorSetLayout.hpp>
#include <vkts/vk/descriptor/IDescriptorPool.hpp>
#include <vkts/vk/descriptor/IDescriptorSets.hpp>

#include <vkts/vk/descriptor/fn_descriptor.hpp>

/**
 * Render pass.
 */

#include <vkts/vk/render_pass/IRenderPass.hpp>

#include <vkts/vk/render_pass/fn_render_pass.hpp>

/**
 * Frame buffer.
 */

#include <vkts/vk/framebuffer/IFramebuffer.hpp>

#include <vkts/vk/framebuffer/fn_framebuffer.hpp>

/**
 * Pipeline.
 */

#include <vkts/vk/pipeline/IComputePipeline.hpp>
#include <vkts/vk/pipeline/IGraphicsPipeline.hpp>
#include <vkts/vk/pipeline/IPipelineCache.hpp>
#include <vkts/vk/pipeline/IPipelineLayout.hpp>

#include <vkts/vk/pipeline/DefaultComputePipeline.hpp>
#include <vkts/vk/pipeline/DefaultGraphicsPipeline.hpp>

#include <vkts/vk/pipeline/fn_pipeline.hpp>

#endif /* VKTS_VK_HPP_ */
