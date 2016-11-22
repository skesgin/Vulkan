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

#ifndef VKTS_CULL_HPP_
#define VKTS_CULL_HPP_

#include <vkts/scenegraph.hpp>

namespace vkts
{

class Cull : public Overwrite
{

private:

	const Frustum* viewFrustum;

public:

	Cull() :
		Overwrite(), viewFrustum(nullptr)
    {
    }

	Cull(const Frustum* viewFrustum) :
		Overwrite(), viewFrustum(viewFrustum)
    {
    }

    virtual ~Cull()
    {
    }

    //

	const Frustum* getViewFrustum() const
	{
		return viewFrustum;
	}

	void setViewFrustum(const Frustum* viewFrustum)
	{
		this->viewFrustum = viewFrustum;
	}

    //

    virtual VkBool32 objectBindDrawIndexedRecursive(const IObject& object, const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t bufferIndex) const
    {
    	if (viewFrustum)
    	{
    		if (viewFrustum->isVisible(object.getRootNode()->getBoundingSphere()))
    		{
    			return VK_TRUE;
    		}

    		return VK_FALSE;
    	}

    	return VK_TRUE;
    }
};

} /* namespace vkts */

#endif /* VKTS_CULL_HPP_ */
