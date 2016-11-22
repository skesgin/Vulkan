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

#include "../scene/PhongMaterial.hpp"

namespace vkts
{

PhongMaterial::PhongMaterial(const VkBool32 forwardRendering) :
    IPhongMaterial(), Material(), forwardRendering(forwardRendering), name(), alpha(), displacement(), normal(), ambient(), emissive(), diffuse(), specular(), specularShininess(), mirror(), mirrorReflectivity(), transparent(VK_FALSE)
{
}

PhongMaterial::PhongMaterial(const PhongMaterial& other) :
    IPhongMaterial(), Material(other), forwardRendering(other.forwardRendering), name(other.name + "_clone"), alpha(other.alpha), displacement(other.displacement), normal(other.normal), ambient(other.ambient), emissive(other.emissive), diffuse(other.diffuse), specular(other.specular), specularShininess(other.specularShininess), mirror(other.mirror), mirrorReflectivity(other.mirrorReflectivity), transparent(other.transparent)
{

    // TextureObjects cannot be cloned, just replaced.

	if (other.descriptorPool.get())
	{
		descriptorPool = descriptorPoolCreate(other.descriptorPool->getDevice(), other.descriptorPool->getFlags(), other.descriptorPool->getMaxSets(), other.descriptorPool->getPoolSizeCount(), other.descriptorPool->getPoolSizes());

		if (!descriptorPool.get())
		{
			destroy();

			return;
		}

		if (other.descriptorSets.get())
		{
			descriptorSets = descriptorSetsCreate(descriptorPool->getDevice(), descriptorPool->getDescriptorPool(), other.descriptorSets->getDescriptorSetCount(), other.descriptorSets->getSetLayouts());

			if (!descriptorSets.get())
			{
				destroy();

				return;
			}
		}

	    //

	    if (other.allDescriptorPools.size() != other.allDescriptorSets.size())
	    {
	    	destroy();

	        return;
	    }


	    for (size_t i = 0; i < other.allDescriptorPools.size(); i++)
	    {
	    	auto currentDescriptorPool = descriptorPoolCreate(other.allDescriptorPools.valueAt(i)->getDevice(), other.allDescriptorPools.valueAt(i)->getFlags(), other.allDescriptorPools.valueAt(i)->getMaxSets(), other.allDescriptorPools.valueAt(i)->getPoolSizeCount(), other.allDescriptorPools.valueAt(i)->getPoolSizes());

	        if (!currentDescriptorPool.get())
	        {
	        	destroy();

	            return;
	        }

	        allDescriptorPools[other.allDescriptorPools.keyAt(i)] = currentDescriptorPool;

	        //

	        auto currentDescriptorSets = descriptorSetsCreate(currentDescriptorPool->getDevice(), currentDescriptorPool->getDescriptorPool(), other.allDescriptorSets.valueAt(i)->getDescriptorSetCount(), other.allDescriptorSets.valueAt(i)->getSetLayouts());

	        if (!currentDescriptorSets.get())
	        {
	        	destroy();

	            return;
	        }

	        allDescriptorSets[other.allDescriptorSets.keyAt(i)] = currentDescriptorSets;
	    }
	}

	//

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_EMISSIVE - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->emissive->getSampler()->getSampler(), this->emissive->getImageObject()->getImageView()->getImageView(), this->emissive->getImageObject()->getImage()->getImageLayout());

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_ALPHA - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->alpha->getSampler()->getSampler(), this->alpha->getImageObject()->getImageView()->getImageView(), this->alpha->getImageObject()->getImage()->getImageLayout());

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DISPLACEMENT - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->displacement->getSampler()->getSampler(), this->displacement->getImageObject()->getImageView()->getImageView(), this->displacement->getImageObject()->getImage()->getImageLayout());

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_NORMAL - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->normal->getSampler()->getSampler(), this->normal->getImageObject()->getImageView()->getImageView(), this->normal->getImageObject()->getImage()->getImageLayout());

    //

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_AMBIENT - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->ambient->getSampler()->getSampler(), this->ambient->getImageObject()->getImageView()->getImageView(), this->ambient->getImageObject()->getImage()->getImageLayout());

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DIFFUSE - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->diffuse->getSampler()->getSampler(), this->diffuse->getImageObject()->getImageView()->getImageView(), this->diffuse->getImageObject()->getImage()->getImageLayout());

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->specular->getSampler()->getSampler(), this->specular->getImageObject()->getImageView()->getImageView(), this->specular->getImageObject()->getImage()->getImageLayout());

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR_SHININESS - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->specularShininess->getSampler()->getSampler(), this->specularShininess->getImageObject()->getImageView()->getImageView(), this->specularShininess->getImageObject()->getImage()->getImageLayout());

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_MIRROR - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->mirror->getSampler()->getSampler(), this->mirror->getImageObject()->getImageView()->getImageView(), this->mirror->getImageObject()->getImage()->getImageLayout());

    updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_MIRROR_REFLECTIVITY - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->mirrorReflectivity->getSampler()->getSampler(), this->mirrorReflectivity->getImageObject()->getImageView()->getImageView(), this->mirrorReflectivity->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_ALPHA - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->alpha->getSampler()->getSampler(), this->alpha->getImageObject()->getImageView()->getImageView(), this->alpha->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DISPLACEMENT - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->displacement->getSampler()->getSampler(), this->displacement->getImageObject()->getImageView()->getImageView(), this->displacement->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_NORMAL - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->normal->getSampler()->getSampler(), this->normal->getImageObject()->getImageView()->getImageView(), this->normal->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_EMISSIVE - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->emissive->getSampler()->getSampler(), this->emissive->getImageObject()->getImageView()->getImageView(), this->emissive->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_AMBIENT - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->ambient->getSampler()->getSampler(), this->ambient->getImageObject()->getImageView()->getImageView(), this->ambient->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DIFFUSE - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->diffuse->getSampler()->getSampler(), this->diffuse->getImageObject()->getImageView()->getImageView(), this->diffuse->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->specular->getSampler()->getSampler(), this->specular->getImageObject()->getImageView()->getImageView(), this->specular->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR_SHININESS - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->specularShininess->getSampler()->getSampler(), this->specularShininess->getImageObject()->getImageView()->getImageView(), this->specularShininess->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_MIRROR - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->mirror->getSampler()->getSampler(), this->mirror->getImageObject()->getImageView()->getImageView(), this->mirror->getImageObject()->getImage()->getImageLayout());
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
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_PHONG_MIRROR_REFLECTIVITY - VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, VKTS_BINDING_UNIFORM_SAMPLER_PHONG_FIRST, this->mirrorReflectivity->getSampler()->getSampler(), this->mirrorReflectivity->getImageObject()->getImageView()->getImageView(), this->mirrorReflectivity->getImageObject()->getImage()->getImageLayout());
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

//

const IDescriptorPoolSP& PhongMaterial::getDescriptorPool() const
{
    return descriptorPool;
}

void PhongMaterial::setDescriptorPool(const IDescriptorPoolSP& descriptorPool)
{
    this->descriptorPool = descriptorPool;
}

IDescriptorSetsSP PhongMaterial::getDescriptorSets() const
{
    return descriptorSets;
}

void PhongMaterial::setDescriptorSets(const IDescriptorSetsSP& descriptorSets)
{
    this->descriptorSets = descriptorSets;
}

void PhongMaterial::updateDescriptorSetsRecursive(const std::string& nodeName, const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets)
{
    auto currentDescriptorSets = createDescriptorSetsByName(nodeName);

    if (!currentDescriptorSets.get())
    {
        return;
    }

    //

    VkWriteDescriptorSet finalWriteDescriptorSets[VKTS_BINDING_UNIFORM_PHONG_TOTAL_BINDING_COUNT];
    uint32_t finalWriteDescriptorSetsCount = 0;

    for (uint32_t i = 0; i < allWriteDescriptorSetsCount; i++)
    {
        for (uint32_t k = 0; k < VKTS_BINDING_UNIFORM_PHONG_BINDING_COUNT; k++)
        {
        	// Assign used descriptor set.
			if (allWriteDescriptorSets[i].dstBinding == writeDescriptorSets[k].dstBinding && writeDescriptorSets[k].sType == VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET)
			{
				allWriteDescriptorSets[i] = writeDescriptorSets[k];
			}
        }

        // Gather valid descriptor sets.
    	if (allWriteDescriptorSets[i].sType == VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET)
    	{
    		finalWriteDescriptorSets[finalWriteDescriptorSetsCount] = allWriteDescriptorSets[i];

    		finalWriteDescriptorSets[finalWriteDescriptorSetsCount].dstSet = currentDescriptorSets->getDescriptorSets()[0];

			finalWriteDescriptorSetsCount++;
    	}
    }

    currentDescriptorSets->updateDescriptorSets(finalWriteDescriptorSetsCount, finalWriteDescriptorSets, 0, nullptr);
}

void PhongMaterial::bindDescriptorSets(const std::string& nodeName, const ICommandBuffersSP& cmdBuffer, const VkPipelineLayout layout, const uint32_t bufferIndex) const
{
    if (!cmdBuffer.get())
    {
        return;
    }

    auto currentDescriptorSets = getDescriptorSetsByName(nodeName);

    if (!currentDescriptorSets.get())
    {
        return;
    }

    vkCmdBindDescriptorSets(cmdBuffer->getCommandBuffer(bufferIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &currentDescriptorSets->getDescriptorSets()[0], 0, nullptr);
}

void PhongMaterial::bindDrawIndexedRecursive(const std::string& nodeName, const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const Overwrite* renderOverwrite, const uint32_t bufferIndex) const
{
    const Overwrite* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->materialBindDrawIndexedRecursive(*this, cmdBuffer, graphicsPipeline, bufferIndex))
    	{
    		return;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

    bindDescriptorSets(nodeName, cmdBuffer, graphicsPipeline->getLayout(), bufferIndex);
}

//
// ICloneable
//

IPhongMaterialSP PhongMaterial::clone() const
{
	auto result = IPhongMaterialSP(new PhongMaterial(*this));

	if (result.get() && getDescriptorPool().get() && !result->getDescriptorPool().get())
	{
		return IPhongMaterialSP();
	}

	if (result.get() && getDescriptorSets().get() && !result->getDescriptorSets().get())
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

    Material::destroy();
}

} /* namespace vkts */
