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

Font::Font(const VkBool32 distanceField) :
    IFont(), distanceField(distanceField), face(""), size(0.0f), lineHeight(0.0f), base(0.0f), scaleWidth(0.0f), scaleHeight(0.0f), allCharacters(), textureObject(nullptr), renderFont(nullptr)
{
}

Font::~Font()
{
    destroy();
}

IRenderFontSP Font::getRenderFont() const
{
	return renderFont;
}

void Font::setRenderFont(const IRenderFontSP& renderFont)
{
	this->renderFont = renderFont;
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

const ITextureObjectSP& Font::getTextureObject() const
{
	return textureObject;
}

void Font::setTextureObject(const ITextureObjectSP& textureObject)
{
	this->textureObject = textureObject;
}

const IChar* Font::getChar(const int32_t c) const
{
	auto currentCharacter = allCharacters.find((int32_t)c);

	if (currentCharacter == allCharacters.end())
	{
		return nullptr;
	}

	return &(currentCharacter->second);
}

void Font::drawText(const ICommandBuffersSP& cmdBuffer, const glm::mat4& viewProjection, const glm::vec2& translate, const std::string& text, const float fontSize, const glm::vec4& color) const
{
	if (renderFont.get())
	{
		renderFont->draw(cmdBuffer, viewProjection, translate, text, fontSize, color, *this);
	}
}

//
// IFont
//

VkBool32 Font::isDistanceField() const
{
	return distanceField;
}

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

float Font::getLineHeight(const float fontSize) const
{
	return lineHeight * fontSize / getSize();
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


//
// IDestroyable
//

void Font::destroy()
{
	allCharacters.clear();

	if (renderFont.get())
	{
		renderFont->destroy();
	}

	if (textureObject.get())
	{
		textureObject->destroy();

		textureObject = ITextureObjectSP(nullptr);
	}
}

} /* namespace vkts */
