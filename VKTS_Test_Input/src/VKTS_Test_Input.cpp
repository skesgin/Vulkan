#include <vkts/vkts_no_vulkan.hpp>

class Test : public vkts::IUpdateThread
{

private:

	int32_t displayIndex;

	int32_t windowIndex;

	vkts::IVisualContextSP visualContext;

public:

	Test(const int32_t displayIndex, const int32_t windowIndex, const vkts::IVisualContextSP& visualContext) :
			IUpdateThread(), displayIndex(displayIndex), windowIndex(windowIndex), visualContext(visualContext)
	{
	}

	virtual ~Test()
	{
	}

	virtual VkBool32 init(const vkts::IUpdateThreadContext& updateContext)
	{
		if (!visualContext.get())
		{
			return VK_FALSE;
		}

		if (!visualContext->isDisplayAttached(displayIndex))
		{
			return VK_FALSE;
		}

		if (!visualContext->isWindowAttached(windowIndex))
		{
			return VK_FALSE;
		}

		return VK_TRUE;
	}

	virtual VkBool32 update(const vkts::IUpdateThreadContext& updateContext)
	{
		//
		// Mouse.
		//

		static glm::ivec2 lastMouseLocation = visualContext->getMouseLocation(windowIndex);

		static VkBool32 lastMouseButtons[3] = { VK_FALSE, VK_FALSE, VK_FALSE };

		static int32_t lastMouseWheel = visualContext->getMouseWheel(windowIndex);

		if (visualContext->getMouseButton(windowIndex, VKTS_MOUSE_BUTTON_LEFT) && !lastMouseButtons[VKTS_MOUSE_BUTTON_LEFT])
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Left mouse button pressed.");

			lastMouseButtons[VKTS_MOUSE_BUTTON_LEFT] = VK_TRUE;
		}
		if (visualContext->getMouseButton(windowIndex, VKTS_MOUSE_BUTTON_RIGHT) && !lastMouseButtons[VKTS_MOUSE_BUTTON_RIGHT])
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Right mouse button pressed.");

			lastMouseButtons[VKTS_MOUSE_BUTTON_RIGHT] = VK_TRUE;
		}
		if (visualContext->getMouseButton(windowIndex, VKTS_MOUSE_BUTTON_MIDDLE) && !lastMouseButtons[VKTS_MOUSE_BUTTON_MIDDLE])
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Middle mouse button pressed.");

			lastMouseButtons[VKTS_MOUSE_BUTTON_MIDDLE] = VK_TRUE;
		}

		if (!visualContext->getMouseButton(windowIndex, VKTS_MOUSE_BUTTON_LEFT) && lastMouseButtons[VKTS_MOUSE_BUTTON_LEFT])
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Left mouse button released.");

			lastMouseButtons[VKTS_MOUSE_BUTTON_LEFT] = VK_FALSE;
		}
		if (!visualContext->getMouseButton(windowIndex, VKTS_MOUSE_BUTTON_RIGHT) && lastMouseButtons[VKTS_MOUSE_BUTTON_RIGHT])
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Right mouse button released.");

			lastMouseButtons[VKTS_MOUSE_BUTTON_RIGHT] = VK_FALSE;
		}
		if (!visualContext->getMouseButton(windowIndex, VKTS_MOUSE_BUTTON_MIDDLE) && lastMouseButtons[VKTS_MOUSE_BUTTON_MIDDLE])
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Middle mouse button released.");

			lastMouseButtons[VKTS_MOUSE_BUTTON_MIDDLE] = VK_FALSE;
		}

		if (lastMouseLocation.x != visualContext->getMouseLocation(windowIndex).x || lastMouseLocation.y != visualContext->getMouseLocation(windowIndex).y)
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Mouse location changed: %d %d", visualContext->getMouseLocation(windowIndex).x, visualContext->getMouseLocation(windowIndex).y);

			lastMouseLocation = visualContext->getMouseLocation(windowIndex);
		}

		if (lastMouseWheel != visualContext->getMouseWheel(windowIndex))
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Mouse wheel changed: %d", visualContext->getMouseWheel(windowIndex));

			lastMouseWheel = visualContext->getMouseWheel(windowIndex);
		}

		//
		// Keyboard.
		//

		static VkBool32 lastKeys[1024 + 1] = { VK_FALSE };

		for (int32_t keyCode = 0; keyCode <= 1024; keyCode++)
		{
			if (visualContext->getKey(windowIndex, keyCode) && !lastKeys[keyCode])
			{
				vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Key pressed: %d", keyCode);

				if (keyCode < 256)
				{
					vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "           : '%c'", keyCode);
				}

				lastKeys[keyCode] = VK_TRUE;
			}
			if (!visualContext->getKey(windowIndex, keyCode) && lastKeys[keyCode])
			{
				vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Key released: %d", keyCode);

				if (keyCode < 256)
				{
					vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "           : '%c'", keyCode);
				}

				lastKeys[keyCode] = VK_FALSE;
			}
		}

		//
		// Gamepad.
		//

		static VkBool32 lastGamepadButtons[VKTS_MAX_GAMEPAD_BUTTONS] = { VK_FALSE };

		static float lastJoystickAxis[VKTS_MAX_GAMEPAD_AXIS] = { 0.0f };

		for (int32_t buttonIndex = 0; buttonIndex < VKTS_MAX_GAMEPAD_BUTTONS; buttonIndex++)
		{
			if (visualContext->getGamepadButton(windowIndex, 0, buttonIndex) && !lastGamepadButtons[buttonIndex])
			{
				vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Gamepad button pressed: %d", buttonIndex);

				lastGamepadButtons[buttonIndex] = VK_TRUE;
			}
			if (!visualContext->getGamepadButton(windowIndex, 0, buttonIndex) && lastGamepadButtons[buttonIndex])
			{
				vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Gamepad button released: %d", buttonIndex);

				lastGamepadButtons[buttonIndex] = VK_FALSE;
			}
		}

		for (int32_t axisIndex = 0; axisIndex < VKTS_MAX_GAMEPAD_AXIS; axisIndex++)
		{
			if (visualContext->getGamepadAxis(windowIndex, 0, axisIndex) != lastJoystickAxis[axisIndex])
			{
				vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Gamepad axis changed: %d %f", axisIndex, visualContext->getGamepadAxis(windowIndex, 0, axisIndex));

				lastJoystickAxis[axisIndex] = visualContext->getGamepadAxis(windowIndex, 0, axisIndex);
			}
		}

		return VK_TRUE;
	}

	virtual void terminate(const vkts::IUpdateThreadContext& updateContext)
	{
	}

};

int main(int argc, char* argv[])
{
	//
	// Engine initialization.
	//

	if (!vkts::engineInit(vkts::visualDispatchMessages))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Test: Could not initialize engine.");

		return -1;
	}

	vkts::logSetLevel(VKTS_LOG_INFO);

	//
	// Visual initialization.
	//

	// Note: If using VK_KHR_display, values have to be set.
	if (!vkts::visualInit(VK_NULL_HANDLE, VK_NULL_HANDLE))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not initialize visual.");

		vkts::engineTerminate();

		return -1;
	}

	auto display = vkts::visualCreateDefaultDisplay().lock();

	if (!display.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create display.");

		vkts::visualTerminate();

		vkts::engineTerminate();

		return -1;
	}

	auto window = vkts::visualCreateWindow(display, "Test", 1024, 768, VK_FALSE, VK_TRUE, VK_FALSE).lock();

	if (!window.get())
	{
		window = vkts::visualGetWindow(VKTS_DEFAULT_WINDOW_INDEX).lock();

		if (!window.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create window.");

			display->destroy();

			vkts::visualTerminate();

			vkts::engineTerminate();

			return -1;
		}
	}

	//
	// Example setup.
	//

	auto visualContext = vkts::visualCreateContext();

	if (!visualContext.get())
	{
		display->destroy();

		vkts::visualTerminate();

		vkts::engineTerminate();

		return -1;
	}

	// Single threaded application, so it is safe to pass display and window.
	vkts::IUpdateThreadSP example = vkts::IUpdateThreadSP(new Test(display->getIndex(), window->getIndex(), visualContext));

	if (!example.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create application.");

		visualContext.reset();

		window->destroy();

		display->destroy();

		vkts::visualTerminate();

		vkts::engineTerminate();

		return -1;
	}

	// This update thread is executed in the main loop. No additional thread is launched.
	vkts::engineAddUpdateThread(example);

	//
	// Execution.
	//

	vkts::engineRun();

	//
	// Termination.
	//

	visualContext.reset();

	window->destroy();

	display->destroy();

	vkts::visualTerminate();

	vkts::engineTerminate();

	return 0;
}
