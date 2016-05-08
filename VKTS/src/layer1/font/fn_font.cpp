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

#include <vkts/vkts.hpp>

#include "../../fn_internal.hpp"

#include "../../layer0/binary_buffer/BinaryBuffer.hpp"

#include "../../layer0/text_buffer/TextBuffer.hpp"

#include "../../layer0/image_view/ImageView.hpp"

#include "../../layer0/sampler/Sampler.hpp"

#include "../buffer_object/BufferObject.hpp"

#include "../texture/Texture.hpp"

#include "Char.hpp"

#include "Font.hpp"

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
        strncpy(directory, filename, (position - filename) + 1);

        return VK_TRUE;
    }

    position = strrchr(filename, '\\');

    if (position)
    {
        strncpy(directory, filename, (position - filename) + 1);

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

    // Skip '='
    start++;

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

    return VK_TRUE;
}

IFontSP VKTS_APIENTRY fontCreate(const char* filename, const IInitialResourcesSP& initialResources, const ICommandBuffersSP& commandBuffer, const IRenderPassSP& renderPass)
{
    if (!filename || !initialResources.get() || !commandBuffer.get())
    {
        return IFontSP();
    }

    auto textBuffer = fileLoadText(filename);

    if (!textBuffer.get())
    {
        return IFontSP();
    }

    char directory[VKTS_MAX_BUFFER_CHARS] = "";

    fontGetDirectory(directory, filename);

    auto font = new Font();

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

        	std::string finalImageDataFilename = std::string(directory) + std::string(sdata);

        	auto imageData = imageDataLoad(finalImageDataFilename.c_str());

        	if (!imageData.get())
        	{
        		return IFontSP();
        	}

        	//

            VkImageTiling imageTiling;
            VkMemoryPropertyFlags memoryPropertyFlags;

            if (!initialResources->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
            {
            	return IFontSP();
            }

            //

            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            VkAccessFlags srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;

            if (imageTiling == VK_IMAGE_TILING_OPTIMAL)
            {
            	initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            	srcAccessMask = 0;
            }

            //

            VkImageCreateInfo imageCreateInfo;

            memset(&imageCreateInfo, 0, sizeof(VkImageCreateInfo));

            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

            imageCreateInfo.flags = 0;
            imageCreateInfo.imageType = imageData->getImageType();
            imageCreateInfo.format = imageData->getFormat();
            imageCreateInfo.extent = imageData->getExtent3D();
            imageCreateInfo.mipLevels = imageData->getMipLevels();
            imageCreateInfo.arrayLayers = 1;
            imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageCreateInfo.tiling = imageTiling;
            imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageCreateInfo.queueFamilyIndexCount = 0;
            imageCreateInfo.pQueueFamilyIndices = nullptr;
            imageCreateInfo.initialLayout = initialLayout;

            VkImageSubresourceRange subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, imageData->getMipLevels(), 0, 1};

            IDeviceMemorySP stageDeviceMemory;
            IBufferSP stageBuffer;
            IImageSP stageImage;

            auto memoryImage = memoryImageCreate(stageImage, stageBuffer, stageDeviceMemory, initialResources, commandBuffer, font->getFace(), imageData, imageCreateInfo, srcAccessMask, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange, memoryPropertyFlags);

            if (!memoryImage.get())
            {
            	return VK_FALSE;
            }

            //

        	VkSamplerCreateInfo samplerCreateInfo;

        	memset(&samplerCreateInfo, 0, sizeof(VkSamplerCreateInfo));

        	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        	samplerCreateInfo.flags = 0;
        	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
        	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
        	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        	samplerCreateInfo.mipLodBias = 0.0f;
        	samplerCreateInfo.maxAnisotropy = 1.0f;
        	samplerCreateInfo.compareEnable = VK_FALSE;
        	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        	samplerCreateInfo.minLod = 0.0f;
        	samplerCreateInfo.maxLod = 0.0f;
        	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

        	VkImageViewCreateInfo imageViewCreateInfo;

        	memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));

        	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        	imageViewCreateInfo.flags = 0;
        	imageViewCreateInfo.image = VK_NULL_HANDLE;				// Defined later.
        	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        	imageViewCreateInfo.format = VK_FORMAT_UNDEFINED;		// Defined later.
        	imageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
        	imageViewCreateInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

            //

            auto texture = textureCreate(initialResources, font->getFace(), VK_FALSE, memoryImage, samplerCreateInfo, imageViewCreateInfo);

            if (!texture.get())
            {
                return VK_FALSE;
            }

            font->setTexture(texture);
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
        else
        {
        	return IFontSP();
        }
    }

    //
    // Create a quad out of a triangle strip.
    //

    const float vertexData[4*4] = {
		0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
    };

    auto binaryBuffer = binaryBufferCreate((const uint8_t*)vertexData, sizeof(vertexData));

    if (!binaryBuffer.get())
    {
    	return IFontSP();
    }

    //

    VkBufferCreateInfo bufferCreateInfo;

    memset(&bufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferCreateInfo.size = sizeof(vertexData);
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    IDeviceMemorySP stageDeviceMemory;
    IBufferSP stageBuffer;

    auto vertexBuffer = bufferObjectCreate(stageBuffer, stageDeviceMemory, initialResources, commandBuffer, binaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

    if (!vertexBuffer.get())
    {
    	return IFontSP();
    }

    font->setVertexBuffer(vertexBuffer);

    //

    defaultGraphicsPipeline gp;

    // TODO: Add settings.

    auto graphicsPipeline = pipelineCreateGraphics(initialResources->getDevice()->getDevice(), VK_NULL_HANDLE, gp.getGraphicsPipelineCreateInfo(), VKTS_VERTEX_BUFFER_TYPE_VERTEX);

    if (!graphicsPipeline.get())
    {
    	return IFontSP();
    }

    font->setGraphicsPipeline(graphicsPipeline);

    return interfaceFont;
}

}
