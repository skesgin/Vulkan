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

#include "JsonDecoder.hpp"

namespace vkts
{

JsonDecoder::JsonDecoder() :
	jsonText("")
{
}

JsonDecoder::~JsonDecoder()
{
}

//

VkBool32 JsonDecoder::match(const std::string token, size_t& index)
{
	size_t length = token.length();

	if (jsonText.length() < index + length)
	{
		return VK_FALSE;
	}

	if (jsonText.substr(index, length).compare(token) == 0)
	{
		index += length;

		return VK_TRUE;
	}

	return VK_FALSE;
}

//

VkBool32 JsonDecoder::decodeWhitespace(size_t& index)
{
	return match(JSON_Encode_character_tabulation, index) || match(JSON_Encode_line_feed, index) ||match(JSON_Encode_carriage_return, index) || match(JSON_space, index);
}

//

VkBool32 JsonDecoder::decodeLeftSquareBracket(size_t& index)
{
	return match(JSON_left_square_bracket, index);
}

VkBool32 JsonDecoder::decodeLeftCurlyBracket(size_t& index)
{
	return match(JSON_left_curly_bracket, index);
}

VkBool32 JsonDecoder::decodeRightSquareBracket(size_t& index)
{
	return match(JSON_right_square_bracket, index);
}

VkBool32 JsonDecoder::decodeRightCurlyBracket(size_t& index)
{
	return match(JSON_right_curly_bracket, index);
}

VkBool32 JsonDecoder::decodeColon(size_t& index)
{
	return match(JSON_colon, index);
}

VkBool32 JsonDecoder::decodeComma(size_t& index)
{
	return match(JSON_comma, index);
}

//

VkBool32 JsonDecoder::decodePlus(size_t& index, std::string& characters)
{
	if (match(JSON_plus, index))
	{
		characters += JSON_plus;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeMinus(size_t& index, std::string& characters)
{
	if (match(JSON_minus, index))
	{
		characters += JSON_minus;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodePoint(size_t& index, std::string& characters)
{
	if (match(JSON_point, index))
	{
		characters += JSON_point;

		return VK_TRUE;
	}

	return VK_FALSE;
}

//

VkBool32 JsonDecoder::decodeHexadecimalDigit(size_t& index, std::string& characters, int32_t& value)
{
	if (match(JSON_A, index) || match(JSON_B, index) || match(JSON_C, index) || match(JSON_D, index) || match(JSON_E, index) || match(JSON_F, index))
	{
		characters += jsonText.substr(index - 1, 1);

		value = static_cast<int32_t>(jsonText.substr(index - 1, 1)[0] - JSON_A[0]);

		return VK_TRUE;
	}
	if (match(JSON_a, index) || match(JSON_b, index) || match(JSON_c, index) || match(JSON_d, index) || match(JSON_e, index) || match(JSON_f, index))
	{
		characters += jsonText.substr(index - 1, 1);

		value = static_cast<int32_t>(jsonText.substr(index - 1, 1)[0] - JSON_a[0]);

		return VK_TRUE;
	}

	return VK_FALSE;
}

//

VkBool32 JsonDecoder::decodeDigit_0(size_t& index, std::string& characters, int32_t& value)
{
	if (match(JSON_0, index))
	{
		characters += JSON_0;

		value = 0;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeDigit_1_9(size_t& index, std::string& characters, int32_t& value)
{
	if (match(JSON_1, index) || match(JSON_2, index) || match(JSON_3, index) || match(JSON_4, index) || match(JSON_5, index) || match(JSON_6, index) || match(JSON_7, index) || match(JSON_8, index) || match(JSON_9, index))
	{
		characters += jsonText.substr(index - 1, 1);

		value = atoi(jsonText.substr(index - 1, 1).c_str());

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeDigit(size_t& index, std::string& characters, int32_t& value)
{
	return decodeDigit_0(index, characters, value) || decodeDigit_1_9(index, characters, value);
}

VkBool32 JsonDecoder::decodeExponent(size_t& index, std::string& characters)
{
	if (match(JSON_e, index) || match(JSON_E, index))
	{
		characters += JSON_e;

		return VK_TRUE;
	}

	return VK_FALSE;
}

//

VkBool32 JsonDecoder::decodeQuotationMark(size_t& index, std::string& characters)
{
	if (match(JSON_quotation_mark, index))
	{
		characters += JSON_quotation_mark;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeReverseSolidus(size_t& index, std::string& characters)
{
	if (match(JSON_reverse_solidus, index))
	{
		// Do nothing.

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeSolidus(size_t& index, std::string& characters)
{
	if (match(JSON_solidus, index))
	{
		characters += JSON_solidus;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeBackspace(size_t& index, std::string& characters)
{
	if (match(JSON_Decode_backspace, index))
	{
		characters += JSON_Encode_backspace;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeFormFeed(size_t& index, std::string& characters)
{
	if (match(JSON_Decode_form_feed, index))
	{
		characters += JSON_Encode_form_feed;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeLineFeed(size_t& index, std::string& characters)
{
	if (match(JSON_Decode_line_feed, index))
	{
		characters += JSON_Encode_line_feed;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeCarriageReturn(size_t& index, std::string& characters)
{
	if (match(JSON_Decode_carriage_return, index))
	{
		characters += JSON_Encode_carriage_return;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeCharacterTabulation(size_t& index, std::string& characters)
{
	if (match(JSON_Decode_character_tabulation, index))
	{
		characters += JSON_Encode_character_tabulation;

		return VK_TRUE;
	}

	return VK_FALSE;
}

VkBool32 JsonDecoder::decodeHexadecimalNumber(size_t& index, std::string& characters)
{
	int32_t value = 0;
	int32_t tempValue = 0;

	size_t tempIndex = index;

	if (!match(JSON_u, tempIndex))
	{
		return VK_FALSE;
	}

	if (tempIndex + 4 > jsonText.length())
	{
		return VK_FALSE;
	}

	while (tempIndex < jsonText.length())
	{
		if (!decodeDigit(tempIndex, characters, tempValue) && !decodeHexadecimalDigit(tempIndex, characters, tempValue))
		{
			return VK_FALSE;
		}

		value = value * 16 + tempValue;
	}

	characters += static_cast<char>(value & 0xFF);

	index = tempIndex;

	return VK_TRUE;
}

//

VkBool32 JsonDecoder::decodeObject(size_t& index, JSONobjectSP& jsonObject)
{
	JSONobjectSP tempJsonObject = JSONobjectSP(new JSONobject());

	size_t tempIndex = index;

	VkBool32 loop = VK_TRUE;

	while (decodeWhitespace(tempIndex));

	if (!decodeLeftCurlyBracket(tempIndex))
	{
		return VK_FALSE;
	}

	while (decodeWhitespace(tempIndex));

	if (decodeRightCurlyBracket(tempIndex))
	{
		//
	}
	else
	{
		while (decodeWhitespace(tempIndex));

		while (loop)
		{
			JSONstringSP jsonString;
			JSONvalueSP jsonValue;

			loop = VK_FALSE;

			if (!decodeString(tempIndex, jsonString))
			{
				return VK_FALSE;
			}

			while (decodeWhitespace(tempIndex));

			if (!decodeColon(tempIndex))
			{
				return VK_FALSE;
			}

			while (decodeWhitespace(tempIndex));

			if (!decodeValue(tempIndex, jsonValue))
			{
				return VK_FALSE;
			}

			tempJsonObject->addKeyValue(jsonString, jsonValue);

			while (decodeWhitespace(tempIndex));

			if (decodeComma(tempIndex))
			{
				loop = VK_TRUE;
			}

			while (decodeWhitespace(tempIndex));
		}

		if (!decodeRightCurlyBracket(tempIndex))
		{
			return VK_FALSE;
		}
	}

	while (decodeWhitespace(tempIndex));

	jsonObject = tempJsonObject;

	index = tempIndex;

	return VK_TRUE;
}

VkBool32 JsonDecoder::decodeArray(size_t& index, JSONarraySP& jsonArray)
{
	JSONarraySP tempJsonArray = JSONarraySP(new JSONarray());

	size_t tempIndex = index;

	VkBool32 loop = VK_TRUE;

	while (decodeWhitespace(tempIndex));

	if (!decodeLeftSquareBracket(tempIndex))
	{
		return VK_FALSE;
	}

	while (decodeWhitespace(tempIndex));

	if (decodeRightSquareBracket(tempIndex))
	{
		//
	}
	else
	{
		while (decodeWhitespace(tempIndex));

		while (loop)
		{
			JSONvalueSP jsonValue;

			loop = VK_FALSE;

			if (!decodeValue(tempIndex, jsonValue))
			{
				return VK_FALSE;
			}

			tempJsonArray->addValue(jsonValue);

			while (decodeWhitespace(tempIndex));

			if (decodeComma(tempIndex))
			{
				loop = VK_TRUE;
			}

			while (decodeWhitespace(tempIndex));
		}

		if (!decodeRightSquareBracket(tempIndex))
		{
			return VK_FALSE;
		}
	}

	while (decodeWhitespace(tempIndex));

	jsonArray = tempJsonArray;

	index = tempIndex;

	return VK_TRUE;
}

VkBool32 JsonDecoder::decodeNumber(size_t& index, JSONnumberSP& jsonNumber)
{
	std::string tempString = "";
	int32_t dummy;

	size_t tempIndex = index;

	VkBool32 isFloat = VK_FALSE;

	while (decodeWhitespace(tempIndex));

	if (decodeMinus(tempIndex, tempString))
	{
		//
	}

	if (decodeDigit_0(tempIndex, tempString, dummy))
	{
		//
	}
	else if (decodeDigit_1_9(tempIndex, tempString, dummy))
	{
		VkBool32 loop = VK_TRUE;

		while (loop)
		{
			loop = VK_FALSE;

			loop = decodeDigit(tempIndex, tempString, dummy);
		}
	}
	else
	{
		return VK_FALSE;
	}

	if (decodePoint(tempIndex, tempString))
	{
		isFloat = VK_TRUE;

		VkBool32 loop = VK_TRUE;

		if (!decodeDigit(tempIndex, tempString, dummy))
		{
			return VK_FALSE;
		}

		while (loop)
		{
			loop = VK_FALSE;

			loop = decodeDigit(tempIndex, tempString, dummy);
		}
	}

	if (decodeExponent(tempIndex, tempString))
	{
		VkBool32 loop = VK_TRUE;

		if (decodePlus(tempIndex, tempString) || decodeMinus(tempIndex, tempString))
		{
			//
		}

		if (!decodeDigit(tempIndex, tempString, dummy))
		{
			return VK_FALSE;
		}

		while (loop)
		{
			loop = VK_FALSE;

			loop = decodeDigit(tempIndex, tempString, dummy);
		}
	}

	while (decodeWhitespace(tempIndex));

	if (isFloat)
	{
		jsonNumber = JSONfloatSP(new JSONfloat(tempString));
	}
	else
	{
		jsonNumber = JSONintegerSP(new JSONinteger(tempString));
	}

	index = tempIndex;

	return VK_TRUE;
}

VkBool32 JsonDecoder::decodeString(size_t& index, JSONstringSP& jsonString)
{
	std::string dummyString = "";
	std::string tempString = "";

	size_t tempIndex = index;

	VkBool32 loop = VK_TRUE;

	while (decodeWhitespace(tempIndex));

	if (!decodeQuotationMark(tempIndex, dummyString))
	{
		return VK_FALSE;
	}

	while (loop)
	{
		loop = VK_FALSE;

		if (decodeQuotationMark(tempIndex, dummyString))
		{
			break;
		}

		if (decodeReverseSolidus(tempIndex, tempString))
		{
			if (decodeQuotationMark(tempIndex, tempString) || decodeReverseSolidus(tempIndex, tempString) || decodeSolidus(tempIndex, tempString) || decodeBackspace(tempIndex, tempString) || decodeSolidus(tempIndex, tempString) || decodeFormFeed(tempIndex, tempString) || decodeLineFeed(tempIndex, tempString) || decodeCarriageReturn(tempIndex, tempString) || decodeCharacterTabulation(tempIndex, tempString) || decodeHexadecimalNumber(tempIndex, tempString))
			{
				loop = VK_TRUE;
			}
			else
			{
				return VK_FALSE;
			}
		}
		else if ((jsonText[tempIndex] >= JSON_C0_start && jsonText[tempIndex] <= JSON_C0_end) || (jsonText[tempIndex] >= JSON_C1_start && jsonText[tempIndex] <= JSON_C1_end))
		{
			return VK_FALSE;
		}
		else
		{
			tempString += jsonText.substr(tempIndex, 1);

			if (tempIndex + 1 < jsonText.length())
			{
				tempIndex++;

				loop = VK_TRUE;
			}
			else
			{
				return VK_FALSE;
			}
		}
	}

	while (decodeWhitespace(tempIndex));

	jsonString = JSONstringSP(new JSONstring(tempString));

	index = tempIndex;

	return VK_TRUE;
}


VkBool32 JsonDecoder::decodeTrue(size_t& index, JSONtrueSP& jsonTrue)
{
	VkBool32 result;

	while (decodeWhitespace(index));

	result = match(JSON_true, index);

	while (decodeWhitespace(index));

	if (result)
	{
		jsonTrue = JSONtrueSP(new JSONtrue());
	}

	return result;
}

VkBool32 JsonDecoder::decodeFalse(size_t& index, JSONfalseSP& jsonFalse)
{
	VkBool32 result;

	while (decodeWhitespace(index));

	result = match(JSON_false, index);

	while (decodeWhitespace(index));

	if (result)
	{
		jsonFalse = JSONfalseSP(new JSONfalse());
	}

	return result;
}

VkBool32 JsonDecoder::decodeNull(size_t& index, JSONnullSP& jsonNull)
{
	VkBool32 result;

	while (decodeWhitespace(index));

	result =  match(JSON_null, index);

	while (decodeWhitespace(index));

	if (result)
	{
		jsonNull = JSONnullSP(new JSONnull());
	}

	return result;
}

//

VkBool32 JsonDecoder::decodeValue(size_t& index, JSONvalueSP& jsonValue)
{
	JSONobjectSP jsonObject;
	JSONarraySP jsonArray;
	JSONnumberSP jsonNumber;
	JSONstringSP jsonString;
	JSONtrueSP jsonTrue;
	JSONfalseSP jsonFalse;
	JSONnullSP jsonNull;

	jsonValue = JSONvalueSP();

	while (decodeWhitespace(index));

	if (decodeObject(index, jsonObject))
	{
		jsonValue = jsonObject;
	}
	else if (decodeArray(index, jsonArray))
	{
		jsonValue = jsonArray;
	}
	else if (decodeNumber(index, jsonNumber))
	{
		jsonValue = jsonNumber;
	}
	else if (decodeString(index, jsonString))
	{
		jsonValue = jsonString;
	}
	else if (decodeTrue(index, jsonTrue))
	{
		jsonValue = jsonTrue;
	}
	else if (decodeFalse(index, jsonFalse))
	{
		jsonValue = jsonFalse;
	}
	else if (decodeNull(index, jsonNull))
	{
		jsonValue = jsonNull;
	}

	while (decodeWhitespace(index));

	return jsonValue.get() != nullptr;
}

//

JSONvalueSP JsonDecoder::decode(const std::string& jsonText)
{
	this->jsonText = jsonText;

	size_t index = 0;
	JSONvalueSP jsonValue;

	if (!decodeValue(index, jsonValue))
	{
		return JSONvalueSP();
	}

	return jsonValue;
}

}
