#include <vkts/vkts_no_vulkan.hpp>

class Test : public vkts::IUpdateThread
{

private:

	std::string name;

	int32_t displayIndex;

	int32_t windowIndex;

	vkts::IVisualContextSP visualContext;

public:

	Test(const std::string& name, const vkts::IVisualContextSP& visualContext) :
			IUpdateThread(), name(name), displayIndex(-1), windowIndex(-1), visualContext(visualContext)
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

		vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Initialize done.", name.c_str());

		vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Thread %d from %d.", name.c_str(), updateContext.getThreadIndex(), updateContext.getThreadCount());

		const auto displayIndexWalker = visualContext->getAttachedDisplayIndices().begin();

		const auto windowIndexWalker = visualContext->getAttachedWindowIndices().begin();

		if (displayIndexWalker != visualContext->getAttachedDisplayIndices().end() && windowIndexWalker != visualContext->getAttachedWindowIndices().end())
		{
			displayIndex = *displayIndexWalker;
			windowIndex = *windowIndexWalker;

			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Found display index = %d", name.c_str(), displayIndex);
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Width x Height = %d x %d", name.c_str(), visualContext->getDisplayDimension(displayIndex).x, visualContext->getDisplayDimension(displayIndex).y);
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Found window index = %d", name.c_str(), windowIndex);
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Width x Height = %d x %d", name.c_str(), visualContext->getWindowDimension(windowIndex).x, visualContext->getWindowDimension(windowIndex).y);
		}

		return VK_TRUE;
	}

	virtual VkBool32 update(const vkts::IUpdateThreadContext& updateContext)
	{
		if (windowIndex >= 0)
		{
			vkts::logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Test '%s': Width x Height = %d x %d", name.c_str(), visualContext->getWindowDimension(windowIndex).x, visualContext->getWindowDimension(windowIndex).y);

			if (visualContext->getKey(windowIndex, VKTS_KEY_Q))
			{
				vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Quit pressed!", name.c_str());

				return VK_FALSE;
			}
		}

		vkts::logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Test '%s': Updating %f", name.c_str(), updateContext.getTotalTime());

		vkts::logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Test '%s': Before barrier.", name.c_str());

		if (!vkts::barrierSync())
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Barrier killed.", name.c_str());

			return VK_FALSE;
		}

		vkts::logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Test '%s': After barrier.", name.c_str());

		if (name == "a" && updateContext.getTotalTime() > 10.0)
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Time reached!", name.c_str());

			return VK_FALSE;
		}

		return VK_TRUE;
	}

	virtual void terminate(const vkts::IUpdateThreadContext& updateContext)
	{
		vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test '%s': Terminate done.", name.c_str());
	}

};

int main(int argc, char* argv[])
{
	vkts::engineInit(vkts::visualDispatchMessages);

	vkts::logSetLevel(VKTS_LOG_INFO);

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test: Number of processors = %u.", vkts::processorGetNumber());

	//
	// Visual setup.
	//

	// Note: If using VK_KHR_display, values have to be set.
	if (!vkts::visualInit(VK_NULL_HANDLE, VK_NULL_HANDLE))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Test: Could not initialize visual.");

		vkts::engineTerminate();

		return -1;
	}

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test: Number of displays = %d", vkts::visualGetNumberDisplays());

	auto display = vkts::visualCreateDefaultDisplay().lock();

	if (!display.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Test: Could not create display.");

		vkts::visualTerminate();

		vkts::engineTerminate();

		return -1;
	}

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test: Display = %d Width x Height = %d x %d", display->getIndex(), display->getWidth(), display->getHeight());

	auto window = vkts::visualCreateWindow(display, "Test", 1024, 768, VK_FALSE, VK_TRUE, VK_FALSE).lock();

	if (!window.get())
	{
		window = vkts::visualGetWindow(VKTS_DEFAULT_WINDOW_INDEX).lock();

		if (!window.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Test: Could not create window.");

			display->destroy();

			vkts::visualTerminate();

			vkts::engineTerminate();

			return -1;
		}
	}

	//

	auto visualContext = vkts::visualCreateContext();

	if (!visualContext.get())
	{
		display->destroy();

		vkts::visualTerminate();

		vkts::engineTerminate();

		return -1;
	}

	//

	vkts::IUpdateThreadSP a = vkts::IUpdateThreadSP(new Test("a", visualContext));
	vkts::IUpdateThreadSP b = vkts::IUpdateThreadSP(new Test("b", visualContext));
	vkts::IUpdateThreadSP c = vkts::IUpdateThreadSP(new Test("c", visualContext));

	if (!a.get() || !b.get() || !c.get())
	{
		printf("Test: Could not create test application.");

		visualContext.reset();

		display->destroy();

		vkts::visualTerminate();

		vkts::engineTerminate();

		return -1;
	}


	//
	// Engine setup.
	//

	vkts::engineAddUpdateThread(a);
	vkts::engineAddUpdateThread(b);
	vkts::engineAddUpdateThread(c);

	//
	// File test.
	//

	auto textBuffer = vkts::fileLoadText("test/general/input.txt");

	if (textBuffer.get())
	{
		if (!vkts::fileSaveText("test/general/output.txt", textBuffer))
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not save file.");
		}
	}
	else
	{
		vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not load file.");
	}

	auto createdTextBuffer = vkts::textBufferCreate("");

	if (createdTextBuffer.get())
	{
		createdTextBuffer->puts("Created text by VKTS.");
		createdTextBuffer->puts("\nAppended text.");

		if (!vkts::fileSaveText("test/general/created.txt", createdTextBuffer))
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not save created text file.");
		}

		createdTextBuffer->seek(0, VKTS_SEARCH_ABSOLUTE);

		char buffer[256];

		while (createdTextBuffer->gets(buffer, 256))
		{
			vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Test: Created text line: '%s'", buffer);
		}
	}
	else
	{
		vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not create text.");
	}

	//
	// Image test.
	//

	auto imageTga = vkts::imageDataLoad("test/general/crate_input.tga");

	if (imageTga.get())
	{
		if (!vkts::imageDataSave("test/general/crate_output.tga", imageTga))
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not save TGA image.");
		}

		auto mipMaps = vkts::imageDataMipmap(imageTga, VK_TRUE, "test/general/crate_output.tga");

		for (uint32_t i = 0; i < mipMaps.size(); i++)
		{
			if (!vkts::imageDataSave(mipMaps[i]->getName().c_str(), mipMaps[i]))
			{
				vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not save mip map TGA image.");
			}
		}
	}
	else
	{
		vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not load TGA image.");
	}

	//

	auto imageHdr = vkts::imageDataLoad("test/general/CraterLake_input.hdr");

	if (imageHdr.get())
	{
		if (!vkts::imageDataSave("test/general/CraterLake_output.hdr", imageHdr))
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not save HDR image.");
		}
	}
	else
	{
		vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not load HDR image.");
	}

	//

	if (imageHdr.get())
	{
		auto cubeMaps = vkts::imageDataCubemap(imageHdr, 256, "test/general/CraterLake.hdr");

		if (cubeMaps.size() != 0)
		{
			for (uint32_t i = 0; i < cubeMaps.size(); i++)
			{
				vkts::imageDataSave(cubeMaps[i]->getName().c_str(), cubeMaps[i]);
			}
		}
		else
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not generate cube maps.");
		}
	}
	else
	{
		vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not load HDR image.");
	}

	//

	auto createdTga = vkts::imageDataCreate("test/general/created.tga", 16, 16, 1, 1.0f, 0.0f, 0.0f, 0.0f, VK_IMAGE_TYPE_2D, VK_FORMAT_R8G8B8_UNORM);

	if (createdTga.get())
	{
		if (!vkts::imageDataSave(createdTga->getName().c_str(), createdTga))
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not save created TGA image.");
		}
	}
	else
	{
		vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not create TGA image.");
	}

	//

	auto createdHdr = vkts::imageDataCreate("test/general/created.hdr", 16, 16, 1, 1.0f, 0.0f, 0.0f, 0.0f, VK_IMAGE_TYPE_2D, VK_FORMAT_R32G32B32_SFLOAT);

	if (createdHdr.get())
	{
		if (!vkts::imageDataSave(createdHdr->getName().c_str(), createdHdr))
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not save created HDR image.");
		}
	}
	else
	{
		vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not create HDR image.");
	}

	//

	auto imageRGB = vkts::imageDataLoad("test/general/parque_rgb.tga");

	if (imageRGB.get())
	{
		auto imageRGBA = vkts::imageDataConvert(imageRGB, VK_FORMAT_R8G8B8A8_UNORM, "test/general/parque_rgba.tga");

		if (imageRGBA.get())
		{
			if (!vkts::imageDataSave("test/general/parque_rgba.tga", imageRGBA))
			{
				vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not save converted RGBA image.");
			}
		}
		else
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not convert RGB image.");
		}
	}
	else
	{
		vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Test: Could not load RGB image.");
	}

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
