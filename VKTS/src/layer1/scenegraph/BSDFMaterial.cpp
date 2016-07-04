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
    IBSDFMaterial(), Material(), name(), fragmentShader(nullptr), attributes(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_NORMAL), allTextures(), descriptorSetLayout(), pipelineLayout()
{
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
    if (texture.get())
    {
        updateDescriptorImageInfo(VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FIRST + (uint32_t)allTextures.size(), texture->getSampler()->getSampler(), texture->getImageView()->getImageView(), texture->getMemoryImage()->getImage()->getImageLayout());
    }

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

IPipelineLayoutSP BSDFMaterial::getPipelineLayout() const
{
    return pipelineLayout;
}

void BSDFMaterial::setPipelineLayout(const IPipelineLayoutSP& pipelineLayout)
{
    this->pipelineLayout = pipelineLayout;
}

void BSDFMaterial::updateDescriptorSetsRecursive(const std::string& nodeName, const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets)
{
    auto currentDescriptorSets = createDescriptorSetsByName(nodeName);

    if (!currentDescriptorSets.get())
    {
        return;
    }

    //

    VkWriteDescriptorSet finalWriteDescriptorSets[VKTS_BINDING_UNIFORM_BSDF_BINDING_COUNT];
    uint32_t finalWriteDescriptorSetsCount = 0;

    for (uint32_t i = 0; i < allWriteDescriptorSetsCount; i++)
    {
        for (uint32_t k = 0; k < VKTS_BINDING_UNIFORM_BSDF_BINDING_COUNT; k++)
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

void BSDFMaterial::bindDescriptorSets(const std::string& nodeName, const ICommandBuffersSP& cmdBuffer, const VkPipelineLayout layout, const uint32_t bufferIndex) const
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

void BSDFMaterial::bindDrawIndexedRecursive(const std::string& nodeName, const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const overwrite* renderOverwrite, const uint32_t bufferIndex) const
{
    const overwrite* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->materialBindDrawIndexedRecursive(*this, cmdBuffer, graphicsPipeline, bufferIndex))
    	{
    		return;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	if (!graphicsPipeline.get())
	{
		return;
	}

	vkCmdBindPipeline(cmdBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

    bindDescriptorSets(nodeName, cmdBuffer, graphicsPipeline->getLayout(), bufferIndex);
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

	for (size_t i = 0; i < allTextures.size(); i++)
	{
		allTextures[i]->destroy();
	}

	if (descriptorSets.get())
	{
		descriptorSets->destroy();
	}

	if (descriptorPool.get())
	{
		descriptorPool->destroy();
	}

	if (descriptorSetLayout.get())
	{
		descriptorSetLayout->destroy();
	}

	if (pipelineLayout.get())
	{
		pipelineLayout->destroy();
	}

	//

	Material::destroy();
}

} /* namespace vkts */
