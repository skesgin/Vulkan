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

#include "PhongMaterial.hpp"

namespace vkts
{

PhongMaterial::PhongMaterial(const VkBool32 forwardRendering) :
    IPhongMaterial(), Material(forwardRendering, VK_TRUE, ""), alpha(), displacement(), normal(), ambient(), emissive(), diffuse(), specular(), specularShininess(), mirror(), mirrorReflectivity(), transparent(VK_FALSE)
{
}

PhongMaterial::PhongMaterial(const PhongMaterial& other) :
    IPhongMaterial(), Material(other), alpha(other.alpha), displacement(other.displacement), normal(other.normal), ambient(other.ambient), emissive(other.emissive), diffuse(other.diffuse), specular(other.specular), specularShininess(other.specularShininess), mirror(other.mirror), mirrorReflectivity(other.mirrorReflectivity), transparent(other.transparent)
{

    // TextureObjects cannot be cloned, just replaced.

	for (uint32_t i = 0; i < materialData.size(); i++)
	{
		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_EMISSIVE - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->emissive->getSampler()->getSampler(), this->emissive->getImageObject()->getImageView()->getImageView(), this->emissive->getImageObject()->getImage()->getImageLayout());

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_ALPHA - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->alpha->getSampler()->getSampler(), this->alpha->getImageObject()->getImageView()->getImageView(), this->alpha->getImageObject()->getImage()->getImageLayout());

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DISPLACEMENT - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->displacement->getSampler()->getSampler(), this->displacement->getImageObject()->getImageView()->getImageView(), this->displacement->getImageObject()->getImage()->getImageLayout());

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_NORMAL - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->normal->getSampler()->getSampler(), this->normal->getImageObject()->getImageView()->getImageView(), this->normal->getImageObject()->getImage()->getImageLayout());

		//

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_AMBIENT - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->ambient->getSampler()->getSampler(), this->ambient->getImageObject()->getImageView()->getImageView(), this->ambient->getImageObject()->getImage()->getImageLayout());

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DIFFUSE - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->diffuse->getSampler()->getSampler(), this->diffuse->getImageObject()->getImageView()->getImageView(), this->diffuse->getImageObject()->getImage()->getImageLayout());

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->specular->getSampler()->getSampler(), this->specular->getImageObject()->getImageView()->getImageView(), this->specular->getImageObject()->getImage()->getImageLayout());

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR_SHININESS - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->specularShininess->getSampler()->getSampler(), this->specularShininess->getImageObject()->getImageView()->getImageView(), this->specularShininess->getImageObject()->getImage()->getImageLayout());

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_MIRROR - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->mirror->getSampler()->getSampler(), this->mirror->getImageObject()->getImageView()->getImageView(), this->mirror->getImageObject()->getImage()->getImageLayout());

		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_MIRROR_REFLECTIVITY - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->mirrorReflectivity->getSampler()->getSampler(), this->mirrorReflectivity->getImageObject()->getImageView()->getImageView(), this->mirrorReflectivity->getImageObject()->getImage()->getImageLayout());
	}
}

PhongMaterial::~PhongMaterial()
{
    destroy();
}

//
// IPhongMaterial
//

VkBool32 PhongMaterial::getForwardRendering() const
{
    return forwardRendering;
}


const std::string& PhongMaterial::getName() const
{
    return name;
}

void PhongMaterial::setName(const std::string& name)
{
    this->name = name;
}

IRenderMaterialSP PhongMaterial::getRenderMaterial(const uint32_t index) const
{
	if (index >= materialData.size())
	{
		return IRenderMaterialSP();
	}

	return materialData[index];
}

VkDeviceSize PhongMaterial::getRenderMaterialSize() const
{
	return materialData.size();
}

void PhongMaterial::addRenderMaterial(const IRenderMaterialSP& materialData)
{
    this->materialData.append(materialData);
}

//

const ITextureObjectSP& PhongMaterial::getAlpha() const
{
    return alpha;
}

void PhongMaterial::setAlpha(const ITextureObjectSP& alpha)
{
    this->alpha = alpha;

    if (this->alpha.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
    		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_ALPHA - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->alpha->getSampler()->getSampler(), this->alpha->getImageObject()->getImageView()->getImageView(), this->alpha->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

const ITextureObjectSP& PhongMaterial::getDisplacement() const
{
    return displacement;
}

void PhongMaterial::setDisplacement(const ITextureObjectSP& displacement)
{
    this->displacement = displacement;

    if (this->displacement.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
        	materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DISPLACEMENT - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->displacement->getSampler()->getSampler(), this->displacement->getImageObject()->getImageView()->getImageView(), this->displacement->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

const ITextureObjectSP& PhongMaterial::getNormal() const
{
    return normal;
}

void PhongMaterial::setNormal(const ITextureObjectSP& normal)
{
    this->normal = normal;

    if (this->normal.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
        	materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_NORMAL - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->normal->getSampler()->getSampler(), this->normal->getImageObject()->getImageView()->getImageView(), this->normal->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

//

const ITextureObjectSP& PhongMaterial::getEmissive() const
{
    return emissive;
}

void PhongMaterial::setEmissive(const ITextureObjectSP& emissive)
{
    this->emissive = emissive;

    if (this->emissive.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
        	materialData [i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_EMISSIVE - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->emissive->getSampler()->getSampler(), this->emissive->getImageObject()->getImageView()->getImageView(), this->emissive->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

const ITextureObjectSP& PhongMaterial::getAmbient() const
{
    return ambient;
}

void PhongMaterial::setAmbient(const ITextureObjectSP& ambient)
{
    this->ambient = ambient;

    if (this->ambient.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
        	materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_AMBIENT - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->ambient->getSampler()->getSampler(), this->ambient->getImageObject()->getImageView()->getImageView(), this->ambient->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

const ITextureObjectSP& PhongMaterial::getDiffuse() const
{
    return diffuse;
}

void PhongMaterial::setDiffuse(const ITextureObjectSP& diffuse)
{
    this->diffuse = diffuse;

    if (this->diffuse.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
    		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DIFFUSE - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->diffuse->getSampler()->getSampler(), this->diffuse->getImageObject()->getImageView()->getImageView(), this->diffuse->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

const ITextureObjectSP& PhongMaterial::getSpecular() const
{
    return specular;
}

void PhongMaterial::setSpecular(const ITextureObjectSP& specular)
{
    this->specular = specular;

    if (this->specular.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
        	materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->specular->getSampler()->getSampler(), this->specular->getImageObject()->getImageView()->getImageView(), this->specular->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

const ITextureObjectSP& PhongMaterial::getSpecularShininess() const
{
    return specularShininess;
}

void PhongMaterial::setSpecularShininess(const ITextureObjectSP& specularShininess)
{
    this->specularShininess = specularShininess;

    if (this->specularShininess.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
        	materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR_SHININESS - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->specularShininess->getSampler()->getSampler(), this->specularShininess->getImageObject()->getImageView()->getImageView(), this->specularShininess->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

const ITextureObjectSP& PhongMaterial::getMirror() const
{
	return mirror;
}

void PhongMaterial::setMirror(const ITextureObjectSP& mirror)
{
	this->mirror = mirror;

    if (this->specularShininess.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
        	materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_MIRROR - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->mirror->getSampler()->getSampler(), this->mirror->getImageObject()->getImageView()->getImageView(), this->mirror->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

const ITextureObjectSP& PhongMaterial::getMirrorReflectivity() const
{
	return mirrorReflectivity;
}

void PhongMaterial::setMirrorReflectivity(const ITextureObjectSP& mirrorReflectivity)
{
	this->mirrorReflectivity = mirrorReflectivity;

    if (this->specularShininess.get())
    {
    	for (uint32_t i = 0; i < materialData.size(); i++)
    	{
    		materialData[i]->addDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_MIRROR_REFLECTIVITY - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->mirrorReflectivity->getSampler()->getSampler(), this->mirrorReflectivity->getImageObject()->getImageView()->getImageView(), this->mirrorReflectivity->getImageObject()->getImage()->getImageLayout());
    	}
    }
}

//

VkBool32 PhongMaterial::isTransparent() const
{
	return transparent;
}

void PhongMaterial::setTransparent(const VkBool32 transparent)
{
	this->transparent = transparent;
}

void PhongMaterial::updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer, const std::string& nodeName)
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

void PhongMaterial::drawRecursive(const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite, const std::string& nodeName)
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

IPhongMaterialSP PhongMaterial::clone() const
{
	auto result = IPhongMaterialSP(new PhongMaterial(*this));

	if (result.get() && (getRenderMaterialSize() != result->getRenderMaterialSize()))
	{
		return IPhongMaterialSP();
	}

	// If something else failed, the above content is also invalid.

    return result;
}

//
// IDestroyable
//

void PhongMaterial::destroy()
{
	try
	{

	    if (emissive.get())
	    {
	        emissive->destroy();
	    }

	    if (alpha.get())
	    {
	        alpha->destroy();
	    }

	    if (displacement.get())
	    {
	        displacement->destroy();
	    }

	    if (normal.get())
	    {
	        normal->destroy();
	    }

	    transparent = VK_FALSE;

	    if (ambient.get())
	    {
	    	ambient->destroy();
	    }

	    if (diffuse.get())
	    {
	    	diffuse->destroy();
	    }

	    if (specular.get())
	    {
	    	specular->destroy();
	    }

	    if (specularShininess.get())
	    {
	    	specularShininess->destroy();
	    }

	    if (mirror.get())
	    {
	    	mirror->destroy();
	    }

	    if (mirrorReflectivity.get())
	    {
	    	mirrorReflectivity->destroy();
	    }

	    //

	    destroyMaterial();
	}
	catch(const std::exception& e)
	{
    	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Catched exception '%s'", e.what());
	}
}

} /* namespace vkts */
