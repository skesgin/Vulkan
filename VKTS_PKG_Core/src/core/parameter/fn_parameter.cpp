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

namespace vkts
{

static int32_t VKTS_APIENTRY parameterGetIndex(const std::string& parameter, int argc, char *argv[])
{
	if (argc == 0 || !argv)
	{
		return -1;
	}

	int i = 0;

	while (i < argc - 1)
	{
		if (parameter.compare(argv[i]) == 0)
		{
			return (int32_t)(i + 1);
		}

		i++;
	}

    return -1;
}

VkBool32 VKTS_APIENTRY parameterIsPresent(const std::string& parameter, int argc, char *argv[])
{
	auto index = parameterGetIndex(parameter, argc, argv);

	if (index < 0)
	{
		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parameterGetInt32(int32_t& out, const std::string& parameter, int argc, char *argv[])
{
	auto index = parameterGetIndex(parameter, argc, argv);

	if (index < 0)
	{
		return VK_FALSE;
	}

	out = (int32_t)atol(argv[index]);

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parameterGetUInt32(uint32_t& out, const std::string& parameter, int argc, char *argv[])
{
	auto index = parameterGetIndex(parameter, argc, argv);

	if (index < 0)
	{
		return VK_FALSE;
	}

	out = (uint32_t)atol(argv[index]);

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parameterGetFloat(float& out, const std::string& parameter, int argc, char *argv[])
{
	auto index = parameterGetIndex(parameter, argc, argv);

	if (index < 0)
	{
		return VK_FALSE;
	}

	out = (float)atof(argv[index]);

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parameterGetString(std::string& out, const std::string& parameter, int argc, char *argv[])
{
	auto index = parameterGetIndex(parameter, argc, argv);

	if (index < 0)
	{
		return VK_FALSE;
	}

	out = argv[index];

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY parameterGetVkBool32(VkBool32& out, const std::string& parameter, int argc, char *argv[])
{
	auto index = parameterGetIndex(parameter, argc, argv);

	if (index < 0)
	{
		return VK_FALSE;
	}

	if (strcmp(argv[index], "true") == 0)
	{
		out = VK_TRUE;

		return VK_TRUE;
	}
	else if (strcmp(argv[index], "false") == 0)
	{
		out = VK_FALSE;

		return VK_TRUE;
	}

	return VK_FALSE;
}

}
