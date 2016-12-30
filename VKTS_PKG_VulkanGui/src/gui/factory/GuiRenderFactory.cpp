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

#include "GuiRenderFactory.hpp"

#include "../font/RenderFont.hpp"

#define VKTS_BINDING_VERTEX_BUFFER 0

#define VKTS_FONT_VERTEX_SHADER_NAME 		"shader/SPIR/V/font.vert.spv"
#define VKTS_FONT_FRAGMENT_SHADER_NAME 		"shader/SPIR/V/font.frag.spv"

#define VKTS_FONT_DF_VERTEX_SHADER_NAME 	"shader/SPIR/V/font_df.vert.spv"
#define VKTS_FONT_DF_FRAGMENT_SHADER_NAME 	"shader/SPIR/V/font_df.frag.spv"

namespace vkts
{

GuiRenderFactory::GuiRenderFactory(const IRenderPassSP& renderPass) :
	IGuiRenderFactory(), renderPass(renderPass)
{
}

GuiRenderFactory::~GuiRenderFactory()
{
}

//
// IDataFactory
//

IRenderFontSP GuiRenderFactory::createRenderFont(const IGuiManagerSP& guiManager, const IFont& font)
{
    //
    // Create a quad out of a triangle strip.
    //

    const float vertexData[4*4] = {
		0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
    };

    auto binaryBuffer = binaryBufferCreate((const uint8_t*)vertexData, sizeof(vertexData));

    if (!binaryBuffer.get())
    {
    	return IRenderFontSP();
    }

    //

    VkBufferCreateInfo bufferCreateInfo{};

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferCreateInfo.size = sizeof(vertexData);
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    IDeviceMemorySP stageDeviceMemory;
    IBufferSP stageBuffer;

    auto vertexBuffer = bufferObjectCreate(stageBuffer, stageDeviceMemory, guiManager->getContextObject(), guiManager->getCommandObject()->getCommandBuffer(), binaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    guiManager->getCommandObject()->addStageDeviceMemory(stageDeviceMemory);
    guiManager->getCommandObject()->addStageBuffer(stageBuffer);

    if (!vertexBuffer.get())
    {
    	return IRenderFontSP();
    }

    //
    // Shader modules.
    //

    const char* vertexShaderFilename = font.isDistanceField() ? VKTS_FONT_DF_VERTEX_SHADER_NAME  : VKTS_FONT_VERTEX_SHADER_NAME;

	auto vertexShaderBinary = fileLoadBinary(vertexShaderFilename);

	if (!vertexShaderBinary.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", vertexShaderFilename);

		return IRenderFontSP();
	}

	const char* fragementShaderFilename = font.isDistanceField() ? VKTS_FONT_DF_FRAGMENT_SHADER_NAME  : VKTS_FONT_FRAGMENT_SHADER_NAME;

	auto fragmentShaderBinary = fileLoadBinary(fragementShaderFilename);

	if (!fragmentShaderBinary.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load fragment shader: '%s'", fragementShaderFilename);

		return IRenderFontSP();
	}

	//

	auto vertexShaderModule = shaderModuleCreate(VKTS_FONT_VERTEX_SHADER_NAME, guiManager->getContextObject()->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!vertexShaderModule.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return IRenderFontSP();
	}

	// No need to store.

	auto fragmentShaderModule = shaderModuleCreate(VKTS_FONT_FRAGMENT_SHADER_NAME, guiManager->getContextObject()->getDevice()->getDevice(), 0, fragmentShaderBinary->getSize(), (uint32_t*)fragmentShaderBinary->getData());

	if (!fragmentShaderModule.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

		return IRenderFontSP();
	}

	// No need to store.

	//
	// Descriptor set layout.
	//

	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};

	descriptorSetLayoutBinding.binding = 0;
	descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetLayoutBinding.descriptorCount = 1;
	descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

    //

    auto descriptorSetLayout = descriptorSetLayoutCreate(guiManager->getContextObject()->getDevice()->getDevice(), 0, 1, &descriptorSetLayoutBinding);

	if (!descriptorSetLayout.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

		return IRenderFontSP();
	}

	//

    VkDescriptorPoolSize descriptorPoolSize{};

    descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSize.descriptorCount = 1;

    auto descriptorPool = descriptorPoolCreate(guiManager->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 1, &descriptorPoolSize);

    if (!descriptorPool.get())
    {
    	return IRenderFontSP();
    }

	//

    auto allDescriptorSetLayouts = descriptorSetLayout->getDescriptorSetLayout();

	auto descriptorSets = descriptorSetsCreate(guiManager->getContextObject()->getDevice()->getDevice(), descriptorPool->getDescriptorPool(), 1, &allDescriptorSetLayouts);

    if (!descriptorSets.get())
    {
    	return IRenderFontSP();
    }

	//

	VkDescriptorImageInfo descriptorImageInfo{};

	descriptorImageInfo.sampler = font.getTextureObject()->getSampler()->getSampler();
	descriptorImageInfo.imageView = font.getTextureObject()->getImageObject()->getImageView()->getImageView();
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkWriteDescriptorSet writeDescriptorSet{};

	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSet.dstSet = descriptorSets->getDescriptorSets()[0];
	writeDescriptorSet.dstBinding = 0;
	writeDescriptorSet.dstArrayElement = 0;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeDescriptorSet.pImageInfo = &descriptorImageInfo;
	writeDescriptorSet.pBufferInfo = nullptr;
	writeDescriptorSet.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(guiManager->getContextObject()->getDevice()->getDevice(), 1, &writeDescriptorSet, 0, nullptr);

	//
	// Pipeline layout.
	//

    // Using push constants for displacement.

    VkPushConstantRange pushConstantRange[1];

    pushConstantRange[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange[0].offset = 0;
    pushConstantRange[0].size = 16 * sizeof(float) + 2 * sizeof(float) * 4 + 4 * sizeof(float);

    if (font.isDistanceField())
    {
    	pushConstantRange[0].size += 1 * (uint32_t)sizeof(float);
    }

	VkDescriptorSetLayout setLayouts[1];

	setLayouts[0] = descriptorSetLayout->getDescriptorSetLayout();

	auto pipelineLayout = pipelineCreateLayout(guiManager->getContextObject()->getDevice()->getDevice(), 0, 1, setLayouts, 1, pushConstantRange);

	if (!pipelineLayout.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

		return  IRenderFontSP();
	}

    //
	// Graphics pipeline.
	//

    DefaultGraphicsPipeline gp;

    gp.getPipelineShaderStageCreateInfo(0).stage = VK_SHADER_STAGE_VERTEX_BIT;
    gp.getPipelineShaderStageCreateInfo(0).module = vertexShaderModule->getShaderModule();

    gp.getPipelineShaderStageCreateInfo(1).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    gp.getPipelineShaderStageCreateInfo(1).module = fragmentShaderModule->getShaderModule();


	VkTsVertexBufferType vertexBufferType = VKTS_VERTEX_BUFFER_TYPE_VERTEX;

    gp.getVertexInputBindingDescription(0).binding = VKTS_BINDING_VERTEX_BUFFER;
    gp.getVertexInputBindingDescription(0).stride = alignmentGetStrideInBytes(vertexBufferType);
    gp.getVertexInputBindingDescription(0).inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    gp.getVertexInputAttributeDescription(0).location = 0;
    gp.getVertexInputAttributeDescription(0).binding = VKTS_BINDING_VERTEX_BUFFER;
    gp.getVertexInputAttributeDescription(0).format = VK_FORMAT_R32G32B32A32_SFLOAT;
    gp.getVertexInputAttributeDescription(0).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_VERTEX, vertexBufferType);


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


    gp.getPipelineMultisampleStateCreateInfo();


    gp.getPipelineColorBlendAttachmentState(0).blendEnable = VK_TRUE;
    gp.getPipelineColorBlendAttachmentState(0).srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    gp.getPipelineColorBlendAttachmentState(0).dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    gp.getPipelineColorBlendAttachmentState(0).colorBlendOp = VK_BLEND_OP_ADD;
    gp.getPipelineColorBlendAttachmentState(0).srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    gp.getPipelineColorBlendAttachmentState(0).dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    gp.getPipelineColorBlendAttachmentState(0).alphaBlendOp = VK_BLEND_OP_ADD;
    gp.getPipelineColorBlendAttachmentState(0).colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;


    gp.getDynamicState(0) = VK_DYNAMIC_STATE_VIEWPORT;
    gp.getDynamicState(1) = VK_DYNAMIC_STATE_SCISSOR;


    gp.getGraphicsPipelineCreateInfo().layout = pipelineLayout->getPipelineLayout();
    gp.getGraphicsPipelineCreateInfo().renderPass = renderPass->getRenderPass();


    auto graphicsPipeline = pipelineCreateGraphics(guiManager->getContextObject()->getDevice()->getDevice(), VK_NULL_HANDLE, gp.getGraphicsPipelineCreateInfo(), VKTS_VERTEX_BUFFER_TYPE_VERTEX);

    if (!graphicsPipeline.get())
    {
    	return IRenderFontSP();
    }

    auto renderFont = new RenderFont();

    if (!renderFont)
    {
    	return IRenderFontSP();
    }

    renderFont->setVertexBuffer(vertexBuffer);
    renderFont->setDescriptorSetLayout(descriptorSetLayout);
    renderFont->setDescriptorPool(descriptorPool);
    renderFont->setDescriptorSets(descriptorSets);
    renderFont->setPipelineLayout(pipelineLayout);
    renderFont->setGraphicsPipeline(graphicsPipeline);

	return IRenderFontSP(renderFont);
}

} /* namespace vkts */
