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

#include <vkts/gui/vkts_gui.hpp>

#include "../font/Char.hpp"
#include "../font/Font.hpp"

namespace vkts
{

static VkBool32 fontGetDirectory(char* directory, const char* filename)
{
    if (!directory || !filename)
    {
        return VK_FALSE;
    }

    auto position = strrchr(filename, '/');

    if (position)
    {
        strncpy(directory, filename, position - filename + 1);
        directory[position - filename + 1] = '\0';

        return VK_TRUE;
    }

    position = strrchr(filename, '\\');

    if (position)
    {
        strncpy(directory, filename, position - filename + 1);
        directory[position - filename + 1] = '\0';

        return VK_TRUE;
    }

    return VK_FALSE;
}

static VkBool32 fontIsToken(const char* buffer, const char* token)
{
    if (!buffer || !token)
    {
        return VK_FALSE;
    }

    if (!(buffer[strlen(token)] == ' '))
    {
        return VK_FALSE;
    }

    if (strncmp(buffer, token, strlen(token)) == 0)
    {
        return VK_TRUE;
    }

    return VK_FALSE;
}

static VkBool32 fontExtractValue(const char* buffer, const char* parameter, char* value)
{
    if (!buffer || !parameter || !value)
    {
        return VK_FALSE;
    }

    const char* start = strstr(buffer, parameter);

    if (!start)
    {
    	return VK_FALSE;
    }

    // Skip parameter and '='
    start += 1 + strlen(parameter);

    const char* end = strstr(start, " ");

    if (!end)
    {
    	end = strstr(start, "\t");

    	if (!end)
    	{
    		end = strstr(start, "\r");

			if (!end)
			{
				end = strstr(start, "\n");

				if (!end)
				{
					return VK_FALSE;
				}
			}
    	}
    }

    strncpy(value, start, end - start);
    value[end - start] = '\0';

    return VK_TRUE;
}

static VkBool32 fontExtractIntValue(const char* buffer, const char* parameter, int& value)
{
    if (!buffer || !parameter)
    {
        return VK_FALSE;
    }

    char temp[VKTS_MAX_TOKEN_CHARS + 1];

    if (!fontExtractValue(buffer, parameter, temp))
    {
    	return VK_FALSE;
    }

    value = atoi(temp);

    return VK_TRUE;
}

static VkBool32 fontExtractStringValue(const char* buffer, const char* parameter, char* value)
{
    if (!buffer || !parameter || !value)
    {
        return VK_FALSE;
    }

    char temp[VKTS_MAX_TOKEN_CHARS + 1];

    if (!fontExtractValue(buffer, parameter, temp))
    {
    	return VK_FALSE;
    }

    char* start = strstr(temp, "\"");
    if (!start)
    {
    	return VK_FALSE;
    }
    start++;

    char* end = strstr(start, "\"");
    if (!end)
    {
    	return VK_FALSE;
    }

    strncpy(value, start, end - start);
    value[end - start] = '\0';

    return VK_TRUE;
}

IFontSP VKTS_APIENTRY loadFont(const char* filename, const IGuiManagerSP& guiManager, const IGuiFactorySP& guiFactory, const VkBool32 distanceField)
{
    if (!filename || !guiManager.get() || !guiFactory.get())
    {
        return IFontSP();
    }

    auto textBuffer = fileLoadText(filename);

    if (!textBuffer.get())
    {
        return IFontSP();
    }

    char directory[VKTS_MAX_BUFFER_CHARS];

    fontGetDirectory(directory, filename);

    auto font = new Font(distanceField);

    auto interfaceFont = IFontSP(font);

    if (!interfaceFont.get())
    {
        return IFontSP();
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    int idata;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (fontIsToken(buffer, "info"))
        {
        	if (!fontExtractStringValue(buffer, "face", sdata))
        	{
            	return IFontSP();
        	}

        	font->setFace(sdata);

        	if (!fontExtractIntValue(buffer, "size", idata))
        	{
            	return IFontSP();
        	}

        	font->setSize((float)idata);
        }
        else if (fontIsToken(buffer, "common"))
        {
        	if (!fontExtractIntValue(buffer, "lineHeight", idata))
        	{
            	return IFontSP();
        	}

        	font->setLineHeight((float)idata);

        	if (!fontExtractIntValue(buffer, "base", idata))
        	{
            	return IFontSP();
        	}

        	font->setBase((float)idata);

        	if (!fontExtractIntValue(buffer, "scaleW", idata))
        	{
            	return IFontSP();
        	}

        	font->setScaleWidth((float)idata);

        	if (!fontExtractIntValue(buffer, "scaleH", idata))
        	{
            	return IFontSP();
        	}

        	font->setScaleHeight((float)idata);
        }
        else if (fontIsToken(buffer, "page"))
        {
        	if (!fontExtractStringValue(buffer, "file", sdata))
        	{
            	return IFontSP();
        	}

        	//
        	// ImageData creation.
        	//

        	auto imageDataFilename = std::string(sdata);

			std::string finalImageDataFilename = directory + imageDataFilename;

			auto imageData = guiManager->useImageData(finalImageDataFilename.c_str());

			if (!imageData.get())
			{
				imageData = guiManager->useImageData(imageDataFilename.c_str());

				if (!imageData.get())
				{
					// Load image data.

					imageData = imageDataLoad(finalImageDataFilename.c_str());

					if (!imageData.get())
					{
						imageData = imageDataLoad(imageDataFilename.c_str());

						if (!imageData.get())
						{
							logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load image data '%s'", finalImageDataFilename.c_str());

							return IFontSP();
						}
					}

					//

					auto mipMappedImageData = imageDataMipmap(imageData, VK_TRUE, imageData->getName());

					if (mipMappedImageData.size() == 0)
					{
						return IFontSP();
					}

					imageData = imageDataMerge(mipMappedImageData, imageData->getName(), (uint32_t)mipMappedImageData.size(), 1);

					if (!imageData.get())
					{
						return IFontSP();
					}

					//

					imageData = createDeviceImageData(guiManager->getAssetManager(), imageData);

					//

					if (imageData.get())
					{
						guiManager->addImageData(imageData);
					}
				}
			}

			if (!imageData.get())
			{
				return IFontSP();
			}

			//
			// ImageObject creation.
			//

			auto imageObject = guiManager->useImageObject(imageDataFilename.c_str());

            if (!imageObject.get())
            {
				imageObject = createImageObject(guiManager->getAssetManager(), imageDataFilename, imageData, VK_FALSE);

				if (!imageObject.get())
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No memory image for '%s'", sdata);

					return IFontSP();
				}

				guiManager->addImageObject(imageObject);
            }

			//
			// TextureObject creation.
			//

            auto textureObject = guiManager->useTextureObject(imageDataFilename.c_str());

            if (!textureObject.get())
            {
            	textureObject = createTextureObject(guiManager->getAssetManager(), font->getFace(), VK_TRUE, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, imageObject);

				if (!textureObject.get())
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No texture object for '%s'", sdata);

					return IFontSP();
				}

				guiManager->addTextureObject(textureObject);
            }

            //

            font->setTextureObject(textureObject);
        }
        else if (fontIsToken(buffer, "chars"))
        {
        	// Do nothing.
        }
        else if (fontIsToken(buffer, "char"))
        {
        	Char character;

        	if (!fontExtractIntValue(buffer, "id", idata))
        	{
            	return IFontSP();
        	}

        	character.setId(idata);

        	if (!fontExtractIntValue(buffer, "x", idata))
        	{
            	return IFontSP();
        	}

        	character.setX((float)idata);

        	if (!fontExtractIntValue(buffer, "y", idata))
        	{
            	return IFontSP();
        	}

        	character.setY((float)idata);

        	if (!fontExtractIntValue(buffer, "width", idata))
        	{
            	return IFontSP();
        	}

        	character.setWidth((float)idata);

        	if (!fontExtractIntValue(buffer, "height", idata))
        	{
            	return IFontSP();
        	}

        	character.setHeight((float)idata);

        	if (!fontExtractIntValue(buffer, "xoffset", idata))
        	{
            	return IFontSP();
        	}

        	character.setXoffset((float)idata);

        	if (!fontExtractIntValue(buffer, "yoffset", idata))
        	{
            	return IFontSP();
        	}

        	character.setYoffset((float)idata);

        	if (!fontExtractIntValue(buffer, "xadvance", idata))
        	{
            	return IFontSP();
        	}

        	character.setXadvance((float)idata);

        	//

        	font->setChar(character);
        }
        else if (fontIsToken(buffer, "kernings"))
        {
        	// Do nothing.
        }
        else if (fontIsToken(buffer, "kerning"))
        {
        	if (!fontExtractIntValue(buffer, "first", idata))
        	{
            	return IFontSP();
        	}

        	int32_t characterId = idata;

        	if (!fontExtractIntValue(buffer, "second", idata))
        	{
            	return IFontSP();
        	}

        	int32_t nextCharacterId = idata;

        	if (!fontExtractIntValue(buffer, "amount", idata))
        	{
            	return IFontSP();
        	}

        	float amount = (float)idata;

        	//

        	font->setKerning(characterId, nextCharacterId, amount);
        }
    }

    auto renderFont = guiFactory->getGuiRenderFactory()->createRenderFont(guiManager, *font);

    if (!renderFont.get())
    {
    	return IFontSP();
    }

    font->setRenderFont(renderFont);

    return interfaceFont;
}

}
