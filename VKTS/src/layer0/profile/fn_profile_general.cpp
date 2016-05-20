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

namespace vkts
{

typedef struct _CpuData {
	uint64_t user;
	uint64_t nice;
	uint64_t system;
	uint64_t idle;
} CpuData;

static CpuData g_cpuData[VKTS_MAX_QUERY_CPU];

static VkBool32 profileGetCpuData(CpuData& cpuData, const uint32_t cpu)
{
	FILE* file = fopen("/proc/stat", "r");

	if (!file)
	{
		return VK_FALSE;
	}

	char counterString[VKTS_MAX_CPU_STRING];

	sprintf(counterString, "cpu%u %%llu %%llu %%llu %%llu", cpu);

	auto result = fscanf(file, counterString, &cpuData.user, &cpuData.nice, &cpuData.system, &cpuData.idle);

	fclose(file);

	if (result != 4)
	{
		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _profileInit()
{
	for (uint32_t cpu = 0; cpu < glm::max(processorGetNumber(), VKTS_MAX_QUERY_CPU); cpu++)
	{
		if (!profileGetCpuData(g_cpuData[cpu], cpu))
		{
			return VK_FALSE;
		}
	}

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY _profileGetUsage(float& usage, const uint32_t cpu)
{
	if (cpu >= processorGetNumber())
	{
		return VK_FALSE;
	}

	CpuData currentCpuData;

	if (!profileGetCpuData(currentCpuData, cpu))
	{
		return VK_FALSE;
	}

	if (currentCpuData.user < g_cpuData[cpu].user || currentCpuData.nice < g_cpuData[cpu].nice || currentCpuData.system < g_cpuData[cpu].system || currentCpuData.idle < g_cpuData[cpu].idle)
	{
		g_cpuData[cpu] = currentCpuData;

		return VK_FALSE;
	}

	uint64_t totalUsage = (currentCpuData.user - g_cpuData[cpu].user) + (currentCpuData.nice - g_cpuData[cpu].nice) + (currentCpuData.system - g_cpuData[cpu].system);

	uint64_t total = totalUsage + (currentCpuData.idle - g_cpuData[cpu].idle);

	g_cpuData[cpu] = currentCpuData;

	if (total == 0)
	{
		return VK_FALSE;
	}

	usage = (float)((double)totalUsage / (double)total) * 100.0f;

	return VK_TRUE;
}

void VKTS_APIENTRY _profileTerminate()
{
	// Nothing for now.
}

}
