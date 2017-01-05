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

#include <vkts/math/vkts_math.hpp>

#define VKTS_UNIFORM_RANDOM_BIAS 0.0000001f

namespace vkts
{

void VKTS_APIENTRY randomSetSeed(const uint32_t seed)
{
	srand(seed);
}

float VKTS_APIENTRY randomUniform(const float start, const float end)
{
	return ((float)rand() / (float)(RAND_MAX - 1)) * (end - start) + start;
}

float VKTS_APIENTRY randomNormal(const float mean, const float standardDeviation)
{
	float x1, x2;

	// Avoid logf(0.0f) and logf(1.0f)
	x1 = randomUniform(VKTS_UNIFORM_RANDOM_BIAS, 1.0f - VKTS_UNIFORM_RANDOM_BIAS);
	x2 = randomUniform(0.0f, 1.0f);

	return mean + standardDeviation * (sqrtf(-2.0f * logf(x1)) * cosf(2.0f * VKTS_MATH_PI * x2));
}

// see http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
glm::vec2 VKTS_APIENTRY randomHammersley(const uint32_t i, const uint32_t n)
{
	if (n == 0)
	{
		throw std::out_of_range("n = 0");
	}

	if (i >= n)
	{
		throw std::out_of_range("i >= n");
	}

    uint32_t bits = i;

    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    return glm::vec2((float)i / (float)n, (float)bits * 2.3283064365386963e-10);
}

}
