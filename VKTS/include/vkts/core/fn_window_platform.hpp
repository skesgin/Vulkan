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

#ifndef VKTS_FN_WINDOW_PLATFORM_HPP_
#define VKTS_FN_WINDOW_PLATFORM_HPP_

// Note: Not included from core by purpose.

#if defined(VKTS_DISPLAY_VISUAL)

// WSI display extension.

// No includes needed.

// No defines needed.

#define VKTS_NATIVE_DISPLAY             VkDisplayKHR
#define VKTS_NATIVE_WINDOW              VkDisplaySurfaceCreateInfoKHR*

#elif defined(VKTS_WAYLAND_VISUAL)

// Wayland windowing.

#include <wayland-client.h>
#include <wayland-cursor.h>

#define VK_USE_PLATFORM_WAYLAND_KHR

#define VKTS_NATIVE_DISPLAY             struct wl_display*
#define VKTS_NATIVE_WINDOW              struct wl_surface*

#elif defined(_WIN32)

// Windows windowing.

// No includes needed.

#define VK_USE_PLATFORM_WIN32_KHR

#define VKTS_NATIVE_DISPLAY             HINSTANCE
#define VKTS_NATIVE_WINDOW              HWND

#elif defined(__ANDROID__)

// Android windowing.

// No includes needed.

#define VK_USE_PLATFORM_ANDROID_KHR

#define VKTS_NATIVE_DISPLAY             void*
#define VKTS_NATIVE_WINDOW              ANativeWindow*

#elif defined(__gnu_linux__)

// X11 and XCB windowing.

#include <X11/X.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XCB_KHR

#define VKTS_NATIVE_DISPLAY             Display*
#define VKTS_NATIVE_WINDOW              Window

#else

// Unsupported window platform!

#error "Unsupported window platform detected!"

#endif

#endif /* VKTS_FN_WINDOW_PLATFORM_HPP_ */
