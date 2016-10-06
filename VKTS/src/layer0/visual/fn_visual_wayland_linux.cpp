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

#include <vkts/vkts.hpp>

#include "fn_visual_wayland_internal.hpp"

#include <linux/input-event-codes.h>

#define VKTS_WINDOWS_MAX_WINDOWS 64

namespace vkts
{

static struct wl_surface* g_currentKeySurface = nullptr;

static struct wl_surface* g_currentPointerSurface = nullptr;

static struct wl_surface* g_currentTouchSurface = nullptr;

//

void _visualWaylandKeymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size)
{
	// Nothing for now.
}

void _visualWaylandKeyEnter(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys)
{
	g_currentKeySurface = surface;
}

void _visualWaylandKeyLeave(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface)
{
	g_currentKeySurface = nullptr;
}

void _visualWaylandKey(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    int32_t keyCode = _visualTranslateKey(key);

    if (keyCode == VKTS_KEY_ESCAPE)
    {
    	// TODO: Implement quit.

        return;
    }

	if (!g_currentKeySurface)
	{
		return;
	}

	auto currentWindowContainer = (VKTS_NATIVE_WINDOW_CONTAINER*)wl_surface_get_user_data(g_currentKeySurface);

	if (!currentWindowContainer)
	{
		return;
	}

    if (keyCode != VKTS_KEY_UNKNOWN)
    {
    	currentWindowContainer->nativeWindow->getKeyInput().setKey(keyCode, state == WL_KEYBOARD_KEY_STATE_PRESSED);
    }
}

void _visualWaylandModifiers(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
	// Nothing for now.
}

void _visualWaylandRepeat_info(void *data, struct wl_keyboard *wl_keyboard, int32_t rate, int32_t delay)
{
	// Nothing for now.
}

//

void _visualWaylandEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	g_currentPointerSurface = surface;
}

void _visualWaylandLeave(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface)
{
	g_currentPointerSurface = nullptr;
}

void _visualWaylandMotion(void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	if (!g_currentPointerSurface)
	{
		return;
	}

	auto currentWindowContainer = (VKTS_NATIVE_WINDOW_CONTAINER*)wl_surface_get_user_data(g_currentPointerSurface);

	if (!currentWindowContainer)
	{
		return;
	}

	currentWindowContainer->nativeWindow->getMouseInput().setLocation(glm::ivec2(surface_x, surface_y));
}

void _visualWaylandButton(void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
	if (!g_currentPointerSurface)
	{
		return;
	}

	auto currentWindowContainer = (VKTS_NATIVE_WINDOW_CONTAINER*)wl_surface_get_user_data(g_currentPointerSurface);

	if (!currentWindowContainer)
	{
		return;
	}

	currentWindowContainer->nativeWindow->getMouseInput().setButton(button - BTN_LEFT, state == WL_POINTER_BUTTON_STATE_PRESSED);
}

void _visualWaylandAxis(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
	if (!g_currentPointerSurface)
	{
		return;
	}

	auto currentWindowContainer = (VKTS_NATIVE_WINDOW_CONTAINER*)wl_surface_get_user_data(g_currentPointerSurface);

	if (!currentWindowContainer)
	{
		return;
	}

	if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
	{
		currentWindowContainer->nativeWindow->getMouseInput().setMouseWheel(value);
	}
}

//

void  VKTS_APIENTRY _visualWaylandDown(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
	g_currentTouchSurface = surface;

	if (!g_currentTouchSurface)
	{
		return;
	}

	auto currentWindowContainer = (VKTS_NATIVE_WINDOW_CONTAINER*)wl_surface_get_user_data(g_currentTouchSurface);

	if (!currentWindowContainer)
	{
		return;
	}

	// TODO: Implement.
}

void  VKTS_APIENTRY _visualWaylandUp(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
	// TODO: Implement.

	g_currentTouchSurface = nullptr;
}

void  VKTS_APIENTRY _visualWaylandTouchMotion(void *data, struct wl_touch *wl_touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
	// TODO: Implement.
}

void  VKTS_APIENTRY _visualWaylandFrame(void *data, struct wl_touch *wl_touch)
{
	// Nothing for now.
}

void  VKTS_APIENTRY _visualWaylandCancel(void *data, struct wl_touch *wl_touch)
{
	// Nothing for now.
}

}
