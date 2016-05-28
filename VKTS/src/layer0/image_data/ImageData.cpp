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

#include "../binary_buffer/BinaryBuffer.hpp"
#include "ImageData.hpp"

namespace vkts
{

void ImageData::reset()
{
    name = "";

    imageType = VK_IMAGE_TYPE_1D;

    format = VK_FORMAT_UNDEFINED;

    extent = { 0, 0, 0};

    mipLevels = 0;
    arrayLayers = 0;

    if (buffer.get())
    {
        buffer->reset();
    }

    UNORM = VK_FALSE;
    SFLOAT = VK_FALSE;
    bytesPerChannel = 0;
    numberChannels = 0;

    allOffsets.clear();
}

size_t ImageData::getOffset(VkExtent3D& currentExtent, const uint32_t mipLevel, const uint32_t arrayLayer) const
{
	if (allOffsets.size() == 0)
	{
		size_t offset = 0;

		for (uint32_t currentArrayLayer = 0; currentArrayLayer < arrayLayers; currentArrayLayer++)
		{
			currentExtent = extent;

			for (uint32_t currentMipLevel = 0; currentMipLevel < mipLevels; currentMipLevel++)
			{
				allOffsets.push_back(offset);

				currentExtent.width = glm::max(extent.width >> (currentMipLevel), 1u);
				currentExtent.height = glm::max(extent.height >> (currentMipLevel), 1u);
				currentExtent.depth = glm::max(extent.depth >> (currentMipLevel), 1u);

				offset += bytesPerChannel * numberChannels * currentExtent.width * currentExtent.height * currentExtent.depth;
			}
		}
	}

	currentExtent.width = glm::max(extent.width >> (mipLevel), 1u);
	currentExtent.height = glm::max(extent.height >> (mipLevel), 1u);
	currentExtent.depth = glm::max(extent.depth >> (mipLevel), 1u);

	return allOffsets[arrayLayer * mipLevels + mipLevel];
}

int32_t ImageData::getTexelLocation(float& fraction, const float a, const int32_t size, const VkSamplerAddressMode addressMode) const
{
	float unnormalizedA = a * (float)size;

	//

	fraction = fabsf(unnormalizedA - floorf(unnormalizedA));

	//

	int32_t coord = (int32_t)unnormalizedA;

	if (addressMode == VK_SAMPLER_ADDRESS_MODE_REPEAT)
	{
		coord = coord % size;

		//

		if (coord < 0)
		{
			coord += size;
		}
	}
	else if	(addressMode == VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT)
	{
		coord = (coord % (2 * size)) - size;

		if (coord < 0)
		{
			coord = -(1 + coord);
		}

		coord = (size - 1) - coord;

		//

		if (coord < 0)
		{
			coord = -coord;
		}
	}
	else if	(addressMode == VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
	{
		coord = glm::clamp(coord, 0, size - 1);
	}
	else if	(addressMode == VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER)
	{
		coord = glm::clamp(coord, -1, size);
	}
	else if	(addressMode == VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE)
	{
		if (coord < 0)
		{
			coord = -(1 + coord);
		}

		coord = glm::clamp(coord, 0, size - 1);
	}

	return coord;
}

ImageData::ImageData(const std::string& name, const VkImageType imageType, const VkFormat& format, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const uint8_t* data, const size_t size) :
    IImageData(), name(name), imageType(imageType), format(format), extent(extent), mipLevels(mipLevels), arrayLayers(arrayLayers), allOffsets()
{
    buffer = IBinaryBufferSP(new BinaryBuffer(data, size));

    if (!buffer.get() || buffer->getSize() != size)
    {
        reset();
    }

    UNORM = commonIsUNORM(format);
    SFLOAT = commonIsSFLOAT(format);
    bytesPerChannel = commonGetBytesPerChannel(format);
    numberChannels = commonGetNumberChannels(format);
}

ImageData::ImageData(const std::string& name, const VkImageType imageType, const VkFormat& format, const VkExtent3D& extent, const uint32_t mipLevels, const uint32_t arrayLayers, const IBinaryBufferSP& buffer) :
    IImageData(), name(name), imageType(imageType), format(format), extent(extent), mipLevels(mipLevels), arrayLayers(arrayLayers), buffer(buffer), allOffsets()
{
    if (!this->buffer.get() || !this->buffer->getData())
    {
        reset();
    }

    UNORM = commonIsUNORM(format);
    SFLOAT = commonIsSFLOAT(format);
    bytesPerChannel = commonGetBytesPerChannel(format);
    numberChannels = commonGetNumberChannels(format);
}

ImageData::~ImageData()
{
    reset();
}

//
// IImageData
//

const std::string& ImageData::getName() const
{
    return name;
}

VkImageType ImageData::getImageType() const
{
    return imageType;
}

const VkFormat& ImageData::getFormat() const
{
    return format;
}

const VkExtent3D& ImageData::getExtent3D() const
{
    return extent;
}

uint32_t ImageData::getWidth() const
{
    return extent.width;
}

uint32_t ImageData::getHeight() const
{
    return extent.height;
}

uint32_t ImageData::getDepth() const
{
    return extent.depth;
}

uint32_t ImageData::getMipLevels() const
{
    return mipLevels;
}

uint32_t ImageData::getArrayLayers() const
{
    return arrayLayers;
}

const void* ImageData::getData() const
{
    if (buffer.get())
    {
        return buffer->getData();
    }

    return nullptr;
}

size_t ImageData::getSize() const
{
    if (buffer.get())
    {
        return buffer->getSize();
    }

    return 0;
}

VkBool32 ImageData::copy(void* data, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout) const
{
    if (!data || mipLevel >= mipLevels || arrayLayer >= arrayLayers || !getData())
    {
        return VK_FALSE;
    }

    if (bytesPerChannel == 0 || numberChannels == 0)
    {
        return VK_FALSE;
    }

	VkExtent3D currentExtent;

    size_t offset = getOffset(currentExtent, mipLevel, arrayLayer);

    //

    const uint8_t* currentSourceBuffer = &(static_cast<const uint8_t*>(buffer->getData())[offset]);

    uint8_t* currentTargetBuffer = static_cast<uint8_t*>(data);

    //

    const uint8_t* currentSourceChannel = nullptr;

    uint8_t* currentTargetChannel = nullptr;

    for (uint32_t z = 0; z < currentExtent.depth; z++)
    {
        for (uint32_t y = 0; y < currentExtent.height; y++)
        {
            currentSourceChannel = &currentSourceBuffer[y * currentExtent.width * numberChannels * bytesPerChannel + z * currentExtent.height * currentExtent.width * numberChannels * bytesPerChannel];

            currentTargetChannel = &currentTargetBuffer[y * subresourceLayout.rowPitch + z * subresourceLayout.depthPitch + arrayLayer * subresourceLayout.arrayPitch + subresourceLayout.offset];

            memcpy(currentTargetChannel, currentSourceChannel, numberChannels * bytesPerChannel * currentExtent.width);
        }
    }

    return VK_TRUE;
}

VkBool32 ImageData::upload(const void* data, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout) const
{
    if (!data || mipLevel >= mipLevels || arrayLayer >= arrayLayers || !getData())
    {
        return VK_FALSE;
    }

    if (bytesPerChannel == 0 || numberChannels == 0)
    {
        return VK_FALSE;
    }

	VkExtent3D currentExtent;

    size_t offset = getOffset(currentExtent, mipLevel, arrayLayer);

    //

    if (subresourceLayout.size > (VkDeviceSize)(buffer->getSize() - offset))
    {
        return VK_FALSE;
    }

    const uint8_t* currentSourceBuffer = static_cast<const uint8_t*>(data);

    if (!buffer->seek(offset, VKTS_SEARCH_ABSOLUTE))
    {
        return VK_FALSE;
    }

    //

    const uint8_t* currentSourceChannel = nullptr;

    for (uint32_t z = 0; z < currentExtent.depth; z++)
    {
        for (uint32_t y = 0; y < currentExtent.height; y++)
        {
            currentSourceChannel = &currentSourceBuffer[y * subresourceLayout.rowPitch + z * subresourceLayout.depthPitch + arrayLayer * subresourceLayout.arrayPitch + subresourceLayout.offset];

            buffer->write(currentSourceChannel, 1, numberChannels * bytesPerChannel * currentExtent.width);
        }
    }

    return VK_TRUE;
}

VkBool32 ImageData::isUNORM() const
{
    return UNORM;
}

VkBool32 ImageData::isSFLOAT() const
{
    return SFLOAT;
}

int32_t ImageData::getBytesPerChannel() const
{
    return bytesPerChannel;
}

int32_t ImageData::getNumberChannels() const
{
    return numberChannels;
}

void ImageData::setTexel(const glm::vec4& rgba, const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t mipLevel, const uint32_t arrayLayer)
{
    if (x >= extent.width || y >= extent.height || z >= extent.depth || mipLevel >= mipLevels || arrayLayer >= arrayLayers)
    {
        return;
    }

	VkExtent3D currentExtent;

    size_t offset = getOffset(currentExtent, mipLevel, arrayLayer);

    //

    buffer->seek((uint32_t)offset + numberChannels * bytesPerChannel * x + numberChannels * bytesPerChannel * y * extent.width + numberChannels * bytesPerChannel * z * extent.width * extent.height, VKTS_SEARCH_ABSOLUTE);

    if (UNORM)
    {
        uint8_t texelStart[4];

        int32_t currentTargetChannelIndex;

        for (int32_t currentChannelIndex = 0; currentChannelIndex < numberChannels; currentChannelIndex++)
        {
            currentTargetChannelIndex = currentChannelIndex;

            if (format == VK_FORMAT_B8G8R8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                if (currentChannelIndex == 0)
                {
                    currentTargetChannelIndex = 2;
                }
                else if (currentChannelIndex == 2)
                {
                    currentTargetChannelIndex = 0;
                }
            }

            texelStart[currentTargetChannelIndex] = (uint8_t)(rgba[currentChannelIndex] * 255.0f);
        }

        buffer->write(texelStart, 1, numberChannels);
    }
    else if (SFLOAT)
    {
        float texelStart[4];

        for (int32_t currentChannelIndex = 0; currentChannelIndex < numberChannels; currentChannelIndex++)
        {
            texelStart[currentChannelIndex] = rgba[currentChannelIndex];
        }

        buffer->write(texelStart, sizeof(float), numberChannels);
    }
}

glm::vec4 ImageData::getTexel(const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t mipLevel, const uint32_t arrayLayer) const
{
    if (x >= extent.width || y >= extent.height || z >= extent.depth || mipLevel >= mipLevels || arrayLayer >= arrayLayers)
    {
        return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

	VkExtent3D currentExtent;

    size_t offset = getOffset(currentExtent, mipLevel, arrayLayer);

    //

    buffer->seek((uint32_t)offset + numberChannels * bytesPerChannel * x + numberChannels * bytesPerChannel * y * extent.width + numberChannels * bytesPerChannel * z * extent.width * extent.height, VKTS_SEARCH_ABSOLUTE);

    glm::vec4 result(0.0f, 0.0f, 0.0f, 1.0f);

    if (UNORM)
    {
        uint8_t texelStart[4];

        buffer->read(texelStart, 1, numberChannels);

        int32_t currentTargetChannelIndex;

        for (int32_t currentChannelIndex = 0; currentChannelIndex < numberChannels; currentChannelIndex++)
        {
            currentTargetChannelIndex = currentChannelIndex;

            if (format == VK_FORMAT_B8G8R8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                if (currentChannelIndex == 0)
                {
                    currentTargetChannelIndex = 2;
                }
                else if (currentChannelIndex == 2)
                {
                    currentTargetChannelIndex = 0;
                }
            }

            result[currentTargetChannelIndex] = (float)(texelStart[currentChannelIndex]) / 255.0f;
        }
    }
    else if (SFLOAT)
    {
        float texelStart[4];

        buffer->read(texelStart, sizeof(float), numberChannels);

        for (int32_t currentChannelIndex = 0; currentChannelIndex < numberChannels; currentChannelIndex++)
        {
            result[currentChannelIndex] = texelStart[currentChannelIndex];
        }
    }

    return result;
}

glm::vec4 ImageData::getSample(const float x, const VkSamplerMipmapMode mipmapModeX, const VkSamplerAddressMode addressModeX, const float y, const VkSamplerMipmapMode mipmapModeY, const VkSamplerAddressMode addressModeY, const float z, const VkSamplerMipmapMode mipmapModeZ, const VkSamplerAddressMode addressModeZ, const uint32_t mipLevel, const uint32_t arrayLayer) const
{
	glm::vec4 result(0.0f, 0.0f, 0.0f, 0.0f);

	//

	float fractionX;
	float fractionY;
	float fractionZ;

	int32_t texelX = getTexelLocation(fractionX, x, (int32_t)extent.width, addressModeX);
	int32_t texelY = getTexelLocation(fractionY, y, (int32_t)extent.height, addressModeY);
	int32_t texelZ = getTexelLocation(fractionZ, z, (int32_t)extent.depth, addressModeZ);

	int32_t texelOffsetX = 0;
	int32_t texelOffsetY = 0;
	int32_t texelOffsetZ = 0;

	//

	float weightX = 1.0f - (fabsf(0.5 - fractionX) * 2.0f);
	float weightY = 1.0f - (fabsf(0.5 - fractionY) * 2.0f);
	float weightZ = 1.0f - (fabsf(0.5 - fractionZ) * 2.0f);

	//

	float weight;

	for (uint32_t currentZ = 0; currentZ < (uint32_t)mipmapModeZ + 1; currentZ++)
	{
		weight = 1.0f;

		//

		texelOffsetZ = 0;

		if (mipmapModeZ)
		{
			if (currentZ == 0)
			{
				weight *= weightZ;
			}
			else
			{
				weight *= (1.0f - weightZ);

				if (fractionZ >= 0.5f)
				{
					texelOffsetZ++;
				}
				else
				{
					texelOffsetZ--;
				}
			}
		}

		//

		for (uint32_t currentY = 0; currentY < (uint32_t)mipmapModeY + 1; currentY++)
		{
			texelOffsetY = 0;

			if (mipmapModeY)
			{
				if (currentY == 0)
				{
					weight *= weightY;
				}
				else
				{
					weight *= (1.0f - weightY);

					if (fractionY >= 0.5f)
					{
						texelOffsetY++;
					}
					else
					{
						texelOffsetY--;
					}
				}
			}

			//

			for (uint32_t currentX = 0; currentX < (uint32_t)mipmapModeX + 1; currentX++)
			{
				texelOffsetX = 0;

				if (mipmapModeX)
				{
					if (currentX == 0)
					{
						weight *= weightX;
					}
					else
					{
						weight *= (1.0f - weightX);

						if (fractionX >= 0.5f)
						{
							texelOffsetX++;
						}
						else
						{
							texelOffsetX--;
						}
					}
				}

				//

				result += getTexel((uint32_t)(texelX + texelOffsetX), (uint32_t)(texelY + texelOffsetY), (uint32_t)(texelZ + texelOffsetZ), mipLevel, arrayLayer) * weight;
			}
		}
	}

	return result;
}

} /* namespace vkts */
