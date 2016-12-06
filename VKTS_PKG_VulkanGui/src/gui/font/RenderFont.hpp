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

#ifndef VKTS_RENDERFONT_HPP_
#define VKTS_RENDERFONT_HPP_

#include <vkts/vulkan/gui/vkts_gui.hpp>

namespace vkts
{

class RenderFont: public IRenderFont
{

private:

	IBufferObjectSP vertexBuffer;

	IDescriptorSetLayoutSP descriptorSetLayout;

	IDescriptorPoolSP descriptorPool;

	IDescriptorSetsSP descriptorSets;

	IPipelineLayoutSP pipelineLayout;

	IGraphicsPipelineSP graphicsPipeline;

public:

    RenderFont();
    RenderFont(const RenderFont& other) = delete;
    RenderFont(RenderFont&& other) = delete;
    virtual ~RenderFont();

    RenderFont& operator =(const RenderFont& other) = delete;
    RenderFont& operator =(RenderFont && other) = delete;


	void setVertexBuffer(const IBufferObjectSP& vertexBuffer);

	void setDescriptorSetLayout(const IDescriptorSetLayoutSP& descriptorSetLayout);

	void setDescriptorPool(const IDescriptorPoolSP& descriptorPool);

	void setDescriptorSets(const IDescriptorSetsSP& descriptorSets);

	void setPipelineLayout(const IPipelineLayoutSP& pipelineLayout);

	void setGraphicsPipeline(const IGraphicsPipelineSP& graphicsPipeline);


    virtual IRenderFontSP create(const VkBool32 createData = VK_TRUE) const override;

    virtual void draw(const ICommandBuffersSP& cmdBuffer, const glm::mat4& viewProjection, const glm::vec2& translate, const std::string& text, const float fontSize, const glm::vec4& color, const IFont& font) override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_RENDERFONT_HPP_ */
