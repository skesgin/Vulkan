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

#include <vkts/core/vkts_core.hpp>

#include "../binary_buffer/BinaryBuffer.hpp"

#include "fn_file_internal.hpp"

#include <sys/stat.h>

extern struct android_app* g_app;

namespace vkts
{

VkBool32 VKTS_APIENTRY _fileInit()
{
	if (!::g_app)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No android application.");

		return VK_FALSE;
	}

    fileSetBaseDirectory(::g_app->activity->externalDataPath);

	return VK_TRUE;
}

IBinaryBufferSP VKTS_APIENTRY _fileLoadBinary(const char* filename)
{
    if (!filename)
    {
        return IBinaryBufferSP();
    }

    //

    if (!::g_app)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No android application.");

		return IBinaryBufferSP();
	}

	AAssetManager* assetManager = ::g_app->activity->assetManager;

    if (!assetManager)
    {
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No asset manager.");

		return IBinaryBufferSP();
    }

    AAsset* sourceAsset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);

    if (!sourceAsset)
    {
		return IBinaryBufferSP();
    }

    const uint8_t* data = (const uint8_t*)AAsset_getBuffer(sourceAsset);

	const uint32_t size = (uint32_t)AAsset_getLength(sourceAsset);

	//

    auto buffer = IBinaryBufferSP(new BinaryBuffer(data, size));

	AAsset_close(sourceAsset);

	if (!buffer.get() || buffer->getSize() != size)
	{
	    return IBinaryBufferSP();
	}

	return buffer;
}

VkBool32 VKTS_APIENTRY _filePrepareSaveBinary(const char* filename)
{
	if (!::g_app)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No android application.");

		return VK_FALSE;
	}

	if (!filename)
	{
		return VK_FALSE;
	}

	std::string foldersToCreate = std::string(filename);

	auto lastSlash = foldersToCreate.rfind('/');

	if (lastSlash != foldersToCreate.npos)
	{
		foldersToCreate = foldersToCreate.substr(0, lastSlash);
	}

	return _fileCreateDirectory(foldersToCreate.c_str());
}

VkBool32 VKTS_APIENTRY _fileCreateDirectory(const char* directory)
{
	if (!directory)
	{
		return VK_FALSE;
	}

	//

	struct stat sb;

	if (stat(directory, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		return VK_TRUE;
	}

	//

	std::string targetDirectory = std::string(fileGetBaseDirectory());

	//

	std::string foldersToCreate = std::string(directory);

	while (foldersToCreate.length())
	{
		auto lastSlash = foldersToCreate.find('/');

		if (lastSlash != foldersToCreate.npos)
		{
			targetDirectory += foldersToCreate.substr(0, lastSlash) + "/";

			foldersToCreate = foldersToCreate.substr(lastSlash + 1);
		}
		else
		{
			targetDirectory += foldersToCreate + "/";

			foldersToCreate = "";
		}

		if (mkdir(targetDirectory.c_str(), S_IRWXU | S_IRWXG | (S_IROTH | S_IXOTH)) != 0)
		{
			continue;
		}
	}

	return VK_TRUE;
}

}
