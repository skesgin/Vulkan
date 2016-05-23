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

#include "BSDFMaterial.hpp"

namespace vkts
{

BSDFMaterial::BSDFMaterial() :
    IBSDFMaterial(), name(), fragmentShader(nullptr), attributes(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_NORMAL), allTextures(), descriptorPool(), descriptorSetLayout(), descriptorSets()
{
}

BSDFMaterial::BSDFMaterial(const BSDFMaterial& other) :
    IBSDFMaterial(), name(other.name), fragmentShader(other.fragmentShader), attributes(other.attributes), allTextures(other.allTextures), descriptorPool(), descriptorSetLayout(), descriptorSets()
{
	// TODO: Clone, as done in phong material.
}

BSDFMaterial::~BSDFMaterial()
{
    destroy();
}

//
// IBSDFMaterial
//

const std::string& BSDFMaterial::getName() const
{
    return name;
}

void BSDFMaterial::setName(const std::string& name)
{
    this->name = name;
}

IShaderModuleSP BSDFMaterial::getFragmentShader() const
{
	return fragmentShader;
}

void BSDFMaterial::setFragmentShader(const IShaderModuleSP& fragmentShader)
{
	this->fragmentShader = fragmentShader;
}


VkTsVertexBufferType BSDFMaterial::getAttributes() const
{
	return attributes;
}

void BSDFMaterial::setAttributes(const VkTsVertexBufferType attributes)
{
	this->attributes = attributes;
}

void BSDFMaterial::addTexture(const ITextureSP& texture)
{
    allTextures.append(texture);
}

VkBool32 BSDFMaterial::removeTexture(const ITextureSP& texture)
{
    return allTextures.remove(texture);
}

size_t BSDFMaterial::getNumberTextures() const
{
    return allTextures.size();
}

const SmartPointerVector<ITextureSP>& BSDFMaterial::getTextures() const
{
    return allTextures;
}

const IDescriptorPoolSP& BSDFMaterial::getDescriptorPool() const
{
    return descriptorPool;
}

void BSDFMaterial::setDescriptorPool(const IDescriptorPoolSP& descriptorPool)
{
    this->descriptorPool = descriptorPool;
}

IDescriptorSetLayoutSP BSDFMaterial::getDescriptorSetLayout() const
{
	return descriptorSetLayout;
}

void BSDFMaterial::setDescriptorSetLayout(const IDescriptorSetLayoutSP& descriptorSetLayout)
{
    this->descriptorSetLayout = descriptorSetLayout;
}

IDescriptorSetsSP BSDFMaterial::getDescriptorSets() const
{
    return descriptorSets;
}

void BSDFMaterial::setDescriptorSets(const IDescriptorSetsSP& descriptorSets)
{
    this->descriptorSets = descriptorSets;
}

void BSDFMaterial::updateDescriptorSetsRecursive(const std::string& nodeName, const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets)
{
    // TODO: Implement.
}

void BSDFMaterial::bindDrawIndexedRecursive(const std::string& nodeName, const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const overwrite* renderOverwrite, const uint32_t bufferIndex) const
{
	// TODO: Bind graphics pipeline.

    // TODO: Implement, as done in phong.
}

//
// ICloneable
//

IBSDFMaterialSP BSDFMaterial::clone() const
{
    return IBSDFMaterialSP(new BSDFMaterial(*this));
}

//
// IDestroyable
//

void BSDFMaterial::destroy()
{
	// TODO: Destroy members.
}

} /* namespace vkts */
