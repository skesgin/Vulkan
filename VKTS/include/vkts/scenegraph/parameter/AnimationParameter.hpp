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

#ifndef VKTS_ANIMATIONPARAMETER_HPP_
#define VKTS_ANIMATIONPARAMETER_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class AnimationParameter : public Parameter
{

private:

	enum AnimationType animationType;
	VkBool32 writeAnimationType;

	float currentTime;
	VkBool32 writeCurrentTime;

public:

	AnimationParameter() :
		Parameter(), animationType(AnimationStop), writeAnimationType(VK_TRUE), currentTime(0.0f), writeCurrentTime(VK_TRUE)
    {
    }

	AnimationParameter(const float currentTime) :
		Parameter(), animationType(AnimationStop), writeAnimationType(VK_TRUE), currentTime(currentTime), writeCurrentTime(VK_TRUE)
    {
    }

	AnimationParameter(const enum AnimationType animationType, const float currentTime) :
		Parameter(), animationType(animationType), writeAnimationType(VK_TRUE), currentTime(currentTime), writeCurrentTime(VK_TRUE)
    {
    }

    virtual ~AnimationParameter()
    {
    }

    //

    enum AnimationType getAnimationType() const
	{
		return animationType;
	}

	void setAnimationType(const enum AnimationType animationType)
	{
		this->animationType = animationType;
	}

	VkBool32 getWriteAnimationType() const
	{
		return writeAnimationType;
	}

	void setWriteAnimationType(const VkBool32 writeAnimationType)
	{
		this->writeAnimationType = writeAnimationType;
	}

	float getCurrentTime() const
	{
		return currentTime;
	}

	void setCurrentTime(const float currentTime)
	{
		this->currentTime = currentTime;
	}

	VkBool32 getWriteCurrentTime() const
	{
		return writeCurrentTime;
	}

	void setWriteCurrentTime(const VkBool32 writeCurrentTime)
	{
		this->writeCurrentTime = writeCurrentTime;
	}

    //

    virtual void visit(INode& node)
    {
    	for (uint32_t i = 0; i < node.getAnimations().size(); i++)
    	{
    		if (writeAnimationType)
    		{
    			node.getAnimations()[i]->setAnimationType(animationType);
    		}

    		if (writeCurrentTime)
    		{
    			node.getAnimations()[i]->setCurrentTime(currentTime);
    		}
    	}
    }
};

} /* namespace vkts */

#endif /* VKTS_ANIMATIONPARAMETER_HPP_ */
