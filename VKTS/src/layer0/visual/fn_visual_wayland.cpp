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

#include "fn_visual_wayland_internal.hpp"

#define VKTS_WINDOWS_MAX_WINDOWS 64

namespace vkts
{


static VkBool32 g_running = VK_TRUE;

void VKTS_APIENTRY _visualWaylandSetRunning(const VkBool32 running)
{
	g_running = running;
}

static struct wl_touch* g_nativeTouch = nullptr;

static struct wl_keyboard* g_nativeKeyboard = nullptr;

static struct wl_pointer* g_nativePointer = nullptr;

static struct wl_seat* g_nativeSeat = nullptr;

static struct wl_shell* g_nativeShell = nullptr;

static struct wl_compositor* g_nativeCompositor = nullptr;

static struct wl_registry* g_nativeRegistry = nullptr;

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

static std::map<struct wl_surface*, VKTS_NATIVE_WINDOW_CONTAINER*> g_allWindows;

//

static int32_t g_numberWindows = 0;

static uint64_t g_windowBits = 0;

//

static const struct wl_keyboard_listener keyboard_listener =
{
	_visualWaylandKeymap,
	_visualWaylandKeyEnter,
	_visualWaylandKeyLeave,
	_visualWaylandKey,
	_visualWaylandModifiers,
	_visualWaylandRepeat_info
};

static const struct wl_pointer_listener pointer_listener =
{
	_visualWaylandEnter,
	_visualWaylandLeave,
	_visualWaylandMotion,
	_visualWaylandButton,
	_visualWaylandAxis
};

static const struct wl_touch_listener touch_listener =
{
		_visualWaylandDown,
		_visualWaylandUp,
		_visualWaylandTouchMotion,
		_visualWaylandFrame,
		_visualWaylandCancel
};

void _visualWaylandCapabilities(void *data, struct wl_seat *wl_seat, uint32_t capabilities)
{
    if (capabilities & WL_SEAT_CAPABILITY_POINTER)
    {
    	g_nativePointer = wl_seat_get_pointer(g_nativeSeat);

    	//

    	wl_pointer_add_listener(g_nativePointer, &pointer_listener, nullptr);
    }

    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD)
    {
    	g_nativeKeyboard = wl_seat_get_keyboard(g_nativeSeat);

    	//

    	wl_keyboard_add_listener(g_nativeKeyboard, &keyboard_listener, nullptr);
	}

    if (capabilities & WL_SEAT_CAPABILITY_TOUCH)
    {
    	g_nativeTouch = wl_seat_get_touch(g_nativeSeat);

    	//

    	wl_touch_add_listener(g_nativeTouch, &touch_listener, nullptr);
    }
}

void _visualWaylandName(void *data, struct wl_seat *wl_seat, const char *name)
{
}

static const struct wl_seat_listener seat_listener =
{
		_visualWaylandCapabilities, _visualWaylandName
};


static void _visualWaylandPing(void* data, struct wl_shell_surface* shell_surface, uint32_t serial)
{
    wl_shell_surface_pong(shell_surface, serial);
}

void _visualWaylandConfigure(void* data, struct wl_shell_surface* shell_surface, uint32_t edges, int32_t width, int32_t height)
{
	if (!data)
	{
		return;
	}

	VKTS_NATIVE_WINDOW_CONTAINER* currentWindowContainer = (VKTS_NATIVE_WINDOW_CONTAINER*)data;

	currentWindowContainer->nativeWindow->setDimension(glm::uvec2((uint32_t)width, (uint32_t)height));

	//

	if (currentWindowContainer->region)
	{
		wl_region_destroy(currentWindowContainer->region);
	}

	currentWindowContainer->region = wl_compositor_create_region(g_nativeCompositor);

	if (!currentWindowContainer->region)
	{
		wl_region_add(currentWindowContainer->region, 0, 0, width, height);
		wl_surface_set_opaque_region(currentWindowContainer->surface, currentWindowContainer->region);
	}
}

void _visualWaylandPopupDone(void* data, struct wl_shell_surface* shell_surface)
{
}

static const struct wl_shell_surface_listener shell_surface_listener =
{
	_visualWaylandPing, _visualWaylandConfigure, _visualWaylandPopupDone
};

//

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
    	g_nativeCompositor = (struct wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, 3);
    }
	else if (strcmp(interface, "wl_shell") == 0)
	{
        g_nativeShell = (struct wl_shell*)wl_registry_bind(registry, name, &wl_shell_interface, 1);
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

    	currentDisplayContainer->output = (wl_output*)wl_registry_bind(registry, name, &wl_output_interface, 1);

    	currentDisplayContainer->display = NativeDisplaySP();


    	g_allDisplays[currentDisplayContainer->index] = currentDisplayContainer;

    	//

    	wl_output_add_listener(currentDisplayContainer->output, &g_output_listener, currentDisplayContainer);
    }
    else if (strcmp(interface, "wl_seat") == 0)
    {
    	g_nativeSeat = (struct wl_seat *)wl_registry_bind(registry, name, &wl_seat_interface, 1);

    	wl_seat_add_listener(g_nativeSeat, &seat_listener, nullptr);
    }
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

    g_nativeRegistry = wl_display_get_registry(g_nativeDisplay);
    wl_registry_add_listener(g_nativeRegistry, &registry_listener, nullptr);

    wl_display_dispatch(g_nativeDisplay);
    wl_display_roundtrip(g_nativeDisplay);

	if (!_visualInitGamepad(instance, physicalDevice))
	{
		return VK_FALSE;
	}

	if (!_visualInitTouchpad(instance, physicalDevice))
	{
		return VK_FALSE;
	}

	if (!_visualInitKey())
	{
		return VK_FALSE;
	}

	g_running = VK_TRUE;

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _visualDispatchMessages()
{
	if (!g_running)
	{
		return VK_FALSE;
	}

	if (g_nativeDisplay)
	{
		wl_display_dispatch_pending(g_nativeDisplay);
	}

    _visualDispatchMessagesGamepad();

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
    const auto sharedDisplay = display.lock();

    if (!sharedDisplay.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No display.");

        return INativeWindowWP();
    }

    if (sharedDisplay->getIndex() < 0 || sharedDisplay->getIndex() >= _visualGetNumberDisplays())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid display index.");

        return INativeWindowWP();
    }

    auto walkerDisplayContainer = g_allDisplays.find(sharedDisplay->getIndex());

    if (walkerDisplayContainer == g_allDisplays.end() || !walkerDisplayContainer->second || !walkerDisplayContainer->second->display)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Display not known.");

        return INativeWindowWP();
    }

    //

    VkBool32 finalResize = resize && !fullscreen;

    //

    uint32_t currentWindowBit = 1;
    int32_t currentWindowIndex = 0;

    if (g_numberWindows == VKTS_WINDOWS_MAX_WINDOWS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Maximum windows reached.");

        return INativeWindowWP();
    }

    while (currentWindowBit & g_windowBits)
    {
        currentWindowBit = currentWindowBit << 1;

        currentWindowIndex++;
    }

    //

    // TODO: Implement move to correct display.

    //

    // TODO: Implement no resize window.
    // TODO: Implement hidden and game mode cursor.

	auto surface = wl_compositor_create_surface(g_nativeCompositor);

	if (!surface)
	{
		return INativeWindowWP();
	}

	auto shell_surface = wl_shell_get_shell_surface(g_nativeShell, surface);

	if (!shell_surface)
	{
		wl_surface_destroy(surface);

		return INativeWindowWP();
	}

	auto region = wl_compositor_create_region(g_nativeCompositor);

	if (!region)
	{
		wl_shell_surface_destroy(shell_surface);

		wl_surface_destroy(surface);

		return INativeWindowWP();
	}

	auto windowContainer = new VKTS_NATIVE_WINDOW_CONTAINER();

	if (!windowContainer)
	{
		wl_region_destroy(region);

		wl_shell_surface_destroy(shell_surface);

		wl_surface_destroy(surface);

		return INativeWindowWP();
	}

	//

	auto nativeWindow = NativeWindowSP(new NativeWindow(display, surface, currentWindowIndex, title, width, height, fullscreen, finalResize, invisibleCursor));

	if (!nativeWindow.get())
	{
		wl_region_destroy(region);

		wl_shell_surface_destroy(shell_surface);

		wl_surface_destroy(surface);

		return INativeWindowWP();
	}

	//

	windowContainer->index = currentWindowIndex;
	windowContainer->surface = surface;
	windowContainer->shell_surface = shell_surface;
	windowContainer->region = region;
	windowContainer->nativeWindow = nativeWindow;

	g_allWindows[surface] = windowContainer;

	//

	wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, windowContainer);

    if (fullscreen)
    {
    	wl_shell_surface_set_fullscreen(shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT, 0, walkerDisplayContainer->second->output);
    }
    else
    {
    	wl_shell_surface_set_toplevel(shell_surface);
    	wl_shell_surface_set_title(shell_surface, title);
    }

	wl_region_add(region, 0, 0, width, height);
	wl_surface_set_opaque_region(surface, region);

    return nativeWindow;
}

NativeWindowSP VKTS_APIENTRY _visualGetWindow(const int32_t windowIndex)
{
	for (auto& currentWindowContainer : g_allWindows)
	{
		if (currentWindowContainer.second->index == windowIndex)
		{
			return currentWindowContainer.second->nativeWindow;
		}
	}

    return NativeWindowSP();
}

const SmartPointerVector<NativeWindowSP>& VKTS_APIENTRY _visualGetActiveWindows()
{
    static SmartPointerVector<NativeWindowSP> windowList;

    windowList.clear();

	for (auto& currentWindowContainer : g_allWindows)
	{
		windowList.append(currentWindowContainer.second->nativeWindow);
	}

    return windowList;
}

void VKTS_APIENTRY _visualDestroyWindow(const NativeWindowSP& window)
{
    if (!window.get())
    {
        return;
    }

	auto foundWindowContainerWalker = g_allWindows.find(window->getNativeWindow());

	if (foundWindowContainerWalker == g_allWindows.end())
	{
		return;
	}

    if (window->getNativeWindow())
    {

        if (window->isFullscreen())
        {
        	wl_shell_surface_set_toplevel(foundWindowContainerWalker->second->shell_surface);
        }

        if (window->isInvisibleCursor())
        {
        	// TODO: Revert hidden cursor.
        }

        wl_region_destroy(foundWindowContainerWalker->second->region);
        wl_shell_surface_destroy(foundWindowContainerWalker->second->shell_surface);
        wl_surface_destroy(foundWindowContainerWalker->second->surface);

        delete foundWindowContainerWalker->second;
    }

    uint32_t currentNegativeWindowBit = ~(1 << window->getIndex());

    g_windowBits &= currentNegativeWindowBit;

    //

    if (window->getNativeWindow())
    {
        g_allWindows.erase(foundWindowContainerWalker);
    }
}

//

void VKTS_APIENTRY _visualTerminate()
{
    _visualTerminateGamepad();

    //

    while (g_allWindows.size() > 0)
    {
        _visualDestroyWindow(g_allWindows.begin()->second->nativeWindow);
    }
    g_allWindows.clear();

    //

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

    if (g_nativeTouch)
    {
    	wl_touch_destroy(g_nativeTouch);

    	g_nativeTouch = nullptr;
    }

    if (g_nativeKeyboard)
    {
    	wl_keyboard_destroy(g_nativeKeyboard);

    	g_nativeKeyboard = nullptr;
    }

    if (g_nativePointer)
    {
    	wl_pointer_destroy(g_nativePointer);

    	g_nativePointer = nullptr;
    }

    if (g_nativeSeat)
    {
    	wl_seat_destroy(g_nativeSeat);

    	g_nativeSeat = nullptr;
    }

    if (g_nativeShell)
    {
    	wl_shell_destroy(g_nativeShell);

    	g_nativeShell = nullptr;
    }

    if (g_nativeCompositor)
    {
    	wl_compositor_destroy(g_nativeCompositor);

    	g_nativeCompositor = nullptr;
    }

    if (g_nativeRegistry)
    {
    	wl_registry_destroy(g_nativeRegistry);

    	g_nativeRegistry = nullptr;
    }

    if (g_nativeDisplay)
    {
    	wl_display_disconnect(g_nativeDisplay);

        g_nativeDisplay = nullptr;
    }
}

//

void VKTS_APIENTRY _visualGamepadSetButton(const int32_t gamepadIndex, const int32_t buttonIndex, const VkBool32 pressed)
{
	for (const auto currentPair : g_allWindows)
	{
		currentPair.second->nativeWindow->getGamepadInput(gamepadIndex).setButton(buttonIndex, pressed);
	}
}

void VKTS_APIENTRY _visualGamepadSetAxis(const int32_t gamepadIndex, const int32_t axisIndex, const float value)
{
    for (const auto currentPair : g_allWindows)
    {
    	currentPair.second->nativeWindow->getGamepadInput(gamepadIndex).setAxis(axisIndex, value);
    }
}

}
