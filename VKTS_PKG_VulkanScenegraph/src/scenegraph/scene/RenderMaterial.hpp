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

#ifndef VKTS_RENDERMATERIAL_HPP_
#define VKTS_RENDERMATERIAL_HPP_

#include <vkts/vulkan/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class RenderMaterial : public IRenderMaterial
{

protected:

    IDescriptorPoolSP descriptorPool;
    IDescriptorSetsSP descriptorSets;
    VkDescriptorImageInfo descriptorImageInfos[VKTS_BINDING_UNIFORM_MATERIAL_TOTAL_BINDING_COUNT];
    VkWriteDescriptorSet writeDescriptorSets[VKTS_BINDING_UNIFORM_MATERIAL_TOTAL_BINDING_COUNT];
    std::string nodeName;

    SmartPointerMap<std::string, IDescriptorPoolSP> allDescriptorPools;
    SmartPointerMap<std::string, IDescriptorSetsSP> allDescriptorSets;
    std::map<std::string, std::map<uint32_t, VkBool32>> allBindingPresent;

    IDescriptorSetsSP createDescriptorSetsByName(const std::string& nodeName);
    IDescriptorSetsSP getDescriptorSetsByName(const std::string& nodeName) const;

    void bindDescriptorSets(const ICommandBuffersSP& cmdBuffer, const VkPipelineLayout layout, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const std::string& nodeName) const;

public:

    RenderMaterial();
    RenderMaterial(const RenderMaterial& other);
    RenderMaterial(RenderMaterial&& other) = delete;
    virtual ~RenderMaterial();

    RenderMaterial& operator =(const RenderMaterial& other) = delete;

    RenderMaterial& operator =(RenderMaterial && other) = delete;


    virtual IRenderMaterialSP create(const VkBool32 createData = VK_TRUE) const override;


    virtual IDescriptorPoolSP getDescriptorPool() const override;

    virtual void setDescriptorPool(const IDescriptorPoolSP& descriptorPool) override;

    virtual IDescriptorSetsSP getDescriptorSets() const override;

    virtual void setDescriptorSets(const IDescriptorSetsSP& descriptorSets) override;


    virtual void addDescriptorImageInfo(const uint32_t colorIndex, const uint32_t dstBindingOffset, const VkSampler sampler, const VkImageView imageView, const VkImageLayout imageLayout) override;

    virtual void updateDescriptorSets(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const std::string& nodeName) override;

    virtual void draw(const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const std::string& nodeName) override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_RENDERMATERIAL_HPP_ */
