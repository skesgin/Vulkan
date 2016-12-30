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

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <vkts/image/vkts_image.hpp>

#include "ImageData.hpp"

namespace vkts
{

static VkFormat imageDataTranslateFormat(const int format)
{
	switch (format)
	{
		case STBI_grey:
			return VK_FORMAT_R8_UNORM;
		case STBI_grey_alpha:
			return VK_FORMAT_R8G8_UNORM;
		case STBI_rgb:
			return VK_FORMAT_R8G8B8_UNORM;
		case STBI_rgb_alpha:
			return VK_FORMAT_R8G8B8A8_UNORM;
		//
		default:
			return VK_FORMAT_UNDEFINED;
	}

	return VK_FORMAT_UNDEFINED;
}

static int imageDataTranslateFormat(const VkFormat format)
{
	switch (format)
	{
		case VK_FORMAT_R8_UNORM:
			return STBI_grey;
		case VK_FORMAT_R8G8_UNORM:
			return STBI_grey_alpha;
		case VK_FORMAT_R8G8B8_UNORM:
			return STBI_rgb;
		case VK_FORMAT_R8G8B8A8_UNORM:
			return STBI_rgb_alpha;
		//
		default:
			return 0;
	}

	return 0;
}

IImageDataSP VKTS_APIENTRY imageDataLoadStb(const std::string& name, const IBinaryBufferSP& buffer)
{
    if (!buffer.get())
    {
        return IImageDataSP();
    }

    int x;
	int y;
	int channels_in_file;

    auto data = stbi_load_from_memory((stbi_uc const*)buffer->getData(), (int)buffer->getSize(), &x, &y, &channels_in_file, 0);
    if (!data)
    {
    	return IImageDataSP();
    }

    VkFormat format = imageDataTranslateFormat(channels_in_file);
    if (format == VK_FORMAT_UNDEFINED)
    {
    	free(data);

    	return IImageDataSP();
    }

    //

    std::vector<uint32_t> allOffsets;
    allOffsets.push_back(0);

	uint32_t totalSize = (uint32_t)(x * y * channels_in_file);

    return IImageDataSP(new ImageData(name, VK_IMAGE_TYPE_2D, format, { (uint32_t)x, (uint32_t)y, 1 }, 1, 1, allOffsets, &data[0], totalSize));
}

VkBool32 VKTS_APIENTRY imageDataSaveStb(const std::string& name, const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer)
{
	if (!imageData.get())
	{
		return VK_FALSE;
	}

	int format = imageDataTranslateFormat(imageData->getFormat());
	if (format == 0)
	{
		return VK_FALSE;
	}

	VkExtent3D currentExtent;
	uint32_t currentOffset;
	if (!imageData->getExtentAndOffset(currentExtent, currentOffset, mipLevel, arrayLayer))
	{
		return VK_FALSE;
	}

	const void* data = &(imageData->getByteData()[currentOffset]);
	int stride_in_bytes = (int)(currentExtent.width * imageData->getNumberChannels());

	auto result = stbi_write_png(name.c_str(), (int)currentExtent.width, (int)currentExtent.height, (int)imageData->getNumberChannels(), data, stride_in_bytes);
	if (!result)
	{
		return VK_FALSE;
	}

	return VK_TRUE;
}

}
