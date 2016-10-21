/**
 * VKTS Examples - Examples for Vulkan using VulKan ToolS.
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

#include "Example.hpp"

Example::Example(const vkts::IInitialResourcesSP& initialResources, const int32_t windowIndex, const vkts::ISurfaceSP& surface) :
		IUpdateThread(), initialResources(initialResources), windowIndex(windowIndex), surface(surface), showStats(VK_TRUE), camera(nullptr), inputController(nullptr), allUpdateables(), commandPool(nullptr), imageAcquiredSemaphore(nullptr), renderingCompleteSemaphore(nullptr), environmentDescriptorSetLayout(nullptr), resolveDescriptorSetLayout(nullptr), resolveDescriptorPool(nullptr), resolveDescriptorSet(nullptr), vertexViewProjectionUniformBuffer(nullptr), environmentVertexViewProjectionUniformBuffer(nullptr), resolveFragmentMatricesUniformBuffer(nullptr), allBSDFVertexShaderModules(), envVertexShaderModule(nullptr), envFragmentShaderModule(nullptr), resolveVertexShaderModule(nullptr), resolveFragmentShaderModule(nullptr), environmentPipelineLayout(nullptr), resolvePipelineLayout(nullptr), font(nullptr), sceneContext(nullptr), scene(nullptr), environmentSceneContext(nullptr), environmentScene(nullptr), screenPlaneVertexBuffer(nullptr), swapchain(nullptr), renderPass(nullptr), gbufferRenderPass(nullptr), allGraphicsPipelines(), resolveGraphicsPipeline(nullptr), allGBufferTextures(), allGBufferImageViews(), gbufferSampler(nullptr), swapchainImagesCount(0), swapchainImageView(), gbufferFramebuffer(), framebuffer(), cmdBuffer(), rebuildCmdBufferCounter(0), fps(0), ram(0), cpuUsageApp(0.0f), processors(0)
{
	processors = glm::min(vkts::processorGetNumber(), VKTS_MAX_CORES);

	for (uint32_t i = 0; i < processors; i++)
	{
		cpuUsage[i] = 0.0f;
	}
}

Example::~Example()
{
}

VkBool32 Example::buildCmdBuffer(const int32_t usedBuffer)
{
	VkResult result;

	if (cmdBuffer[usedBuffer].get())
	{
		result = cmdBuffer[usedBuffer]->reset();

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not reset command buffer.");

			return VK_FALSE;
		}
	}
	else
	{
		cmdBuffer[usedBuffer] = vkts::commandBuffersCreate(initialResources->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

		if (!cmdBuffer[usedBuffer].get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

			return VK_FALSE;
		}
	}

	result = cmdBuffer[usedBuffer]->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

		return VK_FALSE;
	}

    //

    swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, usedBuffer);

    //

	VkViewport viewport{};

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapchain->getImageExtent().width;
	viewport.height = (float) swapchain->getImageExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};

	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = swapchain->getImageExtent();

    //
    // Gbuffer pass.
    //

	VkClearColorValue clearColorValue{};

	clearColorValue.float32[0] = 0.0f;
	clearColorValue.float32[1] = 0.0f;
	clearColorValue.float32[2] = 0.0f;
	clearColorValue.float32[3] = 0.0f;

	VkClearDepthStencilValue clearDepthStencilValue{};

	clearDepthStencilValue.depth = 1.0f;
	clearDepthStencilValue.stencil = 0;

	VkClearValue clearValues[4]{};

	clearValues[0].color = clearColorValue;
	clearValues[1].color = clearColorValue;
	clearValues[2].color = clearColorValue;
	clearValues[3].depthStencil = clearDepthStencilValue;


	VkRenderPassBeginInfo renderPassBeginInfo{};

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	renderPassBeginInfo.renderPass = gbufferRenderPass->getRenderPass();
	renderPassBeginInfo.framebuffer = gbufferFramebuffer[usedBuffer]->getFramebuffer();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = swapchain->getImageExtent();
	renderPassBeginInfo.clearValueCount = 4;
	renderPassBeginInfo.pClearValues = clearValues;

	cmdBuffer[usedBuffer]->cmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdSetViewport(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &viewport);

	vkCmdSetScissor(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &scissor);

	if (scene.get())
	{
		vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> empty;

		scene->bindDrawIndexedRecursive(cmdBuffer[usedBuffer], empty);
	}

	cmdBuffer[usedBuffer]->cmdEndRenderPass();

	//
	// Barrier, that GBuffer can be read.
	//

	VkImageSubresourceRange colorSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

	for (uint32_t i = 0; i < 3; i++)
	{
		allGBufferTextures[i]->getImage()->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, colorSubresourceRange);
	}

	VkImageSubresourceRange depthSubresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};

	allGBufferTextures[3]->getImage()->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, depthSubresourceRange);

    //
    // Default pass.
    //

	memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	renderPassBeginInfo.renderPass = renderPass->getRenderPass();
	renderPassBeginInfo.framebuffer = framebuffer[usedBuffer]->getFramebuffer();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = swapchain->getImageExtent();
	renderPassBeginInfo.clearValueCount = 0;
	renderPassBeginInfo.pClearValues = nullptr;

	cmdBuffer[usedBuffer]->cmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdSetViewport(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &viewport);

	vkCmdSetScissor(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &scissor);

	// Render cube map.

	if (environmentScene.get())
	{
		environmentScene->bindDrawIndexedRecursive(cmdBuffer[usedBuffer], allGraphicsPipelines);
	}

	//
	// Render main scene by resolving content.
	//

	vkCmdBindPipeline(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, resolveGraphicsPipeline->getPipeline());

	vkCmdBindDescriptorSets(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, resolvePipelineLayout->getPipelineLayout(), 0, 1, resolveDescriptorSet->getDescriptorSets(), 0, nullptr);

	VkDeviceSize offsets[1] = { 0 };
	VkBuffer buffers[1] = { screenPlaneVertexBuffer->getBuffer()->getBuffer() };

	vkCmdBindVertexBuffers(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, buffers, offsets);

	vkCmdDraw(cmdBuffer[usedBuffer]->getCommandBuffer(), 4, 1, 0, 0);

	// Render font.

	if (font.get() && showStats)
	{
		glm::mat4 projectionMatrix = vkts::orthoMat4((float)swapchain->getImageExtent().width * -0.5f, (float)swapchain->getImageExtent().width * 0.5f, (float)swapchain->getImageExtent().height * -0.5f, (float)swapchain->getImageExtent().height  * 0.5f, 0.0f, 100.0f);

		char buffer[VKTS_OUTPUT_BUFFER_SIZE];

		sprintf(buffer, "Example FPS: %u\nExample RAM: %" SCNu64 " kb\nExample CPU: %.2f%%", fps, ram, cpuUsageApp);

		float y = (float)swapchain->getImageExtent().height * 0.5f - 10.0f - font->getLineHeight(VKTS_FONT_SCALE);

		font->drawText(cmdBuffer[usedBuffer], projectionMatrix, glm::vec2((float)swapchain->getImageExtent().width * -0.5f + 10.0f, y), buffer, VKTS_FONT_SCALE, glm::vec4(0.64f, 0.12f, 0.13f, 1.0f));

		y -= font->getLineHeight(VKTS_FONT_SCALE) * 4.0f;

		for (uint32_t cpu = 0; cpu < processors; cpu++)
		{
			sprintf(buffer, "CPU%u: %.2f%%", cpu, cpuUsage[cpu]);

			font->drawText(cmdBuffer[usedBuffer], projectionMatrix, glm::vec2((float)swapchain->getImageExtent().width * -0.5f + 10.0f, y), buffer, VKTS_FONT_SCALE, glm::vec4(0.64f, 0.12f, 0.13f, 1.0f));

			y -= font->getLineHeight(VKTS_FONT_SCALE);
		}
	}

	cmdBuffer[usedBuffer]->cmdEndRenderPass();

    //

    swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, usedBuffer);

	//
	// Barrier, that GBuffer can be written.
	//

	for (uint32_t i = 0; i < 3; i++)
	{
		allGBufferTextures[i]->getImage()->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, colorSubresourceRange);
	}

	allGBufferTextures[3]->getImage()->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, depthSubresourceRange);

    //

	result = cmdBuffer[usedBuffer]->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildFramebuffer(const int32_t usedBuffer)
{
	VkImageView imageViews[6];

	for (size_t i = 0; i < allGBufferImageViews.size(); i++)
	{
		imageViews[i] = allGBufferImageViews[i]->getImageView();
	}

	gbufferFramebuffer[usedBuffer] = vkts::framebufferCreate(initialResources->getDevice()->getDevice(), 0, gbufferRenderPass->getRenderPass(), (uint32_t)allGBufferImageViews.size(), imageViews, swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1);

	if (!gbufferFramebuffer[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create frame buffer.");

		return VK_FALSE;
	}

	//

	imageViews[0] = swapchainImageView[usedBuffer]->getImageView();

	framebuffer[usedBuffer] = vkts::framebufferCreate(initialResources->getDevice()->getDevice(), 0, renderPass->getRenderPass(), 1, imageViews, swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1);

	if (!framebuffer[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create frame buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::updateDescriptorSets()
{
	memset(environmentDescriptorBufferInfos, 0, sizeof(environmentDescriptorBufferInfos));

	environmentDescriptorBufferInfos[0].buffer = environmentVertexViewProjectionUniformBuffer->getBuffer()->getBuffer();
	environmentDescriptorBufferInfos[0].offset = 0;
	environmentDescriptorBufferInfos[0].range = environmentVertexViewProjectionUniformBuffer->getBuffer()->getSize();


	memset(environmentDescriptorImageInfos, 0, sizeof(environmentDescriptorImageInfos));

	environmentDescriptorImageInfos[0].sampler = scene->getEnvironment()->getSampler()->getSampler();
	environmentDescriptorImageInfos[0].imageView = scene->getEnvironment()->getImageView()->getImageView();
	environmentDescriptorImageInfos[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;


	memset(environmentWriteDescriptorSets, 0, sizeof(environmentWriteDescriptorSets));

	environmentWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	environmentWriteDescriptorSets[0].dstSet = VK_NULL_HANDLE;	// Defined later.
	environmentWriteDescriptorSets[0].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	environmentWriteDescriptorSets[0].dstArrayElement = 0;
	environmentWriteDescriptorSets[0].descriptorCount = 1;
	environmentWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	environmentWriteDescriptorSets[0].pImageInfo = nullptr;
	environmentWriteDescriptorSets[0].pBufferInfo = &environmentDescriptorBufferInfos[0];
	environmentWriteDescriptorSets[0].pTexelBufferView = nullptr;


	environmentWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	environmentWriteDescriptorSets[1].dstSet = VK_NULL_HANDLE;	// Defined later.
	environmentWriteDescriptorSets[1].dstBinding = VKTS_BINDING_UNIFORM_SAMPLER_ENVIRONMENT;
	environmentWriteDescriptorSets[1].dstArrayElement = 0;
	environmentWriteDescriptorSets[1].descriptorCount = 1;
	environmentWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	environmentWriteDescriptorSets[1].pImageInfo = &environmentDescriptorImageInfos[0];
	environmentWriteDescriptorSets[1].pBufferInfo = nullptr;
	environmentWriteDescriptorSets[1].pTexelBufferView = nullptr;


	environmentWriteDescriptorSets[2].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;

	//
	//

	memset(descriptorBufferInfos, 0, sizeof(descriptorBufferInfos));

	descriptorBufferInfos[0].buffer = vertexViewProjectionUniformBuffer->getBuffer()->getBuffer();
	descriptorBufferInfos[0].offset = 0;
	descriptorBufferInfos[0].range = vertexViewProjectionUniformBuffer->getBuffer()->getSize();


	memset(writeDescriptorSets, 0, sizeof(writeDescriptorSets));

	writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[0].dstSet = VK_NULL_HANDLE;	// Defined later.
	writeDescriptorSets[0].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	writeDescriptorSets[0].dstArrayElement = 0;
	writeDescriptorSets[0].descriptorCount = 1;
	writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSets[0].pImageInfo = nullptr;
	writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfos[0];
	writeDescriptorSets[0].pTexelBufferView = nullptr;


	writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[1].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;


	writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	writeDescriptorSets[2].dstBinding = VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM;

	//
	//

	memset(resolveDescriptorBufferInfos, 0, sizeof(resolveDescriptorBufferInfos));

	memset(resolveDescriptorImageInfos, 0, sizeof(resolveDescriptorImageInfos));

	memset(resolveWriteDescriptorSets, 0, sizeof(resolveWriteDescriptorSets));

	for (uint32_t i = 0; i < 4; i++)
	{
		resolveDescriptorImageInfos[i].sampler = gbufferSampler->getSampler();
		resolveDescriptorImageInfos[i].imageView = allGBufferImageViews[i]->getImageView();
		resolveDescriptorImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		resolveWriteDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

		resolveWriteDescriptorSets[i].dstSet = resolveDescriptorSet->getDescriptorSets()[0];
		resolveWriteDescriptorSets[i].dstBinding = i;
		resolveWriteDescriptorSets[i].dstArrayElement = 0;
		resolveWriteDescriptorSets[i].descriptorCount = 1;
		resolveWriteDescriptorSets[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		resolveWriteDescriptorSets[i].pImageInfo = &resolveDescriptorImageInfos[i];
		resolveWriteDescriptorSets[i].pBufferInfo = nullptr;
		resolveWriteDescriptorSets[i].pTexelBufferView = nullptr;
	}

	// Diffuse/lambert.
	resolveDescriptorImageInfos[4].sampler = scene->getDiffuseEnvironment()->getSampler()->getSampler();
	resolveDescriptorImageInfos[4].imageView = scene->getDiffuseEnvironment()->getImageView()->getImageView();
	resolveDescriptorImageInfos[4].imageLayout = scene->getDiffuseEnvironment()->getMemoryImage()->getImage()->getImageLayout();

	resolveWriteDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	resolveWriteDescriptorSets[4].dstSet = resolveDescriptorSet->getDescriptorSets()[0];
	resolveWriteDescriptorSets[4].dstBinding = 4;
	resolveWriteDescriptorSets[4].dstArrayElement = 0;
	resolveWriteDescriptorSets[4].descriptorCount = 1;
	resolveWriteDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	resolveWriteDescriptorSets[4].pImageInfo = &resolveDescriptorImageInfos[4];
	resolveWriteDescriptorSets[4].pBufferInfo = nullptr;
	resolveWriteDescriptorSets[4].pTexelBufferView = nullptr;

	// Specular/Cook-Torrance.
	resolveDescriptorImageInfos[5].sampler = scene->getSpecularEnvironment()->getSampler()->getSampler();
	resolveDescriptorImageInfos[5].imageView = scene->getSpecularEnvironment()->getImageView()->getImageView();
	resolveDescriptorImageInfos[5].imageLayout = scene->getSpecularEnvironment()->getMemoryImage()->getImage()->getImageLayout();

	resolveWriteDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	resolveWriteDescriptorSets[5].dstSet = resolveDescriptorSet->getDescriptorSets()[0];
	resolveWriteDescriptorSets[5].dstBinding = 5;
	resolveWriteDescriptorSets[5].dstArrayElement = 0;
	resolveWriteDescriptorSets[5].descriptorCount = 1;
	resolveWriteDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	resolveWriteDescriptorSets[5].pImageInfo = &resolveDescriptorImageInfos[5];
	resolveWriteDescriptorSets[5].pBufferInfo = nullptr;
	resolveWriteDescriptorSets[5].pTexelBufferView = nullptr;

	//Lut
	resolveDescriptorImageInfos[6].sampler = scene->getLut()->getSampler()->getSampler();
	resolveDescriptorImageInfos[6].imageView = scene->getLut()->getImageView()->getImageView();
	resolveDescriptorImageInfos[6].imageLayout = scene->getLut()->getMemoryImage()->getImage()->getImageLayout();

	resolveWriteDescriptorSets[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	resolveWriteDescriptorSets[6].dstSet = resolveDescriptorSet->getDescriptorSets()[0];
	resolveWriteDescriptorSets[6].dstBinding = 6;
	resolveWriteDescriptorSets[6].dstArrayElement = 0;
	resolveWriteDescriptorSets[6].descriptorCount = 1;
	resolveWriteDescriptorSets[6].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	resolveWriteDescriptorSets[6].pImageInfo = &resolveDescriptorImageInfos[6];
	resolveWriteDescriptorSets[6].pBufferInfo = nullptr;
	resolveWriteDescriptorSets[6].pTexelBufferView = nullptr;

	// Inverse matrix.
	resolveDescriptorBufferInfos[0].buffer = resolveFragmentMatricesUniformBuffer->getBuffer()->getBuffer();
	resolveDescriptorBufferInfos[0].offset = 0;
	resolveDescriptorBufferInfos[0].range = resolveFragmentMatricesUniformBuffer->getBuffer()->getSize();

	resolveWriteDescriptorSets[7].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

	resolveWriteDescriptorSets[7].dstSet = resolveDescriptorSet->getDescriptorSets()[0];
	resolveWriteDescriptorSets[7].dstBinding = 7;
	resolveWriteDescriptorSets[7].dstArrayElement = 0;
	resolveWriteDescriptorSets[7].descriptorCount = 1;
	resolveWriteDescriptorSets[7].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	resolveWriteDescriptorSets[7].pImageInfo = nullptr;
	resolveWriteDescriptorSets[7].pBufferInfo = &resolveDescriptorBufferInfos[0];
	resolveWriteDescriptorSets[7].pTexelBufferView = nullptr;

	//

	resolveDescriptorSet->updateDescriptorSets(VKTS_BSDF_DESCRIPTOR_SET_COUNT, resolveWriteDescriptorSets, 0, nullptr);

	return VK_TRUE;
}

VkBool32 Example::buildScene(const vkts::ICommandBuffersSP& cmdBuffer)
{
	VkSamplerCreateInfo samplerCreateInfo{};

	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerCreateInfo.flags = 0;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VkImageViewCreateInfo imageViewCreateInfo{};

	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = VK_NULL_HANDLE;		// Defined later.
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_UNDEFINED;		// Defined later.
	imageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
	imageViewCreateInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

	sceneContext = vkts::scenegraphCreateContext(VK_FALSE, initialResources, cmdBuffer, samplerCreateInfo, imageViewCreateInfo, vkts::IDescriptorSetLayoutSP());

	if (!sceneContext.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create cache.");

		return VK_FALSE;
	}

	//

	if (allBSDFVertexShaderModules.size() == 4)
	{
		sceneContext->addVertexShaderModule(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_NORMAL | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD, allBSDFVertexShaderModules[0]);
		sceneContext->addVertexShaderModule(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_NORMAL, allBSDFVertexShaderModules[1]);
		sceneContext->addVertexShaderModule(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TANGENTS | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD, allBSDFVertexShaderModules[2]);
		sceneContext->addVertexShaderModule(VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TANGENTS | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD | VKTS_VERTEX_BUFFER_TYPE_BONES, allBSDFVertexShaderModules[3]);
	}

	sceneContext->setRenderPass(gbufferRenderPass);

	//

	scene = vkts::scenegraphLoadScene(VKTS_SCENE_NAME, sceneContext);

	if (!scene.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load scene.");

		return VK_FALSE;
	}

	vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Number objects: %d", scene->getNumberObjects());

	//

	if (!environmentSceneContext.get() && !environmentScene.get())
	{

		environmentSceneContext = vkts::scenegraphCreateContext(VK_FALSE, initialResources, cmdBuffer, samplerCreateInfo, imageViewCreateInfo, environmentDescriptorSetLayout);

		if (!environmentSceneContext.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create cache.");

			return VK_FALSE;
		}

		//

		environmentScene = vkts::scenegraphLoadScene(VKTS_ENVIRONMENT_SCENE_NAME, environmentSceneContext);

		if (!environmentScene.get() || environmentScene->getNumberObjects() == 0)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load scene.");

			return VK_FALSE;
		}

		// Enlarge the sphere.
		environmentScene->getObjects()[0]->setScale(glm::vec3(10.0f, 10.0f, 10.0f));

		vkts::logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Number objects: %d", environmentScene->getNumberObjects());

		//
		// Full screen plane for later resolving GBuffer.
		//

		// Window clip origin is upper left, but the image is already upside down.
		static const float vertices[4 * (4 + 2)] = {	-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
														1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
														-1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
														1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f };

		auto vertexBinaryBuffer = vkts::binaryBufferCreate((const uint8_t*)vertices, sizeof(vertices));

		if (!vertexBinaryBuffer.get())
		{
	        return VK_FALSE;
		}

	    VkBufferCreateInfo bufferCreateInfo{};

	    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

	    bufferCreateInfo.size = sizeof(vertices);
	    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	    bufferCreateInfo.flags = 0;
	    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	    bufferCreateInfo.queueFamilyIndexCount = 0;
	    bufferCreateInfo.pQueueFamilyIndices = nullptr;

	    vkts::IDeviceMemorySP stageDeviceMemory;
	    vkts::IBufferSP stageBuffer;

	    screenPlaneVertexBuffer = vkts::bufferObjectCreate(stageBuffer, stageDeviceMemory, initialResources, cmdBuffer, vertexBinaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	    // Using this scene context to manage the staging objects.
	    environmentSceneContext->addStageBuffer(stageBuffer);
	    environmentSceneContext->addStageDeviceMemory(stageDeviceMemory);

	    if (!screenPlaneVertexBuffer.get())
	    {
	        return VK_FALSE;
	    }
	}

	return VK_TRUE;
}

VkBool32 Example::buildSwapchainImageView(const int32_t usedBuffer)
{
	VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	swapchainImageView[usedBuffer] = vkts::imageViewCreate(initialResources->getDevice()->getDevice(), 0, swapchain->getAllSwapchainImages()[usedBuffer], VK_IMAGE_VIEW_TYPE_2D, swapchain->getImageFormat(), componentMapping, imageSubresourceRange);

	if (!swapchainImageView[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create color attachment view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildGBufferSampler()
{
	gbufferSampler = vkts::samplerCreate(initialResources->getDevice()->getDevice(), 0, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, 0.0f, VK_FALSE, 1.0f, VK_FALSE, VK_COMPARE_OP_NEVER, 0.0f, 0.0f, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, VK_FALSE);

	if (!gbufferSampler.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create sampler.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildGBufferImageView()
{
	VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	for (uint32_t i = 0; i < 3; i++)
	{
		auto currentImageView = vkts::imageViewCreate(initialResources->getDevice()->getDevice(), 0, allGBufferTextures[i]->getImage()->getImage(), VK_IMAGE_VIEW_TYPE_2D, allGBufferTextures[i]->getImage()->getFormat(), componentMapping, imageSubresourceRange);

		if (!currentImageView.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create color attachment view.");

			return VK_FALSE;
		}

		allGBufferImageViews.append(currentImageView);
	}

	//

	imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	auto currentImageView = vkts::imageViewCreate(initialResources->getDevice()->getDevice(), 0, allGBufferTextures[3]->getImage()->getImage(), VK_IMAGE_VIEW_TYPE_2D, allGBufferTextures[3]->getImage()->getFormat(), componentMapping, imageSubresourceRange);

	if (!currentImageView.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create depth attachment view.");

		return VK_FALSE;
	}

	allGBufferImageViews.append(currentImageView);

	return VK_TRUE;
}

VkBool32 Example::buildGBufferTexture(const vkts::ICommandBuffersSP& cmdBuffer)
{
	VkImageCreateInfo imageCreateInfo{};

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.extent = {swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1};
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	for (uint32_t i = 0; i < 3; i++)
	{
		auto currentTexture = vkts::memoryImageCreate(initialResources, cmdBuffer, "GBuffer" + std::to_string(i), imageCreateInfo, 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (!currentTexture.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create GBuffer texture.");

			return VK_FALSE;
		}

		allGBufferTextures.append(currentTexture);
	}

	//

	imageCreateInfo.format = VK_FORMAT_D16_UNORM;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

	subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

	auto currentTexture = vkts::memoryImageCreate(initialResources, cmdBuffer, "GBufferDepth", imageCreateInfo, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (!currentTexture.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create GBuffer depth texture.");

		return VK_FALSE;
	}

	allGBufferTextures.append(currentTexture);

	return VK_TRUE;
}

VkBool32 Example::buildPipeline()
{
    vkts::DefaultGraphicsPipeline gp;

    gp.getPipelineShaderStageCreateInfo(0).stage = VK_SHADER_STAGE_VERTEX_BIT;
    gp.getPipelineShaderStageCreateInfo(0).module = envVertexShaderModule->getShaderModule();

    gp.getPipelineShaderStageCreateInfo(1).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    gp.getPipelineShaderStageCreateInfo(1).module = envFragmentShaderModule->getShaderModule();


	VkTsVertexBufferType vertexBufferType = VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TANGENTS | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD;

    gp.getVertexInputBindingDescription(0).binding = VKTS_BINDING_VERTEX_BUFFER;
    gp.getVertexInputBindingDescription(0).stride = vkts::commonGetStrideInBytes(vertexBufferType);
    gp.getVertexInputBindingDescription(0).inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    gp.getVertexInputAttributeDescription(0).location = 0;
    gp.getVertexInputAttributeDescription(0).binding = VKTS_BINDING_VERTEX_BUFFER;
    gp.getVertexInputAttributeDescription(0).format = VK_FORMAT_R32G32B32A32_SFLOAT;
    gp.getVertexInputAttributeDescription(0).offset = vkts::commonGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_VERTEX, vertexBufferType);


    gp.getPipelineInputAssemblyStateCreateInfo().topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    gp.getViewports(0).x = 0.0f;
    gp.getViewports(0).y = 0.0f;
    gp.getViewports(0).width = (float) swapchain->getImageExtent().width;
    gp.getViewports(0).height = (float) swapchain->getImageExtent().height;
    gp.getViewports(0).minDepth = 0.0f;
    gp.getViewports(0).maxDepth = 1.0f;


    gp.getScissors(0).offset.x = 0;
    gp.getScissors(0).offset.y = 0;
    gp.getScissors(0).extent = swapchain->getImageExtent();


    gp.getPipelineMultisampleStateCreateInfo();
    gp.getPipelineDepthStencilStateCreateInfo();


    gp.getPipelineColorBlendAttachmentState(0).blendEnable = VK_FALSE;
    gp.getPipelineColorBlendAttachmentState(0).colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;


    gp.getDynamicState(0) = VK_DYNAMIC_STATE_VIEWPORT;
    gp.getDynamicState(1) = VK_DYNAMIC_STATE_SCISSOR;


    gp.getGraphicsPipelineCreateInfo().layout = environmentPipelineLayout->getPipelineLayout();
    gp.getGraphicsPipelineCreateInfo().renderPass = renderPass->getRenderPass();


    auto pipeline = vkts::pipelineCreateGraphics(initialResources->getDevice()->getDevice(), VK_NULL_HANDLE, gp.getGraphicsPipelineCreateInfo(), vertexBufferType);

	if (!pipeline.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create graphics pipeline.");

		return VK_FALSE;
	}

	allGraphicsPipelines.append(pipeline);

	//

    vkts::DefaultGraphicsPipeline resolveGP;

    resolveGP.getPipelineShaderStageCreateInfo(0).stage = VK_SHADER_STAGE_VERTEX_BIT;
    resolveGP.getPipelineShaderStageCreateInfo(0).module = resolveVertexShaderModule->getShaderModule();

    resolveGP.getPipelineShaderStageCreateInfo(1).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    resolveGP.getPipelineShaderStageCreateInfo(1).module = resolveFragmentShaderModule->getShaderModule();


	vertexBufferType = VKTS_VERTEX_BUFFER_TYPE_VERTEX | VKTS_VERTEX_BUFFER_TYPE_TEXCOORD;

	resolveGP.getVertexInputBindingDescription(0).binding = VKTS_BINDING_VERTEX_BUFFER;
	resolveGP.getVertexInputBindingDescription(0).stride = vkts::commonGetStrideInBytes(vertexBufferType);
	resolveGP.getVertexInputBindingDescription(0).inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	resolveGP.getVertexInputAttributeDescription(0).location = 0;
	resolveGP.getVertexInputAttributeDescription(0).binding = VKTS_BINDING_VERTEX_BUFFER;
	resolveGP.getVertexInputAttributeDescription(0).format = VK_FORMAT_R32G32B32A32_SFLOAT;
	resolveGP.getVertexInputAttributeDescription(0).offset = vkts::commonGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_VERTEX, vertexBufferType);

	resolveGP.getVertexInputAttributeDescription(1).location = 1;
	resolveGP.getVertexInputAttributeDescription(1).binding = VKTS_BINDING_VERTEX_BUFFER;
	resolveGP.getVertexInputAttributeDescription(1).format = VK_FORMAT_R32G32_SFLOAT;
	resolveGP.getVertexInputAttributeDescription(1).offset = vkts::commonGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_TEXCOORD, vertexBufferType);

	resolveGP.getPipelineInputAssemblyStateCreateInfo().topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

	resolveGP.getViewports(0).x = 0.0f;
	resolveGP.getViewports(0).y = 0.0f;
	resolveGP.getViewports(0).width = (float) swapchain->getImageExtent().width;
	resolveGP.getViewports(0).height = (float) swapchain->getImageExtent().height;
	resolveGP.getViewports(0).minDepth = 0.0f;
	resolveGP.getViewports(0).maxDepth = 1.0f;


	resolveGP.getScissors(0).offset.x = 0;
	resolveGP.getScissors(0).offset.y = 0;
	resolveGP.getScissors(0).extent = swapchain->getImageExtent();


	resolveGP.getPipelineMultisampleStateCreateInfo();


	resolveGP.getPipelineColorBlendAttachmentState(0).blendEnable = VK_FALSE;
	resolveGP.getPipelineColorBlendAttachmentState(0).colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;


	resolveGP.getDynamicState(0) = VK_DYNAMIC_STATE_VIEWPORT;
	resolveGP.getDynamicState(1) = VK_DYNAMIC_STATE_SCISSOR;


	resolveGP.getGraphicsPipelineCreateInfo().layout = resolvePipelineLayout->getPipelineLayout();
	resolveGP.getGraphicsPipelineCreateInfo().renderPass = renderPass->getRenderPass();


	resolveGraphicsPipeline = vkts::pipelineCreateGraphics(initialResources->getDevice()->getDevice(), VK_NULL_HANDLE, resolveGP.getGraphicsPipelineCreateInfo(), vertexBufferType);

	if (!resolveGraphicsPipeline.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create graphics pipeline.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildRenderPass()
{
	VkAttachmentDescription attachmentDescription[1]{};

	attachmentDescription[0].flags = 0;
	attachmentDescription[0].format = swapchain->getImageFormat();
	attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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

	renderPass = vkts::renderPassCreate( initialResources->getDevice()->getDevice(), 0, 1, attachmentDescription, 1, &subpassDescription, 0, nullptr);

	if (!renderPass.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create render pass.");

		return VK_FALSE;
	}

	//

	//Create G-Buffer render pass.

	VkAttachmentDescription gbufferAttachmentDescription[4]{};

	for (uint32_t i = 0; i < 3; i++)
	{
		gbufferAttachmentDescription[i].flags = 0;
		gbufferAttachmentDescription[i].format = VK_FORMAT_R8G8B8A8_UNORM;
		gbufferAttachmentDescription[i].samples = VK_SAMPLE_COUNT_1_BIT;
		gbufferAttachmentDescription[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		gbufferAttachmentDescription[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		gbufferAttachmentDescription[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		gbufferAttachmentDescription[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		gbufferAttachmentDescription[i].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		gbufferAttachmentDescription[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	gbufferAttachmentDescription[3].flags = 0;
	gbufferAttachmentDescription[3].format = VK_FORMAT_D16_UNORM;
	gbufferAttachmentDescription[3].samples = VK_SAMPLE_COUNT_1_BIT;
	gbufferAttachmentDescription[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	gbufferAttachmentDescription[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	gbufferAttachmentDescription[3].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	gbufferAttachmentDescription[3].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	gbufferAttachmentDescription[3].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	gbufferAttachmentDescription[3].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference gbufferColorAttachmentReference[3];

	for (uint32_t i = 0; i < 3; i++)
	{
		gbufferColorAttachmentReference[i].attachment = i;
		gbufferColorAttachmentReference[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	VkAttachmentReference gbufferDeptStencilAttachmentReference;

	gbufferDeptStencilAttachmentReference.attachment = 3;
	gbufferDeptStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription gbufferSubpassDescription{};

	gbufferSubpassDescription.flags = 0;
	gbufferSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	gbufferSubpassDescription.inputAttachmentCount = 0;
	gbufferSubpassDescription.pInputAttachments = nullptr;
	gbufferSubpassDescription.colorAttachmentCount = 3;
	gbufferSubpassDescription.pColorAttachments = gbufferColorAttachmentReference;
	gbufferSubpassDescription.pResolveAttachments = nullptr;
	gbufferSubpassDescription.pDepthStencilAttachment = &gbufferDeptStencilAttachmentReference;
	gbufferSubpassDescription.preserveAttachmentCount = 0;
	gbufferSubpassDescription.pPreserveAttachments = nullptr;

	gbufferRenderPass = vkts::renderPassCreate(initialResources->getDevice()->getDevice(), 0, 4, gbufferAttachmentDescription, 1, &gbufferSubpassDescription, 0, nullptr);

	if (!gbufferRenderPass.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create render pass.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildPipelineLayout()
{
	VkDescriptorSetLayout setLayouts[1];

	setLayouts[0] = environmentDescriptorSetLayout->getDescriptorSetLayout();

	environmentPipelineLayout = vkts::pipelineCreateLayout(initialResources->getDevice()->getDevice(), 0, 1, setLayouts, 0, nullptr);

	if (!environmentPipelineLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

		return VK_FALSE;
	}

	//

	setLayouts[0] = resolveDescriptorSetLayout->getDescriptorSetLayout();

	resolvePipelineLayout = vkts::pipelineCreateLayout(initialResources->getDevice()->getDevice(), 0, 1, setLayouts, 0, nullptr);

	if (!resolvePipelineLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSets()
{
	const auto allDescriptorSetLayouts = resolveDescriptorSetLayout->getDescriptorSetLayout();

    resolveDescriptorSet = vkts::descriptorSetsCreate(initialResources->getDevice()->getDevice(), resolveDescriptorPool->getDescriptorPool(), 1, &allDescriptorSetLayouts);

    if (!resolveDescriptorSet.get())
    {
    	vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor sets.");

        return VK_FALSE;
    }

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSetPool()
{
    VkDescriptorPoolSize descriptorPoolSize[2]{};

	descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize[0].descriptorCount = 7;

	descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSize[1].descriptorCount = 1;

	resolveDescriptorPool = vkts::descriptorPoolCreate(initialResources->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 2, descriptorPoolSize);

    if (!resolveDescriptorPool.get())
    {
        return VK_FALSE;
    }

	return VK_TRUE;
}

VkBool32 Example::buildDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[3]{};

	descriptorSetLayoutBinding[0].binding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	descriptorSetLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding[0].descriptorCount = 1;
	descriptorSetLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[0].pImmutableSamplers = nullptr;

	descriptorSetLayoutBinding[1].binding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;
	descriptorSetLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding[1].descriptorCount = 1;
	descriptorSetLayoutBinding[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[1].pImmutableSamplers = nullptr;

	descriptorSetLayoutBinding[2].binding = VKTS_BINDING_UNIFORM_SAMPLER_ENVIRONMENT;
	descriptorSetLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetLayoutBinding[2].descriptorCount = 1;
	descriptorSetLayoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	descriptorSetLayoutBinding[2].pImmutableSamplers = nullptr;

	environmentDescriptorSetLayout = vkts::descriptorSetLayoutCreate(initialResources->getDevice()->getDevice(), 0, 3, descriptorSetLayoutBinding);

	if (!environmentDescriptorSetLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

		return VK_FALSE;
	}

	//

	VkDescriptorSetLayoutBinding resolveDescriptorSetLayoutBinding[VKTS_BSDF_DESCRIPTOR_SET_COUNT]{};

	for (uint32_t binding = 0; binding < VKTS_BSDF_DESCRIPTOR_SET_COUNT - 1; binding++)
	{
		resolveDescriptorSetLayoutBinding[binding].binding = binding;
		resolveDescriptorSetLayoutBinding[binding].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		resolveDescriptorSetLayoutBinding[binding].descriptorCount = 1;
		resolveDescriptorSetLayoutBinding[binding].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		resolveDescriptorSetLayoutBinding[binding].pImmutableSamplers = nullptr;
	}

	resolveDescriptorSetLayoutBinding[VKTS_BSDF_DESCRIPTOR_SET_COUNT - 1].binding = VKTS_BSDF_DESCRIPTOR_SET_COUNT - 1;
	resolveDescriptorSetLayoutBinding[VKTS_BSDF_DESCRIPTOR_SET_COUNT - 1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	resolveDescriptorSetLayoutBinding[VKTS_BSDF_DESCRIPTOR_SET_COUNT - 1].descriptorCount = 1;
	resolveDescriptorSetLayoutBinding[VKTS_BSDF_DESCRIPTOR_SET_COUNT - 1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	resolveDescriptorSetLayoutBinding[VKTS_BSDF_DESCRIPTOR_SET_COUNT - 1].pImmutableSamplers = nullptr;

	resolveDescriptorSetLayout = vkts::descriptorSetLayoutCreate(initialResources->getDevice()->getDevice(), 0, VKTS_BSDF_DESCRIPTOR_SET_COUNT, resolveDescriptorSetLayoutBinding);

	if (!resolveDescriptorSetLayout.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildShader()
{
	auto vertexShaderBinary = vkts::fileLoadBinary(VKTS_ENV_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_ENV_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	auto fragmentShaderBinary = vkts::fileLoadBinary(VKTS_ENV_FRAGMENT_SHADER_NAME);

	if (!fragmentShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load fragment shader: '%s'", VKTS_ENV_FRAGMENT_SHADER_NAME);

		return VK_FALSE;
	}

	//

	envVertexShaderModule = vkts::shaderModuleCreate(VKTS_ENV_VERTEX_SHADER_NAME, initialResources->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!envVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	envFragmentShaderModule = vkts::shaderModuleCreate(VKTS_ENV_FRAGMENT_SHADER_NAME, initialResources->getDevice()->getDevice(), 0, fragmentShaderBinary->getSize(), (uint32_t*)fragmentShaderBinary->getData());

	if (!envFragmentShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

		return VK_FALSE;
	}

	//

	vertexShaderBinary = vkts::fileLoadBinary(VKTS_RESOLVE_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_RESOLVE_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	resolveVertexShaderModule = vkts::shaderModuleCreate(VKTS_RESOLVE_VERTEX_SHADER_NAME, initialResources->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!resolveVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	fragmentShaderBinary = vkts::fileLoadBinary(VKTS_RESOLVE_FRAGMENT_SHADER_NAME);

	if (!fragmentShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_RESOLVE_FRAGMENT_SHADER_NAME);

		return VK_FALSE;
	}

	resolveFragmentShaderModule = vkts::shaderModuleCreate(VKTS_RESOLVE_FRAGMENT_SHADER_NAME, initialResources->getDevice()->getDevice(), 0, fragmentShaderBinary->getSize(), (uint32_t*)fragmentShaderBinary->getData());

	if (!resolveFragmentShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

		return VK_FALSE;
	}

	//

	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF0_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF0_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	auto bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF0_VERTEX_SHADER_NAME, initialResources->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF1_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF1_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF1_VERTEX_SHADER_NAME, initialResources->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF2_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF2_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF2_VERTEX_SHADER_NAME, initialResources->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);


	vertexShaderBinary = vkts::fileLoadBinary(VKTS_BSDF3_VERTEX_SHADER_NAME);

	if (!vertexShaderBinary.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load vertex shader: '%s'", VKTS_BSDF3_VERTEX_SHADER_NAME);

		return VK_FALSE;
	}

	bsdfVertexShaderModule = vkts::shaderModuleCreate(VKTS_BSDF3_VERTEX_SHADER_NAME, initialResources->getDevice()->getDevice(), 0, vertexShaderBinary->getSize(), (uint32_t*)vertexShaderBinary->getData());

	if (!bsdfVertexShaderModule.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex shader module.");

		return VK_FALSE;
	}

	allBSDFVertexShaderModules.append(bsdfVertexShaderModule);

	return VK_TRUE;
}

VkBool32 Example::buildUniformBuffers()
{
	VkBufferCreateInfo bufferCreateInfo{};

	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = vkts::commonGetDeviceSize(16 * sizeof(float) * 2, 16);
	bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;


	vertexViewProjectionUniformBuffer = vkts::bufferObjectCreate(initialResources, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	if (!vertexViewProjectionUniformBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex uniform buffer.");

		return VK_FALSE;
	}

	environmentVertexViewProjectionUniformBuffer = vkts::bufferObjectCreate(initialResources, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	if (!environmentVertexViewProjectionUniformBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex uniform buffer.");

		return VK_FALSE;
	}


	bufferCreateInfo.size = vkts::commonGetDeviceSize(16 * sizeof(float) * 2, 16);

	resolveFragmentMatricesUniformBuffer = vkts::bufferObjectCreate(initialResources, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	if (!resolveFragmentMatricesUniformBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex uniform buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildResources(const vkts::IUpdateThreadContext& updateContext)
{
	VkResult result;

	//

	auto lastSwapchain = swapchain;

	VkSwapchainKHR oldSwapchain = lastSwapchain.get() ? lastSwapchain->getSwapchain() : VK_NULL_HANDLE;

	swapchain = vkts::wsiSwapchainCreate(initialResources->getPhysicalDevice()->getPhysicalDevice(), initialResources->getDevice()->getDevice(), 0, surface->getSurface(), VKTS_NUMBER_BUFFERS, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_TRUE, oldSwapchain);

	if (!swapchain.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create swap chain.");

		return VK_FALSE;
	}

    //

    swapchainImagesCount = (uint32_t)swapchain->getAllSwapchainImages().size();

    if (swapchainImagesCount == 0)
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get swap chain images count.");

        return VK_FALSE;
    }

    swapchainImageView = vkts::SmartPointerVector<vkts::IImageViewSP>(swapchainImagesCount);
    gbufferFramebuffer = vkts::SmartPointerVector<vkts::IFramebufferSP>(swapchainImagesCount);
    framebuffer = vkts::SmartPointerVector<vkts::IFramebufferSP>(swapchainImagesCount);
    cmdBuffer = vkts::SmartPointerVector<vkts::ICommandBuffersSP>(swapchainImagesCount);
    rebuildCmdBufferCounter = swapchainImagesCount;

    //

	if (lastSwapchain.get())
	{
		lastSwapchain->destroy();
	}

	//

	if (!buildRenderPass())
	{
		return VK_FALSE;
	}

	if (!buildPipeline())
	{
		return VK_FALSE;
	}

	//

	vkts::ICommandBuffersSP updateCmdBuffer = vkts::commandBuffersCreate(initialResources->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	if (!updateCmdBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

		return VK_FALSE;
	}


	result = updateCmdBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not begin command buffer.");

		return VK_FALSE;
	}

	if (!buildGBufferTexture(updateCmdBuffer))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build texture.");

		return VK_FALSE;
	}

    vkts::SmartPointerVector<vkts::IImageSP> allStageImages;
    vkts::SmartPointerVector<vkts::IBufferSP> allStageBuffers;
    vkts::SmartPointerVector<vkts::IDeviceMemorySP> allStageDeviceMemories;

	if (!font.get())
	{
		font = vkts::fontCreate(VKTS_FONT_NAME, initialResources, updateCmdBuffer, renderPass, allStageImages, allStageBuffers, allStageDeviceMemories);

		if (!font.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build font.");

			return VK_FALSE;
		}
	}

	if (!buildScene(updateCmdBuffer))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build scene.");

		return VK_FALSE;
	}

	result = updateCmdBuffer->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not end command buffer.");

		return VK_FALSE;
	}


	VkSubmitInfo submitInfo{};

	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.commandBufferCount = updateCmdBuffer->getCommandBufferCount();
	submitInfo.pCommandBuffers = updateCmdBuffer->getCommandBuffers();
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	result = initialResources->getQueue()->submit(1, &submitInfo, VK_NULL_HANDLE);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

		return VK_FALSE;
	}

	result = initialResources->getQueue()->waitIdle();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for idle queue.");

		return VK_FALSE;
	}

	updateCmdBuffer->destroy();

	//

	if (!buildGBufferImageView())
	{
		return VK_FALSE;
	}

	if (!buildGBufferSampler())
	{
		return VK_FALSE;
	}

	//

	for (size_t i = 0; i < allStageImages.size(); i++)
	{
		allStageImages[i]->destroy();
	}
	for (size_t i = 0; i < allStageBuffers.size(); i++)
	{
		allStageBuffers[i]->destroy();
	}
	for (size_t i = 0; i < allStageDeviceMemories.size(); i++)
	{
		allStageDeviceMemories[i]->destroy();
	}

	//

	if (!updateDescriptorSets())
	{
		return VK_FALSE;
	}

	if (scene.get())
	{
		scene->updateDescriptorSetsRecursive(VKTS_BINDING_UNIFORM_BSDF_TOTAL_BINDING_COUNT, writeDescriptorSets);
	}

	if (environmentScene.get())
	{
		environmentScene->updateDescriptorSetsRecursive(VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT, environmentWriteDescriptorSets);
	}

	//

	for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
	{
		if (!buildSwapchainImageView(i))
		{
			return VK_FALSE;
		}

		if (!buildFramebuffer(i))
		{
			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

void Example::terminateResources(const vkts::IUpdateThreadContext& updateContext)
{
	if (initialResources.get())
	{
		if (initialResources->getDevice().get())
		{
			for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
			{
				if (gbufferFramebuffer[i].get())
				{
					gbufferFramebuffer[i]->destroy();
				}

				if (framebuffer[i].get())
				{
					framebuffer[i]->destroy();
				}

				if (swapchainImageView[i].get())
				{
					swapchainImageView[i]->destroy();
				}
			}

			if (sceneContext.get())
			{
				sceneContext->destroy();

				sceneContext.reset();
			}

			if (scene.get())
			{
				scene->destroy();
			}

			if (resolveGraphicsPipeline.get())
			{
				resolveGraphicsPipeline->destroy();
			}

			for (size_t i = 0; i < allGraphicsPipelines.size(); i++)
			{
				allGraphicsPipelines[i]->destroy();
			}
			allGraphicsPipelines.clear();


			if (gbufferSampler.get())
			{
				gbufferSampler->destroy();
			}


			for (size_t i = 0; i < allGBufferImageViews.size(); i++)
			{
				allGBufferImageViews[i]->destroy();
			}
			allGBufferImageViews.clear();

			for (size_t i = 0; i < allGBufferTextures.size(); i++)
			{
				allGBufferTextures[i]->destroy();
			}
			allGBufferTextures.clear();

			if (renderPass.get())
			{
				renderPass->destroy();
			}

			if (gbufferRenderPass.get())
			{
				gbufferRenderPass->destroy();
			}
		}
	}
}

//
// Vulkan initialization.
//
VkBool32 Example::init(const vkts::IUpdateThreadContext& updateContext)
{
	if (!updateContext.isWindowAttached(windowIndex))
	{
		return VK_FALSE;
	}

	//

	surface->hasCurrentExtentChanged(initialResources->getPhysicalDevice()->getPhysicalDevice());

	//

	camera = vkts::cameraCreate(glm::vec4(0.0f, 4.0f, 10.0f, 1.0f), glm::vec4(0.0f, 2.0f, 0.0f, 1.0f));

	if (!camera.get())
	{
		return VK_FALSE;
	}

	allUpdateables.append(camera);

	inputController = vkts::inputControllerCreate(updateContext, windowIndex, 0, camera);

	if (!inputController.get())
	{
		return VK_FALSE;
	}

	allUpdateables.insert(0, inputController);

	//

	commandPool = vkts::commandPoolCreate(initialResources->getDevice()->getDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, initialResources->getQueue()->getQueueFamilyIndex());

	if (!commandPool.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get command pool.");

		return VK_FALSE;
	}

	//

    imageAcquiredSemaphore = vkts::semaphoreCreate(initialResources->getDevice()->getDevice(), 0);

    if (!imageAcquiredSemaphore.get())
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create semaphore.");

        return VK_FALSE;
    }

    renderingCompleteSemaphore = vkts::semaphoreCreate(initialResources->getDevice()->getDevice(), 0);

    if (!renderingCompleteSemaphore.get())
    {
        vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create semaphore.");

        return VK_FALSE;
    }

	//

	if (!buildUniformBuffers())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build uniform buffers.");

		return VK_FALSE;
	}

	if (!buildShader())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build shader.");

		return VK_FALSE;
	}

	if (!buildDescriptorSetLayout())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build descriptor set layout.");

		return VK_FALSE;
	}

	if (!buildDescriptorSetPool())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build descriptor set pool.");

		return VK_FALSE;
	}

	if (!buildDescriptorSets())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build descriptor sets.");

		return VK_FALSE;
	}

	if (!buildPipelineLayout())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build pipeline cache.");

		return VK_FALSE;
	}

	if (!buildResources(updateContext))
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

//
// Vulkan update.
//
VkBool32 Example::update(const vkts::IUpdateThreadContext& updateContext)
{
	static VkBool32 pressedA = VK_FALSE;

	if (updateContext.getGamepadButton(windowIndex, 0, VKTS_GAMEPAD_A))
	{
		pressedA = VK_TRUE;
	}
	else if (pressedA && !updateContext.getGamepadButton(windowIndex, 0, VKTS_GAMEPAD_A))
	{
		pressedA = VK_FALSE;

		showStats = !showStats;
	}

	//

	for (size_t i = 0; i < allUpdateables.size(); i++)
	{
		allUpdateables[i]->update(updateContext.getDeltaTime(), updateContext.getDeltaTicks());
	}

	//

	VkResult result = VK_SUCCESS;

	//

	if (surface->hasCurrentExtentChanged(initialResources->getPhysicalDevice()->getPhysicalDevice()))
	{
		const auto& currentExtent = surface->getCurrentExtent(initialResources->getPhysicalDevice()->getPhysicalDevice(), VK_FALSE);

		if (currentExtent.width == 0 || currentExtent.height == 0)
		{
			return VK_TRUE;
		}

		result = VK_ERROR_OUT_OF_DATE_KHR;
	}

	//

	uint32_t currentBuffer;

	if (result == VK_SUCCESS)
	{
		result = swapchain->acquireNextImage(UINT64_MAX, imageAcquiredSemaphore->getSemaphore(), VK_NULL_HANDLE, currentBuffer);
	}

	//

	if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
	{
		glm::mat4 projectionMatrix(1.0f);
		glm::mat4 viewMatrix(1.0f);

		const auto& currentExtent = surface->getCurrentExtent(initialResources->getPhysicalDevice()->getPhysicalDevice(), VK_FALSE);

		projectionMatrix = vkts::perspectiveMat4(45.0f, (float)currentExtent.width / (float)currentExtent.height, 1.0f, 100.0f);

		auto inverseProjectionMatrix = glm::inverse(projectionMatrix);

		viewMatrix = camera->getViewMatrix();

		auto inverseViewMatrix = glm::inverse(viewMatrix);

		if (!vertexViewProjectionUniformBuffer->upload(0 * sizeof(float) * 16, 0, projectionMatrix))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

			return VK_FALSE;
		}
		if (!environmentVertexViewProjectionUniformBuffer->upload(0 * sizeof(float) * 16, 0, projectionMatrix))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

			return VK_FALSE;
		}
		if (!resolveFragmentMatricesUniformBuffer->upload(0 * sizeof(float) * 16, 0, inverseProjectionMatrix))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

			return VK_FALSE;
		}

		//

		if (!vertexViewProjectionUniformBuffer->upload(1 * sizeof(float) * 16, 0, viewMatrix))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

			return VK_FALSE;
		}
		if (!resolveFragmentMatricesUniformBuffer->upload(1 * sizeof(float) * 16, 0, inverseViewMatrix))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

			return VK_FALSE;
		}

		// Environment is not movebale.
		viewMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		if (!environmentVertexViewProjectionUniformBuffer->upload(1 * sizeof(float) * 16, 0, viewMatrix))
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload matrices.");

			return VK_FALSE;
		}

		if (environmentScene.get())
		{
			environmentScene->updateRecursive(updateContext);
		}

		//

		if (scene.get())
		{
			scene->updateRecursive(updateContext);
		}

		//

		uint32_t currentFPS;

		if (vkts::profileApplicationGetFps(currentFPS, updateContext.getDeltaTime()))
		{
			fps = currentFPS;

			//

			uint64_t currentRAM;

			if (vkts::profileApplicationGetRam(currentRAM))
			{
				ram = currentRAM;
			}

			//

			float currentCpuUsageApp;

			if (vkts::profileApplicationGetCpuUsage(currentCpuUsageApp))
			{
				cpuUsageApp = currentCpuUsageApp;
			}

			//

			for (uint32_t cpu = 0; cpu < processors; cpu++)
			{
				if (vkts::profileGetCpuUsage(currentCpuUsageApp, cpu))
				{
					cpuUsage[cpu] = currentCpuUsageApp;
				}
			}

			//

			rebuildCmdBufferCounter = swapchainImagesCount;
		}

		if (rebuildCmdBufferCounter > 0)
		{
			if (!buildCmdBuffer(currentBuffer))
			{
				return VK_FALSE;
			}

			rebuildCmdBufferCounter--;
		}

		//

        VkSemaphore waitSemaphores = imageAcquiredSemaphore->getSemaphore();
        VkSemaphore signalSemaphores = renderingCompleteSemaphore->getSemaphore();


        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        VkSubmitInfo submitInfo{};

        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphores;
        submitInfo.pWaitDstStageMask = &waitDstStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = cmdBuffer[currentBuffer]->getCommandBuffers();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphores;

		result = initialResources->getQueue()->submit(1, &submitInfo, VK_NULL_HANDLE);

		if (result != VK_SUCCESS)
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not submit queue.");

			return VK_FALSE;
		}

        waitSemaphores = renderingCompleteSemaphore->getSemaphore();

        VkSwapchainKHR swapchains = swapchain->getSwapchain();

        result = swapchain->queuePresent(initialResources->getQueue()->getQueue(), 1, &waitSemaphores, 1, &swapchains, &currentBuffer, nullptr);

		if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
		{
			result = initialResources->getQueue()->waitIdle();

			if (result != VK_SUCCESS)
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not wait for idle queue.");

				return VK_FALSE;
			}
		}
		else
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				terminateResources(updateContext);

				if (!buildResources(updateContext))
				{
					vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

					return VK_FALSE;
				}
			}
			else
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not present queue.");

				return VK_FALSE;
			}
		}
	}
	else
	{
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			terminateResources(updateContext);

			if (!buildResources(updateContext))
			{
				vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not build resources.");

				return VK_FALSE;
			}
		}
		else
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not acquire next image.");

			return VK_FALSE;
		}
	}

	return VK_TRUE;
}

//
// Vulkan termination.
//
void Example::terminate(const vkts::IUpdateThreadContext& updateContext)
{
	if (initialResources.get())
	{
		if (initialResources->getDevice().get())
		{
			terminateResources(updateContext);

			for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
			{
				if (cmdBuffer[i].get())
				{
					cmdBuffer[i]->destroy();
				}
			}

			//

			if (screenPlaneVertexBuffer.get())
			{
				screenPlaneVertexBuffer->destroy();
			}

			if (environmentSceneContext.get())
			{
				environmentSceneContext->destroy();

				environmentSceneContext.reset();
			}

			if (environmentScene.get())
			{
				environmentScene->destroy();
			}

			if (font.get())
			{
				font->destroy();
			}

			if (swapchain.get())
			{
				swapchain->destroy();
			}

			if (resolvePipelineLayout.get())
			{
				resolvePipelineLayout->destroy();
			}

			if (environmentPipelineLayout.get())
			{
				environmentPipelineLayout->destroy();
			}

			if (envVertexShaderModule.get())
			{
				envVertexShaderModule->destroy();
			}

			if (envFragmentShaderModule.get())
			{
				envFragmentShaderModule->destroy();
			}

			if (resolveVertexShaderModule.get())
			{
				resolveVertexShaderModule->destroy();
			}

			if (resolveFragmentShaderModule.get())
			{
				resolveFragmentShaderModule->destroy();
			}

			for (size_t i = 0; i < allBSDFVertexShaderModules.size(); i++)
			{
				allBSDFVertexShaderModules[i]->destroy();
			}
			allBSDFVertexShaderModules.clear();

			if (resolveFragmentMatricesUniformBuffer.get())
			{
				resolveFragmentMatricesUniformBuffer->destroy();
			}

			if (environmentVertexViewProjectionUniformBuffer.get())
			{
				environmentVertexViewProjectionUniformBuffer->destroy();
			}

			if (vertexViewProjectionUniformBuffer.get())
			{
				vertexViewProjectionUniformBuffer->destroy();
			}

			if (resolveDescriptorSet.get())
			{
				resolveDescriptorSet->destroy();
			}

			if (resolveDescriptorPool.get())
			{
				resolveDescriptorPool->destroy();
			}

			if (resolveDescriptorSetLayout.get())
			{
				resolveDescriptorSetLayout->destroy();
			}

			if (environmentDescriptorSetLayout.get())
			{
				environmentDescriptorSetLayout->destroy();
			}

	        if (renderingCompleteSemaphore.get())
	        {
	            renderingCompleteSemaphore->destroy();
	        }

	        if (imageAcquiredSemaphore.get())
	        {
	            imageAcquiredSemaphore->destroy();
	        }

			if (commandPool.get())
			{
				commandPool->destroy();
			}
		}
	}
}
