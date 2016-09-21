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

#include "fn_visual_internal.hpp"

#define VKTS_WAYLAND_VERSION 2

namespace vkts
{

static struct wl_display* g_nativeDisplay = nullptr;

typedef struct _VKTS_NATIVE_DISPLAY_CONTAINER
{

    int32_t index;

    int32_t x;
    int32_t y;

    int32_t width;
    int32_t height;

    VkBool32 defaultDisplay;

    wl_output* output;

    NativeDisplaySP display;

} VKTS_NATIVE_DISPLAY_CONTAINER;

static NativeDisplaySP g_defaultDisplay;

static std::map<int32_t, VKTS_NATIVE_DISPLAY_CONTAINER*> g_allDisplays;

static void _visualWaylandGeometry(void* data, struct wl_output* output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char* make, const char* model, int32_t transform)
{
	VKTS_NATIVE_DISPLAY_CONTAINER* currentDisplayContainer = (VKTS_NATIVE_DISPLAY_CONTAINER*)data;

	if (!currentDisplayContainer)
	{
		return;
	}

	currentDisplayContainer->x = x;
	currentDisplayContainer->y = y;

	if (x == 0 && y == 0)
	{
		currentDisplayContainer->defaultDisplay = VK_TRUE;
	}
}

static void _visualWaylandMode(void* data, struct wl_output* output, uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
	VKTS_NATIVE_DISPLAY_CONTAINER* currentDisplayContainer = (VKTS_NATIVE_DISPLAY_CONTAINER*)data;

	if (!currentDisplayContainer)
	{
		return;
	}

	currentDisplayContainer->width = width;
	currentDisplayContainer->height = height;
}

static void _visualWaylandDone(void* data, struct wl_output* output)
{
	// Do nothing for now.
}

static void _visualWaylandScale(void* data, struct wl_output* output, int32_t factor)
{
	// Do nothing for now.
}

static const struct wl_output_listener g_output_listener = {
	_visualWaylandGeometry,
	_visualWaylandMode,
	_visualWaylandDone,
	_visualWaylandScale
};

static void _visualWaylandGlobal(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
    if (strcmp(interface, "wl_compositor") == 0)
    {
    	// TODO: Implement.
    }
    else if (strcmp(interface, "wl_output") == 0)
    {
    	VKTS_NATIVE_DISPLAY_CONTAINER* currentDisplayContainer = new VKTS_NATIVE_DISPLAY_CONTAINER();

    	if (!currentDisplayContainer)
    	{
    		return;
    	}

    	currentDisplayContainer->index = g_allDisplays.size();

    	currentDisplayContainer->x = -1;
    	currentDisplayContainer->y = -1;

    	currentDisplayContainer->width = -1;
    	currentDisplayContainer->height = -1;

    	currentDisplayContainer->defaultDisplay = VK_FALSE;

    	currentDisplayContainer->output = (wl_output*)wl_registry_bind(registry, name, &wl_output_interface, VKTS_WAYLAND_VERSION);

    	currentDisplayContainer->display = NativeDisplaySP();


    	g_allDisplays[currentDisplayContainer->index] = currentDisplayContainer;

    	//

    	wl_output_add_listener(currentDisplayContainer->output, &g_output_listener, currentDisplayContainer);
    }

    // TODO: Implement.
}

static void _visualWaylandGlobal_remove(void *data, struct wl_registry *registry, uint32_t name)
{
	// Do nothing for now.
}

static const struct wl_registry_listener registry_listener = {
	_visualWaylandGlobal,
	_visualWaylandGlobal_remove
};

//

VkBool32 VKTS_APIENTRY _visualInit(const VkInstance instance, const VkPhysicalDevice physicalDevice)
{
	g_nativeDisplay = wl_display_connect(nullptr);

    if (!g_nativeDisplay)
    {
    	return VK_FALSE;
    }

    auto registry = wl_display_get_registry(g_nativeDisplay);
    wl_registry_add_listener(registry, &registry_listener, nullptr);

    wl_display_dispatch(g_nativeDisplay);
    wl_display_roundtrip(g_nativeDisplay);

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _visualDispatchMessages()
{
    // TODO: Implement.

    return VK_TRUE;
}

//

int32_t VKTS_APIENTRY _visualGetNumberDisplays()
{
    return g_allDisplays.size();
}

INativeDisplayWP VKTS_APIENTRY _visualCreateDefaultDisplay()
{
    if (g_defaultDisplay.get())
    {
        return g_defaultDisplay;
    }

    //

    for (auto walkerDisplayContainer : g_allDisplays)
    {
        if (walkerDisplayContainer.second && walkerDisplayContainer.second->defaultDisplay)
        {
        	walkerDisplayContainer.second->display = NativeDisplaySP(new NativeDisplay(walkerDisplayContainer.second->index, walkerDisplayContainer.second->defaultDisplay, walkerDisplayContainer.second->width, walkerDisplayContainer.second->height, g_nativeDisplay));

        	if (!walkerDisplayContainer.second->display.get())
        	{
        		return INativeDisplayWP();
        	}

        	g_defaultDisplay = walkerDisplayContainer.second->display;

        	return g_defaultDisplay;
        }
    }

    return INativeDisplayWP();
}

INativeDisplayWP VKTS_APIENTRY _visualCreateDisplay(const int32_t displayIndex)
{
    if (displayIndex < 0 || displayIndex >= _visualGetNumberDisplays())
    {
        return INativeDisplayWP();
    }

    //

    auto currentDisplayContainer = g_allDisplays[displayIndex];

    if (!currentDisplayContainer)
    {
    	return INativeDisplaySP();
    }

    currentDisplayContainer->display = NativeDisplaySP(new NativeDisplay(currentDisplayContainer->index, currentDisplayContainer->defaultDisplay, currentDisplayContainer->width, currentDisplayContainer->height, g_nativeDisplay));

    if (currentDisplayContainer->defaultDisplay)
    {
    	g_defaultDisplay = currentDisplayContainer->display;
    }

    return currentDisplayContainer->display;
}

NativeDisplaySP VKTS_APIENTRY _visualGetDisplay(const int32_t displayIndex)
{
    if (displayIndex < 0 || displayIndex >= _visualGetNumberDisplays())
    {
        return NativeDisplaySP();
    }

    //

    auto currentDisplayContainer = g_allDisplays[displayIndex];

    if (!currentDisplayContainer)
    {
    	return NativeDisplaySP();
    }


    return currentDisplayContainer->display;
}

const SmartPointerVector<NativeDisplaySP>& VKTS_APIENTRY _visualGetActiveDisplays()
{
    static SmartPointerVector<NativeDisplaySP> displayList;

    displayList.clear();

    for (auto& walkerDisplayContainer : g_allDisplays)
    {
        if (walkerDisplayContainer.second && walkerDisplayContainer.second->display.get())
        {
        	displayList.append(walkerDisplayContainer.second->display);
        }
    }

    return displayList;
}

void VKTS_APIENTRY _visualDestroyDisplay(const NativeDisplaySP& display)
{
    if (display.get() == g_defaultDisplay.get())
    {
        g_defaultDisplay = NativeDisplaySP();
    }

    for (auto& walkerDisplayContainer : g_allDisplays)
    {
        if (walkerDisplayContainer.second && walkerDisplayContainer.second->display.get())
        {
        	walkerDisplayContainer.second->display = NativeDisplaySP();
        }
    }
}

//

INativeWindowWP VKTS_APIENTRY _visualCreateWindow(const INativeDisplayWP& display, const char* title, const int32_t width, const int32_t height, const VkBool32 fullscreen, const VkBool32 resize, const VkBool32 invisibleCursor)
{
    // TODO: Implement.

    return INativeWindowWP();
}

NativeWindowSP VKTS_APIENTRY _visualGetWindow(const int32_t windowIndex)
{
    // TODO: Implement.

    return NativeWindowSP();
}

const SmartPointerVector<NativeWindowSP>& VKTS_APIENTRY _visualGetActiveWindows()
{
    static SmartPointerVector<NativeWindowSP> windowList;

    // TODO: Implement.

    return windowList;
}

void VKTS_APIENTRY _visualDestroyWindow(const NativeWindowSP& window)
{
    // TODO: Implement.
}

//

void VKTS_APIENTRY _visualTerminate()
{
    for (auto& walkerDisplayContainer : g_allDisplays)
    {
        if (walkerDisplayContainer.second)
        {
            _visualDestroyDisplay(walkerDisplayContainer.second->display);

            delete walkerDisplayContainer.second;
        }
    }
    g_allDisplays.clear();

    g_defaultDisplay.reset();

    //

    if (g_nativeDisplay)
    {
    	wl_display_disconnect(g_nativeDisplay);

        g_nativeDisplay = nullptr;
    }
}

}
