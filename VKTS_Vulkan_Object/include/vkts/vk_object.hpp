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

#ifndef VKTS_VK_OBJECT_HPP_
#define VKTS_VK_OBJECT_HPP_

/**
 *
 * Depends on VKTS image and Vulkan.
 *
 */

#include <vkts/vk.hpp>

#include <vkts/image.hpp>

/**
 *
 * VKTS Start.
 *
 */

/**
 * Context object.
 */

#include <vkts/vk_object/context_object/IContextObject.hpp>

#include <vkts/vk_object/context_object/fn_context_object.hpp>

/**
 * Buffer object.
 */

#include <vkts/vk_object/buffer_object/IBufferObject.hpp>

#include <vkts/vk_object/buffer_object/fn_buffer_object.hpp>

/**
 * Image object.
 */

#include <vkts/vk_object/image_object/IImageObject.hpp>

#include <vkts/vk_object/image_object/fn_image_object.hpp>

/**
 * Texture object.
 */

#include <vkts/vk_object/texture_object/ITextureObject.hpp>

#include <vkts/vk_object/texture_object/fn_texture_object.hpp>

#endif /* VKTS_VK_OBJECT_HPP_ */
