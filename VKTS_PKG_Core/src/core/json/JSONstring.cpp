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

JSONstring::JSONstring() :
	JSONvalue(), value("")
{
}

JSONstring::JSONstring(const std::string& value) : JSONvalue(), value(value)
{
}

JSONstring::~JSONstring()
{
}

const std::string& JSONstring::getValue() const
{
	return value;
}

void JSONstring::setValue(const std::string& value)
{
	this->value = value;
}

VkBool32 JSONstring::encode(std::string& jsonText, std::int32_t& spaces) const
{
	jsonText += JSON_quotation_mark;

	for (uint32_t i = 0; i < getValue().length(); i++)
	{
		char c = getValue()[i];

		if (((int32_t)c >= JSON_C0_start && (int32_t)c <= JSON_C0_end) || ((int32_t)c >= JSON_C1_start && (int32_t)c <= JSON_C1_end))
		{
			return VK_FALSE;
		}

		if (c == JSON_quotation_mark[0])
		{
			jsonText += JSON_reverse_solidus;
			jsonText += JSON_quotation_mark;
		}
		else if (c == JSON_reverse_solidus[0])
		{
			jsonText += JSON_reverse_solidus;
			jsonText += JSON_reverse_solidus;
		}
		else if (c == JSON_solidus[0])
		{
			jsonText += JSON_reverse_solidus;
			jsonText += JSON_solidus;
		}
		else if (c == JSON_Encode_backspace[0])
		{
			jsonText += JSON_reverse_solidus;
			jsonText += JSON_Decode_backspace;
		}
		else if (c == JSON_Encode_form_feed[0])
		{
			jsonText += JSON_reverse_solidus;
			jsonText += JSON_Decode_form_feed;
		}
		else if (c == JSON_Encode_line_feed[0])
		{
			jsonText += JSON_reverse_solidus;
			jsonText += JSON_Decode_line_feed;
		}
		else if (c == JSON_Encode_carriage_return[0])
		{
			jsonText += JSON_reverse_solidus;
			jsonText += JSON_Decode_carriage_return;
		}
		else if (c == JSON_Encode_character_tabulation[0])
		{
			jsonText += JSON_reverse_solidus;
			jsonText += JSON_Decode_character_tabulation;
		}
		else
		{
			jsonText += getValue().substr(i, 1);
		}
	}

	jsonText += JSON_quotation_mark;

	return VK_TRUE;
}

void JSONstring::visit(JsonVisitor& jsonVisitor)
{
	jsonVisitor.visit(*this);
}

}
