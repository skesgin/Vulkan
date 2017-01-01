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

#include "RenderMaterial.hpp"

namespace vkts
{

IDescriptorSetsSP RenderMaterial::createDescriptorSetsByName(const std::string& nodeName)
{
	if (!descriptorPool.get() || !descriptorSets.get())
	{
		return IDescriptorSetsSP();
	}

	if (allDescriptorSets.contains(nodeName))
	{
		return allDescriptorSets[nodeName];
	}

	//

	if (this->nodeName == "")
	{
		this->nodeName = nodeName;

		allDescriptorPools[nodeName] = descriptorPool;

		allDescriptorSets[nodeName] = descriptorSets;

		allBindingPresent[nodeName].clear();

		return descriptorSets;
	}

	//

	auto currentDescriptorPool = descriptorPoolCreate(descriptorPool->getDevice(), descriptorPool->getFlags(), descriptorPool->getMaxSets(), descriptorPool->getPoolSizeCount(), descriptorPool->getPoolSizes());

    if (!currentDescriptorPool.get())
    {
        return IDescriptorSetsSP();
    }

	auto currentDescriptorSets = descriptorSetsCreate(currentDescriptorPool->getDevice(), currentDescriptorPool->getDescriptorPool(), descriptorSets->getDescriptorSetCount(), descriptorSets->getSetLayouts());

    if (!currentDescriptorSets.get())
    {
        return IDescriptorSetsSP();
    }

    allDescriptorPools[nodeName] = currentDescriptorPool;

    allDescriptorSets[nodeName] = currentDescriptorSets;

    allBindingPresent[nodeName].clear();

    //

    return allDescriptorSets[nodeName];
}

IDescriptorSetsSP RenderMaterial::getDescriptorSetsByName(const std::string& nodeName) const
{
	if (allDescriptorSets.contains(nodeName))
	{
		return allDescriptorSets[nodeName];
	}

	return IDescriptorSetsSP();
}

void RenderMaterial::bindDescriptorSets(const ICommandBuffersSP& cmdBuffer, const VkPipelineLayout layout, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const std::string& nodeName) const
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

    //
    uint32_t localDynamicOffsetCount = 0;

    std::vector<uint32_t> localDynamicOffsets;

    //

    const auto& currentBindingPresent = allBindingPresent.at(nodeName);
    for (const auto& currentBinding : currentBindingPresent)
    {
    	if (currentBinding.second)
    	{
    		auto currentOffset = dynamicOffsetMappings.find(currentBinding.first);

			if (currentOffset != dynamicOffsetMappings.end())
			{
	    		localDynamicOffsetCount++;

	    		localDynamicOffsets.push_back(currentOffset->second.stride * currentBuffer + currentOffset->second.offset);
			}
    	}
    }

    vkCmdBindDescriptorSets(cmdBuffer->getCommandBuffer(0), VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &currentDescriptorSets->getDescriptorSets()[0], localDynamicOffsetCount, &localDynamicOffsets[0]);
}

RenderMaterial::RenderMaterial() :
    IRenderMaterial(), descriptorPool(), descriptorSets(), descriptorImageInfos{}, writeDescriptorSets{}, nodeName(), allDescriptorPools(), allDescriptorSets(), allBindingPresent()
{
}

RenderMaterial::RenderMaterial(const RenderMaterial& other) :
	IRenderMaterial(), descriptorPool(), descriptorSets(), descriptorImageInfos{}, writeDescriptorSets{}, nodeName(), allDescriptorPools(), allDescriptorSets(), allBindingPresent(other.allBindingPresent)
{
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


		for (uint32_t i = 0; i < other.allDescriptorPools.size(); i++)
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
}

RenderMaterial::~RenderMaterial()
{
    destroy();
}

IDescriptorPoolSP RenderMaterial::getDescriptorPool() const
{
	return descriptorPool;
}

void RenderMaterial::setDescriptorPool(const IDescriptorPoolSP& descriptorPool)
{
	this->descriptorPool = descriptorPool;
}

IDescriptorSetsSP RenderMaterial::getDescriptorSets() const
{
	return descriptorSets;
}

void RenderMaterial::setDescriptorSets(const IDescriptorSetsSP& descriptorSets)
{
	this->descriptorSets = descriptorSets;
}

void RenderMaterial::addDescriptorImageInfo(const uint32_t colorIndex, const uint32_t dstBindingOffset, const VkSampler sampler, const VkImageView imageView, const VkImageLayout imageLayout)
{
    if (colorIndex >= VKTS_BINDING_UNIFORM_MATERIAL_TOTAL_BINDING_COUNT)
    {
        return;
    }

    memset(&descriptorImageInfos[colorIndex], 0, sizeof(VkDescriptorImageInfo));

    descriptorImageInfos[colorIndex].sampler = sampler;
    descriptorImageInfos[colorIndex].imageView = imageView;
    descriptorImageInfos[colorIndex].imageLayout = imageLayout;

    memset(&writeDescriptorSets[colorIndex], 0, sizeof(VkWriteDescriptorSet));

    writeDescriptorSets[colorIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSets[colorIndex].dstSet = VK_NULL_HANDLE; // Defined later.
    writeDescriptorSets[colorIndex].dstBinding = dstBindingOffset + colorIndex;
    writeDescriptorSets[colorIndex].dstArrayElement = 0;
    writeDescriptorSets[colorIndex].descriptorCount = 1;
    writeDescriptorSets[colorIndex].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSets[colorIndex].pImageInfo = &descriptorImageInfos[colorIndex];
    writeDescriptorSets[colorIndex].pBufferInfo = nullptr;
    writeDescriptorSets[colorIndex].pTexelBufferView = nullptr;
}

void RenderMaterial::updateDescriptorSets(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const std::string& nodeName)
{
    auto currentDescriptorSets = createDescriptorSetsByName(nodeName);

    if (!currentDescriptorSets.get())
    {
        return;
    }

    //

    VkWriteDescriptorSet finalWriteDescriptorSets[VKTS_BINDING_UNIFORM_MATERIAL_TOTAL_BINDING_COUNT];
    uint32_t finalWriteDescriptorSetsCount = 0;

	// Copy from parent nodes.
    for (uint32_t i = 0; i < allWriteDescriptorSetsCount; i++)
    {
		if (allWriteDescriptorSets[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
		{
			allBindingPresent[nodeName][allWriteDescriptorSets[i].dstBinding] = VK_FALSE;
		}

		if (allWriteDescriptorSets[i].descriptorCount > 0)
    	{
    		finalWriteDescriptorSets[finalWriteDescriptorSetsCount] = allWriteDescriptorSets[i];

    		finalWriteDescriptorSets[finalWriteDescriptorSetsCount].dstSet = currentDescriptorSets->getDescriptorSets()[0];

			finalWriteDescriptorSetsCount++;

			if (allWriteDescriptorSets[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
			{
				allBindingPresent[nodeName][allWriteDescriptorSets[i].dstBinding] = VK_TRUE;
			}
    	}
    }

    uint32_t currentWriteDescriptorSetsCount = finalWriteDescriptorSetsCount;

    // Copy from material.
    for (uint32_t k = 0; k < VKTS_BINDING_UNIFORM_MATERIAL_TOTAL_BINDING_COUNT; k++)
    {
		for (uint32_t i = currentWriteDescriptorSetsCount; i < allWriteDescriptorSetsCount; i++)
		{
        	// Assign used descriptor set.
			if (allWriteDescriptorSets[i].dstBinding == writeDescriptorSets[k].dstBinding && writeDescriptorSets[k].sType == VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET)
			{
	    		finalWriteDescriptorSets[finalWriteDescriptorSetsCount] = writeDescriptorSets[k];

	    		finalWriteDescriptorSets[finalWriteDescriptorSetsCount].dstSet = currentDescriptorSets->getDescriptorSets()[0];

				finalWriteDescriptorSetsCount++;

				if (allWriteDescriptorSets[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
				{
					allBindingPresent[nodeName][allWriteDescriptorSets[i].dstBinding] = VK_TRUE;
				}
			}
        }
    }

    currentDescriptorSets->updateDescriptorSets(finalWriteDescriptorSetsCount, finalWriteDescriptorSets, 0, nullptr);
}

void RenderMaterial::draw(const ICommandBuffersSP& cmdBuffer, const IGraphicsPipelineSP& graphicsPipeline, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const std::string& nodeName)
{
    bindDescriptorSets(cmdBuffer, graphicsPipeline->getLayout(), currentBuffer, dynamicOffsetMappings, nodeName);
}

//
// ICloneable
//

IRenderMaterialSP RenderMaterial::create(const VkBool32 createData) const
{
	IRenderMaterialSP result;

	if (createData)
	{
		result = IRenderMaterialSP(new RenderMaterial(*this));

		if (result.get() && getDescriptorPool().get() && !result->getDescriptorPool().get())
		{
			return IRenderMaterialSP();
		}

		if (result.get() && getDescriptorSets().get() && !result->getDescriptorSets().get())
		{
			return IRenderMaterialSP();
		}
	}
	else
	{
		result = IRenderMaterialSP(new RenderMaterial());
	}

	return result;
}

void RenderMaterial::destroy()
{
    for (uint32_t i = 0; i < allDescriptorSets.size(); i++)
    {
        allDescriptorSets.valueAt(i)->destroy();
    }
    allDescriptorSets.clear();

    for (uint32_t i = 0; i < allDescriptorPools.size(); i++)
    {
    	allDescriptorPools.valueAt(i)->destroy();
    }
    allDescriptorPools.clear();

    allBindingPresent.clear();

    memset(writeDescriptorSets, 0, sizeof(writeDescriptorSets));
    memset(descriptorImageInfos, 0, sizeof(descriptorImageInfos));

    nodeName = "";

    descriptorSets = IDescriptorSetsSP();
    descriptorPool = IDescriptorPoolSP();
}

} /* namespace vkts */
