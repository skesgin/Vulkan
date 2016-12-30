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

#include <vkts/vulkan/wrapper/vkts_wrapper.hpp>

namespace vkts
{

VkDeviceSize VKTS_APIENTRY alignmentGetSizeInBytes(const VkDeviceSize currentSize, const VkDeviceSize alignment)
{
    if (currentSize == 0 || alignment == 0)
    {
        return 0;
    }

    return (currentSize / alignment) * alignment + ((currentSize % alignment) > 0 ? alignment : 0);
}

uint32_t VKTS_APIENTRY alignmentGetOffsetInBytes(const VkTsVertexBufferType element, const VkTsVertexBufferType allElements)
{
    uint32_t result = 0;

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_VERTEX)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_VERTEX == element)
        {
            return result;
        }

        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_NORMAL)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_NORMAL == element)
        {
            return result;
        }

        result += 3 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BITANGENT)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_BITANGENT == element)
        {
            return result;
        }

        result += 3 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_TANGENT)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_TANGENT == element)
        {
            return result;
        }

        result += 3 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_TEXCOORD == element)
        {
            return result;
        }

        result += 2 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0 == element)
        {
            return result;
        }

        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1 == element)
        {
            return result;
        }

        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0 == element)
        {
            return result;
        }

        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1 == element)
        {
            return result;
        }

        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS)
    {
        if (VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS == element)
        {
            return result;
        }

        result += 1 * (uint32_t)sizeof(float);
    }

    return -1;
}

uint32_t VKTS_APIENTRY alignmentGetStrideInBytes(const VkTsVertexBufferType allElements)
{
    uint32_t result = 0;

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_VERTEX)
    {
        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_NORMAL)
    {
        result += 3 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BITANGENT)
    {
        result += 3 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_TANGENT)
    {
        result += 3 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD)
    {
        result += 2 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0)
    {
        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1)
    {
        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0)
    {
        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1)
    {
        result += 4 * (uint32_t)sizeof(float);
    }

    if (allElements & VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS)
    {
        result += 1 * (uint32_t)sizeof(float);
    }

    return result;
}

}
