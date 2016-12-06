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

#ifndef VKTS_PHONGMATERIAL_HPP_
#define VKTS_PHONGMATERIAL_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

#include "Material.hpp"

namespace vkts
{

class PhongMaterial: public IPhongMaterial, public Material
{

protected:

    ITextureObjectSP alpha;

    ITextureObjectSP displacement;

    ITextureObjectSP normal;

    ITextureObjectSP ambient;

    ITextureObjectSP emissive;

    ITextureObjectSP diffuse;

    ITextureObjectSP specular;

    ITextureObjectSP specularShininess;

    ITextureObjectSP mirror;

    ITextureObjectSP mirrorReflectivity;

    VkBool32 transparent;

public:

    PhongMaterial(const VkBool32 forwardRendering);
    PhongMaterial(const PhongMaterial& other);
    PhongMaterial(PhongMaterial&& other) = delete;
    virtual ~PhongMaterial();

    PhongMaterial& operator =(const PhongMaterial& other) = delete;

    PhongMaterial& operator =(PhongMaterial && other) = delete;


    //
    // IPhongMaterial
    //

    virtual VkBool32 getForwardRendering() const override;

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;


    virtual IRenderMaterialSP getRenderMaterial() const override;

    virtual void setRenderMaterial(const IRenderMaterialSP& materialData) override;


    virtual const ITextureObjectSP& getAlpha() const override;

    virtual void setAlpha(const ITextureObjectSP& alpha) override;

    virtual const ITextureObjectSP& getDisplacement() const override;

    virtual void setDisplacement(const ITextureObjectSP& displacement) override;

    virtual const ITextureObjectSP& getNormal() const override;

    virtual void setNormal(const ITextureObjectSP& normal) override;

    //

    virtual const ITextureObjectSP& getEmissive() const override;

    virtual void setEmissive(const ITextureObjectSP& emissive) override;

    virtual const ITextureObjectSP& getAmbient() const override;

    virtual void setAmbient(const ITextureObjectSP& ambient) override;

    virtual const ITextureObjectSP& getDiffuse() const override;

    virtual void setDiffuse(const ITextureObjectSP& diffuse) override;

    virtual const ITextureObjectSP& getSpecular() const override;

    virtual void setSpecular(const ITextureObjectSP& specular) override;

    virtual const ITextureObjectSP& getSpecularShininess() const override;

    virtual void setSpecularShininess(const ITextureObjectSP& specularShininess) override;

    virtual const ITextureObjectSP& getMirror() const override;

    virtual void setMirror(const ITextureObjectSP& mirror) override;

    virtual const ITextureObjectSP& getMirrorReflectivity() const override;

    virtual void setMirrorReflectivity(const ITextureObjectSP& mirrorReflectivity) override;

    //

    virtual VkBool32 isTransparent() const override;

    virtual void setTransparent(const VkBool32 transparent) override;

    //

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const std::string& nodeName) override;

    //

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const OverwriteDraw* renderOverwrite, const uint32_t bufferIndex, const std::string& nodeName) override;

    //
    // ICloneable
    //

    virtual IPhongMaterialSP clone() const override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_PHONGMATERIAL_HPP_ */
