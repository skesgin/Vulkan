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

#include "VisualContext.hpp"

namespace vkts
{

NativeDisplaySP VisualContext::getDisplay(int32_t displayIndex) const
{
    uint32_t index = allAttachedDisplays.find(displayIndex);

    if (index == allAttachedDisplays.size())
    {
        return NativeDisplaySP();
    }

    return allAttachedDisplays.valueAt(index);
}

NativeWindowSP VisualContext::getWindow(int32_t windowIndex) const
{
    uint32_t index = allAttachedWindows.find(windowIndex);

    if (index == allAttachedWindows.size())
    {
        return NativeWindowSP();
    }

    return allAttachedWindows.valueAt(index);
}

VisualContext::VisualContext() :
    IVisualContext(), allAttachedDisplays(), allAttachedDisplayKeys(), allAttachedWindows(), allAttachedWindowKeys()
{
}

VisualContext::~VisualContext()
{
    allAttachedDisplays.clear();
    allAttachedDisplayKeys.clear();

    allAttachedWindows.clear();
    allAttachedWindowKeys.clear();
}

void VisualContext::attachDisplay(const NativeDisplaySP& display)
{
    if (!display.get())
    {
        return;
    }

    allAttachedDisplays[display->getIndex()] = display;

    if (std::find(allAttachedDisplayKeys.begin(), allAttachedDisplayKeys.end(), display->getIndex()) == allAttachedDisplayKeys.end())
    {
        allAttachedDisplayKeys.push_back(display->getIndex());
    }
}

void VisualContext::attachWindow(const NativeWindowSP& window)
{
    if (!window.get())
    {
        return;
    }

    allAttachedWindows[window->getIndex()] = window;

    if (std::find(allAttachedWindowKeys.begin(), allAttachedWindowKeys.end(), window->getIndex()) == allAttachedWindowKeys.end())
    {
        allAttachedWindowKeys.push_back(window->getIndex());
    }
}

//
// IVisualContext
//

// Display functions.

VkBool32 VisualContext::isDisplayAttached(const int32_t displayIndex) const
{
    return getDisplay(displayIndex).get() != nullptr;
}

const std::vector<int32_t>& VisualContext::getAttachedDisplayIndices() const
{
    return allAttachedDisplayKeys;
}

const glm::uvec2& VisualContext::getDisplayDimension(const int32_t displayIndex) const
{
    static glm::uvec2 noDimension = glm::uvec2(0, 0);

    auto currentDisplay = getDisplay(displayIndex);

    if (currentDisplay.get())
    {
        return currentDisplay->getDimension();
    }

    return noDimension;
}

VKTS_NATIVE_DISPLAY VisualContext::getNativeDisplay(const int32_t displayIndex) const
{
    static VKTS_NATIVE_DISPLAY noDisplay;

    auto currentDisplay = getDisplay(displayIndex);

    if (currentDisplay.get())
    {
        return currentDisplay->getNativeDisplay();
    }

    return noDisplay;
}

// Window functions.

VkBool32 VisualContext::isWindowAttached(const int32_t windowIndex) const
{
    return getWindow(windowIndex).get() != nullptr;
}

const std::vector<int32_t>& VisualContext::getAttachedWindowIndices() const
{
    return allAttachedWindowKeys;
}

const glm::uvec2& VisualContext::getWindowDimension(const int32_t windowIndex) const
{
    static glm::uvec2 noDimension = glm::uvec2(0, 0);

    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->getDimension();
    }

    return noDimension;
}

VKTS_NATIVE_WINDOW VisualContext::getNativeWindow(const int32_t windowIndex) const
{
    static VKTS_NATIVE_WINDOW noWindow;

    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->getNativeWindow();
    }

    return noWindow;
}

// Input functions.

VkBool32 VisualContext::getKey(const int32_t windowIndex, const int32_t keyIndex) const
{
    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->getKeyInput().getKey(keyIndex);
    }

    return VK_FALSE;
}

VkBool32 VisualContext::getMouseButton(const int32_t windowIndex, const int32_t buttonIndex) const
{
    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->getMouseInput().getButton(buttonIndex);
    }

    return VK_FALSE;
}

const glm::ivec2& VisualContext::getMouseLocation(const int32_t windowIndex) const
{
    static glm::ivec2 noLocation = glm::ivec2(-1, -1);

    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->getMouseInput().getLocation();
    }

    return noLocation;
}

VkBool32 VisualContext::isGameMouse(const int32_t windowIndex) const
{
    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->isInvisibleCursor();
    }

    return VK_FALSE;
}

int32_t VisualContext::getMouseWheel(const int32_t windowIndex) const
{
    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->getMouseInput().getMouseWheel();
    }

    return 0;
}

VkBool32 VisualContext::getGamepadButton(const int32_t windowIndex, const int32_t gamepadIndex, const int32_t buttonIndex) const
{
    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        if (gamepadIndex < 0 || gamepadIndex >= VKTS_MAX_GAMEPADS)
        {
            return VK_FALSE;
        }

        return currentWindow->getGamepadInput(gamepadIndex).getButton(buttonIndex);
    }

    return VK_FALSE;
}

float VisualContext::getGamepadAxis(const int32_t windowIndex, const int32_t gamepadIndex, const int32_t axisIndex) const
{
    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        if (gamepadIndex < 0 || gamepadIndex >= VKTS_MAX_GAMEPADS)
        {
            return 0.0f;
        }

        return currentWindow->getGamepadInput(gamepadIndex).getAxis(axisIndex);
    }

    return 0.0f;
}

VkBool32 VisualContext::getTouchpadPressed(const int32_t windowIndex, const int32_t slotIndex) const
{
    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->getTouchpadInput().getPressed(slotIndex);
    }

    return VK_FALSE;
}

const glm::ivec2& VisualContext::getTouchpadLocation(const int32_t windowIndex, const int32_t slotIndex) const
{
    static glm::ivec2 noLocation = glm::ivec2(-1, -1);

    auto currentWindow = getWindow(windowIndex);

    if (currentWindow.get())
    {
        return currentWindow->getTouchpadInput().getLocation(slotIndex);
    }

    return noLocation;
}

} /* namespace vkts */
