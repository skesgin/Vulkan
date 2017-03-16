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

#include <vkts/image/vkts_image.hpp>

#include "fn_image_data_internal.hpp"
#include "ImageData.hpp"

#define VKTS_HDR_HEADER_SIZE 52

namespace vkts
{

static PFN_imageDataLoadFunction g_loadFunction = nullptr;
static VkBool32 g_loadFallback = VK_TRUE;
static PFN_imageDataSaveFunction g_saveFunction = nullptr;
static VkBool32 g_saveFallback = VK_TRUE;

template<typename T>
static void imageDataSwapRedBlueChannel(const uint32_t numberChannels, T* data, const uint32_t length)
{
    if (!(numberChannels == 3 || numberChannels == 4) || !data || length == 0)
    {
        return;
    }

    T copyChannel;

    for (uint32_t i = 0; i < length; i++)
    {
        copyChannel = data[i * numberChannels + 0];
        data[i * numberChannels + 0] = data[i * numberChannels + 2];
        data[i * numberChannels + 2] = copyChannel;
    }
}

template<typename T>
static void imageDataConvertRGBtoRGBA(const T alpha, T* targetData, const T* sourceData, const uint32_t length)
{
    if (!targetData || !sourceData || length == 0)
    {
        return;
    }

    for (uint32_t i = 0; i < length; i++)
    {
        for (uint32_t k = 0; k < 3; k++)
        {
            targetData[i * 4 + k] = sourceData[i * 3 + k];
        }

        targetData[i * 4 + 3] = alpha;
    }
}

static void imageDataConvertRGBEtoRGB(float* rgb, const uint8_t* rgbe)
{
    if (!rgb || !rgbe)
    {
        return;
    }

    float exponent = static_cast<float>(rgbe[3] - 128);

    float twoRaisedExponent = powf(2.0f, exponent);

    rgb[0] = static_cast<float>(rgbe[0]) / 256.0f * twoRaisedExponent;
    rgb[1] = static_cast<float>(rgbe[1]) / 256.0f * twoRaisedExponent;
    rgb[2] = static_cast<float>(rgbe[2]) / 256.0f * twoRaisedExponent;
}

static void imageDataConvertRGBtoRGBE(uint8_t* rgbe, const float* rgb)
{
    if (!rgbe || !rgb)
    {
        return;
    }

    float significant[3];
    int32_t exponent[3];

    significant[0] = frexpf(rgb[0], &exponent[0]);
    significant[1] = frexpf(rgb[1], &exponent[1]);
    significant[2] = frexpf(rgb[2], &exponent[2]);

    int32_t maxExponent = exponent[0];
    if (exponent[1] > maxExponent)
    {
        maxExponent = exponent[1];
    }
    if (exponent[2] > maxExponent)
    {
        maxExponent = exponent[2];
    }

    rgbe[0] = static_cast<uint8_t>(significant[0] * 256.0f * powf(2.0f, static_cast<float>(exponent[0] - maxExponent)));
    rgbe[1] = static_cast<uint8_t>(significant[1] * 256.0f * powf(2.0f, static_cast<float>(exponent[1] - maxExponent)));
    rgbe[2] = static_cast<uint8_t>(significant[2] * 256.0f * powf(2.0f, static_cast<float>(exponent[2] - maxExponent)));
    rgbe[3] = static_cast<uint8_t>(maxExponent + 128);
}

static IImageDataSP imageDataLoadTga(const std::string& name, const IBinaryBufferSP& buffer)
{
    if (!buffer.get())
    {
        return IImageDataSP();
    }

    if (!buffer->seek(2, VKTS_SEARCH_RELATVE))
    {
        return IImageDataSP();
    }

    // read the image type
    uint8_t imageType;

    if (buffer->read(&imageType, 1, 1) != 1)
    {
        return IImageDataSP();
    }

    // check the image type
    if (imageType != 1 && imageType != 2 && imageType != 3 && imageType != 9 && imageType != 10 && imageType != 11)
    {
        return IImageDataSP();
    }

    VkBool32 hasColorMap = VK_FALSE;
    if (imageType == 1 || imageType == 9)
    {
        hasColorMap = VK_TRUE;
    }

    uint16_t offsetIndexColorMap;
    uint16_t lengthColorMap;
    uint16_t bitsPerPixelColorMap;
    uint32_t numberChannelsColorMap = 0;

    if (hasColorMap)
    {
        if (buffer->read(&offsetIndexColorMap, 2, 1) != 1)
        {
            return IImageDataSP();
        }

        if (buffer->read(&lengthColorMap, 2, 1) != 1)
        {
            return IImageDataSP();
        }

        if (buffer->read(&bitsPerPixelColorMap, 1, 1) != 1)
        {
            return IImageDataSP();
        }

        numberChannelsColorMap = bitsPerPixelColorMap / 8;

        // seek through the tga header, up to the width/height:
        if (!buffer->seek(4, VKTS_SEARCH_RELATVE))
        {
            return IImageDataSP();
        }
    }
    else
    {
        // seek through the tga header, up to the width/height:
        if (!buffer->seek(9, VKTS_SEARCH_RELATVE))
        {
            return IImageDataSP();
        }
    }

    uint16_t width;
    uint16_t height;
    uint16_t depth = 1;

    // read the width
    if (buffer->read(&width, 2, 1) != 1)
    {
        return IImageDataSP();
    }

    // read the height
    if (buffer->read(&height, 2, 1) != 1)
    {
        return IImageDataSP();
    }

    uint8_t bitsPerPixel;

    // read the bits per pixel
    if (buffer->read(&bitsPerPixel, 1, 1) != 1)
    {
        return IImageDataSP();
    }

    if (bitsPerPixel != 8 && bitsPerPixel != 24 && bitsPerPixel != 32)
    {
        return IImageDataSP();
    }

    uint32_t numberChannels = bitsPerPixel / 8;

    // move file pointer to beginning of targa data
    if (!buffer->seek(1, VKTS_SEARCH_RELATVE))
    {
        return IImageDataSP();
    }

    std::vector<uint8_t> colorMap;

    if (hasColorMap)
    {
        // Create color map space.

        colorMap = std::vector<uint8_t>((uint32_t)lengthColorMap * numberChannelsColorMap);

        // Read in the color map.
        if (buffer->read(&colorMap[0], 1, lengthColorMap * numberChannelsColorMap) != lengthColorMap * numberChannelsColorMap)
        {
            return IImageDataSP();
        }

        // swap the color if necessary
        if (bitsPerPixelColorMap == 24 || bitsPerPixelColorMap == 32)
        {
            imageDataSwapRedBlueChannel(numberChannelsColorMap, &colorMap[0], lengthColorMap);
        }
    }

    uint32_t widthHeightDepth = (uint32_t)width * (uint32_t)height * (uint32_t)depth;

    std::vector<uint8_t> data(widthHeightDepth * numberChannels);

    if (imageType == 1 || imageType == 2 || imageType == 3)
    {
        if (buffer->read(&data[0], 1, widthHeightDepth * numberChannels) != widthHeightDepth * numberChannels)
        {
            return IImageDataSP();
        }
    }
    else
    {
        // RLE encoded
        uint32_t pixelsRead = 0;

        while (pixelsRead < widthHeightDepth)
        {
            uint8_t amount;

            if (buffer->read(&amount, 1, 1) != 1)
            {
                return IImageDataSP();
            }

            if (amount & 0x80)
            {
                amount &= 0x7F;

                amount++;

                if (buffer->read(&data[pixelsRead * numberChannels], 1, numberChannels) != numberChannels)
                {
                    return IImageDataSP();
                }

                for (uint32_t i = 1; i < (uint32_t)amount; i++)
                {
                    for (uint32_t k = 0; k < numberChannels; k++)
                    {
                        data[(pixelsRead + i) * numberChannels + k] = data[pixelsRead * numberChannels + k];
                    }
                }
            }
            else
            {
                amount &= 0x7F;

                amount++;

                if (buffer->read(&data[pixelsRead * numberChannels], 1, (uint32_t)amount * numberChannels) != (uint32_t)amount * numberChannels)
                {
                    return IImageDataSP();
                }
            }

            pixelsRead += (uint32_t)amount;
        }
    }

    // swap the color if necessary
    if (bitsPerPixel == 24 || bitsPerPixel == 32)
    {
        imageDataSwapRedBlueChannel(numberChannels, &data[0], widthHeightDepth);
    }

    if (hasColorMap)
    {
        // Allocating new memory, as current memory is a look up table index and not a color.

        auto lookUp = data;

        data = std::vector<uint8_t>(widthHeightDepth * numberChannelsColorMap);

        // Copy color values from the color map into the image data.

        for (uint32_t i = 0; i < widthHeightDepth; i++)
        {
            for (uint32_t k = 0; k < numberChannelsColorMap; k++)
            {
                data[i * numberChannelsColorMap + k] = colorMap[(offsetIndexColorMap + (uint32_t)(lookUp[i])) * numberChannelsColorMap + k];
            }
        }

        numberChannels = numberChannelsColorMap;
    }

    VkFormat format = VK_FORMAT_R8_UNORM;

    if (numberChannels == 3)
    {
        format = VK_FORMAT_R8G8B8_UNORM;
    }
    else if (numberChannels == 4)
    {
        format = VK_FORMAT_R8G8B8A8_UNORM;
    }

    std::vector<uint32_t> allOffsets{0};

    return IImageDataSP(new ImageData(name, VK_IMAGE_TYPE_2D, format, { (uint32_t)width, (uint32_t)height, (uint32_t)depth }, 1, 1, allOffsets, &data[0], widthHeightDepth * numberChannels, 1.0f));
}

static IImageDataSP imageDataLoadHdr(const std::string& name, const IBinaryBufferSP& buffer)
{
    if (!buffer.get())
    {
        return IImageDataSP();
    }

    uint8_t tempBuffer[256];

    if (buffer->read(tempBuffer, 1, 10) != 10)
    {
        return IImageDataSP();
    }

    //
    // Information header
    //

    // Identifier
    if (strncmp((const char*)tempBuffer, "#?RADIANCE", 10))
    {
        return IImageDataSP();
    }

    // Go to variables
    if (!buffer->seek(1, VKTS_SEARCH_RELATVE))
    {
        return IImageDataSP();
    }

    // Variables
    char currentChar = 0;
    while (VK_TRUE)
    {
    	char oldChar = currentChar;

        if (buffer->read(&currentChar, 1, 1) != 1)
        {
            return IImageDataSP();
        }

        // Empty line indicates end of header
        if ((currentChar == '\r' || currentChar == '\n') && oldChar == '\n')
        {
        	if (currentChar == '\r')
        	{
                if (buffer->read(&currentChar, 1, 1) != 1)
                {
                    return IImageDataSP();
                }
        	}

            break;
        }
    }

    // Resolution
    int32_t charIndex = 0;
    while (VK_TRUE)
    {
        if (buffer->read(&currentChar, 1, 1) != 1)
        {
            return IImageDataSP();
        }

        tempBuffer[charIndex++] = currentChar;

        if (currentChar == '\r' || currentChar == '\n')
        {
        	if (currentChar == '\r')
        	{
                if (buffer->read(&currentChar, 1, 1) != 1)
                {
                    return IImageDataSP();
                }

                tempBuffer[charIndex++] = currentChar;
        	}

        	break;
        }
    }
    tempBuffer[charIndex++] = '\0';

    int32_t width;
    int32_t height;

    if (!sscanf((const char*)tempBuffer, "-Y %d +X %d", &height, &width))
    {
        return IImageDataSP();
    }

    int32_t depth = 1;
    uint32_t numberChannels = 3;

    std::vector<float> data(width * height * depth * numberChannels);

    // Scanlines
    std::vector<uint8_t> scanline(width * 4);

    float rgb[3] = {0.0f, 0.0f, 0.0f};

    float maxLuminance = 0.0;

    int32_t y = height - 1;
    while (y >= 0)
    {
        if (buffer->read(tempBuffer, 1, 4) != 4)
        {
            return IImageDataSP();
        }

        // Examine value
        if (width < 32768 && tempBuffer[0] == 2 && tempBuffer[1] == 2 && tempBuffer[2] == ((width >> 8) & 0xFF) && tempBuffer[3] == (width & 0xFF))
        {
			// New RLE decoding

        	for (int32_t channel = 0; channel < 4; channel++)
			{
        		int32_t currentX = 0;

				while (currentX < width)
				{
					if (buffer->read(tempBuffer, 1, 1) != 1)
					{
						return IImageDataSP();
					}

					int32_t loop = (int32_t)tempBuffer[0];

					if (loop > 128)
					{
						loop &= 127;

						if (buffer->read(tempBuffer, 1, 1) != 1)
						{
							return IImageDataSP();
						}

						for (int32_t g = 0; g < loop; g++)
						{
							scanline[currentX++ * 4 + channel] = tempBuffer[0];
						}
					}
					else
					{
						for (int32_t g = 0; g < loop; g++)
						{
							if (buffer->read(tempBuffer, 1, 1) != 1)
							{
								return IImageDataSP();
							}

			    			scanline[currentX++ * 4 + channel] = tempBuffer[0];
						}
					}
				}
			}
        }
        else
        {
        	// Old RLE decoding

        	if (!buffer->seek(-4, VKTS_SEARCH_RELATVE))
        	{
        		return IImageDataSP();
        	}

            int32_t rshift = 0;
            int32_t currentX = 0;

            while (currentX < width)
            {
                if (buffer->read(tempBuffer, 1, 4) != 4)
                {
                    return IImageDataSP();
                }

    			scanline[currentX * 4 + 0] = tempBuffer[0];
            	scanline[currentX * 4 + 1] = tempBuffer[1];
            	scanline[currentX * 4 + 2] = tempBuffer[2];
            	scanline[currentX * 4 + 3] = tempBuffer[3];

            	if (tempBuffer[0] == 1 && tempBuffer[1] == 1 && tempBuffer[2] == 1)
            	{
            		int32_t loop = ((int32_t)tempBuffer[3]) << rshift;

            		for (int32_t i = 0; i < loop; i++)
            		{
                    	scanline[currentX * 4 + 0] = scanline[(currentX - 1) * 4 + 0];
                    	scanline[currentX * 4 + 1] = scanline[(currentX - 1) * 4 + 1];
                    	scanline[currentX * 4 + 2] = scanline[(currentX - 1) * 4 + 2];
                    	scanline[currentX * 4 + 3] = scanline[(currentX - 1) * 4 + 3];

                    	currentX++;
            		}

    				rshift += 8;
            	}
            	else
            	{
                	currentX++;

            		rshift = 0;
            	}
            }
    	}

        maxLuminance = glm::max(maxLuminance, glm::dot(glm::vec3(rgb[0], rgb[1], rgb[2]), glm::vec3(0.2126f, 0.7152f, 0.0722f)));

        for (int32_t x = 0; x < width; x++)
        {
            imageDataConvertRGBEtoRGB(rgb, &scanline[x * 4]);

            data[width * y * numberChannels + x * numberChannels + 0] = rgb[0];
            data[width * y * numberChannels + x * numberChannels + 1] = rgb[1];
            data[width * y * numberChannels + x * numberChannels + 2] = rgb[2];
        }

        y--;
    }

    std::vector<uint32_t> allOffsets{0};

    return IImageDataSP(new ImageData(name, VK_IMAGE_TYPE_2D, VK_FORMAT_R32G32B32_SFLOAT, { (uint32_t)width, (uint32_t)height, (uint32_t)depth }, 1, 1, allOffsets, reinterpret_cast<const uint8_t*>(&data[0]), width * height * depth * numberChannels * (uint32_t)sizeof(float), maxLuminance));
}

void VKTS_APIENTRY imageDataSetLoadFunction(const PFN_imageDataLoadFunction loadFunction, const VkBool32 fallback)
{
	g_loadFunction = loadFunction;
	g_loadFallback = fallback;
}

void VKTS_APIENTRY imageDataSetSaveFunction(const PFN_imageDataSaveFunction saveFunction, const VkBool32 fallback)
{
	g_saveFunction = saveFunction;
	g_saveFallback = fallback;
}

IImageDataSP VKTS_APIENTRY imageDataLoad(const char* filename)
{
	if (g_loadFunction)
	{
		auto externalImageData = g_loadFunction(filename);

		if (externalImageData.get())
		{
			return externalImageData;
		}

		if (!g_loadFallback)
		{
			return IImageDataSP();
		}
	}

	//

    if (!filename)
    {
        return IImageDataSP();
    }

    auto buffer = fileLoadBinary(filename);

    if (!buffer.get())
    {
        return IImageDataSP();
    }

    std::string lowerCaseFilename(filename);
    std::transform(lowerCaseFilename.begin(), lowerCaseFilename.end(), lowerCaseFilename.begin(), ::tolower);

    auto dotPos = lowerCaseFilename.rfind('.');
    if (dotPos == lowerCaseFilename.npos)
    {
        return IImageDataSP();
    }

    std::string lowerCaseExtension = lowerCaseFilename.substr(dotPos);

    if (lowerCaseExtension == ".tga")
    {
        return imageDataLoadTga(filename, buffer);
    }
    else if (lowerCaseExtension == ".hdr")
    {
        return imageDataLoadHdr(filename, buffer);
    }
    else if (lowerCaseExtension == ".dds" || lowerCaseExtension == ".ktx")
    {
        return imageDataLoadGli(filename, buffer);
    }
    else if (lowerCaseExtension == ".png" || lowerCaseExtension == ".jpg" || lowerCaseExtension == ".jpeg")
    {
        return imageDataLoadStb(filename, buffer);
    }

    return IImageDataSP();
}

IImageDataSP VKTS_APIENTRY imageDataLoadRaw(const char* filename, const uint32_t width, const uint32_t height, const VkFormat format)
{
    if (!filename || width == 0 || height == 0)
    {
        return IImageDataSP();
    }

    if (!(imageDataIsUNORM(format) || imageDataIsSFLOAT(format)))
    {
    	return IImageDataSP();
    }

    auto buffer = fileLoadBinary(filename);

    if (!buffer.get())
    {
        return IImageDataSP();
    }

    uint32_t expectedSize = width * height * imageDataGetBytesPerChannel(format) * imageDataGetNumberChannels(format);

    if (buffer->getSize() != expectedSize)
    {
        return IImageDataSP();
    }

    std::vector<uint32_t> allOffsets{0};

    return IImageDataSP(new ImageData(filename, VK_IMAGE_TYPE_2D, format, { width, height, 1 }, 1, 1, allOffsets, buffer->getByteData(), expectedSize, 1.0f));
}

static IBinaryBufferSP imageDataSaveTga(const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer)
{
    if (!imageData.get())
    {
        return IBinaryBufferSP();
    }

    if (!(imageData->getFormat() == VK_FORMAT_R8_UNORM || imageData->getFormat() == VK_FORMAT_R8G8B8_UNORM || imageData->getFormat() == VK_FORMAT_B8G8R8_UNORM || imageData->getFormat() == VK_FORMAT_R8G8B8A8_UNORM || imageData->getFormat() == VK_FORMAT_B8G8R8A8_UNORM))
    {
        return IBinaryBufferSP();
    }

    uint8_t bitsPerPixel;

    if (imageData->getFormat() == VK_FORMAT_R8_UNORM)
    {
        bitsPerPixel = 8;
    }
    else if (imageData->getFormat() == VK_FORMAT_R8G8B8_UNORM || imageData->getFormat() == VK_FORMAT_B8G8R8_UNORM)
    {
        bitsPerPixel = 24;
    }
    else if (imageData->getFormat() == VK_FORMAT_R8G8B8A8_UNORM || imageData->getFormat() == VK_FORMAT_B8G8R8A8_UNORM)
    {
        bitsPerPixel = 32;
    }
    else
    {
        return IBinaryBufferSP();
    }

    uint32_t numberChannels = bitsPerPixel / 8;

	VkExtent3D currentExtent;
	uint32_t offset;
    if (!imageData->getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer))
    {
    	return IBinaryBufferSP();
    }

    uint32_t size = currentExtent.width * currentExtent.height * currentExtent.depth * numberChannels + 18;

    // 18 bytes is the size of the header.
    IBinaryBufferSP buffer = binaryBufferCreate(size);

    if (!buffer.get() || buffer->getSize() != size)
    {
        return IBinaryBufferSP();
    }

    //

    uint8_t header[12];

    memset(header, 0, 12);

    if (bitsPerPixel == 8)
    {
        header[2] = 3;
    }
    else
    {
        header[2] = 2;
    }

    //

    if (buffer->write(header, 1, 12) != 12)
    {
        return IBinaryBufferSP();
    }

    uint16_t width = static_cast<uint16_t>(currentExtent.width);
    uint16_t height = static_cast<uint16_t>(currentExtent.height);
    uint16_t depth = 1;

    if (buffer->write(&width, 2, 1) != 1)
    {
        return IBinaryBufferSP();
    }

    if (buffer->write(&height, 2, 1) != 1)
    {
        return IBinaryBufferSP();
    }

    if (buffer->write(&bitsPerPixel, 1, 1) != 1)
    {
        return IBinaryBufferSP();
    }

    header[0] = 0;

    if (buffer->write(header, 1, 1) != 1)
    {
        return IBinaryBufferSP();
    }

    //
    uint32_t widthHeightDepth = (uint32_t)width * (uint32_t)height * (uint32_t)depth;

    std::vector<uint8_t> data(widthHeightDepth * numberChannels);

    memcpy(&data[0], &(imageData->getByteData()[offset]), widthHeightDepth * numberChannels);

    if (imageData->getFormat() == VK_FORMAT_R8G8B8_UNORM || imageData->getFormat() == VK_FORMAT_R8G8B8A8_UNORM)
    {
        imageDataSwapRedBlueChannel(numberChannels, &data[0], widthHeightDepth);
    }

    if (buffer->write(&data[0], 1, widthHeightDepth * numberChannels) != widthHeightDepth * numberChannels)
    {
        return IBinaryBufferSP();
    }

    return buffer;
}

static IBinaryBufferSP imageDataSaveHdr(const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer)
{
    if (!imageData.get())
    {
        return IBinaryBufferSP();
    }

    if (imageData->getFormat() != VK_FORMAT_R32G32B32_SFLOAT)
    {
        return IBinaryBufferSP();
    }


	VkExtent3D currentExtent;
	uint32_t offset;
    if (!imageData->getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer))
    {
    	return IBinaryBufferSP();
    }

    char tempBuffer[256];

    if (snprintf(tempBuffer, 256, "-Y %d +X %d\n", currentExtent.height, currentExtent.width) < 0)
    {
        return IBinaryBufferSP();
    }

    uint32_t numberChannels = 3;

    uint32_t size = (currentExtent.width * currentExtent.height * currentExtent.depth) * 4 * (uint32_t)sizeof(uint8_t) + VKTS_HDR_HEADER_SIZE + (uint32_t)strlen(tempBuffer);

    // 52 bytes is the size of the header. RGB, where each channel is 4 bytes, is encoded in total of 4 bytes.
    IBinaryBufferSP buffer = binaryBufferCreate(size);

    if (!buffer.get() || buffer->getSize() != size)
    {
        return IBinaryBufferSP();
    }

    // Header
    if (buffer->write("#?RADIANCE\n#Saved with VKTS\nFORMAT=32-bit_rle_rgbe\n\n", 1, VKTS_HDR_HEADER_SIZE) != VKTS_HDR_HEADER_SIZE)
    {
        return IBinaryBufferSP();
    }

    // Resolution
    if (buffer->write(tempBuffer, 1, (uint32_t)strlen(tempBuffer)) != (uint32_t)strlen(tempBuffer))
    {
        return IBinaryBufferSP();
    }

    uint8_t rgbe[4];

    const float* tempData = reinterpret_cast<const float*>(&(imageData->getByteData()[offset]));

    // Non compressed data
    for (int32_t y = (int32_t)currentExtent.height - 1; y >= 0; y--)
    {
        for (int32_t x = 0; x < (int32_t)currentExtent.width; x++)
        {
            imageDataConvertRGBtoRGBE(rgbe, &tempData[(y * currentExtent.width + x) * numberChannels]);

            if (buffer->write(rgbe, 1, 4) != 4)
            {
                return IBinaryBufferSP();
            }
        }
    }

    return buffer;
}

VkBool32 VKTS_APIENTRY imageDataSave(const char* filename, const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer)
{
	if (g_saveFunction)
	{
		if (g_saveFunction(filename, imageData, mipLevel, arrayLayer))
		{
			return VK_TRUE;
		}

		if (!g_saveFallback)
		{
			return VK_FALSE;
		}
	}

	//

    if (!filename || !imageData.get())
    {
        return VK_FALSE;
    }

    if (imageData->getImageType() != VK_IMAGE_TYPE_2D || imageData->getWidth() < 1 || imageData->getHeight() < 1 || imageData->getDepth() != 1 || mipLevel >= imageData->getMipLevels() || arrayLayer >= imageData->getArrayLayers())
    {
        return VK_FALSE;
    }

    std::string lowerCaseFilename(filename);
    std::transform(lowerCaseFilename.begin(), lowerCaseFilename.end(), lowerCaseFilename.begin(), ::tolower);

    auto dotPos = lowerCaseFilename.rfind('.');
    if (dotPos == lowerCaseFilename.npos)
    {
        return VK_FALSE;
    }

    std::string lowerCaseExtension = lowerCaseFilename.substr(dotPos);

    if (lowerCaseExtension == ".tga")
    {
        auto buffer = imageDataSaveTga(imageData, mipLevel, arrayLayer);

        if (!buffer.get())
        {
            return VK_FALSE;
        }

        return fileSaveBinary(filename, buffer);
    }
    else if (lowerCaseExtension == ".hdr")
    {
        auto buffer = imageDataSaveHdr(imageData, mipLevel, arrayLayer);

        if (!buffer.get())
        {
            return VK_FALSE;
        }

        return fileSaveBinary(filename, buffer);
    }
    else if (lowerCaseExtension == ".data")
    {
    	VkExtent3D currentExtent;
    	uint32_t offset;

    	if (!imageData->getExtentAndOffset(currentExtent, offset, mipLevel, arrayLayer))
        {
        	return VK_FALSE;
        }

    	uint32_t currentSize = currentExtent.width * currentExtent.height * imageData->getNumberChannels() * imageData->getBytesPerChannel();

        return fileSaveBinaryData(filename, &imageData->getByteData()[offset], currentSize);
    }
    else if (lowerCaseExtension == ".dds" || lowerCaseExtension == ".ktx" )
    {
    	return imageDataSaveGli(filename, imageData, mipLevel, arrayLayer);
    }
    else if (lowerCaseExtension == ".png")
    {
    	return imageDataSaveStb(filename, imageData, mipLevel, arrayLayer);
    }

    return VK_FALSE;
}

IImageDataSP VKTS_APIENTRY imageDataCreate(const std::string& name, const uint32_t width, const uint32_t height, const uint32_t depth, const VkImageType imageType, const VkFormat& format)
{
    if (width < 1 || height < 1 || depth < 1)
    {
        return IImageDataSP();
    }

    if (imageType == VK_IMAGE_TYPE_1D && height != 1 && depth != 1)
    {
        return IImageDataSP();
    }
    else if (imageType == VK_IMAGE_TYPE_2D && depth != 1)
    {
        return IImageDataSP();
    }

    uint32_t bytesPerChannel;
    uint32_t numberChannels;

    if (format == VK_FORMAT_R8_UNORM)
    {
        bytesPerChannel = 1;

        numberChannels = 1;
    }
    else if (format == VK_FORMAT_R8_SRGB)
    {
        bytesPerChannel = 1;

        numberChannels = 1;
    }
    else if (format == VK_FORMAT_R8G8_UNORM)
    {
        bytesPerChannel = 1;

        numberChannels = 2;
    }
    else if (format == VK_FORMAT_R8G8_SRGB)
    {
        bytesPerChannel = 1;

        numberChannels = 2;
    }
    else if (format == VK_FORMAT_R8G8B8_UNORM)
    {
        bytesPerChannel = 1;

        numberChannels = 3;
    }
    else if (format == VK_FORMAT_R8G8B8_SRGB)
    {
        bytesPerChannel = 1;

        numberChannels = 3;
    }
    else if (format == VK_FORMAT_B8G8R8_UNORM)
    {
        bytesPerChannel = 1;

        numberChannels = 3;
    }
    else if (format == VK_FORMAT_B8G8R8_SRGB)
    {
        bytesPerChannel = 1;

        numberChannels = 3;
    }
    else if (format == VK_FORMAT_R8G8B8A8_UNORM)
    {
        bytesPerChannel = 1;

        numberChannels = 4;
    }
    else if (format == VK_FORMAT_R8G8B8A8_SRGB)
    {
        bytesPerChannel = 1;

        numberChannels = 4;
    }
    else if (format == VK_FORMAT_B8G8R8A8_UNORM)
    {
        bytesPerChannel = 1;

        numberChannels = 4;
    }
    else if (format == VK_FORMAT_B8G8R8A8_SRGB)
    {
        bytesPerChannel = 1;

        numberChannels = 4;
    }
    else if (format == VK_FORMAT_R32_SFLOAT)
    {
        bytesPerChannel = 4;

        numberChannels = 1;
    }
    else if (format == VK_FORMAT_R32G32_SFLOAT)
    {
        bytesPerChannel = 4;

        numberChannels = 2;
    }
    else if (format == VK_FORMAT_R32G32B32_SFLOAT)
    {
        bytesPerChannel = 4;

        numberChannels = 3;
    }
    else if (format == VK_FORMAT_R32G32B32A32_SFLOAT)
    {
        bytesPerChannel = 4;

        numberChannels = 4;
    }
    else
    {
        return IImageDataSP();
    }

    std::vector<uint8_t> data(width * height * depth * numberChannels * bytesPerChannel);

    memset(&data[0], 0, (width * height * depth * numberChannels * bytesPerChannel));

    std::vector<uint32_t> allOffsets{0};

    return IImageDataSP(new ImageData(name, imageType, format, { width, height, depth }, 1, 1, allOffsets, &data[0], width * height * depth * numberChannels * bytesPerChannel, 1.0f));
}

IImageDataSP VKTS_APIENTRY imageDataCreate(const std::string& name, const uint32_t width, const uint32_t height, const uint32_t depth, const glm::vec4& color, const VkImageType imageType, const VkFormat& format)
{
	auto currentImageData = imageDataCreate(name, width, height, depth, imageType, format);

	if (!currentImageData.get())
	{
        return IImageDataSP();
    }

    for (uint32_t z = 0; z < depth; z++)
    {
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
            	currentImageData->setTexel(color, x, y, z, 0, 0);
            }
        }
    }

    return currentImageData;
}

IImageDataSP VKTS_APIENTRY imageDataCreate(const std::string& name, const uint32_t width, const uint32_t height, const uint32_t depth, const float red, const float green, const float blue, const float alpha, const VkImageType imageType, const VkFormat& format)
{
	return imageDataCreate(name, width, height, depth, glm::vec4(red, green, blue, alpha), imageType, format);
}

IImageDataSP VKTS_APIENTRY imageDataUnite(const IImageDataSP& sourceImage0, const IImageDataSP& sourceImage1, const std::string& name, const VkTsImageDataChannels red, const VkTsImageDataChannels green, const VkTsImageDataChannels blue, const VkTsImageDataChannels alpha, const VkFormat format)
{
	if (!sourceImage0.get() || !sourceImage1.get())
	{
		return IImageDataSP();
	}

	if (sourceImage0->getImageType() != sourceImage1->getImageType())
	{
		return IImageDataSP();
	}

	if ((sourceImage0->getWidth() != sourceImage1->getWidth()) || (sourceImage0->getHeight() != sourceImage1->getHeight()) || (sourceImage0->getDepth() != sourceImage1->getDepth()))
	{
		return IImageDataSP();
	}

	auto width = sourceImage0->getWidth();
	auto height = sourceImage0->getHeight();
	auto depth = sourceImage0->getDepth();

	//

	auto currentImageData = imageDataCreate(name, width, height, depth, sourceImage0->getImageType(), format);

	if (!currentImageData.get())
	{
        return IImageDataSP();
    }

    for (uint32_t z = 0; z < depth; z++)
    {
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
            	glm::vec4 color;

            	for (uint32_t i = 0; i < 4; i++)
            	{
            		VkTsImageDataChannels current;

            		switch (i)
            		{
            			case 0:
            				current = red;
            				break;
            			case 1:
            				current = green;
            				break;
            			case 2:
            				current = blue;
            				break;
            			case 3:
            				current = alpha;
            				break;
            		}

            		float c = 0.0f;

            		switch (current)
            		{
						case VKTS_SOURCE_0_RED:
							c = sourceImage0->getTexel(x, y, z, 0, 0).r;
							break;
						case VKTS_SOURCE_0_GREEN:
							c = sourceImage0->getTexel(x, y, z, 0, 0).g;
							break;
						case VKTS_SOURCE_0_BLUE:
							c = sourceImage0->getTexel(x, y, z, 0, 0).b;
							break;
						case VKTS_SOURCE_0_ALPHA:
							c = sourceImage0->getTexel(x, y, z, 0, 0).a;
							break;
						case VKTS_SOURCE_1_RED:
							c = sourceImage1->getTexel(x, y, z, 0, 0).r;
							break;
						case VKTS_SOURCE_1_GREEN:
							c = sourceImage1->getTexel(x, y, z, 0, 0).g;
							break;
						case VKTS_SOURCE_1_BLUE:
							c = sourceImage1->getTexel(x, y, z, 0, 0).b;
							break;
						case VKTS_SOURCE_1_ALPHA:
							c = sourceImage1->getTexel(x, y, z, 0, 0).a;
							break;
						case VKTS_SOURCE_ZERO:
							c = 0.0f;
							break;
						case VKTS_SOURCE_ONE:
							c = 1.0f;
							break;
            		}

            		color[i] = c;
            	}

            	currentImageData->setTexel(color, x, y, z, 0, 0);
            }
        }
    }

	return currentImageData;
}

IImageDataSP VKTS_APIENTRY imageDataConvert(const IImageDataSP& sourceImage, const VkFormat targetFormat, const std::string& name, const enum VkTsImageDataType targetImageDataType, const enum VkTsImageDataType sourceImageDataType, const glm::vec4& factor, const std::array<VkBool32, 3>& mirror)
{
    if (!sourceImage.get() || sourceImage->getMipLevels() != 1 || sourceImage->getArrayLayers() != 1)
    {
        return IImageDataSP();
    }

    //

    if ((targetFormat == sourceImage->getFormat()) && (targetImageDataType == sourceImageDataType) && (factor[0] == 1.0f) && (factor[1] == 1.0f) && (factor[2] == 1.0f) && (factor[3] == 1.0f) && !mirror[0] && !mirror[1] && !mirror[2])
    {
    	return imageDataCopy(sourceImage, name);
    }

    //

    int32_t sourceNumberChannels;

    VkBool32 sourceIsUNORM = VK_TRUE;
    VkBool32 sourceIsSFLOAT = VK_FALSE;
    VkBool32 sourceIsSRGB = VK_FALSE;

    int8_t sourceRgbaIndices[4] = { 0, -1, -1, -1 };

    int32_t targetBytesPerChannel;
    int32_t targetNumberChannels;

    VkBool32 targetIsUNORM = VK_TRUE;
    VkBool32 targetIsSFLOAT = VK_FALSE;
    VkBool32 targetIsSRGB = VK_FALSE;

    int8_t targetRgbaIndices[4] = { 0, -1, -1, -1 };

    switch (sourceImage->getFormat())
    {
        case VK_FORMAT_R8_UNORM:

            sourceNumberChannels = 1;

            break;

        case VK_FORMAT_R8_SRGB:

            sourceNumberChannels = 1;

            sourceIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_R8G8_UNORM:

            sourceNumberChannels = 2;

            sourceRgbaIndices[1] = 1;

            break;

        case VK_FORMAT_R8G8_SRGB:

            sourceNumberChannels = 2;

            sourceRgbaIndices[1] = 1;

            sourceIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_R8G8B8_UNORM:

            sourceNumberChannels = 3;

            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 2;

            break;

        case VK_FORMAT_R8G8B8_SRGB:

            sourceNumberChannels = 3;

            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 2;

            sourceIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_B8G8R8_UNORM:

            sourceNumberChannels = 3;

            sourceRgbaIndices[0] = 2;
            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 0;

            break;

        case VK_FORMAT_B8G8R8_SRGB:

            sourceNumberChannels = 3;

            sourceRgbaIndices[0] = 2;
            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 0;

            sourceIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_R8G8B8A8_UNORM:

            sourceNumberChannels = 4;

            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 2;
            sourceRgbaIndices[3] = 3;

            break;

        case VK_FORMAT_R8G8B8A8_SRGB:

            sourceNumberChannels = 4;

            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 2;
            sourceRgbaIndices[3] = 3;

            sourceIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_B8G8R8A8_UNORM:

            sourceNumberChannels = 4;

            sourceRgbaIndices[0] = 2;
            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 0;
            sourceRgbaIndices[3] = 3;

            break;

        case VK_FORMAT_B8G8R8A8_SRGB:

            sourceNumberChannels = 4;

            sourceRgbaIndices[0] = 2;
            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 0;
            sourceRgbaIndices[3] = 3;

            sourceIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_R32_SFLOAT:

            sourceNumberChannels = 1;

            sourceIsUNORM = VK_FALSE;
            sourceIsSFLOAT = VK_TRUE;

            break;

        case VK_FORMAT_R32G32_SFLOAT:

            sourceNumberChannels = 2;

            sourceRgbaIndices[1] = 1;

            sourceIsUNORM = VK_FALSE;
            sourceIsSFLOAT = VK_TRUE;

            break;

        case VK_FORMAT_R32G32B32_SFLOAT:

            sourceNumberChannels = 3;

            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 2;

            sourceIsUNORM = VK_FALSE;
            sourceIsSFLOAT = VK_TRUE;

            break;

        case VK_FORMAT_R32G32B32A32_SFLOAT:

            sourceNumberChannels = 4;

            sourceRgbaIndices[1] = 1;
            sourceRgbaIndices[2] = 2;
            sourceRgbaIndices[3] = 3;

            sourceIsUNORM = VK_FALSE;
            sourceIsSFLOAT = VK_TRUE;

            break;

        default:
            return IImageDataSP();
    }

    //

    switch (targetFormat)
    {
        case VK_FORMAT_R8_UNORM:

            targetBytesPerChannel = 1;

            targetNumberChannels = 1;

            break;

        case VK_FORMAT_R8_SRGB:

            targetBytesPerChannel = 1;

            targetNumberChannels = 1;

            targetIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_R8G8_UNORM:

            targetBytesPerChannel = 1;

            targetNumberChannels = 2;

            targetRgbaIndices[1] = 1;

            break;

        case VK_FORMAT_R8G8_SRGB:

            targetBytesPerChannel = 1;

            targetNumberChannels = 2;

            targetRgbaIndices[1] = 1;

            targetIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_R8G8B8_UNORM:

            targetBytesPerChannel = 1;

            targetNumberChannels = 3;

            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 2;

            break;

        case VK_FORMAT_R8G8B8_SRGB:

            targetBytesPerChannel = 1;

            targetNumberChannels = 3;

            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 2;

            targetIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_B8G8R8_UNORM:

            targetBytesPerChannel = 1;

            targetNumberChannels = 3;

            targetRgbaIndices[0] = 2;
            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 0;

            break;

        case VK_FORMAT_B8G8R8_SRGB:

            targetBytesPerChannel = 1;

            targetNumberChannels = 3;

            targetRgbaIndices[0] = 2;
            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 0;

            targetIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_R8G8B8A8_UNORM:

            targetBytesPerChannel = 1;

            targetNumberChannels = 4;

            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 2;
            targetRgbaIndices[3] = 3;

            break;

        case VK_FORMAT_R8G8B8A8_SRGB:

            targetBytesPerChannel = 1;

            targetNumberChannels = 4;

            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 2;
            targetRgbaIndices[3] = 3;

            targetIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_B8G8R8A8_UNORM:

            targetBytesPerChannel = 1;

            targetNumberChannels = 4;

            targetRgbaIndices[0] = 2;
            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 0;
            targetRgbaIndices[3] = 3;

            break;

        case VK_FORMAT_B8G8R8A8_SRGB:

            targetBytesPerChannel = 1;

            targetNumberChannels = 4;

            targetRgbaIndices[0] = 2;
            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 0;
            targetRgbaIndices[3] = 3;

            targetIsSRGB = VK_TRUE;

            break;

        case VK_FORMAT_R32_SFLOAT:

            targetBytesPerChannel = 4;

            targetNumberChannels = 1;

            targetIsUNORM = VK_FALSE;
            targetIsSFLOAT = VK_TRUE;

            break;

        case VK_FORMAT_R32G32_SFLOAT:

            targetBytesPerChannel = 4;

            targetNumberChannels = 2;

            targetRgbaIndices[1] = 1;

            targetIsUNORM = VK_FALSE;
            targetIsSFLOAT = VK_TRUE;

            break;

        case VK_FORMAT_R32G32B32_SFLOAT:

            targetBytesPerChannel = 4;

            targetNumberChannels = 3;

            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 2;

            targetIsUNORM = VK_FALSE;
            targetIsSFLOAT = VK_TRUE;

            break;

        case VK_FORMAT_R32G32B32A32_SFLOAT:

            targetBytesPerChannel = 4;

            targetNumberChannels = 4;

            targetRgbaIndices[1] = 1;
            targetRgbaIndices[2] = 2;
            targetRgbaIndices[3] = 3;

            targetIsUNORM = VK_FALSE;
            targetIsSFLOAT = VK_TRUE;

            break;

        default:
            return IImageDataSP();
    }

    //

    uint32_t targetDataSize = sourceImage->getWidth() * sourceImage->getHeight() * sourceImage->getDepth() * targetNumberChannels * targetBytesPerChannel;

    std::vector<uint8_t> targetData(targetDataSize);

    const uint8_t* currentSourceUINT8 = (const uint8_t*) sourceImage->getData();
    const float* currentSourceFLOAT = (const float*) sourceImage->getData();

    uint8_t* currentTargetUINT8 = &targetData[0];
    float* currentTargetFLOAT = (float*) &targetData[0];

    for (int32_t z = 0; z < (int32_t)sourceImage->getDepth(); z++)
    {
        for (int32_t y = 0; y < (int32_t)sourceImage->getHeight(); y++)
        {
            for (int32_t x = 0; x < (int32_t)sourceImage->getWidth(); x++)
            {
            	int32_t xTarget = mirror[0] ? (sourceImage->getWidth() - 1 - x) : x;
            	int32_t yTarget = mirror[1] ? (sourceImage->getHeight() - 1 - y) : y;
            	int32_t zTarget = mirror[2] ? (sourceImage->getDepth() - 1 - z) : z;

            	float L = 1.0f;

            	if (sourceImageDataType == VKTS_HDR_COLOR_DATA)
				{
            		L = renderColorGetLuminance(sourceImage->getTexel(x, y, z, 0, 0));
				}

                for (int32_t channel = 0; channel < targetNumberChannels; channel++)
                {
                	// Default target channel values, when less source channel values are present.

                    uint8_t currentByte = 0;
                    float currentFloat = 0.0;

                	if (targetRgbaIndices[channel] == 3)
                	{
                		// Alpha is opaque.

                        currentByte = 255;
                        currentFloat = 1.0;
                	}

                	//

                    if (channel < sourceNumberChannels)
                    {
                        if (sourceIsUNORM)
                        {
                        	currentByte = currentSourceUINT8[sourceRgbaIndices[channel] + x * sourceNumberChannels + y * sourceImage->getWidth() * sourceNumberChannels + z * sourceImage->getHeight() * sourceImage->getWidth() * sourceNumberChannels];
                        }
                        else if (sourceIsSFLOAT)
                        {
                        	currentFloat = currentSourceFLOAT[sourceRgbaIndices[channel] + x * sourceNumberChannels + y * sourceImage->getWidth() * sourceNumberChannels + z * sourceImage->getHeight() * sourceImage->getWidth() * sourceNumberChannels];
                        }
                    }

                    //

                    //
                    // Conversions
                    //

                    if (!((targetFormat == sourceImage->getFormat()) && (targetImageDataType == sourceImageDataType) && (factor[0] == 1.0f) && (factor[1] == 1.0f) && (factor[2] == 1.0f) && (factor[3] == 1.0f)))
					{
                    	float c = currentFloat;

                    	float f = factor[targetRgbaIndices[channel]];

                    	if (sourceIsUNORM)
                    	{
                    		c = (float)currentByte / 255.0f;
                    	}

                    	if (targetRgbaIndices[channel] != 3)
                    	{
                    		// Non-linear to linear.
							if (sourceIsSRGB || sourceImageDataType == VKTS_LDR_COLOR_DATA)
							{
								c = powf(c, VKTS_GAMMA);
							}

							// Convert normal data.
							if (!sourceIsSFLOAT && sourceImageDataType == VKTS_NORMAL_DATA)
							{
								c = c * 2.0f - 1.0f;
							}
                    	}

                    	//

						c = c * f;

						//

                    	if (targetRgbaIndices[channel] != 3)
                    	{
							// Tonemap if needed.
							if ((!targetIsSFLOAT || targetImageDataType != VKTS_HDR_COLOR_DATA) && sourceImageDataType == VKTS_HDR_COLOR_DATA)
							{
								c = c * 1.0f / (1.0f + L);
							}

							// Linear to non-linear.
							if (targetIsSRGB || targetImageDataType == VKTS_LDR_COLOR_DATA)
							{
								c = powf(c, 1.0f / VKTS_GAMMA);
							}

							// Convert normal data.
							if (!targetIsSFLOAT && targetImageDataType == VKTS_NORMAL_DATA)
							{
								c = (c + 1.0f) * 0.5f;
							}
                    	}

						currentFloat = c;

                    	if (targetIsUNORM)
                    	{
                    		currentByte = (uint8_t)(255.0f * c);
                    	}
					}

                    //

					if (targetIsUNORM)
					{
						if (sourceIsUNORM)
						{
							currentTargetUINT8[targetRgbaIndices[channel] + xTarget * targetNumberChannels + yTarget * sourceImage->getWidth() * targetNumberChannels + zTarget * sourceImage->getHeight() * sourceImage->getWidth() * targetNumberChannels] = currentByte;
						}
						else if (sourceIsSFLOAT)
						{
							currentTargetUINT8[targetRgbaIndices[channel] + xTarget * targetNumberChannels + yTarget * sourceImage->getWidth() * targetNumberChannels + zTarget * sourceImage->getHeight() * sourceImage->getWidth() * targetNumberChannels] = static_cast<uint8_t>(glm::clamp(currentFloat, 0.0f, 1.0f) * 255.0f);
						}
					}
					else if (targetIsSFLOAT)
					{
						if (sourceIsUNORM)
						{
							currentTargetFLOAT[targetRgbaIndices[channel] + xTarget * targetNumberChannels + yTarget * sourceImage->getWidth() * targetNumberChannels + zTarget * sourceImage->getHeight() * sourceImage->getWidth() * targetNumberChannels] = static_cast<float>(currentByte) / 255.0f;
						}
						else if (sourceIsSFLOAT)
						{
							currentTargetFLOAT[targetRgbaIndices[channel] + xTarget * targetNumberChannels + yTarget * sourceImage->getWidth() * targetNumberChannels + zTarget * sourceImage->getHeight() * sourceImage->getWidth() * targetNumberChannels] = currentFloat;
						}
					}
                }
            }
        }
    }

    std::vector<uint32_t> allOffsets{0};

    return IImageDataSP(new ImageData(name, sourceImage->getImageType(), targetFormat, sourceImage->getExtent3D(), 1, 1, allOffsets, &targetData[0], sourceImage->getWidth() * sourceImage->getHeight() * sourceImage->getDepth() * targetNumberChannels * targetBytesPerChannel, sourceImage->getMaxLuminance()));
}

IImageDataSP VKTS_APIENTRY imageDataCopy(const IImageDataSP& sourceImage, const std::string& name)
{
    if (!sourceImage.get())
    {
        return IImageDataSP();
    }

    return IImageDataSP(new ImageData(name, sourceImage->getImageType(), sourceImage->getFormat(), sourceImage->getExtent3D(), sourceImage->getMipLevels(), sourceImage->getArrayLayers(), sourceImage->getAllOffsets(), (const uint8_t*) sourceImage->getData(), sourceImage->getSize(), sourceImage->getMaxLuminance()));
}

IImageDataSP VKTS_APIENTRY imageDataMerge(const SmartPointerVector<IImageDataSP>& sourceImages, const std::string& name, const uint32_t mipLevels, const uint32_t arrayLayers)
{
    if (sourceImages.size() == 0)
    {
        return IImageDataSP();
    }

    if (mipLevels * arrayLayers != sourceImages.size())
    {
        return IImageDataSP();
    }

    uint32_t totalSize = 0;

    VkImageType imageType;
    VkFormat format;
    VkExtent3D extent;

    for (uint32_t i = 0; i < sourceImages.size(); i++)
    {
    	if (!sourceImages[i].get())
    	{
    		return IImageDataSP();
    	}

        totalSize += sourceImages[i]->getSize();

        if (i == 0)
        {
            imageType = sourceImages[i]->getImageType();
            format = sourceImages[i]->getFormat();
            extent = sourceImages[i]->getExtent3D();
        }
        else
        {
            if (imageType != sourceImages[i]->getImageType())
            {
                return IImageDataSP();
            }
            else if (format != sourceImages[i]->getFormat())
            {
                return IImageDataSP();
            }
        }
    }

    auto mergedImageData = binaryBufferCreate(totalSize);

    if (!mergedImageData.get() || mergedImageData->getSize() != totalSize)
    {
        return IImageDataSP();
    }

    //

    std::vector<uint32_t> allOffsets;

    uint32_t offset = 0;

    float maxLuminance = 0.0f;

    for (uint32_t i = 0; i < sourceImages.size(); i++)
    {
    	allOffsets.push_back(offset);

        mergedImageData->write(sourceImages[i]->getData(), 1, sourceImages[i]->getSize());

        offset += sourceImages[i]->getSize();

        maxLuminance = glm::max(maxLuminance, sourceImages[i]->getMaxLuminance());
    }

    return IImageDataSP(new ImageData(name, imageType, format, extent, mipLevels, arrayLayers, allOffsets, mergedImageData, maxLuminance));
}

}
