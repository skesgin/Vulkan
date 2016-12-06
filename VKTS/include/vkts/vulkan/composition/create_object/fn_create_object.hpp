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

#ifndef VKTS_FN_CREATE_OBJECT_HPP_
#define VKTS_FN_CREATE_OBJECT_HPP_

#include <vkts/vulkan/composition/vkts_composition.hpp>

namespace vkts
{

VKTS_APICALL IImageDataSP VKTS_APIENTRY createDeviceImageData(const IAssetManagerSP& assetManager, IImageDataSP& imageData);

VKTS_APICALL IImageObjectSP VKTS_APIENTRY createImageObject(const IAssetManagerSP& assetManager, const std::string& imageObjectName, const IImageDataSP& imageData, const VkBool32 environment);

VKTS_APICALL ITextureObjectSP VKTS_APIENTRY createTextureObject(const IAssetManagerSP& assetManager, const std::string& textureObjectName, const VkBool32 mipmap, const VkFilter filter, const VkSamplerAddressMode samplerAddressMode, const IImageObjectSP& imageObject);

VKTS_APICALL ITextureObjectSP VKTS_APIENTRY createTextureObject(const IAssetManagerSP& assetManager, const glm::vec4& color, const VkFormat format);

VKTS_APICALL IBufferObjectSP VKTS_APIENTRY createUniformBufferObject(const IAssetManagerSP& assetManager, const VkDeviceSize size);

VKTS_APICALL IBufferObjectSP VKTS_APIENTRY createIndexBufferObject(const IAssetManagerSP& assetManager, const IBinaryBufferSP& binaryBuffer);

VKTS_APICALL IBufferObjectSP VKTS_APIENTRY createVertexBufferObject(const IAssetManagerSP& assetManager, const IBinaryBufferSP& binaryBuffer);

VKTS_APICALL IShaderModuleSP VKTS_APIENTRY createShaderModule(const IAssetManagerSP& assetManager, const std::string& shaderModuleName, const IBinaryBufferSP& binaryBuffer);

}

#endif /* VKTS_FN_CREATE_OBJECT_HPP_ */
