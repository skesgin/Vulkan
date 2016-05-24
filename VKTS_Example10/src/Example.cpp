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
		IUpdateThread(), initialResources(initialResources), windowIndex(windowIndex), surface(surface), windowDimension(0, 0), commandPool(nullptr), imageAcquiredSemaphore(nullptr), renderingCompleteSemaphore(nullptr), font(nullptr), sceneContext(nullptr), scene(nullptr), swapchain(nullptr), renderPass(nullptr), depthTexture(nullptr), depthStencilImageView(nullptr), swapchainImagesCount(0), swapchainImageView(), framebuffer(), cmdBuffer(), fps(0), ram(0), cpuUsageApp(0.0f), processors(0)
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
			vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not reset command buffer.");

			return VK_FALSE;
		}
	}
	else
	{
		cmdBuffer[usedBuffer] = vkts::commandBuffersCreate(initialResources->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

		if (!cmdBuffer[usedBuffer].get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create command buffer.");

			return VK_FALSE;
		}
	}

	result = cmdBuffer[usedBuffer]->beginCommandBuffer(0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not begin command buffer.");

		return VK_FALSE;
	}

    //

    swapchain->cmdPipelineBarrier(cmdBuffer[usedBuffer]->getCommandBuffer(), VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, usedBuffer);

    //

	VkClearColorValue clearColorValue;

	memset(&clearColorValue, 0, sizeof(VkClearColorValue));

	clearColorValue.float32[0] = 0.2f;
	clearColorValue.float32[1] = 0.2f;
	clearColorValue.float32[2] = 0.2f;
	clearColorValue.float32[3] = 1.0f;

	VkClearDepthStencilValue clearDepthStencilValue;

	memset(&clearDepthStencilValue, 0, sizeof(VkClearDepthStencilValue));

	clearDepthStencilValue.depth = 1.0f;
	clearDepthStencilValue.stencil = 0;

	VkClearValue clearValues[2];

	memset(clearValues, 0, sizeof(clearValues));

	clearValues[0].color = clearColorValue;
	clearValues[1].depthStencil = clearDepthStencilValue;

	VkRenderPassBeginInfo renderPassBeginInfo;

	memset(&renderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));

	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

	renderPassBeginInfo.renderPass = renderPass->getRenderPass();
	renderPassBeginInfo.framebuffer = framebuffer[usedBuffer]->getFramebuffer();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = swapchain->getImageExtent();
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	cmdBuffer[usedBuffer]->cmdBeginRenderPass(&renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport;
	memset(&viewport, 0, sizeof(VkViewport));

	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) swapchain->getImageExtent().width;
	viewport.height = (float) swapchain->getImageExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vkCmdSetViewport(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor;
	memset(&scissor, 0, sizeof(VkRect2D));

	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = swapchain->getImageExtent();

	vkCmdSetScissor(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &scissor);

	// Render font.

	if (font.get())
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

	result = cmdBuffer[usedBuffer]->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not end command buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildFramebuffer(const int32_t usedBuffer)
{
	VkImageView imageViews[2];

	imageViews[0] = swapchainImageView[usedBuffer]->getImageView();
	imageViews[1] = depthStencilImageView->getImageView();

	framebuffer[usedBuffer] = vkts::framebufferCreate(initialResources->getDevice()->getDevice(), 0, renderPass->getRenderPass(), 2, imageViews, swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1);

	if (!framebuffer[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create frame buffer.");

		return VK_FALSE;
	}

	return VK_TRUE;
}
VkBool32 Example::buildScene(const vkts::ICommandBuffersSP& cmdBuffer)
{
	VkSamplerCreateInfo samplerCreateInfo;

	memset(&samplerCreateInfo, 0, sizeof(VkSamplerCreateInfo));

	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerCreateInfo.flags = 0;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VkImageViewCreateInfo imageViewCreateInfo;

	memset(&imageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));

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
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create cache.");

		return VK_FALSE;
	}

	//

	scene = vkts::scenegraphLoadScene(VKTS_SCENE_NAME, sceneContext);

	if (!scene.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not load scene.");

		return VK_FALSE;
	}

	vkts::logPrint(VKTS_LOG_INFO, "Example: Number objects: %d", scene->getNumberObjects());

	return VK_TRUE;
}

VkBool32 Example::buildSwapchainImageView(const int32_t usedBuffer)
{
	VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	swapchainImageView[usedBuffer] = vkts::imageViewCreate(initialResources->getDevice()->getDevice(), 0, swapchain->getAllSwapchainImages()[usedBuffer], VK_IMAGE_VIEW_TYPE_2D, swapchain->getImageFormat(), componentMapping, imageSubresourceRange);

	if (!swapchainImageView[usedBuffer].get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create color attachment view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDepthStencilImageView()
{
	VkComponentMapping componentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

	depthStencilImageView = vkts::imageViewCreate(initialResources->getDevice()->getDevice(), 0, depthTexture->getImage()->getImage(), VK_IMAGE_VIEW_TYPE_2D, depthTexture->getImage()->getFormat(), componentMapping, imageSubresourceRange);

	if (!depthStencilImageView.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create depth attachment view.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildDepthTexture(const vkts::ICommandBuffersSP& cmdBuffer)
{
	VkImageCreateInfo imageCreateInfo;

	memset(&imageCreateInfo, 0, sizeof(VkImageCreateInfo));

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	imageCreateInfo.flags = 0;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_D16_UNORM;
	imageCreateInfo.extent = {swapchain->getImageExtent().width, swapchain->getImageExtent().height, 1};
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

	depthTexture = vkts::memoryImageCreate(initialResources, cmdBuffer, "DepthTexture", imageCreateInfo, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (!depthTexture.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create depth texture.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

VkBool32 Example::buildRenderPass()
{
	VkAttachmentDescription attachmentDescription[2];

	memset(&attachmentDescription, 0, sizeof(attachmentDescription));

	attachmentDescription[0].flags = 0;
	attachmentDescription[0].format = swapchain->getImageFormat();
	attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachmentDescription[1].flags = 0;
	attachmentDescription[1].format = VK_FORMAT_D16_UNORM;
	attachmentDescription[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachmentDescription[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentReference;

	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference deptStencilAttachmentReference;

	deptStencilAttachmentReference.attachment = 1;
	deptStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription;

	memset(&subpassDescription, 0, sizeof(VkSubpassDescription));

	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.pDepthStencilAttachment = &deptStencilAttachmentReference;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;

	renderPass = vkts::renderPassCreate( initialResources->getDevice()->getDevice(), 0, 2, attachmentDescription, 1, &subpassDescription, 0, nullptr);

	if (!renderPass.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create render pass.");

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
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create swap chain.");

		return VK_FALSE;
	}

    //

    swapchainImagesCount = (uint32_t)swapchain->getAllSwapchainImages().size();

    if (swapchainImagesCount == 0)
    {
        vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not get swap chain images count.");

        return VK_FALSE;
    }

    swapchainImageView = vkts::SmartPointerVector<vkts::IImageViewSP>(swapchainImagesCount);
    framebuffer = vkts::SmartPointerVector<vkts::IFramebufferSP>(swapchainImagesCount);
    cmdBuffer = vkts::SmartPointerVector<vkts::ICommandBuffersSP>(swapchainImagesCount);

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

	//

	vkts::ICommandBuffersSP updateCmdBuffer = vkts::commandBuffersCreate(initialResources->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	if (!updateCmdBuffer.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create command buffer.");

		return VK_FALSE;
	}


	result = updateCmdBuffer->beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, VK_FALSE, 0, 0);

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not begin command buffer.");

		return VK_FALSE;
	}

	if (!buildDepthTexture(updateCmdBuffer))
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not build texture.");

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
			vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not build font.");

			return VK_FALSE;
		}
	}

	if (!scene.get())
	{
		if (!buildScene(updateCmdBuffer))
		{
			vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not build scene.");

			return VK_FALSE;
		}
	}

	result = updateCmdBuffer->endCommandBuffer();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not end command buffer.");

		return VK_FALSE;
	}


	VkSubmitInfo submitInfo;

	memset(&submitInfo, 0, sizeof(VkSubmitInfo));

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
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not submit queue.");

		return VK_FALSE;
	}

	result = initialResources->getQueue()->waitIdle();

	if (result != VK_SUCCESS)
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not wait for idle queue.");

		return VK_FALSE;
	}

	updateCmdBuffer->destroy();

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

	if (!buildDepthStencilImageView())
	{
		return VK_FALSE;
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

		if (!buildCmdBuffer(i))
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
				if (framebuffer[i].get())
				{
					framebuffer[i]->destroy();
				}

				if (swapchainImageView[i].get())
				{
					swapchainImageView[i]->destroy();
				}
			}

			if (depthStencilImageView.get())
			{
				depthStencilImageView->destroy();
			}

			if (depthTexture.get())
			{
				depthTexture->destroy();
			}

			if (renderPass.get())
			{
				renderPass->destroy();
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

	windowDimension = updateContext.getWindowDimension(windowIndex);

	//

	commandPool = vkts::commandPoolCreate(initialResources->getDevice()->getDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, initialResources->getQueue()->getQueueFamilyIndex());

	if (!commandPool.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not get command pool.");

		return VK_FALSE;
	}

	//

    imageAcquiredSemaphore = vkts::semaphoreCreate(initialResources->getDevice()->getDevice(), 0);

    if (!imageAcquiredSemaphore.get())
    {
        vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create semaphore.");

        return VK_FALSE;
    }

    renderingCompleteSemaphore = vkts::semaphoreCreate(initialResources->getDevice()->getDevice(), 0);

    if (!renderingCompleteSemaphore.get())
    {
        vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not create semaphore.");

        return VK_FALSE;
    }

	//

	if (!buildResources(updateContext))
	{
		vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not build resources.");

		return VK_FALSE;
	}

	return VK_TRUE;
}

//
// Vulkan update.
//
VkBool32 Example::update(const vkts::IUpdateThreadContext& updateContext)
{

	VkResult result = VK_SUCCESS;

	//

	if (windowDimension != updateContext.getWindowDimension(windowIndex))
	{
		windowDimension = updateContext.getWindowDimension(windowIndex);

		if (windowDimension.x == 0 || windowDimension.y == 0)
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

			for (int32_t i = 0; i < (int32_t)swapchainImagesCount; i++)
			{
				if (!buildCmdBuffer(i))
				{
					return VK_FALSE;
				}
			}
		}

		//

        VkSemaphore waitSemaphores = imageAcquiredSemaphore->getSemaphore();
        VkSemaphore signalSemaphores = renderingCompleteSemaphore->getSemaphore();


        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

        VkSubmitInfo submitInfo;

        memset(&submitInfo, 0, sizeof(VkSubmitInfo));

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
			vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not submit queue.");

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
				vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not wait for idle queue.");

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
					vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not build resources.");

					return VK_FALSE;
				}
			}
			else
			{
				vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not present queue.");

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
				vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not build resources.");

				return VK_FALSE;
			}
		}
		else
		{
			vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not acquire next image.");

			return VK_FALSE;
		}
	}

	//

    result = imageAcquiredSemaphore->reset();

    if (result != VK_SUCCESS)
    {
        vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not reset semaphore.");

        return VK_FALSE;
    }

    result = renderingCompleteSemaphore->reset();

    if (result != VK_SUCCESS)
    {
        vkts::logPrint(VKTS_LOG_ERROR, "Example: Could not reset semaphore.");

        return VK_FALSE;
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

			if (sceneContext.get())
			{
				sceneContext->destroy();

				sceneContext.reset();
			}

			if (scene.get())
			{
				scene->destroy();
			}

			if (font.get())
			{
				font->destroy();
			}

			if (swapchain.get())
			{
				swapchain->destroy();
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
