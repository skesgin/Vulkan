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

#ifndef VKTS_FN_VISUAL_WAYLAND_INTERNAL_HPP_
#define VKTS_FN_VISUAL_WAYLAND_INTERNAL_HPP_

#include <vkts/vkts.hpp>

#include "fn_visual_internal.hpp"

#include "fn_visual_gamepad_internal.hpp"

namespace vkts
{

typedef struct _VKTS_NATIVE_WINDOW_CONTAINER
{

    int32_t index;

    struct wl_surface* surface;
    struct wl_shell_surface* shell_surface;
    struct wl_region* region;

    NativeWindowSP nativeWindow;

} VKTS_NATIVE_WINDOW_CONTAINER;

VKTS_APICALL VkBool32 VKTS_APIENTRY _visualInitKey();

VKTS_APICALL int32_t VKTS_APIENTRY _visualTranslateKey(int scancode);


VKTS_APICALL VkBool32 VKTS_APIENTRY _visualInitTouchpad(const VkInstance instance, const VkPhysicalDevice physicalDevice);


VKTS_APICALL void VKTS_APIENTRY _visualWaylandKeymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandKeyEnter(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandKeyLeave(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, struct wl_surface *surface);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandKey(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandModifiers(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandRepeat_info(void *data, struct wl_keyboard *wl_keyboard, int32_t rate, int32_t delay);


VKTS_APICALL void VKTS_APIENTRY _visualWaylandEnter(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandLeave(void *data, struct wl_pointer *wl_pointer, uint32_t serial, struct wl_surface *surface);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandMotion(void *data, struct wl_pointer *wl_pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandButton(void *data, struct wl_pointer *wl_pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandAxis(void *data, struct wl_pointer *wl_pointer, uint32_t time, uint32_t axis, wl_fixed_t value);


VKTS_APICALL void VKTS_APIENTRY _visualWaylandDown(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandUp(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, int32_t id);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandTouchMotion(void *data, struct wl_touch *wl_touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandFrame(void *data, struct wl_touch *wl_touch);

VKTS_APICALL void VKTS_APIENTRY _visualWaylandCancel(void *data, struct wl_touch *wl_touch);

//

VKTS_APICALL void VKTS_APIENTRY _visualWaylandSetRunning(const VkBool32 running);

}

#endif /* VKTS_FN_VISUAL_WAYLAND_INTERNAL_HPP_ */
