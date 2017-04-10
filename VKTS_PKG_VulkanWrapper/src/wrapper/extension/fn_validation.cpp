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

#include <vkts/vulkan/wrapper/vkts_wrapper.hpp>

namespace vkts
{

VkBool32 VKTS_APIENTRY validationGatherNeededInstanceLayers()
{
    VkResult result;

    //

    uint32_t propertyCount = 0;

    result = vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);

    if (result != VK_SUCCESS || propertyCount == 0)
    {
        return VK_FALSE;
    }

    std::vector<VkLayerProperties> allInstanceLayerProperties(propertyCount);

    result = vkEnumerateInstanceLayerProperties(&propertyCount, &allInstanceLayerProperties[0]);

    if (result != VK_SUCCESS)
    {
        return VK_FALSE;
    }

    //

    static const char* validationLayerNames[8] =
    {
		"VK_LAYER_GOOGLE_threading",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_device_limits",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_image",
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_swapchain",
		"VK_LAYER_GOOGLE_unique_objects"
	};

    for (auto validationLayerName : validationLayerNames)
    {
    	VkBool32 extensionFound = VK_FALSE;

		for (uint32_t i = 0; i < propertyCount; i++)
		{
			if (strcmp(validationLayerName, allInstanceLayerProperties[i].layerName) == 0)
			{
				if (!layerAddInstanceLayers(validationLayerName))
				{
					vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not add validation layer: %s", validationLayerName);
				}

				vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Successfully added validation layer: %s", validationLayerName);

				extensionFound = VK_TRUE;

				break;
			}
		}

		if (!extensionFound)
		{
			vkts::logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not find validation layer: %s", validationLayerName);
		}
    }

    return VK_TRUE;
}

}
