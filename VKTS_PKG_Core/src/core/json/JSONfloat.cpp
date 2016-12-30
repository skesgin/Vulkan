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

JSONfloat::JSONfloat() :
	JSONnumber(), floatValue(0.0f)
{
}

JSONfloat::JSONfloat(const std::string& value) :
	JSONnumber()
{
	this->floatValue = static_cast<float>(atof(value.c_str()));
}

JSONfloat::JSONfloat(float value) :
	JSONnumber()
{
	this->floatValue = value;
}

JSONfloat::~JSONfloat()
{
}

float JSONfloat::getValue() const
{
	return floatValue;
}

void JSONfloat::setValue(float value)
{
	this->floatValue = value;
}

VkBool32 JSONfloat::encode(std::string& jsonText, int32_t& spaces) const
{
	char buffer[2048];

	float dummy;
	float fractpart = fabsf(modff(getValue(), &dummy));

	if (fabsf(getValue()) >= 1000000.0f || (fractpart <= 0.0000001f && fractpart > 0.0f))
	{
		sprintf(buffer, "%e", getValue());
	}
	else
	{
		sprintf(buffer, "%.6f", getValue());
	}

	jsonText += std::string(buffer);

	return VK_TRUE;
}

void JSONfloat::visit(JsonVisitor& jsonVisitor)
{
	jsonVisitor.visit(*this);
}

}
