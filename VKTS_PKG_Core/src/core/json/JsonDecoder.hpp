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

	VkBool32 match(const std::string& token, uint32_t& index);

	//

	VkBool32 decodeWhitespace(uint32_t& index);

	//

	VkBool32 decodeLeftSquareBracket(uint32_t& index);
	VkBool32 decodeLeftCurlyBracket(uint32_t& index);
	VkBool32 decodeRightSquareBracket(uint32_t& index);
	VkBool32 decodeRightCurlyBracket(uint32_t& index);
	VkBool32 decodeColon(uint32_t& index);
	VkBool32 decodeComma(uint32_t& index);

	//

	VkBool32 decodePlus(uint32_t& index, std::string& characters);
	VkBool32 decodeMinus(uint32_t& index, std::string& characters);
	VkBool32 decodePoint(uint32_t& index, std::string& characters);

	VkBool32 decodeHexadecimalDigit(uint32_t& index, std::string& characters, std::int32_t& value);

	VkBool32 decodeDigit_0(uint32_t& index, std::string& characters, std::int32_t& value);
	VkBool32 decodeDigit_1_9(uint32_t& index, std::string& characters, std::int32_t& value);
	VkBool32 decodeDigit(uint32_t& index, std::string& characters, std::int32_t& value);

	VkBool32 decodeExponent(uint32_t& index, std::string& characters);

	VkBool32 decodeQuotationMark(uint32_t& index, std::string& characters);
	VkBool32 decodeReverseSolidus(uint32_t& index, std::string& characters);
	VkBool32 decodeSolidus(uint32_t& index, std::string& characters);
	VkBool32 decodeBackspace(uint32_t& index, std::string& characters);
	VkBool32 decodeFormFeed(uint32_t& index, std::string& characters);
	VkBool32 decodeLineFeed(uint32_t& index, std::string& characters);
	VkBool32 decodeCarriageReturn(uint32_t& index, std::string& characters);
	VkBool32 decodeCharacterTabulation(uint32_t& index, std::string& characters);
	VkBool32 decodeHexadecimalNumber(uint32_t& index, std::string& characters);

	//

	VkBool32 decodeObject(uint32_t& index, JSONobjectSP& jsonObject);
	VkBool32 decodeArray(uint32_t& index, JSONarraySP& jsonArray);
	VkBool32 decodeNumber(uint32_t& index, JSONnumberSP& jsonNumber);
	VkBool32 decodeString(uint32_t& index, JSONstringSP& jsonString);
	VkBool32 decodeTrue(uint32_t& index, JSONtrueSP& jsonTrue);
	VkBool32 decodeFalse(uint32_t& index, JSONfalseSP& jsonFalse);
	VkBool32 decodeNull(uint32_t& index, JSONnullSP& jsonNull);

	//

	VkBool32 decodeValue(uint32_t& index, JSONvalueSP& jsonValue);

public:

	JsonDecoder();
	~JsonDecoder();

	JSONvalueSP decode(const std::string& jsonText);

};

}

#endif /* VKTS_JSONDECODER_HPP_ */
