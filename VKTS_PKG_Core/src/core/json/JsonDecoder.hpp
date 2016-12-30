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

#ifndef VKTS_JSONDECODER_HPP_
#define VKTS_JSONDECODER_HPP_

#include <vkts/core/vkts_core.hpp>

// see http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf

namespace vkts
{

class JsonDecoder
{

private:

	std::string jsonText;

	//

	VkBool32 match(const std::string& token, size_t& index);

	//

	VkBool32 decodeWhitespace(size_t& index);

	//

	VkBool32 decodeLeftSquareBracket(size_t& index);
	VkBool32 decodeLeftCurlyBracket(size_t& index);
	VkBool32 decodeRightSquareBracket(size_t& index);
	VkBool32 decodeRightCurlyBracket(size_t& index);
	VkBool32 decodeColon(size_t& index);
	VkBool32 decodeComma(size_t& index);

	//

	VkBool32 decodePlus(size_t& index, std::string& characters);
	VkBool32 decodeMinus(size_t& index, std::string& characters);
	VkBool32 decodePoint(size_t& index, std::string& characters);

	VkBool32 decodeHexadecimalDigit(size_t& index, std::string& characters, std::int32_t& value);

	VkBool32 decodeDigit_0(size_t& index, std::string& characters, std::int32_t& value);
	VkBool32 decodeDigit_1_9(size_t& index, std::string& characters, std::int32_t& value);
	VkBool32 decodeDigit(size_t& index, std::string& characters, std::int32_t& value);

	VkBool32 decodeExponent(size_t& index, std::string& characters);

	VkBool32 decodeQuotationMark(size_t& index, std::string& characters);
	VkBool32 decodeReverseSolidus(size_t& index, std::string& characters);
	VkBool32 decodeSolidus(size_t& index, std::string& characters);
	VkBool32 decodeBackspace(size_t& index, std::string& characters);
	VkBool32 decodeFormFeed(size_t& index, std::string& characters);
	VkBool32 decodeLineFeed(size_t& index, std::string& characters);
	VkBool32 decodeCarriageReturn(size_t& index, std::string& characters);
	VkBool32 decodeCharacterTabulation(size_t& index, std::string& characters);
	VkBool32 decodeHexadecimalNumber(size_t& index, std::string& characters);

	//

	VkBool32 decodeObject(size_t& index, JSONobjectSP& jsonObject);
	VkBool32 decodeArray(size_t& index, JSONarraySP& jsonArray);
	VkBool32 decodeNumber(size_t& index, JSONnumberSP& jsonNumber);
	VkBool32 decodeString(size_t& index, JSONstringSP& jsonString);
	VkBool32 decodeTrue(size_t& index, JSONtrueSP& jsonTrue);
	VkBool32 decodeFalse(size_t& index, JSONfalseSP& jsonFalse);
	VkBool32 decodeNull(size_t& index, JSONnullSP& jsonNull);

	//

	VkBool32 decodeValue(size_t& index, JSONvalueSP& jsonValue);

public:

	JsonDecoder();
	~JsonDecoder();

	JSONvalueSP decode(const std::string& jsonText);

};

}

#endif /* VKTS_JSONDECODER_HPP_ */
