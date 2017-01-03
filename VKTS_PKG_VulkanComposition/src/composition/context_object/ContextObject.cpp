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

#include "ContextObject.hpp"

namespace vkts
{

ContextObject::ContextObject(const IInstanceSP& instance, const IPhysicalDeviceSP& physicalDevice, const IDeviceSP& device, const IQueueSP& queue, const VkBool32 manage) :
    IContextObject(), instance(instance), physicalDevice(physicalDevice), device(device), queue(queue), manage(manage)
{
}

ContextObject::~ContextObject()
{
    destroy();
}

//
// IContextObject
//

const IInstanceSP& ContextObject::getInstance() const
{
    return instance;
}

const IPhysicalDeviceSP& ContextObject::getPhysicalDevice() const
{
    return physicalDevice;
}

void ContextObject::destroyInstance()
{
	physicalDevice.reset();

    if (instance.get())
    {
    	if (manage)
    	{
    		instance->destroy();
    	}

        instance.reset();
    }
}

const IDeviceSP& ContextObject::getDevice() const
{
    return device;
}

const IQueueSP& ContextObject::getQueue() const
{
    return queue;
}

void ContextObject::destroyDevice()
{
	queue.reset();

    if (device.get())
    {
    	if (manage)
    	{
    		device->destroy();
    	}

    	device.reset();
    }
}

//
// IDeytroyable
//

void ContextObject::destroy()
{
	destroyDevice();

	destroyInstance();
}

} /* namespace vkts */
