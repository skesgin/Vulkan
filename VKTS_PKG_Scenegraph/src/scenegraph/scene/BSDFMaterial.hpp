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

#ifndef VKTS_BSDFMATERIAL_HPP_
#define VKTS_BSDFMATERIAL_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

#include "Material.hpp"

namespace vkts
{

class BSDFMaterial : public IBSDFMaterial, public Material
{

protected:

    IShaderModuleSP fragmentShader;

    VkTsVertexBufferType attributes;

    SmartPointerVector<ITextureObjectSP> allTextureObjects;
    std::vector<int32_t> texCoordIndices;

    VkBool32 transparent;

    VkBool32 sorted;

    VkBool32 packed;

    float alphaCutoff;

    float ambientOcclusionStrength;

    VkBool32 specularGlossiness;

public:

    BSDFMaterial() = delete;
    explicit BSDFMaterial(const VkBool32 forwardRendering);
    BSDFMaterial(const BSDFMaterial& other);
    BSDFMaterial(BSDFMaterial&& other) = delete;
    virtual ~BSDFMaterial();

    BSDFMaterial& operator =(const BSDFMaterial& other) = delete;
    BSDFMaterial& operator =(BSDFMaterial && other) = delete;

    //
    // IBSDFMaterial
    //

    virtual VkBool32 getForwardRendering() const override;

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;


    virtual IRenderMaterialSP getRenderMaterial(const uint32_t index) const override;

    virtual VkDeviceSize getRenderMaterialSize() const override;

    virtual void addRenderMaterial(const IRenderMaterialSP& materialData) override;


    virtual IShaderModuleSP getFragmentShader() const override;

	virtual void setFragmentShader(const IShaderModuleSP& fragmentShader) override;

    virtual VkTsVertexBufferType getAttributes() const override;

	virtual void setAttributes(const VkTsVertexBufferType attributes) override;

    virtual void addTextureObject(const ITextureObjectSP& textureObject) override;

    virtual VkBool32 removeTextureObject(const ITextureObjectSP& textureObject) override;

    virtual uint32_t getNumberTextureObjects() const override;

    virtual const SmartPointerVector<ITextureObjectSP>& getTextureObjects() const override;


    virtual VkBool32 isTransparent() const override;

    virtual void setTransparent(const VkBool32 transparent) override;


    virtual VkBool32 isSorted() const override;

    virtual void setSorted(const VkBool32 sorted) override;


    virtual VkBool32 isPacked() const override;

    virtual void setPacked(const VkBool32 packed) override;


    virtual float getAlphaCutoff() const override;

    virtual void setAlphaCutoff(const float alphaCutoff) override;


    virtual float getAmbientOcclusionStrength() const override;

    virtual void setAmbientOcclusionStrength(const float ambientOcclusionStrength) override;


    virtual VkBool32 isSpecularGlossiness() const override;

    virtual void setSpecularGlossiness(const VkBool32 specularGlossiness) override;


    virtual int32_t getTexCoordIndex(const int32_t textureIndex) const override;

    virtual void setTexCoordIndex(const int32_t textureIndex, const int32_t texCoordIndex) override;

    //

    virtual void updateParameterRecursive(Parameter* parameter) override;

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer, const std::string& nodeName) override;

    //

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite, const std::string& nodeName) override;

    //
    // ICloneable
    //

    virtual IBSDFMaterialSP clone() const override;

    //
    // IDestroyable
    //

    virtual void destroy() override;
};

} /* namespace vkts */

#endif /* VKTS_BSDFMATERIAL_HPP_ */
