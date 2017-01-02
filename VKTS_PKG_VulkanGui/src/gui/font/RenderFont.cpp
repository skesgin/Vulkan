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

#include "RenderFont.hpp"

#define VKTS_SPREAD 4.0f

namespace vkts
{

RenderFont::RenderFont() :
    IRenderFont(), vertexBuffer(nullptr), descriptorSetLayout(nullptr), descriptorPool(nullptr), descriptorSets(nullptr), pipelineLayout(nullptr), graphicsPipeline(nullptr)
{
}

RenderFont::~RenderFont()
{
}

void RenderFont::setVertexBuffer(const IBufferObjectSP& vertexBuffer)
{
	this->vertexBuffer = vertexBuffer;
}

void RenderFont::setDescriptorSetLayout(const IDescriptorSetLayoutSP& descriptorSetLayout)
{
	this->descriptorSetLayout = descriptorSetLayout;
}

void RenderFont::setDescriptorPool(const IDescriptorPoolSP& descriptorPool)
{
	this->descriptorPool = descriptorPool;
}

void RenderFont::setDescriptorSets(const IDescriptorSetsSP& descriptorSets)
{
	this->descriptorSets = descriptorSets;
}

void RenderFont::setPipelineLayout(const IPipelineLayoutSP& pipelineLayout)
{
	this->pipelineLayout = pipelineLayout;
}

void RenderFont::setGraphicsPipeline(const IGraphicsPipelineSP& graphicsPipeline)
{
	this->graphicsPipeline = graphicsPipeline;
}

void RenderFont::draw(const ICommandBuffersSP& cmdBuffer, const glm::mat4& viewProjection, const glm::vec2& translate, const std::string& text, const float fontSize, const glm::vec4& color, const IFont& font)
{
	if (!cmdBuffer.get() || !graphicsPipeline.get() || !pipelineLayout.get() || !descriptorSets.get() || !vertexBuffer.get())
	{
		return;
	}

	//

	vkCmdBindPipeline(cmdBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

	//

	vkCmdBindDescriptorSets(cmdBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->getPipelineLayout(), 0, 1, descriptorSets->getDescriptorSets(), 0, nullptr);

	//

	const VkBuffer buffers[1] = {vertexBuffer->getBuffer()->getBuffer()};

	VkDeviceSize offsets[1] = {0};

	vkCmdBindVertexBuffers(cmdBuffer->getCommandBuffer(), 0, 1, buffers, offsets);

	//

	float fontScale = fontSize / font.getSize();

	glm::vec2 cursor = translate;
	cursor.y -= font.getBase() * fontScale;

	const IChar* lastCharacter = nullptr;

	//

	float smoothing = 1.0f / (VKTS_SPREAD * fontScale);

	const glm::vec3 texCoordOrigin[4] = {glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f)};
	glm::vec3 texCoord[4];
	float packedTexCoord[4 * 2];

	for (auto c : text)
	{
		// Wait for line break.
		if (c == '\r')
		{
			lastCharacter = nullptr;

			continue;
		}

		// Line break.
		if (c == '\n')
		{
			cursor.x = translate.x;
			cursor.y -= font.getLineHeight() * fontScale;

			lastCharacter = nullptr;

			continue;
		}

		const IChar* currentCharacter = font.getChar((int32_t)c);

		if (!currentCharacter)
		{
			// Character not found.

			lastCharacter = nullptr;

			continue;
		}

		//
		// Advance, depending on kerning of current and last character.
		//

		if (lastCharacter)
		{
			cursor.x += lastCharacter->getKerning(currentCharacter->getId()) * fontScale;
		}

		//
		// Get bottom left corner of the character textureObject.
		//

		glm::vec2 origin = cursor;

		origin.y += (font.getBase() - (currentCharacter->getYoffset() + currentCharacter->getHeight() - font.getBase())) * fontScale;

		origin.x += currentCharacter->getXoffset() * fontScale;

		// Draw Character.

		glm::mat4 transformVertex = viewProjection * translateMat4(origin.x, origin.y, 0.0f) * scaleMat4(currentCharacter->getWidth() * fontScale, currentCharacter->getHeight() * fontScale, 1.0f);

		vkCmdPushConstants(cmdBuffer->getCommandBuffer(), graphicsPipeline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, glm::value_ptr(transformVertex));

		glm::mat3 translateTexCoord = translateMat3(currentCharacter->getX() / font.getScaleWidth(), (font.getScaleHeight() - currentCharacter->getY() - currentCharacter->getHeight()) / font.getScaleHeight());
		glm::mat3 scaleTexCoord = scaleMat3(currentCharacter->getWidth() / font.getScaleWidth(), currentCharacter->getHeight() / font.getScaleHeight(), 1.0f);

		glm::mat3 transformTexCoord = translateTexCoord * scaleTexCoord;

		for (uint32_t i = 0; i < 4; i++)
		{
			texCoord[i] = transformTexCoord * texCoordOrigin[i];

			packedTexCoord[i * 2 + 0] = texCoord[i].s;
			packedTexCoord[i * 2 + 1] = texCoord[i].t;
		}

		vkCmdPushConstants(cmdBuffer->getCommandBuffer(), graphicsPipeline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 16, sizeof(float) * 2 * 4, packedTexCoord);

		vkCmdPushConstants(cmdBuffer->getCommandBuffer(), graphicsPipeline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 16 + sizeof(float) * 2 * 4, sizeof(float) * 4, glm::value_ptr(color));

		if (font.isDistanceField())
		{
			vkCmdPushConstants(cmdBuffer->getCommandBuffer(), graphicsPipeline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 16 + sizeof(float) * 2 * 4 + sizeof(float) * 4, sizeof(float) * 1, &smoothing);
		}

		// Expecting triangle strip as primitive topology.
		vkCmdDraw(cmdBuffer->getCommandBuffer(), 4, 1, 0, 0);

		//
		// Advance, as character has been drawn.
		//

		cursor.x += currentCharacter->getXadvance() * fontScale;

		lastCharacter = currentCharacter;
	}
}

IRenderFontSP RenderFont::create(const VkBool32 createData) const
{
	return IRenderFontSP(new RenderFont());
}

void RenderFont::destroy()
{
	if (graphicsPipeline.get())
	{
		graphicsPipeline->destroy();

		graphicsPipeline = IGraphicsPipelineSP(nullptr);
	}

	if (pipelineLayout.get())
	{
		pipelineLayout->destroy();

		pipelineLayout = IPipelineLayoutSP(nullptr);
	}

	if (descriptorSets.get())
	{
		descriptorSets->destroy();

		descriptorSets = IDescriptorSetsSP(nullptr);
	}

	if (descriptorPool.get())
	{
		descriptorPool->destroy();

		descriptorPool = IDescriptorPoolSP(nullptr);
	}

	if (descriptorSetLayout.get())
	{
		descriptorSetLayout->destroy();

		descriptorSetLayout = IDescriptorSetLayoutSP(nullptr);
	}

	if (vertexBuffer.get())
	{
		vertexBuffer->destroy();

		vertexBuffer = IBufferObjectSP(nullptr);
	}
}

} /* namespace vkts */
