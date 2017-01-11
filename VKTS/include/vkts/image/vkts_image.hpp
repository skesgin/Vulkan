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

#ifndef VKTS_VKTS_IMAGE_HPP_
#define VKTS_VKTS_IMAGE_HPP_

/**
 *
 * Depends on VKTS math.
 *
 */

#include <vkts/math/vkts_math.hpp>

/**
 *
 * VKTS Start.
 *
 */

#define VKTS_GAMMA 2.2f

enum VkTsImageDataType {VKTS_NON_COLOR_DATA, VKTS_LDR_COLOR_DATA, VKTS_HDR_COLOR_DATA, VKTS_NORMAL_DATA};

/**
 * Image data.
 */

#include <vkts/image/data/IImageData.hpp>

#include <vkts/image/data/fn_image_data.hpp>

/**
 * Render.
 */

#include <vkts/image/render/fn_render.hpp>

/**
 * Cache.
 */

#include <vkts/image/cache/fn_cache.hpp>

#endif /* VKTS_VKTS_IMAGE_HPP_ */
