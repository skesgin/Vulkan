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

#include "SceneRenderFactory.hpp"

#include "../scene/RenderMaterial.hpp"
#include "../scene/RenderNode.hpp"
#include "../scene/RenderSubMesh.hpp"

#define VKTS_PREFILTER_VERTEX_SHADER_NAME "shader/SPIR/V/prefilter.vert.spv"

#define VKTS_LAMBERT_FRAGMENT_SHADER_NAME "shader/SPIR/V/prefilter_lambert.frag.spv"
#define VKTS_COOKTORRANCE_FRAGMENT_SHADER_NAME "shader/SPIR/V/prefilter_cooktorrance.frag.spv"

namespace vkts
{

SceneRenderFactory::SceneRenderFactory(const IDescriptorSetLayoutSP& descriptorSetLayout, const IRenderPassSP& renderPass, const IPipelineCacheSP& pipelineCache, const VkDeviceSize bufferCount) :
	ISceneRenderFactory(), descriptorSetLayout(descriptorSetLayout), renderPass(renderPass), pipelineCache(pipelineCache), bufferCount(bufferCount)
{
}

SceneRenderFactory::~SceneRenderFactory()
{
}

//
// IDataFactory
//

VkDeviceSize SceneRenderFactory::getBufferCount() const
{
	return bufferCount;
}

IRenderNodeSP SceneRenderFactory::createRenderNode(const ISceneManagerSP& sceneManager)
{
	return IRenderNodeSP(new RenderNode());
}

IRenderSubMeshSP SceneRenderFactory::createRenderSubMesh(const ISceneManagerSP& sceneManager)
{
	return IRenderSubMeshSP(new RenderSubMesh());
}

IRenderMaterialSP SceneRenderFactory::createRenderMaterial(const ISceneManagerSP& sceneManager)
{
	return IRenderMaterialSP(new RenderMaterial());
}

VkBool32 SceneRenderFactory::preparePhongMaterial(const ISceneManagerSP& sceneManager, const IPhongMaterialSP& phongMaterial)
{
	if (!sceneManager.get() || !phongMaterial.get() || (phongMaterial->getRenderMaterialSize() != bufferCount))
	{
		return VK_FALSE;
	}

	// Create all possibilities, even when not used.

	VkDescriptorPoolSize descriptorPoolSize[3]{};

	descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorPoolSize[0].descriptorCount = VKTS_BINDING_UNIFORM_BUFFER_COUNT;

	descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize[1].descriptorCount = VKTS_BINDING_UNIFORM_PHONG_BINDING_COUNT + 2;

	descriptorPoolSize[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorPoolSize[2].descriptorCount = VKTS_BINDING_STORAGE_IMAGE_COUNT;

	for (uint32_t currentBuffer = 0; currentBuffer < (uint32_t)bufferCount; currentBuffer++)
	{
		auto descriptorPool = descriptorPoolCreate(sceneManager->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 3, descriptorPoolSize);

		if (!descriptorPool.get())
		{
			return VK_FALSE;
		}

		phongMaterial->getRenderMaterial(currentBuffer)->setDescriptorPool(descriptorPool);

		//

		auto allDescriptorSetLayouts = descriptorSetLayout->getDescriptorSetLayout();

		auto descriptorSets = descriptorSetsCreate(sceneManager->getContextObject()->getDevice()->getDevice(), descriptorPool->getDescriptorPool(), 1, &allDescriptorSetLayouts);

		if (!descriptorSets.get())
		{
			return VK_FALSE;
		}

		phongMaterial->getRenderMaterial(currentBuffer)->setDescriptorSets(descriptorSets);
	}

    return VK_TRUE;
}

VkBool32 SceneRenderFactory::prepareBSDFMaterial(const ISceneManagerSP& sceneManager, const ISubMeshSP& subMesh)
{
	if (!sceneManager.get() || !subMesh.get() || !subMesh->getRenderSubMesh().get() || !subMesh->getBSDFMaterial().get() || (subMesh->getBSDFMaterial()->getRenderMaterialSize() != bufferCount))
	{
		return VK_FALSE;
	}

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[VKTS_BINDING_UNIFORM_MATERIAL_TOTAL_BINDING_COUNT]{};

	uint32_t bindingCount = 0;

	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

	bindingCount++;

	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;
	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

	bindingCount++;

	if ((subMesh->getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_BONES) == VKTS_VERTEX_BUFFER_TYPE_BONES)
	{
    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;
	}


	// For forward rendering, add more buffers and textureObjects.
	if (subMesh->getBSDFMaterial()->getForwardRendering())
	{
    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_BSDF_FORWARD_LIGHT;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;

    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_BSDF_FORWARD_INVERSE;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;

    	//

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_DIFFUSE;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_SPECULAR;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_LUT;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;
	}

	if (subMesh->getBSDFMaterial()->getNumberTextureObjects() > VKTS_BINDING_UNIFORM_BSDF_BINDING_COUNT)
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Too many textureObjects.");

		return VK_FALSE;
	}

    for (uint32_t i = 0; i < subMesh->getBSDFMaterial()->getNumberTextureObjects(); i++)
    {
    	uint32_t bindingStart = subMesh->getBSDFMaterial()->getForwardRendering() ? VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_FIRST : VKTS_BINDING_UNIFORM_SAMPLER_BSDF_DEFERRED_FIRST;

		descriptorSetLayoutBinding[bindingCount].binding = bindingStart + i;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;
    }

    //

    auto descriptorSetLayout = descriptorSetLayoutCreate(sceneManager->getContextObject()->getDevice()->getDevice(), 0, bindingCount, descriptorSetLayoutBinding);

	if (!descriptorSetLayout.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

		return VK_FALSE;
	}

	subMesh->setDescriptorSetLayout(descriptorSetLayout);

	//
	//
	//

	auto bsdfMaterial = subMesh->getBSDFMaterial();

	// Create all possibilities, even when not used.

	VkDescriptorPoolSize descriptorPoolSize[2]{};

	descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorPoolSize[0].descriptorCount = 5;

	descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize[1].descriptorCount = 18;

	for (uint32_t currentBuffer = 0; currentBuffer < (uint32_t)bufferCount; currentBuffer++)
	{
		auto descriptorPool = descriptorPoolCreate(sceneManager->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 2, descriptorPoolSize);

		if (!descriptorPool.get())
		{
			logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor pool.");

			return VK_FALSE;
		}

		bsdfMaterial->getRenderMaterial(currentBuffer)->setDescriptorPool(descriptorPool);

		//

		const auto allDescriptorSetLayouts = descriptorSetLayout->getDescriptorSetLayout();

		auto descriptorSets = descriptorSetsCreate(sceneManager->getContextObject()->getDevice()->getDevice(), bsdfMaterial->getRenderMaterial(currentBuffer)->getDescriptorPool()->getDescriptorPool(), 1, &allDescriptorSetLayouts);

		if (!descriptorSets.get())
		{
			logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor sets.");

			return VK_FALSE;
		}

		bsdfMaterial->getRenderMaterial(currentBuffer)->setDescriptorSets(descriptorSets);
	}

    //
    //
    //

	VkPushConstantRange pushConstantRange[1];

	pushConstantRange[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange[0].offset = 0;
	pushConstantRange[0].size = sizeof(int32_t) + sizeof(float) + sizeof(float);

	uint32_t finalPushConstantRangeCount = 0;
	VkPushConstantRange* finalPushConstantRange = nullptr;

	if (subMesh->getBSDFMaterial()->getForwardRendering())
	{
		finalPushConstantRangeCount = 1;
		finalPushConstantRange = pushConstantRange;
	}

	//

	VkDescriptorSetLayout setLayouts[1];

	setLayouts[0] = subMesh->getDescriptorSetLayout()->getDescriptorSetLayout();

	auto pipelineLayout = pipelineCreateLayout(sceneManager->getContextObject()->getDevice()->getDevice(), 0, 1, setLayouts, finalPushConstantRangeCount, finalPushConstantRange);

	if (!pipelineLayout.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

		return VK_FALSE;
	}

	subMesh->setPipelineLayout(pipelineLayout);

	// Create graphics pipeline for this sub mesh.

	auto currentPipelineLayout = subMesh->getPipelineLayout();

	if (!currentPipelineLayout.get())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No pipeline layout found");

        return VK_FALSE;
	}


	auto vertexBufferType = subMesh->getVertexBufferType() & subMesh->getBSDFMaterial()->getAttributes();

	if (vertexBufferType != subMesh->getBSDFMaterial()->getAttributes())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh vertex buffer type does not match with material");

        return VK_FALSE;
	}

	if ((subMesh->getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_BONES) == VKTS_VERTEX_BUFFER_TYPE_BONES)
	{
		vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_BONES;
	}

	auto currentVertexShaderModule = sceneManager->useVertexShaderModule(vertexBufferType);

	if (!currentVertexShaderModule.get())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No shader module found");

		return VK_FALSE;
	}

	DefaultGraphicsPipeline gp;

	gp.getPipelineShaderStageCreateInfo(0).stage = VK_SHADER_STAGE_VERTEX_BIT;
	gp.getPipelineShaderStageCreateInfo(0).module = currentVertexShaderModule->getShaderModule();

	gp.getPipelineShaderStageCreateInfo(1).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	gp.getPipelineShaderStageCreateInfo(1).module = subMesh->getBSDFMaterial()->getFragmentShader()->getShaderModule();


	gp.getVertexInputBindingDescription(0).binding = 0;
	gp.getVertexInputBindingDescription(0).stride = alignmentGetStrideInBytes(subMesh->getVertexBufferType());
	gp.getVertexInputBindingDescription(0).inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


	uint32_t location = 0;

	gp.getVertexInputAttributeDescription(location).location = location;
	gp.getVertexInputAttributeDescription(location).binding = 0;
	gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
	gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_VERTEX, subMesh->getVertexBufferType());

	if ((vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_NORMAL) == VKTS_VERTEX_BUFFER_TYPE_NORMAL)
	{
		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_NORMAL, subMesh->getVertexBufferType());

		if ((vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TANGENTS) == VKTS_VERTEX_BUFFER_TYPE_TANGENTS)
		{
			location++;

			gp.getVertexInputAttributeDescription(location).location = location;
			gp.getVertexInputAttributeDescription(location).binding = 0;
			gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32_SFLOAT;
			gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BITANGENT, subMesh->getVertexBufferType());

			location++;

			gp.getVertexInputAttributeDescription(location).location = location;
			gp.getVertexInputAttributeDescription(location).binding = 0;
			gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32_SFLOAT;
			gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_TANGENT, subMesh->getVertexBufferType());
		}
	}

	if ((vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD) == VKTS_VERTEX_BUFFER_TYPE_TEXCOORD)
	{
		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_TEXCOORD, subMesh->getVertexBufferType());
	}


	if ((vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONES) == VKTS_VERTEX_BUFFER_TYPE_BONES)
	{
		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0, subMesh->getVertexBufferType());

		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1, subMesh->getVertexBufferType());

		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0, subMesh->getVertexBufferType());

		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1, subMesh->getVertexBufferType());

		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS, subMesh->getVertexBufferType());
	}

	//

	gp.getPipelineInputAssemblyStateCreateInfo().topology = subMesh->getPrimitiveTopology();

	gp.getViewports(0).x = 0.0f;
	gp.getViewports(0).y = 0.0f;
	gp.getViewports(0).width = 1.0f;
	gp.getViewports(0).height = 1.0f;
	gp.getViewports(0).minDepth = 0.0f;
	gp.getViewports(0).maxDepth = 1.0f;


	gp.getScissors(0).offset.x = 0;
	gp.getScissors(0).offset.y = 0;
	gp.getScissors(0).extent = {1, 1};

	gp.getPipelineRasterizationStateCreateInfo();
	if (!subMesh->getDoubleSided())
	{
		gp.getPipelineRasterizationStateCreateInfo().cullMode = VK_CULL_MODE_BACK_BIT;
	}

	gp.getPipelineMultisampleStateCreateInfo();
	if (subMesh->getBSDFMaterial()->getForwardRendering())
	{
		for (uint32_t i = 0; i < renderPass->getAttachmentCount(); i++)
		{
			if (renderPass->getAttachments()[i].samples > gp.getPipelineMultisampleStateCreateInfo().rasterizationSamples)
			{
				gp.getPipelineMultisampleStateCreateInfo().rasterizationSamples = renderPass->getAttachments()[i].samples;
			}
		}
	}

	gp.getPipelineDepthStencilStateCreateInfo().depthTestEnable = VK_TRUE;
	gp.getPipelineDepthStencilStateCreateInfo().depthWriteEnable = VK_TRUE;
	gp.getPipelineDepthStencilStateCreateInfo().depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

	for (uint32_t i = 0; i < 3; i++)
	{
		if (subMesh->getBSDFMaterial()->getForwardRendering())
		{
			if (subMesh->getBSDFMaterial()->isTransparent())
			{
			    gp.getPipelineColorBlendAttachmentState(i).blendEnable = VK_TRUE;
			    gp.getPipelineColorBlendAttachmentState(i).srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			    gp.getPipelineColorBlendAttachmentState(i).dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			    gp.getPipelineColorBlendAttachmentState(i).colorBlendOp = VK_BLEND_OP_ADD;
			    gp.getPipelineColorBlendAttachmentState(i).srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			    gp.getPipelineColorBlendAttachmentState(i).dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			    gp.getPipelineColorBlendAttachmentState(i).alphaBlendOp = VK_BLEND_OP_ADD;
			}
			else
			{
				gp.getPipelineColorBlendAttachmentState(i).blendEnable = VK_FALSE;
			}
		}
		else
		{
			gp.getPipelineColorBlendAttachmentState(i).blendEnable = VK_FALSE;
		}
		gp.getPipelineColorBlendAttachmentState(i).colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    	// For forward rendering, only one color buffer is attached.
    	if (subMesh->getBSDFMaterial()->getForwardRendering())
    	{
    		break;
    	}
	}

	gp.getDynamicState(0) = VK_DYNAMIC_STATE_VIEWPORT;
	gp.getDynamicState(1) = VK_DYNAMIC_STATE_SCISSOR;


	gp.getGraphicsPipelineCreateInfo().layout = currentPipelineLayout->getPipelineLayout();
	gp.getGraphicsPipelineCreateInfo().renderPass = renderPass->getRenderPass();

	//

	VkPipelineCache pipelineCache = VK_NULL_HANDLE;

	if (this->pipelineCache.get())
	{
		pipelineCache = this->pipelineCache->getPipelineCache();
	}

	//

	auto pipeline = pipelineCreateGraphics(sceneManager->getContextObject()->getDevice()->getDevice(), pipelineCache, gp.getGraphicsPipelineCreateInfo(), vertexBufferType);

	if (!pipeline.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create graphics pipeline.");

		return VK_FALSE;
	}

	subMesh->setGraphicsPipeline(pipeline);

	return VK_TRUE;
}

VkBool32 SceneRenderFactory::prepareTransformUniformBuffer(const ISceneManagerSP& sceneManager, const INodeSP& node)
{
	if (!sceneManager.get() || !node.get())
	{
		return VK_FALSE;
	}

	// mat3 in std140 consumes three vec4 columns.
	VkDeviceSize size = alignmentGetSizeInBytes(16 * sizeof(float) + 12 * sizeof(float), 16);

    auto transformUniformBuffer = createUniformBufferObject(sceneManager->getAssetManager(), size, bufferCount);

    if (!transformUniformBuffer.get())
    {
        return VK_FALSE;
    }

    //

    node->setTransformUniformBuffer(transformUniformBuffer);

	return VK_TRUE;
}

VkDeviceSize SceneRenderFactory::getTransformUniformBufferAlignmentSize(const ISceneManagerSP& sceneManager) const
{
	if (!sceneManager.get())
	{
		return 0;
	}

	auto size = alignmentGetSizeInBytes(16 * sizeof(float) + 12 * sizeof(float), 16);

	//

	return sceneManager->getContextObject()->getPhysicalDevice()->getUniformBufferAlignmentSizeInBytes(size);
}

VkBool32 SceneRenderFactory::prepareJointsUniformBuffer(const ISceneManagerSP& sceneManager, const INodeSP& node, const int32_t joints)
{
	if (!sceneManager.get() || !node.get())
	{
		return VK_FALSE;
	}

    // mat3 in std140 consumes three vec4 columns.
	VkDeviceSize size = alignmentGetSizeInBytes(16 * sizeof(float) * (VKTS_MAX_JOINTS + 1) + 12 * sizeof(float) * (VKTS_MAX_JOINTS + 1), 16);

    auto jointsUniformBuffer = createUniformBufferObject(sceneManager->getAssetManager(), size, bufferCount);

    if (!jointsUniformBuffer.get())
    {
        return VK_FALSE;
    }

    node->setJointsUniformBuffer(joints, jointsUniformBuffer);

	return VK_TRUE;
}

VkDeviceSize SceneRenderFactory::getJointsUniformBufferAlignmentSize(const ISceneManagerSP& sceneManager) const
{
	if (!sceneManager.get())
	{
		return 0;
	}

	auto size = alignmentGetSizeInBytes(16 * sizeof(float) * (VKTS_MAX_JOINTS + 1) + 12 * sizeof(float) * (VKTS_MAX_JOINTS + 1), 16);

	//

	return sceneManager->getContextObject()->getPhysicalDevice()->getUniformBufferAlignmentSizeInBytes(size);
}

SmartPointerVector<IImageDataSP> SceneRenderFactory::prefilter(const ISceneManagerSP& sceneManager, const IImageDataSP& sourceImage, const uint32_t samples, const std::string& name, const VkBool32 useLambert) const
{
    if (name.size() == 0 || !sourceImage.get() || sourceImage->getArrayLayers() != 6 || sourceImage->getDepth() != 1 || sourceImage->getWidth() != sourceImage->getHeight() || samples == 0)
    {
        return SmartPointerVector<IImageDataSP>();
    }

    std::string sourceImageFilename = name;

    auto dotIndex = sourceImageFilename.rfind(".");

    if (dotIndex == sourceImageFilename.npos)
    {
        return SmartPointerVector<IImageDataSP>();
    }

    auto sourceImageName = sourceImageFilename.substr(0, dotIndex);
    auto sourceImageExtension = sourceImageFilename.substr(dotIndex);

    IImageDataSP currentTargetImage;
    std::string targetImageFilename;

    std::vector<std::string> resultNames;

    if (useLambert)
    {
		for (uint32_t layer = 0; layer < 6; layer++)
		{
			targetImageFilename = sourceImageName + "_LEVEL0_LAYER" + std::to_string(layer) + "_LAMBERT" + sourceImageExtension;

			resultNames.push_back(targetImageFilename);
		}
    }
    else
    {
        // Create mip maps for all six layers.
        for (uint32_t layer = 0; layer < 6; layer++)
        {
            int32_t level = 0;

            int32_t width = sourceImage->getWidth();
            int32_t height = sourceImage->getHeight();

    		while (width > 0 || height > 0)
    		{
    			targetImageFilename = sourceImageName + "_LEVEL" + std::to_string(level++) + "_LAYER" + std::to_string(layer) + "_COOKTORRANCE" + sourceImageExtension;

    			resultNames.push_back(targetImageFilename);

    			//

    			width = width / 2;
    			height = height / 2;
    		}
        }
    }

    //
    // Prepare render path.
    //

    const char* vertexFilename = VKTS_PREFILTER_VERTEX_SHADER_NAME;

    auto vertexShaderBinary = fileLoadBinary(vertexFilename);

	if (!vertexShaderBinary.get())
	{
		return SmartPointerVector<IImageDataSP>();
	}

	auto vertexShaderModule = shaderModuleCreate(vertexFilename, sceneManager->getContextObject()->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (const uint32_t*)vertexShaderBinary->getData());

	if (!vertexShaderModule.get())
	{
		return SmartPointerVector<IImageDataSP>();
	}


    const char* fragmentFilename = VKTS_LAMBERT_FRAGMENT_SHADER_NAME;

    if (!useLambert)
    {
    	fragmentFilename = VKTS_COOKTORRANCE_FRAGMENT_SHADER_NAME;
    }

    auto fragmentShaderBinary = fileLoadBinary(fragmentFilename);

	if (!fragmentShaderBinary.get())
	{
		return SmartPointerVector<IImageDataSP>();
	}

	auto fragmentShaderModule = shaderModuleCreate(fragmentFilename, sceneManager->getContextObject()->getDevice()->getDevice(), 0, fragmentShaderBinary->getSize(), (const uint32_t*)fragmentShaderBinary->getData());

	if (!fragmentShaderModule.get())
	{
		return SmartPointerVector<IImageDataSP>();
	}
	//

	auto sourceImageObject = createImageObject(sceneManager->getAssetManager(), "DummyCubeMap", sourceImage, VK_TRUE);

    if (!sourceImageObject.get())
    {
    	return SmartPointerVector<IImageDataSP>();
    }

    sceneManager->addImageObject(sourceImageObject);

	auto sourceTextureObject = createTextureObject(sceneManager->getAssetManager(), "DummyCubeMap", VK_TRUE, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, sourceImageObject);

    if (!sourceTextureObject.get())
    {
    	return SmartPointerVector<IImageDataSP>();
    }

    sceneManager->addTextureObject(sourceTextureObject);

    //

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[1]{};

	descriptorSetLayoutBinding[0].binding = 0;
	descriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetLayoutBinding[0].descriptorCount = 1;
	descriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	descriptorSetLayoutBinding[0].pImmutableSamplers = nullptr;

	auto descriptorSetLayout = descriptorSetLayoutCreate(sceneManager->getContextObject()->getDevice()->getDevice(), 0, 1, descriptorSetLayoutBinding);

	if (!descriptorSetLayout.get())
	{
		return SmartPointerVector<IImageDataSP>();
	}


    VkDescriptorPoolSize descriptorPoolSize[1]{};

    descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSize[0].descriptorCount = 1;

    auto descriptorPool = descriptorPoolCreate(sceneManager->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 1, descriptorPoolSize);

    if (!descriptorPool.get())
    {
    	return SmartPointerVector<IImageDataSP>();
    }


	const VkDescriptorSetLayout currentDescriptorSetLayout = descriptorSetLayout->getDescriptorSetLayout();

	auto descriptorSet = descriptorSetsCreate(sceneManager->getContextObject()->getDevice()->getDevice(), descriptorPool->getDescriptorPool(), 1, &currentDescriptorSetLayout);

	if (!descriptorSet.get())
	{
		return SmartPointerVector<IImageDataSP>();
	}

	//

    VkPushConstantRange pushConstantRange[2]{};

    pushConstantRange[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange[0].offset = 0;
    pushConstantRange[0].size = sizeof(float) * 4 * 4;

    pushConstantRange[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange[1].offset = pushConstantRange[0].size;
    pushConstantRange[1].size = sizeof(uint32_t);
    if (!useLambert)
    {
    	pushConstantRange[1].size += sizeof(float);
    }

	const VkDescriptorSetLayout currentDescriptorSet = descriptorSetLayout->getDescriptorSetLayout();

	auto pipelineLayout = pipelineCreateLayout(sceneManager->getContextObject()->getDevice()->getDevice(), 0, 1, &currentDescriptorSet, 2, pushConstantRange);

	if (!pipelineLayout.get())
	{
		return SmartPointerVector<IImageDataSP>();
	}

	//
	// Render pass
	//

	VkAttachmentDescription attachmentDescription{};

	attachmentDescription.flags = 0;
	attachmentDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentReference{};

	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription{};

	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.pDepthStencilAttachment = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;

	auto renderPass = vkts::renderPassCreate(sceneManager->getContextObject()->getDevice()->getDevice(), 0, 1, &attachmentDescription, 1, &subpassDescription, 0, nullptr);

	if (!renderPass.get())
	{
		return SmartPointerVector<IImageDataSP>();
	}

	//
	// Graphics pipeline setup
	//

    DefaultGraphicsPipeline gp;

    gp.getPipelineShaderStageCreateInfo(0).stage = VK_SHADER_STAGE_VERTEX_BIT;
    gp.getPipelineShaderStageCreateInfo(0).module = vertexShaderModule->getShaderModule();

    gp.getPipelineShaderStageCreateInfo(1).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    gp.getPipelineShaderStageCreateInfo(1).module = fragmentShaderModule->getShaderModule();


    gp.getPipelineInputAssemblyStateCreateInfo().topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;


    gp.getViewports(0).x = 0.0f;
    gp.getViewports(0).y = 0.0f;
    gp.getViewports(0).width = 1.0f;
    gp.getViewports(0).height = 1.0f;
    gp.getViewports(0).minDepth = 0.0f;
    gp.getViewports(0).maxDepth = 1.0f;


    gp.getScissors(0).offset.x = 0;
    gp.getScissors(0).offset.y = 0;
    gp.getScissors(0).extent = {1, 1};


    gp.getPipelineRasterizationStateCreateInfo();

    gp.getPipelineMultisampleStateCreateInfo();

    gp.getPipelineColorBlendAttachmentState(0).colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    gp.getDynamicState(0) = VK_DYNAMIC_STATE_VIEWPORT;
    gp.getDynamicState(1) = VK_DYNAMIC_STATE_SCISSOR;


    gp.getGraphicsPipelineCreateInfo().layout = pipelineLayout->getPipelineLayout();
    gp.getGraphicsPipelineCreateInfo().renderPass = renderPass->getRenderPass();

    //

    VkPipelineCache pipelineCache = VK_NULL_HANDLE;

    if (this->pipelineCache.get())
    {
    	pipelineCache = this->pipelineCache->getPipelineCache();
    }

    //

    auto graphicsPipeline = pipelineCreateGraphics(sceneManager->getContextObject()->getDevice()->getDevice(), pipelineCache, gp.getGraphicsPipelineCreateInfo(), 0);

    if (!graphicsPipeline.get())
    {
    	return SmartPointerVector<IImageDataSP>();
    }

    //
    //

	if (sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->endCommandBuffer() != VK_SUCCESS)
	{
		return SmartPointerVector<IImageDataSP>();
	}

	VkSubmitInfo submitInfo{};

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffers();
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	if (sceneManager->getContextObject()->getQueue()->submit(1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		return SmartPointerVector<IImageDataSP>();
	}

	if (sceneManager->getContextObject()->getQueue()->waitIdle() != VK_SUCCESS)
	{
		return SmartPointerVector<IImageDataSP>();
	}

	if (sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->reset() != VK_SUCCESS)
	{
		return SmartPointerVector<IImageDataSP>();
	}

	//
	//
	//

	uint32_t roughnessSamples = 1;

	if (!useLambert)
	{
		roughnessSamples = (uint32_t)resultNames.size() / 6;
	}

	SmartPointerMap<std::string, IImageDataSP> tempResult;

	if (sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0) != VK_SUCCESS)
	{
		return SmartPointerVector<IImageDataSP>();
	}

	for (uint32_t roughnessSampleIndex = 0; roughnessSampleIndex < roughnessSamples; roughnessSampleIndex++)
	{
		uint32_t imageLength = sourceImage->getWidth() / (1 << roughnessSampleIndex);

		VkImageCreateInfo imageCreateInfo{};

		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		imageCreateInfo.extent = {imageLength, imageLength, 1};
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		auto targetImageObject = imageObjectCreate(sceneManager->getContextObject(), sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer(), "DummyBuffer", imageCreateInfo, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (!targetImageObject.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create GBuffer texture.");

			return VK_FALSE;
		}

		//
		// Framebuffer
		//

		auto imageView = targetImageObject->getImageView()->getImageView();

		auto framebuffer = framebufferCreate(sceneManager->getContextObject()->getDevice()->getDevice(), 0, renderPass->getRenderPass(), 1, &imageView, imageLength, imageLength, 1);

		if (!framebuffer.get())
		{
			return SmartPointerVector<IImageDataSP>();
		}

		//

		VkDescriptorImageInfo descriptorImageInfo[1]{};

		descriptorImageInfo[0].sampler = sourceTextureObject->getSampler()->getSampler();
		descriptorImageInfo[0].imageView = sourceTextureObject->getImageObject()->getImageView()->getImageView();
		descriptorImageInfo[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkWriteDescriptorSet writeDescriptorSet[1]{};

		writeDescriptorSet[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

		writeDescriptorSet[0].dstSet = descriptorSet->getDescriptorSets()[0];
		writeDescriptorSet[0].dstBinding = 0;
		writeDescriptorSet[0].dstArrayElement = 0;
		writeDescriptorSet[0].descriptorCount = 1;
		writeDescriptorSet[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSet[0].pImageInfo = &descriptorImageInfo[0];
		writeDescriptorSet[0].pBufferInfo = nullptr;
		writeDescriptorSet[0].pTexelBufferView = nullptr;

		descriptorSet->updateDescriptorSets(1, writeDescriptorSet, 0, nullptr);

		//

		float roughness = 0.0f;

		if (!useLambert)
		{
			roughness = (float)roughnessSampleIndex / (float)(roughnessSamples - 1);
		}

		for (uint32_t side = 0; side < 6; side++)
		{
        	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

        	targetImageObject->getImage()->cmdPipelineBarrier(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, imageSubresourceRange);

            //

        	VkClearColorValue clearColorValue{};

        	clearColorValue.float32[0] = 0.0f;
        	clearColorValue.float32[1] = 0.0f;
        	clearColorValue.float32[2] = 1.0f;
        	clearColorValue.float32[3] = 1.0f;

        	VkClearValue clearValues[1] = {clearColorValue};

        	VkRenderPassBeginInfo renderPassBeginInfo{};

        	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        	renderPassBeginInfo.renderPass = renderPass->getRenderPass();
        	renderPassBeginInfo.framebuffer = framebuffer->getFramebuffer();
        	renderPassBeginInfo.renderArea.offset.x = 0;
        	renderPassBeginInfo.renderArea.offset.y = 0;
        	renderPassBeginInfo.renderArea.extent = {imageLength, imageLength};
        	renderPassBeginInfo.clearValueCount = 1;
        	renderPassBeginInfo.pClearValues = clearValues;

        	sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->cmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        	//

        	vkCmdBindPipeline(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

        	//

        	VkViewport viewport{};

        	viewport.x = 0.0f;
        	viewport.y = 0.0f;
        	viewport.width = (float)imageLength;
        	viewport.height = (float)imageLength;
        	viewport.minDepth = 0.0f;
        	viewport.maxDepth = 1.0f;

        	vkCmdSetViewport(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), 0, 1, &viewport);


        	VkRect2D scissor{};

        	scissor.offset.x = 0;
        	scissor.offset.y = 0;
        	scissor.extent = {imageLength, imageLength};

        	vkCmdSetScissor(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), 0, 1, &scissor);

        	//

        	vkCmdBindDescriptorSets(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->getPipelineLayout(), 0, 1, descriptorSet->getDescriptorSets(), 0, nullptr);

        	//
        	//

        	// Push side normals.
        	float normals[(3 + 1) * 4]{};

        	for (uint32_t normalIndex = 0; normalIndex < 4; normalIndex++)
        	{
				glm::vec3 currentNormal = glm::vec3(0.0f, 0.0f, 0.0f);

				float s = (float)(normalIndex % 2) * 2.0f - 1.0f;
				float t = (float)(normalIndex / 2) * 2.0f - 1.0f;

				switch (side)
				{
					case 0:
						currentNormal.x = 1.0f;

						currentNormal.y = -t;
						currentNormal.z = -s;
						break;
					case 1:
						currentNormal.x = -1.0f;

						currentNormal.y = -t;
						currentNormal.z = s;
						break;
					case 2:
						currentNormal.y = 1.0f;

						currentNormal.x = s;
						currentNormal.z = t;
						break;
					case 3:
						currentNormal.y = -1.0f;

						currentNormal.x = s;
						currentNormal.z = -t;
						break;
					case 4:
						currentNormal.z = 1.0f;

						currentNormal.x = s;
						currentNormal.y = -t;
						break;
					case 5:
						currentNormal.z = -1.0f;

						currentNormal.x = -s;
						currentNormal.y = -t;
						break;
					default:
						return SmartPointerVector<IImageDataSP>();
				}

				currentNormal = glm::normalize(currentNormal);

				normals[normalIndex * 4 + 0] = currentNormal.x;
				normals[normalIndex * 4 + 1] = currentNormal.y;
				normals[normalIndex * 4 + 2] = currentNormal.z;
				normals[normalIndex * 4 + 3] = 0.0f;
        	}

        	vkCmdPushConstants(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), pipelineLayout->getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(normals), normals);

        	//
        	//


        	vkCmdPushConstants(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), pipelineLayout->getPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, pushConstantRange[0].size, sizeof(uint32_t), &samples);
        	if (!useLambert)
        	{
        		vkCmdPushConstants(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), pipelineLayout->getPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, pushConstantRange[0].size + sizeof(uint32_t), sizeof(float), &roughness);
        	}

        	// Draw.
        	vkCmdDraw(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), 4, 1, 0, 0);

        	sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->cmdEndRenderPass();

    		// Prepare target image for final layout etc.
        	targetImageObject->getImage()->cmdPipelineBarrier(sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffer(), VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageSubresourceRange);

        	//

        	if (sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->endCommandBuffer() != VK_SUCCESS)
        	{
        		return SmartPointerVector<IImageDataSP>();
        	}

        	//

    		VkSubmitInfo submitInfo{};

    		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    		submitInfo.waitSemaphoreCount = 0;
    		submitInfo.pWaitSemaphores = nullptr;
    		submitInfo.pWaitDstStageMask = nullptr;
    		submitInfo.commandBufferCount = 1;
    		submitInfo.pCommandBuffers = sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->getCommandBuffers();
    		submitInfo.signalSemaphoreCount = 0;
    		submitInfo.pSignalSemaphores = nullptr;

    		if (sceneManager->getContextObject()->getQueue()->submit(1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    		{
    			return SmartPointerVector<IImageDataSP>();
    		}

    		if (sceneManager->getContextObject()->getQueue()->waitIdle() != VK_SUCCESS)
    		{
    			return SmartPointerVector<IImageDataSP>();
    		}

    		if (sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->reset() != VK_SUCCESS)
    		{
    			return SmartPointerVector<IImageDataSP>();
    		}

    		auto currentImageData = imageObjectGetDeviceImageData(sceneManager->getContextObject(), sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer(), resultNames[side * roughnessSamples + roughnessSampleIndex], targetImageObject->getImage());

    		currentImageData = imageDataConvert(currentImageData, sourceImage->getFormat(), currentImageData->getName());

    		if (!currentImageData.get())
    		{
    			return SmartPointerVector<IImageDataSP>();
    		}

    		tempResult[resultNames[side * roughnessSamples + roughnessSampleIndex]] = currentImageData;

    		if (sceneManager->getAssetManager()->getCommandObject()->getCommandBuffer()->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0) != VK_SUCCESS)
    		{
    			return SmartPointerVector<IImageDataSP>();
    		}
		}
    }

	SmartPointerVector<IImageDataSP> result;

	for (uint32_t i = 0; i < (uint32_t)resultNames.size(); i++)
	{
		result.append(tempResult[resultNames[i]]);
	}

    return result;
}

SmartPointerVector<IImageDataSP> SceneRenderFactory::prefilterLambert(const ISceneManagerSP& sceneManager, const IImageDataSP& sourceImage, const uint32_t samples, const std::string& name) const
{
	return prefilter(sceneManager, sourceImage, samples, name, VK_TRUE);
}

SmartPointerVector<IImageDataSP> SceneRenderFactory::prefilterCookTorrance(const ISceneManagerSP& sceneManager, const IImageDataSP& sourceImage, const uint32_t samples, const std::string& name) const
{
	return prefilter(sceneManager, sourceImage, samples, name, VK_FALSE);
}

} /* namespace vkts */
