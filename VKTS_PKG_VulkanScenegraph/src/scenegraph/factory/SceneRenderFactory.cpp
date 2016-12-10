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

namespace vkts
{

SceneRenderFactory::SceneRenderFactory(const IDescriptorSetLayoutSP& descriptorSetLayout, const IRenderPassSP& renderPass, const uint64_t buffers) :
	ISceneRenderFactory(), descriptorSetLayout(descriptorSetLayout), renderPass(renderPass), buffers(buffers)
{
}

SceneRenderFactory::~SceneRenderFactory()
{
}

//
// IDataFactory
//

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
	if (!sceneManager.get() || !phongMaterial.get() || !phongMaterial->getRenderMaterial().get())
	{
		return VK_FALSE;
	}

    // Create all possibilities, even when not used.

    VkDescriptorPoolSize descriptorPoolSize[3]{};

	descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSize[0].descriptorCount = VKTS_BINDING_UNIFORM_BUFFER_COUNT;

	descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize[1].descriptorCount = VKTS_BINDING_UNIFORM_PHONG_BINDING_COUNT + 2;

	descriptorPoolSize[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorPoolSize[2].descriptorCount = VKTS_BINDING_UNIFORM_VOXEL_COUNT;

    auto descriptorPool = descriptorPoolCreate(sceneManager->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 3, descriptorPoolSize);

    if (!descriptorPool.get())
    {
    	return VK_FALSE;
    }

    phongMaterial->getRenderMaterial()->setDescriptorPool(descriptorPool);

    //

    auto allDescriptorSetLayouts = descriptorSetLayout->getDescriptorSetLayout();

    auto descriptorSets = descriptorSetsCreate(sceneManager->getContextObject()->getDevice()->getDevice(), descriptorPool->getDescriptorPool(), 1, &allDescriptorSetLayouts);

    if (!descriptorSets.get())
    {
    	return VK_FALSE;
    }

    phongMaterial->getRenderMaterial()->setDescriptorSets(descriptorSets);

    return VK_TRUE;
}

VkBool32 SceneRenderFactory::prepareBSDFMaterial(const ISceneManagerSP& sceneManager, const ISubMeshSP& subMesh)
{
	if (!sceneManager.get() || !subMesh.get() || !subMesh->getRenderSubMesh().get() || !subMesh->getBSDFMaterial().get() || !subMesh->getBSDFMaterial()->getRenderMaterial().get())
	{
		return VK_FALSE;
	}

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[VKTS_BINDING_UNIFORM_MATERIAL_TOTAL_BINDING_COUNT]{};

	uint32_t bindingCount = 0;

	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

	bindingCount++;

	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;
	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

	bindingCount++;

	if ((subMesh->getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_BONES) == VKTS_VERTEX_BUFFER_TYPE_BONES)
	{
    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;
	}


	// For forward rendering, add more buffers and textureObjects.
	if (subMesh->getBSDFMaterial()->getForwardRendering())
	{
    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_LIGHT;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;

    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_BSDF_INVERSE;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;

    	//

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_DIFFUSE;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_SPECULAR;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_LUT;
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

    for (size_t i = 0; i < subMesh->getBSDFMaterial()->getNumberTextureObjects(); i++)
    {
    	uint32_t bindingStart = subMesh->getBSDFMaterial()->getForwardRendering() ? VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_FIRST : VKTS_BINDING_UNIFORM_SAMPLER_BSDF_DEFERRED_FIRST;

		descriptorSetLayoutBinding[bindingCount].binding = bindingStart + (uint32_t)i;
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

	descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSize[0].descriptorCount = 5;

	descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize[1].descriptorCount = 18;

	auto descriptorPool = descriptorPoolCreate(sceneManager->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 2, descriptorPoolSize);

	if (!descriptorPool.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor pool.");

		return VK_FALSE;
	}

	bsdfMaterial->getRenderMaterial()->setDescriptorPool(descriptorPool);

	//

	const auto allDescriptorSetLayouts = descriptorSetLayout->getDescriptorSetLayout();

	auto descriptorSets = descriptorSetsCreate(sceneManager->getContextObject()->getDevice()->getDevice(), bsdfMaterial->getRenderMaterial()->getDescriptorPool()->getDescriptorPool(), 1, &allDescriptorSetLayouts);

	if (!descriptorSets.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor sets.");

		return VK_FALSE;
	}

	bsdfMaterial->getRenderMaterial()->setDescriptorSets(descriptorSets);

    //
    //
    //

	VkDescriptorSetLayout setLayouts[1];

	setLayouts[0] = subMesh->getDescriptorSetLayout()->getDescriptorSetLayout();

	auto pipelineLayout = pipelineCreateLayout(sceneManager->getContextObject()->getDevice()->getDevice(), 0, 1, setLayouts, 0, nullptr);

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


	gp.getPipelineMultisampleStateCreateInfo();

	gp.getPipelineDepthStencilStateCreateInfo().depthTestEnable = VK_TRUE;
	gp.getPipelineDepthStencilStateCreateInfo().depthWriteEnable = VK_TRUE;
	gp.getPipelineDepthStencilStateCreateInfo().depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

	for (uint32_t i = 0; i < 3; i++)
	{
		gp.getPipelineColorBlendAttachmentState(i).blendEnable = VK_FALSE;
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


	auto pipeline = pipelineCreateGraphics(sceneManager->getContextObject()->getDevice()->getDevice(), VK_NULL_HANDLE, gp.getGraphicsPipelineCreateInfo(), vertexBufferType);

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

    auto transformUniformBuffer = createUniformBufferObject(sceneManager->getAssetManager(), size * (VkDeviceSize)buffers);

    if (!transformUniformBuffer.get())
    {
        return VK_FALSE;
    }

    //

    node->setTransformUniformBuffer(transformUniformBuffer);

	return VK_TRUE;
}

VkBool32 SceneRenderFactory::prepareJointsUniformBuffer(const ISceneManagerSP& sceneManager, const INodeSP& node, const int32_t joints)
{
	if (!sceneManager.get() || !node.get())
	{
		return VK_FALSE;
	}

    // mat3 in std140 consumes three vec4 columns.
	VkDeviceSize size = alignmentGetSizeInBytes(16 * sizeof(float) * (VKTS_MAX_JOINTS + 1) + 12 * sizeof(float) * (VKTS_MAX_JOINTS + 1), 16);

    auto jointsUniformBuffer = createUniformBufferObject(sceneManager->getAssetManager(), size * (VkDeviceSize)buffers);

    if (!jointsUniformBuffer.get())
    {
        return VK_FALSE;
    }

    node->setJointsUniformBuffer(joints, jointsUniformBuffer);

	return VK_TRUE;
}

} /* namespace vkts */
