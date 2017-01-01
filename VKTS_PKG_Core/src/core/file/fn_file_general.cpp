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

namespace vkts
{

VkBool32 VKTS_APIENTRY _fileInit()
{
	std::string testFilename = "vk_layer_settings.txt";

	auto testFile = fopen(testFilename.c_str(), "rb");

	if (testFile)
	{
		fclose(testFile);

		return VK_TRUE;
	}

	std::string directory = "../VKTS_Binaries/";

	for (uint32_t i = 0; i < 3; i++)
	{
		testFile = fopen((directory + testFilename).c_str(), "rb");

		if (testFile)
		{
			_fileSetBaseDirectory(directory.c_str());

			fclose(testFile);

			return VK_TRUE;
		}

		directory = "../" + directory;
	}

	logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not find base directory");

	return VK_TRUE;
}

IBinaryBufferSP VKTS_APIENTRY _fileLoadBinary(const char* filename)
{
    if (!filename)
    {
        return IBinaryBufferSP();
    }

    //

    FILE* file;

    std::string loadFilename = _fileGetBaseDirectory() + std::string(filename);

    //

    file = fopen(loadFilename.c_str(), "rb");

    if (!file)
    {
        return IBinaryBufferSP();
    }

    if (fseek(file, 0, SEEK_END))
    {
        fclose(file);

        return IBinaryBufferSP();
    }

    int64_t length = ftell(file);

    if (length < 0)
    {
        fclose(file);

        return IBinaryBufferSP();
    }

    uint32_t size = static_cast<uint32_t>(length);

    if (size == 0)
    {
        fclose(file);

        return IBinaryBufferSP();
    }

    auto data = new uint8_t[size];

    if (!data)
    {
        fclose(file);

        return IBinaryBufferSP();
    }

    memset(data, 0, size);

    rewind(file);

    auto elementsRead = fread(data, 1, size, file);

    fclose(file);

    //

    if (elementsRead != size)
    {
        delete[] data;

        return IBinaryBufferSP();
    }

    auto buffer = IBinaryBufferSP(new BinaryBuffer(data, size));

    //

	if (!buffer.get())
	{
	    delete[] data;

	    return IBinaryBufferSP();
	}

	if  (buffer->getSize() != size)
	{
		return IBinaryBufferSP();
	}

	//

    return buffer;
}

VkBool32 VKTS_APIENTRY _filePrepareSaveBinary(const char* filename)
{
	// Do nothing.

	return VK_TRUE;
}

}
