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

BSDFMaterial::BSDFMaterial(const VkBool32 forwardRendering) :
    IBSDFMaterial(), Material(forwardRendering, VK_FALSE, ""), fragmentShader(nullptr), attributes(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_NORMAL), allTextureObjects(), transparent(VK_FALSE), sorted(VK_FALSE), packed(VK_FALSE), alphaCutoff(0.0f), ambientOcclusionStrength(1.0f), specularGlossiness(VK_FALSE)
{
}

BSDFMaterial::BSDFMaterial(const BSDFMaterial& other) :
    IBSDFMaterial(), Material(other), fragmentShader(other.fragmentShader), attributes(other.attributes), allTextureObjects(), transparent(other.transparent), sorted(other.sorted), packed(other.packed), alphaCutoff(other.alphaCutoff), ambientOcclusionStrength(other.ambientOcclusionStrength), specularGlossiness(other.specularGlossiness)
{
	for (uint32_t i = 0; i < other.allTextureObjects.size(); i++)
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

IRenderMaterialSP BSDFMaterial::getRenderMaterial(const uint32_t index) const
{
	if (index >= materialData.size())
	{
		return IRenderMaterialSP();
	}
	return materialData[index];
}

VkDeviceSize BSDFMaterial::getRenderMaterialSize() const
{
	return materialData.size();
}

void BSDFMaterial::addRenderMaterial(const IRenderMaterialSP& materialData)
{
    this->materialData.append(materialData);
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

    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
        	materialData[i]->addDescriptorImageInfo(allTextureObjects.size(), offset, textureObject->getSampler()->getSampler(), textureObject->getImageObject()->getImageView()->getImageView(), textureObject->getImageObject()->getImage()->getImageLayout());
    	}
    }

    allTextureObjects.append(textureObject);
}

VkBool32 BSDFMaterial::removeTextureObject(const ITextureObjectSP& textureObject)
{
    return allTextureObjects.remove(textureObject);
}

uint32_t BSDFMaterial::getNumberTextureObjects() const
{
    return allTextureObjects.size();
}

const SmartPointerVector<ITextureObjectSP>& BSDFMaterial::getTextureObjects() const
{
    return allTextureObjects;
}

VkBool32 BSDFMaterial::isTransparent() const
{
	return transparent;
}

void BSDFMaterial::setTransparent(const VkBool32 transparent)
{
	this->transparent = transparent;
}

VkBool32 BSDFMaterial::isSorted() const
{
	return sorted;
}

void BSDFMaterial::setSorted(const VkBool32 sorted)
{
	this->sorted = sorted;
}

VkBool32 BSDFMaterial::isPacked() const
{
	return packed;
}

void BSDFMaterial::setPacked(const VkBool32 packed)
{
	this->packed = packed;
}

float BSDFMaterial::getAlphaCutoff() const
{
	return alphaCutoff;
}

void BSDFMaterial::setAlphaCutoff(const float alphaCutoff)
{
	this->alphaCutoff = alphaCutoff;
}

float BSDFMaterial::getAmbientOcclusionStrength() const
{
	return ambientOcclusionStrength;
}

void BSDFMaterial::setAmbientOcclusionStrength(const float ambientOcclusionStrength)
{
	this->ambientOcclusionStrength = ambientOcclusionStrength;
}

VkBool32 BSDFMaterial::isSpecularGlossiness() const
{
	return specularGlossiness;
}

void BSDFMaterial::setSpecularGlossiness(const VkBool32 specularGlossiness)
{
	this->specularGlossiness = specularGlossiness;
}

void BSDFMaterial::updateParameterRecursive(Parameter* parameter)
{
	if (parameter)
	{
		parameter->visit(*this);
	}
}

void BSDFMaterial::updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer, const std::string& nodeName)
{
	if (currentBuffer >= materialData.size())
	{
		return;
	}

	if (materialData[currentBuffer].get())
	{
		materialData[currentBuffer]->updateDescriptorSets(allWriteDescriptorSetsCount, allWriteDescriptorSets, nodeName);
	}
}

void BSDFMaterial::drawRecursive(const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite, const std::string& nodeName)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(*this, cmdBuffer, graphicsPipeline, currentBuffer, dynamicOffsetMappings))
    	{
    		return;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

    //

	if (currentBuffer >= materialData.size())
	{
		return;
	}

	if (materialData[currentBuffer].get())
	{
		materialData[currentBuffer]->draw(cmdBuffer, graphicsPipeline, currentBuffer, dynamicOffsetMappings, nodeName);
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
	try
	{
		if (fragmentShader.get())
		{
			fragmentShader->destroy();
		}

		for (uint32_t i = 0; i < allTextureObjects.size(); i++)
		{
			allTextureObjects[i]->destroy();
		}

		destroyMaterial();
	}
	catch(const std::exception& e)
	{
    	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Catched exception '%s'", e.what());
	}
}

} /* namespace vkts */
