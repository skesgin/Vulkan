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

#ifndef VKTS_IPHONGMATERIAL_HPP_
#define VKTS_IPHONGMATERIAL_HPP_

#include <vkts/scenegraph.hpp>

namespace vkts
{

class Overwrite;

class IPhongMaterial : public ICloneable<IPhongMaterial>, public IDestroyable
{

public:

    IPhongMaterial() :
        ICloneable<IPhongMaterial>(), IDestroyable()
    {
    }

    virtual ~IPhongMaterial()
    {
    }

    virtual VkBool32 getForwardRendering() const = 0;

    virtual const std::string& getName() const = 0;

    virtual void setName(const std::string& name) = 0;

    //

    virtual const ITextureObjectSP& getAlpha() const = 0;

    virtual void setAlpha(const ITextureObjectSP& alpha) = 0;

    virtual const ITextureObjectSP& getDisplacement() const = 0;

    virtual void setDisplacement(const ITextureObjectSP& displacement) = 0;

    virtual const ITextureObjectSP& getNormal() const = 0;

    virtual void setNormal(const ITextureObjectSP& normal) = 0;

    //

    virtual const ITextureObjectSP& getAmbient() const = 0;

    virtual void setAmbient(const ITextureObjectSP& ambient) = 0;

    virtual const ITextureObjectSP& getEmissive() const = 0;

    virtual void setEmissive(const ITextureObjectSP& emissive) = 0;

    virtual const ITextureObjectSP& getDiffuse() const = 0;

    virtual void setDiffuse(const ITextureObjectSP& diffuse) = 0;

    virtual const ITextureObjectSP& getSpecular() const = 0;

    virtual void setSpecular(const ITextureObjectSP& specular) = 0;

    virtual const ITextureObjectSP& getSpecularShininess() const = 0;

    virtual void setSpecularShininess(const ITextureObjectSP& specularShininess) = 0;

    virtual const ITextureObjectSP& getMirror() const = 0;

    virtual void setMirror(const ITextureObjectSP& mirror) = 0;

    virtual const ITextureObjectSP& getMirrorReflectivity() const = 0;

    virtual void setMirrorReflectivity(const ITextureObjectSP& mirrorReflectivity) = 0;

    //

    virtual VkBool32 isTransparent() const = 0;

    virtual void setTransparent(const VkBool32 transparent) = 0;

    //

    virtual const IDescriptorPoolSP& getDescriptorPool() const = 0;

    virtual void setDescriptorPool(const IDescriptorPoolSP& descriptorPool) = 0;

    virtual IDescriptorSetsSP getDescriptorSets() const = 0;

    virtual void setDescriptorSets(const IDescriptorSetsSP& descriptorSets) = 0;

    virtual void updateDescriptorSetsRecursive(const std::string& nodeName, const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets) = 0;

    virtual void bindDescriptorSets(const std::string& nodeName, const ICommandBuffersSP& cmdBuffer, const VkPipelineLayout layout, const uint32_t bufferIndex = 0) const = 0;

    virtual void bindDrawIndexedRecursive(const std::string& nodeName, const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const Overwrite* renderOverwrite = nullptr, const uint32_t bufferIndex = 0) const = 0;

};

typedef std::shared_ptr<IPhongMaterial> IPhongMaterialSP;

} /* namespace vkts */

#endif /* VKTS_IPHONGMATERIAL_HPP_ */
