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
#include <Psapi.h>

namespace vkts
{

static PDH_HQUERY g_query;

static PDH_HCOUNTER g_counter[VKTS_MAX_QUERY_CPU];

static HANDLE g_process = INVALID_HANDLE_VALUE;

static ULARGE_INTEGER g_lastTime;

static ULARGE_INTEGER g_lastKernelTime;
static ULARGE_INTEGER g_lastUserTime;

VkBool32 VKTS_APIENTRY _profileInit()
{
	auto result = PdhOpenQuery(NULL, 0, &g_query);

	if (result != ERROR_SUCCESS)
	{
		return VK_FALSE;
	}

	char counterString[VKTS_MAX_CPU_STRING];

	for (uint32_t cpu = 0; cpu < glm::min(processorGetNumber(), VKTS_MAX_QUERY_CPU); cpu++)
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

	//

	g_process = GetCurrentProcess();

	if (g_process == INVALID_HANDLE_VALUE)
	{
		return VK_FALSE;
	}

	FILETIME time;

	FILETIME creationTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;

	GetSystemTimeAsFileTime(&time);
	memcpy(&g_lastTime, &time, sizeof(FILETIME));

	if (!GetProcessTimes(g_process, &creationTime, &exitTime, &kernelTime, &userTime))
	{
		return VK_FALSE;
	}

	memcpy(&g_lastKernelTime, &kernelTime, sizeof(FILETIME));
	memcpy(&g_lastUserTime, &userTime, sizeof(FILETIME));

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _profileGetCpuUsage(float& usage, const uint32_t cpu)
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

VkBool32 VKTS_APIENTRY _profileApplicationGetCpuUsage(float& usage)
{
	FILETIME time;

	FILETIME creationTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;

	ULARGE_INTEGER currentTime;

	GetSystemTimeAsFileTime(&time);
	memcpy(&currentTime, &time, sizeof(FILETIME));

	auto deltaTime = currentTime.QuadPart - g_lastTime.QuadPart;

	if (deltaTime == 0)
	{
		return VK_FALSE;
	}

	if (!GetProcessTimes(g_process, &creationTime, &exitTime, &kernelTime, &userTime))
	{
		return VK_FALSE;
	}

	ULARGE_INTEGER currentKernelTime;
	ULARGE_INTEGER currentUserTime;

	memcpy(&currentKernelTime, &kernelTime, sizeof(FILETIME));
	memcpy(&currentUserTime, &userTime, sizeof(FILETIME));

	//

	usage = (float)(((currentKernelTime.QuadPart - g_lastKernelTime.QuadPart) + (currentUserTime.QuadPart - g_lastUserTime.QuadPart)) / deltaTime) / (float)processorGetNumber() * 100.0f;

	//

	g_lastTime = currentTime;

	g_lastKernelTime = currentKernelTime;
	g_lastUserTime = currentUserTime;

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _profileApplicationGetRam(uint64_t& ram)
{
	PROCESS_MEMORY_COUNTERS pmc;

	if (!GetProcessMemoryInfo(g_process, &pmc, sizeof(pmc)))
	{
		return VK_FALSE;
	}

	ram = pmc.WorkingSetSize / 1024;

	return VK_TRUE;
}

void VKTS_APIENTRY _profileTerminate()
{
	g_process = INVALID_HANDLE_VALUE;

	//

	for (uint32_t cpu = 0; cpu < processorGetNumber(); cpu++)
	{
		PdhRemoveCounter(&g_counter[cpu]);
	}

	PdhCloseQuery(&g_query);
}

}
