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

#ifndef VKTS_UPDATETHREADCONTEXT_HPP_
#define VKTS_UPDATETHREADCONTEXT_HPP_

#include <vkts/runtime/vkts_runtime.hpp>

#include "TaskQueue.hpp"

namespace vkts
{

class UpdateThreadContext: public IUpdateThreadContext
{

private:

    const int32_t threadIndex;

    const int32_t threadCount;

    double startTime;
    double lastTime;
    double currentTime;

    double tickTime;

    TaskQueueSP sendTaskQueue;
    TaskQueueSP executedTaskQueue;

    uint64_t lastTicks;
    uint64_t currentTicks;

public:

    UpdateThreadContext() = delete;
    UpdateThreadContext(const UpdateThreadContext& other) = delete;
    UpdateThreadContext(UpdateThreadContext&& other) = delete;
    UpdateThreadContext(const int32_t threadIndex, const int32_t threadCount, const double tickTime, const TaskQueueSP& sendTaskQueue, const TaskQueueSP& executedTaskQueue);
    virtual ~UpdateThreadContext();

    UpdateThreadContext& operator =(const UpdateThreadContext& other) = delete;
    UpdateThreadContext& operator =(UpdateThreadContext && other) = delete;

    void update();

    //
    // IUpdateThreadContext
    //

    // Thread functions.

    virtual int32_t getThreadIndex() const override;

    virtual int32_t getThreadCount() const override;

    // Time functions.

    virtual double getTotalTime() const override;

    virtual double getDeltaTime() const override;

    virtual double getTickTime() const override;

    virtual uint64_t getTotalTicks() const override;

    virtual uint64_t getDeltaTicks() const override;

    // Task functions

    virtual VkBool32 sendTask(const ITaskSP& task) const override;

    virtual VkBool32 receiveExecutedTask(ITaskSP& task, const VkBool32 wait = VK_TRUE) const override;

    virtual void resetSendTasks() const override;

    virtual void resetExecutedTasks() const override;

};

typedef std::shared_ptr<UpdateThreadContext> UpdateThreadContextSP;

} /* namespace vkts */

#endif /* VKTS_UPDATETHREADCONTEXT_HPP_ */
