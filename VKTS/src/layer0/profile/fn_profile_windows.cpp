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

#include <vkts/vkts.hpp>

#include "fn_profile_internal.hpp"

#include <windows.h>

#include <pdh.h>

namespace vkts
{

static PDH_HQUERY g_query;

static PDH_HCOUNTER g_counter[VKTS_MAX_QUERY_CPU];

VkBool32 VKTS_APIENTRY _profileInit()
{
	auto result = PdhOpenQuery(NULL, 0, &g_query);

	if (result != ERROR_SUCCESS)
	{
		return VK_FALSE;
	}

	char counterString[VKTS_MAX_CPU_STRING];

	for (uint32_t cpu = 0; cpu < glm::max(processorGetNumber(), VKTS_MAX_QUERY_CPU); cpu++)
	{
		sprintf(counterString, "\\Processor(%u)\\%% Processor Time", cpu);

		result = PdhAddCounter(g_query, counterString, 0, &g_counter[cpu]);

		if (result != ERROR_SUCCESS)
		{
			return VK_FALSE;
		}
	}

	result = PdhCollectQueryData(g_query);

	if (result != ERROR_SUCCESS)
	{
		return VK_FALSE;
	}

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _profileGetUsage(float& usage, const uint32_t cpu)
{
	if (cpu >= processorGetNumber())
	{
		return VK_FALSE;
	}

	auto result = PdhCollectQueryData(g_query);

	if (result != ERROR_SUCCESS)
	{
		return VK_FALSE;
	}

	PDH_FMT_COUNTERVALUE counterValue;

	result = PdhGetFormattedCounterValue(g_counter[cpu], PDH_FMT_DOUBLE, NULL, &counterValue);

	if (result != ERROR_SUCCESS)
	{
		return VK_FALSE;
	}

	usage = (float)counterValue.doubleValue;

	return VK_TRUE;
}

void VKTS_APIENTRY _profileTerminate()
{
	for (uint32_t cpu = 0; cpu < processorGetNumber(); cpu++)
	{
		PdhRemoveCounter(&g_counter[cpu]);
	}

	PdhCloseQuery(&g_query);
}

}
