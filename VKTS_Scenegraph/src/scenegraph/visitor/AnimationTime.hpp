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

#ifndef VKTS_ANIMATIONTIME_HPP_
#define VKTS_ANIMATIONTIME_HPP_

#include <vkts/scenegraph.hpp>

#include "SceneVisitor.hpp"

namespace vkts
{

class AnimationTime : public SceneVisitor
{

private:

	float currentTime;

public:

	AnimationTime() :
		SceneVisitor(), currentTime(0.0f)
    {
    }

	AnimationTime(const float currentTime) :
		SceneVisitor(), currentTime(currentTime)
    {
    }

    virtual ~AnimationTime()
    {
    }

    //

	float getCurrentTime() const
	{
		return currentTime;
	}

	void setCurrentTime(const float currentTime)
	{
		this->currentTime = currentTime;
	}

    //

    virtual VkBool32 visit(Node& node)
    {
    	for (size_t i = 0; i < node.getAnimations().size(); i++)
    	{
    		node.getAnimations()[i]->setCurrentTime(currentTime);
    	}

    	return VK_TRUE;
    }
};

} /* namespace vkts */

#endif /* VKTS_ANIMATIONTIME_HPP_ */
