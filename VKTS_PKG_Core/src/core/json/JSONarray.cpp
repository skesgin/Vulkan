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

JSONarray::JSONarray() :
	JSONvalue(), allValues()
{
}

JSONarray::~JSONarray()
{
}

void JSONarray::addValue(const JSONvalueSP& value)
{
	allValues.push_back(value);
}

JSONvalueSP JSONarray::getValueAt(std::int32_t index) const
{
	return allValues.at(index);
}

const std::vector<JSONvalueSP>& JSONarray::getAllValues() const
{
	return allValues;
}

size_t JSONarray::size() const
{
	return allValues.size();
}

VkBool32 JSONarray::encode(std::string& jsonText, std::int32_t& spaces) const
{
	jsonText += JSON_left_square_bracket;

	const auto& allValues = getAllValues();

	if (allValues.size() > 0)
	{
		doLineFeed(jsonText, spaces, 1);

		auto walker = allValues.begin();

		while (walker != allValues.end())
		{
			if (!(*walker)->encode(jsonText, spaces))
			{
				return VK_FALSE;
			}

			walker++;

			if (walker != allValues.end())
			{
				jsonText += JSON_comma;

				doLineFeed(jsonText, spaces, 0);
			}
		}

		doLineFeed(jsonText, spaces, -1);
	}

	jsonText += JSON_right_square_bracket;

	return VK_TRUE;
}

void JSONarray::visit(JsonVisitor& jsonVisitor)
{
	jsonVisitor.visit(*this);
}

}
