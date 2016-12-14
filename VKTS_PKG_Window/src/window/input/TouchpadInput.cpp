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

#include "TouchpadInput.hpp"

namespace vkts
{

TouchpadInput::TouchpadInput() :
    mutexPressed(), mutexLocation()
{
    resetTouchpad();
}

TouchpadInput::~TouchpadInput()
{

}

VkBool32 TouchpadInput::getPressed(const int32_t slotIndex) const
{
	if (slotIndex < 0 || slotIndex >= VKTS_MAX_TOUCHPAD_SLOTS)
	{
		return VK_FALSE;
	}

    std::lock_guard<std::mutex> lockGuard(mutexPressed);

    return pressed[slotIndex];
}

void TouchpadInput::setPressed(const int32_t slotIndex, const VkBool32 pressed)
{
	if (slotIndex < 0 || slotIndex >= VKTS_MAX_TOUCHPAD_SLOTS)
	{
		return;
	}

    std::lock_guard<std::mutex> lockGuard(mutexPressed);

    this->pressed[slotIndex] = pressed;
}

const glm::ivec2& TouchpadInput::getLocation(const int32_t slotIndex) const
{
	if (slotIndex < 0 || slotIndex >= VKTS_MAX_TOUCHPAD_SLOTS)
	{
		static glm::ivec2 noLocation = glm::ivec2(-1, -1);

		return noLocation;
	}

    std::lock_guard<std::mutex> lockGuard(mutexLocation);

    return location[slotIndex];
}

void TouchpadInput::setLocation(const int32_t slotIndex, const glm::ivec2& location)
{
	if (slotIndex < 0 || slotIndex >= VKTS_MAX_TOUCHPAD_SLOTS)
	{
		return;
	}

	std::lock_guard<std::mutex> lockGuard(mutexLocation);

    this->location[slotIndex] = location;
}

void TouchpadInput::setLocationX(const int32_t slotIndex, const int32_t x)
{
	if (slotIndex < 0 || slotIndex >= VKTS_MAX_TOUCHPAD_SLOTS)
	{
		return;
	}

	std::lock_guard<std::mutex> lockGuard(mutexLocation);

    this->location[slotIndex].x = x;
}

void TouchpadInput::setLocationY(const int32_t slotIndex, const int32_t y)
{
	if (slotIndex < 0 || slotIndex >= VKTS_MAX_TOUCHPAD_SLOTS)
	{
		return;
	}

	std::lock_guard<std::mutex> lockGuard(mutexLocation);

    this->location[slotIndex].y = y;
}

void TouchpadInput::resetTouchpad()
{
    std::lock_guard<std::mutex> lockGuardButtons(mutexPressed);
    std::lock_guard<std::mutex> lockGuardLocation(mutexLocation);

    for (int32_t slotIndex = 0; slotIndex < VKTS_MAX_TOUCHPAD_SLOTS; slotIndex++)
    {
        pressed[slotIndex] = VK_FALSE;
        location[slotIndex] = glm::ivec2(0, 0);
    }
}

} /* namespace vkts */
