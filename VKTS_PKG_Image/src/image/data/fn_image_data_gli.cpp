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

#include <gli/format.hpp>
#include <gli/load.hpp>
#include <gli/save.hpp>
#include <vkts/image/vkts_image.hpp>

#include "ImageData.hpp"

namespace vkts
{

static VkFormat imageDataTranslateFormat(const gli::format format)
{
	switch (format)
	{
		case gli::FORMAT_R8_UNORM_PACK8:
			return VK_FORMAT_R8_UNORM;
		case gli::FORMAT_RG8_UNORM_PACK8:
			return VK_FORMAT_R8G8_UNORM;
		case gli::FORMAT_RGB8_UNORM_PACK8:
			return VK_FORMAT_R8G8B8_UNORM;
		case gli::FORMAT_BGR8_UNORM_PACK8:
			return VK_FORMAT_B8G8R8_UNORM;
		case gli::FORMAT_RGBA8_UNORM_PACK8:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case gli::FORMAT_BGRA8_UNORM_PACK8:
			return VK_FORMAT_B8G8R8A8_UNORM;
		//
		case gli::FORMAT_R8_SRGB_PACK8:
			return VK_FORMAT_R8_SRGB;
		case gli::FORMAT_RG8_SRGB_PACK8:
			return VK_FORMAT_R8G8_SRGB;
		case gli::FORMAT_RGB8_SRGB_PACK8:
			return VK_FORMAT_R8G8B8_SRGB;
		case gli::FORMAT_BGR8_SRGB_PACK8:
			return VK_FORMAT_B8G8R8_SRGB;
		case gli::FORMAT_RGBA8_SRGB_PACK8:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case gli::FORMAT_BGRA8_SRGB_PACK8:
			return VK_FORMAT_B8G8R8A8_SRGB;
		//
		case gli::FORMAT_R32_SFLOAT_PACK32:
			return VK_FORMAT_R32_SFLOAT;
		case gli::FORMAT_RG32_SFLOAT_PACK32:
			return VK_FORMAT_R32G32_SFLOAT;
		case gli::FORMAT_RGB32_SFLOAT_PACK32:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case gli::FORMAT_RGBA32_SFLOAT_PACK32:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		//
		case gli::FORMAT_RGB_DXT1_UNORM_BLOCK8:
			return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		case gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8:
			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16:
			return VK_FORMAT_BC2_UNORM_BLOCK;
		case gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16:
			return VK_FORMAT_BC3_UNORM_BLOCK;
		case gli::FORMAT_R_ATI1N_UNORM_BLOCK8:
			return VK_FORMAT_BC4_UNORM_BLOCK;
		case gli::FORMAT_RG_ATI2N_UNORM_BLOCK16:
			return VK_FORMAT_BC5_UNORM_BLOCK;
		case gli::FORMAT_RGB_BP_SFLOAT_BLOCK16:
			return VK_FORMAT_BC6H_SFLOAT_BLOCK;
		case gli::FORMAT_RGBA_BP_UNORM_BLOCK16:
			return VK_FORMAT_BC7_UNORM_BLOCK;
		//
		case gli::FORMAT_RGB_ETC2_UNORM_BLOCK8:
			return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ETC2_UNORM_BLOCK8:
			return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ETC2_UNORM_BLOCK16:
			return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
		//
		case gli::FORMAT_R_EAC_UNORM_BLOCK8:
			return VK_FORMAT_EAC_R11_UNORM_BLOCK;
		case gli::FORMAT_RG_EAC_UNORM_BLOCK16:
			return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
		//
		case gli::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_5X4_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_5X5_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_6X5_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_6X6_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_8X5_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_8X6_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_8X8_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_10X5_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_10X6_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_10X8_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_10X10_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_12X10_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
		case gli::FORMAT_RGBA_ASTC_12X12_UNORM_BLOCK16:
			return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
		//
		default:
			return VK_FORMAT_UNDEFINED;
	}

	return VK_FORMAT_UNDEFINED;
}

static gli::format imageDataTranslateFormat(const VkFormat format)
{
	switch (format)
	{
		case VK_FORMAT_R8_UNORM:
			return gli::FORMAT_R8_UNORM_PACK8;
		case VK_FORMAT_R8G8_UNORM:
			return gli::FORMAT_RG8_UNORM_PACK8;
		case VK_FORMAT_R8G8B8_UNORM:
			return gli::FORMAT_RGB8_UNORM_PACK8;
		case VK_FORMAT_B8G8R8_UNORM:
			return gli::FORMAT_BGR8_UNORM_PACK8;
		case VK_FORMAT_R8G8B8A8_UNORM:
			return gli::FORMAT_RGBA8_UNORM_PACK8;
		case VK_FORMAT_B8G8R8A8_UNORM:
			return gli::FORMAT_BGRA8_UNORM_PACK8;
		//
		case VK_FORMAT_R8_SRGB:
			return gli::FORMAT_R8_SRGB_PACK8;
		case VK_FORMAT_R8G8_SRGB:
			return gli::FORMAT_RG8_SRGB_PACK8;
		case VK_FORMAT_R8G8B8_SRGB:
			return gli::FORMAT_RGB8_SRGB_PACK8;
		case VK_FORMAT_B8G8R8_SRGB:
			return gli::FORMAT_BGR8_SRGB_PACK8;
		case VK_FORMAT_R8G8B8A8_SRGB:
			return gli::FORMAT_RGBA8_SRGB_PACK8;
		case VK_FORMAT_B8G8R8A8_SRGB:
			return gli::FORMAT_BGRA8_SRGB_PACK8;
		//
		case VK_FORMAT_R32_SFLOAT:
			return gli::FORMAT_R32_SFLOAT_PACK32;
		case VK_FORMAT_R32G32_SFLOAT:
			return gli::FORMAT_RG32_SFLOAT_PACK32;
		case VK_FORMAT_R32G32B32_SFLOAT:
			return gli::FORMAT_RGB32_SFLOAT_PACK32;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			return gli::FORMAT_RGBA32_SFLOAT_PACK32;
		//
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
			return gli::FORMAT_RGB_DXT1_UNORM_BLOCK8;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
			return gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8;
		case VK_FORMAT_BC2_UNORM_BLOCK:
			return gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16;
		case VK_FORMAT_BC3_UNORM_BLOCK:
			return gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16;
		case VK_FORMAT_BC4_UNORM_BLOCK:
			return gli::FORMAT_R_ATI1N_UNORM_BLOCK8;
		case VK_FORMAT_BC5_UNORM_BLOCK:
			return gli::FORMAT_RG_ATI2N_UNORM_BLOCK16;
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
			return gli::FORMAT_RGB_BP_SFLOAT_BLOCK16;
		case VK_FORMAT_BC7_UNORM_BLOCK:
			return gli::FORMAT_RGBA_BP_UNORM_BLOCK16;
		//
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
			return gli::FORMAT_RGB_ETC2_UNORM_BLOCK8;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ETC2_UNORM_BLOCK8;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ETC2_UNORM_BLOCK16;
		//
		case VK_FORMAT_EAC_R11_UNORM_BLOCK:
			return gli::FORMAT_R_EAC_UNORM_BLOCK8;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
			return gli::FORMAT_RG_EAC_UNORM_BLOCK16;
		//
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_4X4_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_5X4_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_5X5_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_6X5_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_6X6_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_8X5_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_8X6_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_8X8_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_10X5_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_10X6_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_10X8_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_10X10_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_12X10_UNORM_BLOCK16;
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
			return gli::FORMAT_RGBA_ASTC_12X12_UNORM_BLOCK16;
		//
		default:
			return gli::FORMAT_UNDEFINED;
	}

	return gli::FORMAT_UNDEFINED;
}

IImageDataSP VKTS_APIENTRY imageDataLoadGli(const std::string& name, const IBinaryBufferSP& buffer)
{
    if (!buffer.get())
    {
        return IImageDataSP();
    }

    auto texture = gli::load((char const *)buffer->getData(), (uint32_t)buffer->getSize());
    if (texture.empty())
    {
    	return IImageDataSP();
    }
    if (texture.layers() > 1 && texture.faces() > 1)
    {
    	return IImageDataSP();
    }

    VkFormat format = imageDataTranslateFormat(texture.format());
    if (format == VK_FORMAT_UNDEFINED)
    {
    	return IImageDataSP();
    }

    uint32_t arrayLayers = (uint32_t)(texture.layers() * texture.faces());
    uint32_t mipLevels = (uint32_t)texture.levels();

    VkImageType imageType = VK_IMAGE_TYPE_2D;
    if (texture.extent().z > 1)
    {
    	imageType = VK_IMAGE_TYPE_3D;
    }

    //

    std::vector<uint32_t> allOffsets;

    uint32_t offset = 0;
	for (uint32_t arrayLayer = (uint32_t)texture.base_layer(); arrayLayer <= (uint32_t)texture.max_layer(); arrayLayer++)
	{
		for (uint32_t face = (uint32_t)texture.base_face(); face <= (uint32_t)texture.max_face(); face++)
		{
			for (uint32_t mipLevel = (uint32_t)texture.base_level(); mipLevel <= (uint32_t)texture.max_level(); mipLevel++)
			{
				allOffsets.push_back(offset);

				offset += (uint32_t)texture.size(mipLevel);
			}
		}
	}
	uint32_t totalSize = offset;

    std::vector<uint8_t> data(totalSize);

    offset = 0;
	for (uint32_t arrayLayer = (uint32_t)texture.base_layer(); arrayLayer <= (uint32_t)texture.max_layer(); arrayLayer++)
	{
		for (uint32_t face = (uint32_t)texture.base_face(); face <= (uint32_t)texture.max_face(); face++)
		{
			for (uint32_t mipLevel = (uint32_t)texture.base_level(); mipLevel <= (uint32_t)texture.max_level(); mipLevel++)
			{
				memcpy(&data[offset], texture.data(arrayLayer, face, mipLevel), texture.size(mipLevel));

				offset += (uint32_t)texture.size(mipLevel);
			}
		}
	}

    return IImageDataSP(new ImageData(name, imageType, format, { (uint32_t)texture.extent().x, (uint32_t)texture.extent().y, (uint32_t)texture.extent().z }, mipLevels, arrayLayers, allOffsets, &data[0], totalSize, 1.0f));
}

VkBool32 VKTS_APIENTRY imageDataSaveGli(const std::string& name, const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer)
{
	if (!imageData.get())
	{
		return VK_FALSE;
	}

	gli::format format = imageDataTranslateFormat(imageData->getFormat());

	if (format == gli::FORMAT_UNDEFINED)
	{
		return VK_FALSE;
	}

	VkExtent3D currentExtent;
	uint32_t currentOffset;
	if (!imageData->getExtentAndOffset(currentExtent, currentOffset, mipLevel, arrayLayer))
	{
		return VK_FALSE;
	}

	gli::texture::extent_type extent{currentExtent.width, currentExtent.height, currentExtent.depth};

	gli::texture texture(gli::TARGET_2D, format, extent, 1, 1, 1);

	VkSubresourceLayout subresourceLayout{0, (VkDeviceSize)texture.size(0), texture.size(0) / extent.y, 0, 0};

	if (!imageData->copy(texture.data(0, 0, 0), mipLevel, arrayLayer, subresourceLayout))
	{
		return VK_FALSE;
	}

	return (VkBool32)gli::save(texture, name.c_str());
}

}
