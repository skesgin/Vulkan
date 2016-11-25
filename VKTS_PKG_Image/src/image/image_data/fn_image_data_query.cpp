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

#include <vkts/image.hpp>

namespace vkts
{

VkBool32 VKTS_APIENTRY imageDataIsBLOCK(const VkFormat format)
{
	switch (format)
	{
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		case VK_FORMAT_BC2_UNORM_BLOCK:
		case VK_FORMAT_BC3_UNORM_BLOCK:
		case VK_FORMAT_BC4_UNORM_BLOCK:
		case VK_FORMAT_BC5_UNORM_BLOCK:
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
		case VK_FORMAT_BC7_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
		case VK_FORMAT_EAC_R11_UNORM_BLOCK:
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
			return VK_TRUE;
		default:
			return VK_FALSE;
	}

    return VK_FALSE;
}

VkBool32 VKTS_APIENTRY imageDataIsUNORM(const VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8_UNORM:
        case VK_FORMAT_R8G8_UNORM:
        case VK_FORMAT_R8G8B8_UNORM:
        case VK_FORMAT_B8G8R8_UNORM:
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_B8G8R8A8_UNORM:
            return VK_TRUE;
        default:
            return VK_FALSE;
    }

    return VK_FALSE;
}

VkBool32 VKTS_APIENTRY imageDataIsSFLOAT(const VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R32_SFLOAT:
        case VK_FORMAT_R32G32_SFLOAT:
        case VK_FORMAT_R32G32B32_SFLOAT:
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return VK_TRUE;
        default:
            return VK_FALSE;
    }

    return VK_FALSE;
}

int32_t VKTS_APIENTRY imageDataGetBytesPerTexel(const VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8_UNORM:
        case VK_FORMAT_R8G8_UNORM:
        case VK_FORMAT_R8G8B8_UNORM:
        case VK_FORMAT_B8G8R8_UNORM:
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_B8G8R8A8_UNORM:
        case VK_FORMAT_R32_SFLOAT:
        case VK_FORMAT_R32G32_SFLOAT:
        case VK_FORMAT_R32G32B32_SFLOAT:
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return imageDataGetBytesPerChannel(format) * imageDataGetNumberChannels(format);
        //
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
			return 8;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
			return 8;
		case VK_FORMAT_BC2_UNORM_BLOCK:
			return 16;
		case VK_FORMAT_BC3_UNORM_BLOCK:
			return 16;
		case VK_FORMAT_BC4_UNORM_BLOCK:
			return 8;
		case VK_FORMAT_BC5_UNORM_BLOCK:
			return 16;
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
			return 16;
		case VK_FORMAT_BC7_UNORM_BLOCK:
			return 16;
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
			return 8;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
			return 8;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
			return 16;
		case VK_FORMAT_EAC_R11_UNORM_BLOCK:
			return 8;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
			return 16;
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
			return 16;
        default:
            return 0;
    }

    return 0;
}

int32_t VKTS_APIENTRY imageDataGetBytesPerChannel(const VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8_UNORM:
        case VK_FORMAT_R8G8_UNORM:
        case VK_FORMAT_R8G8B8_UNORM:
        case VK_FORMAT_B8G8R8_UNORM:
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_B8G8R8A8_UNORM:
            return 1;

        case VK_FORMAT_R32_SFLOAT:
        case VK_FORMAT_R32G32_SFLOAT:
        case VK_FORMAT_R32G32B32_SFLOAT:
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return 4;
        default:
            return 0;
    }

    return 0;
}

int32_t VKTS_APIENTRY imageDataGetNumberChannels(const VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8_UNORM:
            return 1;
        case VK_FORMAT_R8G8_UNORM:
            return 2;
        case VK_FORMAT_R8G8B8_UNORM:
            return 3;
        case VK_FORMAT_B8G8R8_UNORM:
            return 3;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return 4;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return 4;

        case VK_FORMAT_R32_SFLOAT:
            return 1;
        case VK_FORMAT_R32G32_SFLOAT:
            return 2;
        case VK_FORMAT_R32G32B32_SFLOAT:
            return 3;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return 4;
        default:
            return 0;
    }

    return 0;
}

std::string VKTS_APIENTRY imageDataGetColorName(const VkFormat format, const glm::vec4& color)
{
	char colorName[VKTS_MAX_BUFFER_CHARS + 1];

    switch (format)
    {
        case VK_FORMAT_R8_UNORM:
            sprintf(colorName, "R%1.3f_UNORM", color.r);
            break;
        case VK_FORMAT_R8G8_UNORM:
            sprintf(colorName, "R%1.3f_G%1.3f_UNORM", color.r, color.g);
            break;
        case VK_FORMAT_R8G8B8_UNORM:
            sprintf(colorName, "R%1.3f_G%1.3f_B%1.3f_UNORM", color.r, color.g, color.b);
            break;
        case VK_FORMAT_B8G8R8_UNORM:
            sprintf(colorName, "B%1.3f_G%1.3f_R%1.3f_UNORM", color.b, color.g, color.r);
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            sprintf(colorName, "R%1.3f_G%1.3f_B%1.3f_A%1.3f_UNORM", color.r, color.g, color.b, color.a);
            break;
        case VK_FORMAT_B8G8R8A8_UNORM:
            sprintf(colorName, "B%1.3f_G%1.3f_R%1.3f_A%1.3f_UNORM", color.b, color.g, color.r, color.a);
            break;

        case VK_FORMAT_R32_SFLOAT:
            sprintf(colorName, "R%1.3f_SFLOAT", color.r);
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            sprintf(colorName, "R%1.3f_G%1.3f_SFLOAT", color.r, color.g);
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            sprintf(colorName, "R%1.3f_G%1.3f_B%1.3f_SFLOAT", color.r, color.g, color.b);
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            sprintf(colorName, "R%1.3f_G%1.3f_B%1.3f_A%1.3f_SFLOAT", color.r, color.g, color.b, color.a);
            break;
        default:
            return "";
    }

    return std::string(colorName);
}

}
