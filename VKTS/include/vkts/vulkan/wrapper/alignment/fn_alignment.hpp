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

#ifndef VKTS_FN_ALIGNMENT_HPP_
#define VKTS_FN_ALIGNMENT_HPP_

#include <vkts/vulkan/wrapper/vkts_wrapper.hpp>

namespace vkts
{

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL VkDeviceSize VKTS_APIENTRY alignmentGetSizeInBytes(const VkDeviceSize currentSize, const VkDeviceSize alignment);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL uint32_t VKTS_APIENTRY alignmentGetOffsetInBytes(const VkTsVertexBufferType element, const VkTsVertexBufferType allElements);

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL uint32_t VKTS_APIENTRY alignmentGetStrideInBytes(const VkTsVertexBufferType allElements);

}

#endif /* VKTS_FN_ALIGNMENT_HPP_ */
