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

#ifndef VKTS_IRENDERMATERIAL_HPP_
#define VKTS_IRENDERMATERIAL_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class IRenderMaterial: public IDestroyable
{

public:

    IRenderMaterial() :
        IDestroyable()
    {
    }

    virtual ~IRenderMaterial()
    {
    }

    virtual std::shared_ptr<IRenderMaterial> create(const VkBool32 createData = VK_TRUE) const = 0;


    virtual IDescriptorPoolSP getDescriptorPool() const = 0;

    virtual void setDescriptorPool(const IDescriptorPoolSP& descriptorPool) = 0;

    virtual IDescriptorSetsSP getDescriptorSets() const = 0;

    virtual void setDescriptorSets(const IDescriptorSetsSP& descriptorSets) = 0;


    virtual void addDescriptorImageInfo(const uint32_t colorIndex, const uint32_t dstBindingOffset, const VkSampler sampler, const VkImageView imageView, const VkImageLayout imageLayout) = 0;

    virtual void updateDescriptorSets(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const std::string& nodeName) = 0;

    virtual void draw(const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets, const std::string& nodeName) = 0;

};

typedef std::shared_ptr<IRenderMaterial> IRenderMaterialSP;

} /* namespace vkts */

#endif /* VKTS_IRENDERMATERIAL_HPP_ */
