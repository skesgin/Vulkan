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

#include "BuildCommandTask.hpp"

vkts::OverwriteDraw* BuildCommandTask::overwrite = nullptr;

VkBool32 BuildCommandTask::execute()
{
    if (!commandBufferInheritanceInfo)
    {
        return VK_FALSE;
    }

    if (!cmdBuffer[usedBuffer].get())
    {
    	return VK_FALSE;
    }
    else
    {
    	cmdBuffer[usedBuffer]->reset();
    }

    // Update / transform the scene.
    if (scene.get())
    {
        scene->updateTransformRecursive(updateContext.getDeltaTime(), updateContext.getDeltaTicks(), updateContext.getTickTime(), usedBuffer, objectOffset, objectStep);
    }

    //
    // Record secondary command buffer.
    //

    VkCommandBufferBeginInfo commandBufferBeginInfo{};

    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    commandBufferBeginInfo.pInheritanceInfo = commandBufferInheritanceInfo;

    vkBeginCommandBuffer(cmdBuffer[usedBuffer]->getCommandBuffer(), &commandBufferBeginInfo);

    VkViewport viewport{};

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &viewport);

    VkRect2D scissor{};

    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = extent;

    vkCmdSetScissor(cmdBuffer[usedBuffer]->getCommandBuffer(), 0, 1, &scissor);

    if (scene.get())
    {
        scene->drawRecursive(cmdBuffer[usedBuffer], allGraphicsPipelines, usedBuffer, dynamicOffsets, overwrite, objectOffset, objectStep);
    }

    vkEndCommandBuffer(cmdBuffer[usedBuffer]->getCommandBuffer());

    //
    // Record secondary command buffer end.
    //

    commandBufferInheritanceInfo = nullptr;

	return VK_TRUE;
}

BuildCommandTask::BuildCommandTask(const uint64_t id, const vkts::IUpdateThreadContext& updateContext, const vkts::IContextObjectSP& contextObject, const vkts::SmartPointerVector<vkts::IGraphicsPipelineSP>& allGraphicsPipelines, const vkts::ISceneSP& scene, const uint32_t buffers, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsets, const uint32_t& objectOffset, const uint32_t& objectStep) :
	ITask(id), updateContext(updateContext), contextObject(contextObject), allGraphicsPipelines(allGraphicsPipelines), scene(scene), dynamicOffsets(dynamicOffsets), objectOffset(objectOffset), objectStep(objectStep), commandBufferInheritanceInfo(nullptr), extent{0, 0}, usedBuffer(0), commandPool(nullptr), cmdBuffer()
{
	// This pool will contain secondary command buffers, which will be reset.
	commandPool = vkts::commandPoolCreate(contextObject->getDevice()->getDevice(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, contextObject->getQueue()->getQueueFamilyIndex());

	if (!commandPool.get())
	{
		vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not get command pool.");
	}

	//

	for (uint32_t i = 0; i < buffers; i++)
	{
		auto currentCmdBuffer = vkts::commandBuffersCreate(contextObject->getDevice()->getDevice(), commandPool->getCmdPool(), VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1);

		if (!currentCmdBuffer.get())
		{
			vkts::logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create command buffer.");

			commandPool->destroy();

			for (uint32_t k = 0; k < cmdBuffer.size(); k++)
			{
				cmdBuffer[k]->destroy();
			}

			return;
		}

		cmdBuffer.append(currentCmdBuffer);
	}
}

BuildCommandTask::~BuildCommandTask()
{
	for (uint32_t i = 0; i < cmdBuffer.size(); i++)
	{
		cmdBuffer[i]->destroy();
	}

	if (commandPool.get())
	{
		commandPool->destroy();
	}
}

void BuildCommandTask::setCommandBufferInheritanceInfo(VkCommandBufferInheritanceInfo* commandBufferInheritanceInfo)
{
    this->commandBufferInheritanceInfo = commandBufferInheritanceInfo;
}

void BuildCommandTask::setExtent(const VkExtent2D& extent)
{
    this->extent = extent;
}

void BuildCommandTask::setUsedBuffer(const uint32_t usedBuffer)
{
	this->usedBuffer = usedBuffer;
}

VkCommandBuffer BuildCommandTask::getCommandBuffer() const
{
	if (usedBuffer >= (uint32_t)cmdBuffer.size())
	{
		return VK_NULL_HANDLE;
	}

	if (cmdBuffer[usedBuffer].get())
	{
		return cmdBuffer[usedBuffer]->getCommandBuffer();
	}

	return VK_NULL_HANDLE;
}

void BuildCommandTask::setOverwrite(vkts::OverwriteDraw* overwrite)
{
    BuildCommandTask::overwrite = overwrite;
}
