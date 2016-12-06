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

#ifndef VKTS_RENDERNODE_HPP_
#define VKTS_RENDERNODE_HPP_

#include <vkts/vulkan/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class RenderNode: public IRenderNode
{

private:

    VkDescriptorBufferInfo transformDescriptorBufferInfo;
    VkWriteDescriptorSet transformWriteDescriptorSet;

    VkDescriptorBufferInfo jointDescriptorBufferInfo;
    VkWriteDescriptorSet jointWriteDescriptorSet;

    void updateTransformDescriptorBufferInfo(const VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range);

    void updateJointDescriptorBufferInfo(const VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range);

public:

    RenderNode();
    RenderNode(const RenderNode& other);
    RenderNode(RenderNode&& other) = delete;
    virtual ~RenderNode();

    RenderNode& operator =(const RenderNode& other) = delete;
    RenderNode& operator =(RenderNode && other) = delete;

    virtual IRenderNodeSP create(const VkBool32 createData = VK_TRUE) const override;

    virtual void reset() override;

    virtual void updateTransformUniformBuffer(const IBufferObjectSP& transformUniformBuffer) override;

    virtual void updateJointsUniformBuffer(const IBufferObjectSP& jointsUniformBuffer) override;

    virtual void updateDescriptorSets(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets) override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_RENDERNODE_HPP_ */
