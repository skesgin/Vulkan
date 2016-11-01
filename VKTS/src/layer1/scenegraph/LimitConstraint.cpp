/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Limitright (c) since 2014 Norbert Nopper
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
 * AUTHORS OR LIMITRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "LimitConstraint.hpp"

namespace vkts
{

LimitConstraint::LimitConstraint(const enum LimitConstraintType type) :
	IConstraint(), type(type), useMin{VK_FALSE, VK_FALSE, VK_FALSE}, useMax{VK_FALSE, VK_FALSE, VK_FALSE}, min(0.0f, 0.0f, 0.0f), max(0.0f, 0.0f, 0.0f), influence(0.0f)
{
}

LimitConstraint::LimitConstraint(const LimitConstraint& other) :
	IConstraint(), type(other.type), useMin(other.useMin), useMax(other.useMax), min(other.min), max(other.max), influence(other.influence)
{
}

LimitConstraint::~LimitConstraint()
{
}

enum LimitConstraintType LimitConstraint::getType() const
{
	return type;
}

const std::array<VkBool32, 3>& LimitConstraint::getUseMin() const
{
	return useMin;
}

void LimitConstraint::setUseMin(const std::array<VkBool32, 3>& use)
{
	this->useMin = use;
}

const std::array<VkBool32, 3>& LimitConstraint::getUseMax() const
{
	return useMax;
}

void LimitConstraint::setUseMax(const std::array<VkBool32, 3>& use)
{
	this->useMax = use;
}

const glm::vec3& LimitConstraint::getMin() const
{
	return min;
}

void LimitConstraint::setMin(const glm::vec3& min)
{
	this->min = min;
}

const glm::vec3& LimitConstraint::getMax() const
{
	return max;
}

void LimitConstraint::setMax(const glm::vec3& max)
{
	this->max = max;
}

float LimitConstraint::getInfluence() const
{
	return influence;
}

void LimitConstraint::setInfluence(const float influence)
{
	this->influence = influence;
}

//
// ILimitLocationConstraint
//

VkBool32 LimitConstraint::applyConstraint(INode& node)
{
	if (influence == 0.0f)
	{
		return VK_TRUE;
	}

	//

	glm::vec3 transform;

	switch (type)
	{
		case LIMIT_LOCATION:
			transform = node.getFinalTranslate();
			break;
		case LIMIT_ROTATION:
			transform = node.getFinalRotate();
			break;
		case LIMIT_SCALE:
			transform = node.getFinalScale();
			break;
	}

	//

	glm::vec3 finaleTransform = transform;

	for (int32_t i = 0; i < 3; i++)
	{
		if (getUseMin()[i])
		{
			finaleTransform[i] = glm::min(transform[i], min[i]);
		}
		if (getUseMax()[i])
		{
			finaleTransform[i] = glm::max(transform[i], max[i]);
		}
	}

	//

	switch (type)
	{
		case LIMIT_LOCATION:
			node.setFinalTranslate(glm::mix(transform, finaleTransform, influence));
			break;
		case LIMIT_ROTATION:
			node.setFinalRotate(glm::mix(transform, finaleTransform, influence));
			break;
		case LIMIT_SCALE:
			node.setFinalScale(glm::mix(transform, finaleTransform, influence));
			break;
	}

	return VK_TRUE;
}

//
// ICloneable
//

IConstraintSP LimitConstraint::clone() const
{
    return IConstraintSP(new LimitConstraint(*this));
}

} /* namespace vkts */
