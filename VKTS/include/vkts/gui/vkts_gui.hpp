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

#ifndef VKTS_GUI_HPP_
#define VKTS_GUI_HPP_

// TODO: Move vk_gui code into own library and update project imports.

// TODO: Implement distance field font.

/**
 *
 * Depends on VKTS Vulkan object.
 *
 */

#include <vkts/vulkan/composition/vkts_composition.hpp>

/**
 *
 * VKTS Start.
 *
 */

/**
 * Font.
 */

#include <vkts/gui/font/IChar.hpp>

#include <vkts/gui/font/IRenderFont.hpp>
#include <vkts/gui/font/IFont.hpp>

/**
 * Gui manager.
 */

#include <vkts/gui/manager/IGuiManager.hpp>

#include <vkts/gui/manager/fn_gui_manager.hpp>


/**
 * Gui factory.
 */

#include <vkts/gui/factory/IGuiRenderFactory.hpp>
#include <vkts/gui/factory/IGuiFactory.hpp>

#include <vkts/gui/factory/fn_gui_factory.hpp>

/**
 * Font load.
 */

#include <vkts/gui/load/fn_load_font.hpp>

#endif /* VKTS_GUI_HPP_ */
