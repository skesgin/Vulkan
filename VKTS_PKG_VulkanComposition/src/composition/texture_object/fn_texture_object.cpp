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

#include <vkts/vulkan/composition/vkts_composition.hpp>
#include "TextureObject.hpp"

namespace vkts
{

ITextureObjectSP VKTS_APIENTRY textureObjectCreate(const IContextObjectSP& contextObject, const std::string& name, const VkBool32 mipmap, const IImageObjectSP& imageObject, const VkSamplerCreateInfo& samplerCreateInfo)
{
    if (!imageObject.get())
    {
        return ITextureObjectSP();
    }

    //

    VkSamplerCreateInfo finalSamplerCreateInfo;

    memcpy(&finalSamplerCreateInfo, &samplerCreateInfo, sizeof(VkImageViewCreateInfo));

    finalSamplerCreateInfo.maxLod = mipmap ? (float) imageObject->getImageData()->getMipLevels() : 0.0f;

    auto sampler = samplerCreate(contextObject->getDevice()->getDevice(), finalSamplerCreateInfo.flags, finalSamplerCreateInfo.magFilter, finalSamplerCreateInfo.minFilter, finalSamplerCreateInfo.mipmapMode, samplerCreateInfo.addressModeU, samplerCreateInfo.addressModeV, finalSamplerCreateInfo.addressModeW, finalSamplerCreateInfo.mipLodBias, finalSamplerCreateInfo.anisotropyEnable, finalSamplerCreateInfo.maxAnisotropy, finalSamplerCreateInfo.compareEnable, finalSamplerCreateInfo.compareOp, finalSamplerCreateInfo.minLod, finalSamplerCreateInfo.maxLod, finalSamplerCreateInfo.borderColor, finalSamplerCreateInfo.unnormalizedCoordinates);

    if (!sampler.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create sampler.");

        return ITextureObjectSP();
    }

    //

    auto newInstance = new TextureObject(contextObject, name, imageObject, sampler);

    if (!newInstance)
    {
        newInstance->destroy();

        return ITextureObjectSP();
    }

    return ITextureObjectSP(newInstance);
}

}
