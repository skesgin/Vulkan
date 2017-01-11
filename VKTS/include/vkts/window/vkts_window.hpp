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

#ifndef VKTS_WINDOW_HPP_
#define VKTS_WINDOW_HPP_

/**
 * Window platform.
 */

#include <vkts/core/fn_window_platform.hpp>

/**
 *
 * Depends on VKTS core.
 *
 */

#include <vkts/core/vkts_core.hpp>

/**
 *
 * VKTS Start.
 *
 */

/**
 * Defines.
 */

#define VKTS_DEFAULT_WINDOW_INDEX   0

/**
 * Types.
 */

typedef struct _VkTsWindowCapabilites {
	// Available and used parameters, when creating a window.
	VkBool32 titleSetable;
	VkBool32 widthSetable;
	VkBool32 heightSetable;
	VkBool32 fullscreenSetable;
	VkBool32 resizeSetable;
	VkBool32 gameCursorSetable;

	// If parameter not used or available these are the used default values.
	VkBool32 isTitleVisible;
	//
	//
	VkBool32 isFullscreen;
	VkBool32 isResizable;
	VkBool32 isGameCursor;
} VkTsWindowCapabilites;

/**
 * Input.
 */

#include <vkts/window/input/fn_input_gamepad.hpp>
#include <vkts/window/input/fn_input_key.hpp>
#include <vkts/window/input/fn_input_mouse.hpp>
#include <vkts/window/input/fn_input_touchpad.hpp>

/**
 * Visual.
 */

#include <vkts/window/visual/INativeDisplay.hpp>
#include <vkts/window/visual/INativeWindow.hpp>

#include <vkts/window/visual/IVisualContext.hpp>

#include <vkts/window/visual/fn_visual.hpp>

#endif /* VKTS_WINDOW_HPP_ */
