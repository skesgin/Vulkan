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

#include "RenderNode.hpp"

namespace vkts
{

void RenderNode::reset()
{
    memset(&transformDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
    memset(&transformWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));

    memset(&jointDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
    memset(&jointWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));
}

RenderNode::RenderNode() :
    IRenderNode(), transformDescriptorBufferInfo{}, transformWriteDescriptorSet{}, jointDescriptorBufferInfo{}, jointWriteDescriptorSet{}
{
    reset();
}

RenderNode::RenderNode(const RenderNode& other) :
    IRenderNode(), transformDescriptorBufferInfo{}, transformWriteDescriptorSet{}, jointDescriptorBufferInfo{}, jointWriteDescriptorSet{}
{
	reset();
}

RenderNode::~RenderNode()
{
    destroy();
}

void RenderNode::updateTransformDescriptorBufferInfo(const VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range)
{
    memset(&transformDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));

    transformDescriptorBufferInfo.buffer = buffer;
    transformDescriptorBufferInfo.offset = offset;
    transformDescriptorBufferInfo.range = range;

    memset(&transformWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));

    transformWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    transformWriteDescriptorSet.dstSet = VK_NULL_HANDLE; // Defined later.
    transformWriteDescriptorSet.dstBinding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;
    transformWriteDescriptorSet.dstArrayElement = 0;
    transformWriteDescriptorSet.descriptorCount = 1;
    transformWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    transformWriteDescriptorSet.pImageInfo = nullptr;
    transformWriteDescriptorSet.pBufferInfo = &transformDescriptorBufferInfo;
    transformWriteDescriptorSet.pTexelBufferView = nullptr;
}

void RenderNode::updateJointDescriptorBufferInfo(const VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range)
{
    memset(&jointDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));

    jointDescriptorBufferInfo.buffer = buffer;
    jointDescriptorBufferInfo.offset = offset;
    jointDescriptorBufferInfo.range = range;

    memset(&jointWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));

    jointWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    jointWriteDescriptorSet.dstSet = VK_NULL_HANDLE; // Defined later.
    jointWriteDescriptorSet.dstBinding = VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM;
    jointWriteDescriptorSet.dstArrayElement = 0;
    jointWriteDescriptorSet.descriptorCount = 1;
    jointWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    jointWriteDescriptorSet.pImageInfo = nullptr;
    jointWriteDescriptorSet.pBufferInfo = &jointDescriptorBufferInfo;
    jointWriteDescriptorSet.pTexelBufferView = nullptr;
}

void RenderNode::updateTransformUniformBuffer(const IBufferObjectSP& transformUniformBuffer)
{
	updateTransformDescriptorBufferInfo(transformUniformBuffer->getBuffer()->getBuffer(), 0, transformUniformBuffer->getBuffer()->getSize());
}

void RenderNode::updateJointsUniformBuffer(const IBufferObjectSP& jointsUniformBuffer)
{
	updateJointDescriptorBufferInfo(jointsUniformBuffer->getBuffer()->getBuffer(), 0, jointsUniformBuffer->getBuffer()->getSize());
}

void RenderNode::updateDescriptorSets(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets)
{
	for (uint32_t i = 0; i < allWriteDescriptorSetsCount; i++)
	{
		if (transformWriteDescriptorSet.dstBinding == VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM && transformWriteDescriptorSet.sType == VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET)
		{
			if (allWriteDescriptorSets[i].dstBinding == VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM)
			{
				allWriteDescriptorSets[i] = transformWriteDescriptorSet;
			}
		}

		if (jointWriteDescriptorSet.dstBinding == VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM && jointWriteDescriptorSet.sType == VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET)
		{
			if (allWriteDescriptorSets[i].dstBinding == VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM)
			{
				allWriteDescriptorSets[i] = jointWriteDescriptorSet;
			}
		}
	}
}

//
// ICloneable
//

IRenderNodeSP RenderNode::create(const VkBool32 createData) const
{
	auto result = IRenderNodeSP(new RenderNode(*this));

	if (result.get())
	{
		return IRenderNodeSP();
	}

    return result;
}

//
// IDestroyable
//

void RenderNode::destroy()
{
	reset();
}

} /* namespace vkts */
