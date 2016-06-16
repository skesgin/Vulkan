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

#ifndef VKTS_FN_RENDER_HPP_
#define VKTS_FN_RENDER_HPP_

#include <vkts/vkts.hpp>

namespace vkts
{

VKTS_APICALL float VKTS_APIENTRY renderGetGeometricShadowingSchlickBeckmann(float NdotV, float k);

VKTS_APICALL float VKTS_APIENTRY renderGetGeometricShadowingSmith(float NdotL, float NdotV, float k);

VKTS_APICALL glm::vec3 VKTS_APIENTRY renderGetMicrofacetWeightedVector(const glm::vec2& e, const float roughness);

VKTS_APICALL glm::vec3 VKTS_APIENTRY renderGetCosineWeightedVector(const glm::vec2& e);

VKTS_APICALL glm::vec3 VKTS_APIENTRY renderLightCookTorrance(const IImageDataSP& cubeMap, const VkFilter filter, const uint32_t mipLevel, const glm::vec2& randomPoint, const glm::mat3& basis, const glm::vec3& N, const glm::vec3& V, const float roughness);

VKTS_APICALL glm::vec3 VKTS_APIENTRY renderLightLambert(const IImageDataSP& cubeMap, const VkFilter filter, const uint32_t mipLevel, const glm::vec2& randomPoint, const glm::mat3& basis);

VKTS_APICALL glm::vec2 VKTS_APIENTRY renderIntegrateCookTorrance(const glm::vec2& randomPoint, glm::vec3& N, glm::vec3& V, const float k, const float roughness);

}

#endif /* VKTS_FN_RENDER_HPP_ */
