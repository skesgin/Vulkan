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

#include <vkts/vkts.hpp>

namespace vkts
{

glm::vec3 VKTS_APIENTRY renderGetMicrofacetWeightedVector(const glm::vec2& e, const float roughness)
{
	float alpha = roughness * roughness;

	// Note: Polar Coordinates
	// x = sin(theta)*cos(phi)
	// y = sin(theta)*sin(phi)
	// z = cos(theta)

	float phi = 2.0f * VKTS_PI * e.y;
	float cosTheta = sqrtf((1.0f - e.x) / (1.0f + (alpha*alpha - 1.0f) * e.x));
	float sinTheta = sqrtf(1.0f - cosTheta*cosTheta);

	float x = sinTheta * cosf(phi);
	float y = sinTheta * sinf(phi);
	float z = cosTheta;

	return glm::vec3(x, y, z);
}

glm::vec3 VKTS_APIENTRY renderGetCosineWeightedVector(const glm::vec2& e)
{
	float x = sqrtf(1.0f - e.x) * cosf(2.0f * VKTS_PI * e.y);
	float y = sqrtf(1.0f - e.x) * sinf(2.0f * VKTS_PI * e.y);
	float z = sqrtf(e.x);

	return glm::vec3(x, y, z);
}

glm::vec3 VKTS_APIENTRY renderLightCookTorrance(const IImageDataSP& cubeMap, const VkFilter filter, const uint32_t mipLevel, const glm::vec2& randomPoint, const glm::mat3& basis, const glm::vec3& N, const glm::vec3& V, const float roughness)
{
	if (!cubeMap.get())
	{
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	glm::vec3 HtangentSpace = renderGetMicrofacetWeightedVector(randomPoint, roughness);

	// Transform H to world space.
	glm::vec3 H = basis * HtangentSpace;

	// Note: reflect takes incident vector.
	glm::vec3 L = glm::reflect(-V, H);

	float NdotL = glm::dot(N, L);
	float NdotV = glm::dot(N, V);

	// Lighted and visible
	if (NdotL > 0.0f && NdotV > 0.0f)
	{
		return glm::vec3(cubeMap->getSampleCubeMap(L.x, L.y, L.z, filter, mipLevel));
	}

	return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 VKTS_APIENTRY renderLightLambert(const IImageDataSP& cubeMap, const VkFilter filter, const uint32_t mipLevel, const glm::vec2& randomPoint, const glm::mat3& basis)
{
	if (!cubeMap.get())
	{
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	glm::vec3 LtangentSpace = renderGetCosineWeightedVector(randomPoint);

	// Transform light ray to world space.
	glm::vec3 L = basis * LtangentSpace;

	return glm::vec3(cubeMap->getSampleCubeMap(L.x, L.y, L.z, filter, mipLevel));
}

}
