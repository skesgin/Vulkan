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

#include "UpdateThreadContext.hpp"

namespace vkts
{

UpdateThreadContext::UpdateThreadContext(const int32_t threadIndex, const int32_t threadCount, const double tickTime, const TaskQueueSP& sendTaskQueue, const TaskQueueSP& executedTaskQueue) :
    IUpdateThreadContext(), threadIndex(threadIndex), threadCount(threadCount), sendTaskQueue(sendTaskQueue), executedTaskQueue(executedTaskQueue)
{
    this->startTime = timeGetRaw();
    this->lastTime = startTime;
    this->currentTime = startTime;

    this->tickTime = tickTime;
    this->lastTicks = 0;
    this->currentTicks = 0;
}

UpdateThreadContext::~UpdateThreadContext()
{
}

void UpdateThreadContext::update()
{
    lastTime = currentTime;
    currentTime = timeGetRaw();

    lastTicks = currentTicks;
    currentTicks = static_cast<uint64_t>(getTotalTime() / getTickTime());
}

//
// IUpdateThreadContext
//

// Thread functions.

int32_t UpdateThreadContext::getThreadIndex() const
{
    return threadIndex;
}

int32_t UpdateThreadContext::getThreadCount() const
{
    return threadCount;
}

// Time functions.

double UpdateThreadContext::getTotalTime() const
{
    return currentTime - startTime;
}

double UpdateThreadContext::getDeltaTime() const
{
    return currentTime - lastTime;
}

double UpdateThreadContext::getTickTime() const
{
    return tickTime;
}

uint64_t UpdateThreadContext::getTotalTicks() const
{
    return currentTicks;
}

uint64_t UpdateThreadContext::getDeltaTicks() const
{
    return currentTicks - lastTicks;
}

VkBool32 UpdateThreadContext::sendTask(const ITaskSP& task) const
{
    if (!sendTaskQueue.get())
    {
        return VK_FALSE;
    }

    return sendTaskQueue->addTask(task);
}

VkBool32 UpdateThreadContext::receiveExecutedTask(ITaskSP& task, const VkBool32 wait) const
{
    if (!executedTaskQueue.get())
    {
        return VK_FALSE;
    }

    return executedTaskQueue->receiveTask(task, wait);
}

void UpdateThreadContext::resetSendTasks() const
{
    if (sendTaskQueue.get())
    {
    	sendTaskQueue->reset();
    }
}

void UpdateThreadContext::resetExecutedTasks() const
{
    if (executedTaskQueue.get())
    {
    	executedTaskQueue->reset();
    }
}

} /* namespace vkts */
