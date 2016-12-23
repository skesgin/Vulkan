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

#define VKTS_JSON_TAB_STEP 3

namespace vkts
{

JSONvalue::JSONvalue()
{
}

JSONvalue::~JSONvalue()
{
}

VkBool32 JSONvalue::doLineFeed(std::string& jsonText, int32_t& spaces, int32_t tabs) const
{
	if (spaces + VKTS_JSON_TAB_STEP * tabs < 0)
	{
		return VK_FALSE;
	}

	jsonText += JSON_Encode_line_feed;

	spaces += VKTS_JSON_TAB_STEP * tabs;

	for (int32_t i = 0; i < spaces; i++)
	{
		jsonText += JSON_space;
	}

	return VK_TRUE;
}

}
