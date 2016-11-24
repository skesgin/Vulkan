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
#include "../visitor/SceneVisitor.hpp"

namespace vkts
{

BSDFMaterial::BSDFMaterial(const VkBool32 forwardRendering) :
    IBSDFMaterial(), Material(), forwardRendering(forwardRendering), name(), fragmentShader(nullptr), attributes(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_NORMAL), allTextureObjects()
{
}

BSDFMaterial::BSDFMaterial(const BSDFMaterial& other) :
    IBSDFMaterial(), Material(other), forwardRendering(other.forwardRendering), name(other.name + "_clone"), fragmentShader(other.fragmentShader), attributes(other.attributes), allTextureObjects()
{
	for (size_t i = 0; i < other.allTextureObjects.size(); i++)
	{
		addTextureObject(other.allTextureObjects[i]);
	}
}

BSDFMaterial::~BSDFMaterial()
{
    destroy();
}

//
// IBSDFMaterial
//

VkBool32 BSDFMaterial::getForwardRendering() const
{
    return forwardRendering;
}

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

void BSDFMaterial::addTextureObject(const ITextureObjectSP& textureObject)
{
    if (textureObject.get())
    {
    	uint32_t offset = forwardRendering ? VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_FIRST : VKTS_BINDING_UNIFORM_SAMPLER_BSDF_DEFERRED_FIRST;
        updateDescriptorImageInfo((uint32_t)allTextureObjects.size(), offset, textureObject->getSampler()->getSampler(), textureObject->getImageObject()->getImageView()->getImageView(), textureObject->getImageObject()->getImage()->getImageLayout());
    }

    allTextureObjects.append(textureObject);
}

VkBool32 BSDFMaterial::removeTextureObject(const ITextureObjectSP& textureObject)
{
    return allTextureObjects.remove(textureObject);
}

size_t BSDFMaterial::getNumberTextureObjects() const
{
    return allTextureObjects.size();
}

const SmartPointerVector<ITextureObjectSP>& BSDFMaterial::getTextureObjects() const
{
    return allTextureObjects;
}

const IDescriptorPoolSP& BSDFMaterial::getDescriptorPool() const
{
    return descriptorPool;
}

void BSDFMaterial::setDescriptorPool(const IDescriptorPoolSP& descriptorPool)
{
    this->descriptorPool = descriptorPool;
}

IDescriptorSetsSP BSDFMaterial::getDescriptorSets() const
{
    return descriptorSets;
}

void BSDFMaterial::setDescriptorSets(const IDescriptorSetsSP& descriptorSets)
{
    this->descriptorSets = descriptorSets;
}

void BSDFMaterial::visitRecursive(SceneVisitor* sceneVisitor)
{
	SceneVisitor* currentSceneVisitor = sceneVisitor;

	while (currentSceneVisitor)
	{
		if (!currentSceneVisitor->visit(*this))
		{
			return;
		}

		currentSceneVisitor = currentSceneVisitor->getNextSceneVisitor();
	}
}


//
// ICloneable
//

IBSDFMaterialSP BSDFMaterial::clone() const
{
	auto result = IBSDFMaterialSP(new BSDFMaterial(*this));

	if (result && result->getNumberTextureObjects() != getNumberTextureObjects())
	{
		return IBSDFMaterialSP();
	}

    return result;
}


//
// IDestroyable
//

void BSDFMaterial::destroy()
{
	if (fragmentShader.get())
	{
		fragmentShader->destroy();
	}

	for (size_t i = 0; i < allTextureObjects.size(); i++)
	{
		allTextureObjects[i]->destroy();
	}

	//

	Material::destroy();
}

} /* namespace vkts */
