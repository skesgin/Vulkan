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

#include "CommandObject.hpp"

namespace vkts
{

CommandObject::CommandObject(const ICommandBuffersSP& cmdBuffer) :
    ICommandObject(), cmdBuffer(cmdBuffer)
{
}

CommandObject::~CommandObject()
{
    destroy();
}

//
// ICommandObject
//

const ICommandBuffersSP& CommandObject::getCommandBuffer() const
{
    return cmdBuffer;
}

void CommandObject::addStageImage(const IImageSP& stageImage)
{
    if (stageImage.get())
    {
        allStageImages.append(stageImage);
    }
}

void CommandObject::addStageBuffer(const IBufferSP& stageBuffer)
{
    if (stageBuffer.get())
    {
        allStageBuffers.append(stageBuffer);
    }
}

void CommandObject::addStageDeviceMemory(const IDeviceMemorySP& stageDeviceMemory)
{
    if (stageDeviceMemory.get())
    {
        allStageDeviceMemories.append(stageDeviceMemory);
    }
}

//
// IDestroyable
//

void CommandObject::destroy()
{
    // Stage resources have to be deleted.

    for (uint32_t i = 0; i < allStageImages.size(); i++)
    {
        if (allStageImages[i].get())
        {
            allStageImages[i]->destroy();
        }
    }
    allStageImages.clear();

    for (uint32_t i = 0; i < allStageBuffers.size(); i++)
    {
        if (allStageBuffers[i].get())
        {
            allStageBuffers[i]->destroy();
        }
    }
    allStageBuffers.clear();

    for (uint32_t i = 0; i < allStageDeviceMemories.size(); i++)
    {
        if (allStageDeviceMemories[i].get())
        {
            allStageDeviceMemories[i]->destroy();
        }
    }
    allStageDeviceMemories.clear();
}

} /* namespace vkts */
