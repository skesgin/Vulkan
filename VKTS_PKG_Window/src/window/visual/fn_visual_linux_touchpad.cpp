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

#include "fn_visual_internal.hpp"

#include "../input/GamepadInput.hpp"

#include "fn_visual_touchpad_internal.hpp"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <vkts/window/vkts_window.hpp>

// Event numbering from evtest.
#define HAS_EVENT(eventNumber, events) ((1 << eventNumber) & events)

namespace vkts
{

typedef struct _TouchInfo {
	VkBool32 pressed;
	int32_t x;
	int32_t y;
} TouchInfo;

static int touchpadFile = -1;

static TouchInfo touchInfo[VKTS_MAX_TOUCHPAD_SLOTS];

static int32_t rangeX;
static int32_t rangeY;

//

VkBool32 VKTS_APIENTRY _visualInitTouchpad(const VkInstance instance, const VkPhysicalDevice physicalDevice)
{
	char eventFilename[256];
	int eventNumber;

	int fileDescriptor;

    // Detect touchpad.

	uint64_t eventBits;

	strcpy(eventFilename, "/dev/input/event0");

	for (eventNumber = 0; eventNumber < 32; eventNumber++)
	{
		if (eventNumber < 10)
		{
			eventFilename[16] = '0' + (char)eventNumber;
			eventFilename[17] = 0;
		}
		else
		{
			eventFilename[16] = '0' + (char)(eventNumber / 10);
			eventFilename[17] = '0' + (char)(eventNumber % 10);
			eventFilename[18] = 0;
		}

		fileDescriptor = open(eventFilename, O_RDONLY | O_NONBLOCK);

		if (fileDescriptor < 0)
		{
			continue;
		}

		eventBits = 0;

		if (ioctl(fileDescriptor, EVIOCGBIT(0, EV_MAX), &eventBits) < 0)
		{
			close(fileDescriptor);

			continue;
		}

		if (HAS_EVENT(3, eventBits))
		{
			touchpadFile = fileDescriptor;

			//

			struct input_absinfo absinfo;

			ioctl(fileDescriptor, EVIOCGABS(ABS_X), &absinfo);
			rangeX = absinfo.maximum - absinfo.minimum;

			ioctl(fileDescriptor, EVIOCGABS(ABS_Y), &absinfo);
			rangeY = absinfo.maximum - absinfo.minimum;

			break;
		}
        else
        {
            close(fileDescriptor);
        }
	}

	//

	for (int32_t i = 0; i < VKTS_MAX_TOUCHPAD_SLOTS; i++)
	{
		touchInfo[i].pressed = VK_FALSE;
		touchInfo[i].x = 0;
		touchInfo[i].y = 0;
	}

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _visualDispatchMessagesTouchpad()
{
    struct input_event touchpadEvent;

    int64_t bytesRead;

    VkBool32 keepPooling;

	if (touchpadFile != -1)
	{
		keepPooling = VK_TRUE;

		int32_t currentSlot = 0;

		while (keepPooling)
		{
			bytesRead = read(touchpadFile, &touchpadEvent, sizeof(touchpadEvent));

			if (bytesRead == -1 || bytesRead != sizeof(touchpadEvent))
			{
				keepPooling = VK_FALSE;

				break;
			}

			if (touchpadEvent.type == EV_ABS)
			{
				switch (touchpadEvent.code)
				{
					case ABS_X:
						if (currentSlot >= 0 && currentSlot < VKTS_MAX_TOUCHPAD_SLOTS)
						{
							_visualTouchpadSetLocationX(currentSlot, (int32_t)touchpadEvent.value, rangeX, rangeY);
						}
						break;
					case ABS_Y:
						if (currentSlot >= 0 && currentSlot < VKTS_MAX_TOUCHPAD_SLOTS)
						{
							_visualTouchpadSetLocationY(currentSlot, (int32_t)touchpadEvent.value, rangeX, rangeY);
						}
						break;
					case ABS_MT_SLOT:
						currentSlot = (int32_t)touchpadEvent.value;
						break;
					case ABS_MT_POSITION_X:
						if (currentSlot >= 0 && currentSlot < VKTS_MAX_TOUCHPAD_SLOTS)
						{
							_visualTouchpadSetLocationX(currentSlot, (int32_t)touchpadEvent.value, rangeX, rangeY);
						}
						break;
					case ABS_MT_POSITION_Y:
						if (currentSlot >= 0 && currentSlot < VKTS_MAX_TOUCHPAD_SLOTS)
						{
							_visualTouchpadSetLocationY(currentSlot, (int32_t)touchpadEvent.value, rangeX, rangeY);
						}
						break;
					case ABS_MT_TRACKING_ID:
						if (currentSlot >= 0 && currentSlot < VKTS_MAX_TOUCHPAD_SLOTS)
						{
							if (touchpadEvent.value == -1)
							{
								_visualTouchpadSetPressed(currentSlot, VK_FALSE);
							}
							else
							{
								_visualTouchpadSetPressed(currentSlot, VK_TRUE);
							}
						}
						break;
					default:
						// Do nothing.
						break;
				}
			}
			else if (touchpadEvent.type == SYN_MT_REPORT)
			{
				currentSlot++;
			}
			else if (touchpadEvent.type == SYN_REPORT)
			{
				currentSlot = 0;
			}
		}
	}

	return VK_TRUE;
}

void VKTS_APIENTRY _visualTerminateTouchpad()
{
	if (touchpadFile != -1)
	{
		close(touchpadFile);

		touchpadFile = -1;
	}
}

}
