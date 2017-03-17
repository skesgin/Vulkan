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

static const std::string base64Characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::vector<uint8_t> VKTS_APIENTRY base64Decode(const std::string& encoded)
{
	std::vector<uint8_t> decoded;

	//
	size_t encodedIndex = 0;

	while (encodedIndex < encoded.size())
	{
		uint8_t out[3] = {0, 0, 0};

		//

		size_t gathered = 0;

		for (gathered = 0; gathered < 4; gathered++)
		{
			// Check, if out of bounds. If yes, invalid encoded.
			if (encodedIndex >= encoded.size())
			{
				return std::vector<uint8_t>();
			}

			size_t index = base64Characters.find(encoded[encodedIndex]);

			// Check, if BASE64 character. If not, invalid encoded.
			if (index == base64Characters.npos && encoded[encodedIndex] != '=')
			{
				return std::vector<uint8_t>();
			}

			if (encoded[encodedIndex] == '=')
			{
				break;
			}

			uint8_t in = (uint8_t)index;

			if (gathered == 0)
			{
				out[0] = in << 2;
			}
			else if (gathered == 1)
			{
				out[0] |= in >> 4;

				out[1] = (in << 4) & 0xF0;
			}
			else if (gathered == 2)
			{
				out[1] |= in >> 2;

				out[2] = (in << 6) & 0xC0;
			}
			else if (gathered == 3)
			{
				out[2] |= in;
			}

			encodedIndex++;
		}

		//

		for (size_t i = 0; i < 3; i++)
		{
			if (gathered != 4 && i == 2)
			{
				return decoded;
			}

			decoded.push_back(out[i]);
		}
	}

	//

	return decoded;
}

std::string VKTS_APIENTRY base64Encode(const std::vector<uint8_t> data)
{
	std::string encoded = "";

	//

	size_t dataIndex = 0;

	while (dataIndex < data.size())
	{
		uint8_t in[3] = {0, 0, 0};

		//

		size_t gathered = 0;

		for (gathered = 0; gathered < 3; gathered++)
		{
			if (dataIndex == data.size())
			{
				break;
			}

			in[gathered] = data[dataIndex];

			//

			dataIndex++;
		}

		//

		uint8_t out[4] = {0, 0, 0, 0};

		out[0] = (in[0] >> 2) & 0x3F;
		out[1] = ((in[0] << 4) & 0x30) | ((in[1] >> 4) & 0x0F);

		if (gathered >= 2)
		{
			out[2] = (in[1] << 2) & 0x3C;

			if (gathered == 3)
			{
				out[2] |= (in[2] >> 6) & 0x03;
				out[3] = in[2] & 0x3F;
			}
		}

		//

		for (uint32_t i = 0; i < 4; i++)
		{
			if (gathered >= i)
			{
				encoded.push_back(base64Characters[out[i]]);
			}
			else
			{
				encoded.push_back('=');
			}
		}
	}

	return encoded;
}

}
