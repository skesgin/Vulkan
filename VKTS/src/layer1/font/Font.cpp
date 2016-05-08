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

#include "Font.hpp"

namespace vkts
{

Font::Font() :
    IFont(), face(""), size(0.0f), lineHeight(0.0f), base(0.0f), scaleWidth(0.0f), scaleHeight(0.0f), texture(nullptr), allCharacters(), vertexBuffer(nullptr)
{
}

Font::~Font()
{
    destroy();
}

void Font::setFace(const std::string& face)
{
	this->face = face;
}

void Font::setSize(const float size)
{
	this->size = size;
}

void Font::setLineHeight(const float lineHeight)
{
	this->lineHeight = lineHeight;
}

void Font::setBase(const float base)
{
	this->base = base;
}

void Font::setScaleWidth(const float scaleWidth)
{
	this->scaleWidth = scaleWidth;
}

void Font::setScaleHeight(const float scaleHeight)
{
	this->scaleHeight = scaleHeight;
}

void Font::setTexture(const ITextureSP& texture)
{
	this->texture = texture;
}

void Font::setChar(const Char& character)
{
	allCharacters[character.getId()] = character;
}

void Font::setKerning(const int32_t characterId, const int32_t nextCharacterId, const float amount)
{
	auto currentCharacter = allCharacters.find(characterId);

	if (currentCharacter == allCharacters.end())
	{
		return;
	}

	currentCharacter->second.setKerning(nextCharacterId, amount);
}

void Font::setVertexBuffer(const IBufferObjectSP& vertexBuffer)
{
	this->vertexBuffer = vertexBuffer;
}

void Font::setGraphicsPipeline(const IGraphicsPipelineSP& graphicsPipeline)
{
	this->graphicsPipeline = graphicsPipeline;
}

void Font::drawText(const ICommandBuffersSP& cmdBuffer, const glm::mat4& viewProjection, const glm::vec2& translate, const std::string& text, const glm::vec4& color) const
{
	if (!cmdBuffer.get())
	{
		return;
	}

	//

	const VkBuffer buffers[1] = {vertexBuffer->getBuffer()->getBuffer()};

	VkDeviceSize offsets[1] = {0};

	vkCmdBindVertexBuffers(cmdBuffer->getCommandBuffer(), 0, 1, buffers, offsets);

	//

	glm::vec2 cursor = translate;

	const Char* lastCharacter = nullptr;

	for (auto c : text)
	{
		// Line break.
		if (c == '\n')
		{
			cursor.x = translate.x;
			cursor.y += getLineHeight();

			lastCharacter = nullptr;

			continue;
		}

		auto currentCharacter = allCharacters.find((int32_t)c);

		if (currentCharacter == allCharacters.end())
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
			cursor.x += lastCharacter->getKerning(currentCharacter->second.getId());
		}

		//
		// Get bottom left corner of the character texture.
		//

		glm::vec2 origin = cursor;

		origin.y += getBase();

		origin.x -= currentCharacter->second.getXoffset();

		// Draw Character.

		glm::mat4 transformVertex = viewProjection * translateMat4(origin.x, origin.y, 0.0f);

		vkCmdPushConstants(cmdBuffer->getCommandBuffer(), graphicsPipeline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, glm::value_ptr(transformVertex));

		glm::mat3 translateTexCoord = translateMat3(currentCharacter->second.getX() / getScaleWidth(), (getScaleHeight() - currentCharacter->second.getY() - currentCharacter->second.getHeight()) / getScaleHeight());
		glm::mat3 scaleTexCoord = scaleMat3(currentCharacter->second.getWidth() / getScaleWidth(), currentCharacter->second.getHeight() / getScaleHeight(), 1.0f);

		glm::mat3 transformTexCoord = translateTexCoord * scaleTexCoord;

		vkCmdPushConstants(cmdBuffer->getCommandBuffer(), graphicsPipeline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 16, sizeof(float) * 9, glm::value_ptr(transformTexCoord));

		vkCmdPushConstants(cmdBuffer->getCommandBuffer(), graphicsPipeline->getLayout(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 16 + sizeof(float) * 12, sizeof(float) * 4, glm::value_ptr(color));

		// Expecting triangle strip as primitive topology.
		vkCmdDraw(cmdBuffer->getCommandBuffer(), 4, 1, 0, 0);

		//
		// Advance, as character has been drawn.
		//

		cursor.x += currentCharacter->second.getXadvance();

		lastCharacter = &currentCharacter->second;
	}
}

//
// IFont
//

const std::string& Font::getFace() const
{
	return face;
}

float Font::getSize() const
{
	return size;
}

float Font::getLineHeight() const
{
	return lineHeight;
}

float Font::getBase() const
{
	return base;
}

float Font::getScaleWidth() const
{
	return scaleWidth;
}

float Font::getScaleHeight() const
{
	return scaleHeight;
}

const ITextureSP& Font::getTexture() const
{
	return texture;
}


//
// IDestroyable
//

void Font::destroy()
{
	if (texture.get())
	{
		texture->destroy();

		texture = ITextureSP(nullptr);
	}

	allCharacters.clear();

	if (vertexBuffer.get())
	{
		vertexBuffer->destroy();

		vertexBuffer = IBufferObjectSP(nullptr);
	}
}

} /* namespace vkts */
