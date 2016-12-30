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

#ifndef VKTS_COMMANDOBJECT_HPP_
#define VKTS_COMMANDOBJECT_HPP_

#include <vkts/vulkan/composition/vkts_composition.hpp>

namespace vkts
{

class CommandObject: public ICommandObject
{

private:

	const ICommandBuffersSP cmdBuffer;

    //

    SmartPointerVector<IImageSP> allStageImages;
    SmartPointerVector<IBufferSP> allStageBuffers;
    SmartPointerVector<IDeviceMemorySP> allStageDeviceMemories;

public:

    CommandObject() = delete;
    explicit CommandObject(const ICommandBuffersSP& cmdBuffer);
    CommandObject(const CommandObject& other) = delete;
    CommandObject(CommandObject&& other) = delete;
    virtual ~CommandObject();

    CommandObject& operator =(const CommandObject& other) = delete;

    CommandObject& operator =(CommandObject && other) = delete;

    //
    // ICommandObject
    //

    virtual const ICommandBuffersSP& getCommandBuffer() const override;

    //

    virtual void addStageImage(const IImageSP& stageImage) override;

    virtual void addStageBuffer(const IBufferSP& stageBuffer) override;

    virtual void addStageDeviceMemory(const IDeviceMemorySP& stageDeviceMemory) override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_COMMANDOBJECT_HPP_ */
