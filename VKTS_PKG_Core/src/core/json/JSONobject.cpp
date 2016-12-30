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

JSONobject::JSONobject() :
	JSONvalue(), allKeyValues()
{
}

JSONobject::~JSONobject()
{
}

void JSONobject::addKeyValue(const std::string& key, const JSONvalueSP& value)
{
	allKeyValues[key] = value;
}

VkBool32 JSONobject::hasKey(const std::string& key) const
{
	return allKeyValues.contains(key);
}

JSONvalueSP JSONobject::getValue(const std::string& key) const
{
	return allKeyValues[key];
}

const SmartPointerMap<std::string, JSONvalueSP>& JSONobject::getAllKeyValues() const
{
	return allKeyValues;
}

const Vector<std::string>& JSONobject::getAllKeys() const
{
	return allKeyValues.keys();
}

uint32_t JSONobject::size() const
{
	return allKeyValues.size();
}

VkBool32 JSONobject::encode(std::string& jsonText, std::int32_t& spaces) const
{
	jsonText += JSON_left_curly_bracket;

	const auto& allKeys = getAllKeys();

	if (allKeys.size() > 0)
	{
		doLineFeed(jsonText, spaces, 1);

		for (uint32_t key = 0; key < allKeys.size(); key++)
		{
			JSONstring tempJSONstring(allKeys[key]);

			if (!tempJSONstring.encode(jsonText, spaces))
			{
				return VK_FALSE;
			}

			jsonText += JSON_space;
			jsonText += JSON_colon;
			jsonText += JSON_space;

			if (!getValue(allKeys[key])->encode(jsonText, spaces))
			{
				return VK_FALSE;
			}

			if (key + 1 < allKeys.size())
			{
				jsonText += JSON_comma;

				doLineFeed(jsonText, spaces, 0);
			}
		}

		doLineFeed(jsonText, spaces, -1);
	}

	jsonText += JSON_right_curly_bracket;

	return VK_TRUE;
}

void JSONobject::visit(JsonVisitor& jsonVisitor)
{
	jsonVisitor.visit(*this);
}

}
