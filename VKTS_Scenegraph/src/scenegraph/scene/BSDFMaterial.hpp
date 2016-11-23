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

#include <vkts/scenegraph.hpp>

#include "Material.hpp"

namespace vkts
{

class SceneVisitor;

class BSDFMaterial : public IBSDFMaterial, Material
{

friend class Draw;
friend class Parameter;
friend class UpdateDescriptorSets;

protected:

	const VkBool32 forwardRendering;

    std::string name;

    IShaderModuleSP fragmentShader;

    VkTsVertexBufferType attributes;

    SmartPointerVector<ITextureObjectSP> allTextureObjects;

public:

    BSDFMaterial(const VkBool32 forwardRendering);
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

    virtual IShaderModuleSP getFragmentShader() const override;

	virtual void setFragmentShader(const IShaderModuleSP& fragmentShader) override;

    virtual VkTsVertexBufferType getAttributes() const override;

	virtual void setAttributes(const VkTsVertexBufferType attributes) override;

    virtual void addTextureObject(const ITextureObjectSP& textureObject) override;

    virtual VkBool32 removeTextureObject(const ITextureObjectSP& textureObject) override;

    virtual size_t getNumberTextureObjects() const override;

    virtual const SmartPointerVector<ITextureObjectSP>& getTextureObjects() const override;

    virtual const IDescriptorPoolSP& getDescriptorPool() const override;

    virtual void setDescriptorPool(const IDescriptorPoolSP& descriptorPool) override;

    virtual IDescriptorSetsSP getDescriptorSets() const override;

    virtual void setDescriptorSets(const IDescriptorSetsSP& descriptorSets) override;

    //

    virtual void visitRecursive(SceneVisitor* sceneVisitor) override;

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
