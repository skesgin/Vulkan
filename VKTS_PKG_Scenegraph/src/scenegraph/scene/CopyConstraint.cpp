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

#include "CopyConstraint.hpp"

namespace vkts
{

CopyConstraint::CopyConstraint(const enum CopyConstraintType type) :
	ICopyConstraint(), type(type), target(nullptr), use{VK_FALSE, VK_FALSE, VK_FALSE}, invert{VK_FALSE, VK_FALSE, VK_FALSE}, offset(VK_FALSE), influence(0.0f)
{
}

CopyConstraint::CopyConstraint(const CopyConstraint& other) :
	ICopyConstraint(), type(other.type), target(other.target), use(other.use), invert(other.invert), offset(other.offset), influence(other.influence)
{
}

CopyConstraint::~CopyConstraint()
{
}

enum CopyConstraintType CopyConstraint::getType() const
{
	return type;
}

const INodeSP& CopyConstraint::getTarget() const
{
	return target;
}

void CopyConstraint::setTarget(const INodeSP& target)
{
	this->target = target;
}

const std::array<VkBool32, 3>& CopyConstraint::getUse() const
{
	return use;
}

void CopyConstraint::setUse(const std::array<VkBool32, 3>& use)
{
	this->use = use;
}

VkBool32 CopyConstraint::getOffset() const
{
	return offset;
}

void CopyConstraint::setOffset(const VkBool32 offset)
{
	this->offset = offset;
}

const std::array<VkBool32, 3>& CopyConstraint::getInvert() const
{
	return invert;
}

void CopyConstraint::setInvert(const std::array<VkBool32, 3>& invert)
{
	this->invert = invert;
}

float CopyConstraint::getInfluence() const
{
	return influence;
}

void CopyConstraint::setInfluence(const float influence)
{
	this->influence = influence;
}

//
// ICopyLocationConstraint
//

VkBool32 CopyConstraint::applyConstraint(INode& node)
{
	if (!target.get())
	{
		return VK_FALSE;
	}

	if (influence == 0.0f)
	{
		return VK_TRUE;
	}

	//

	glm::vec3 currentTransform;
	glm::vec3 targetTransform;

	switch (type)
	{
		case COPY_LOCATION:
			currentTransform = node.getFinalTranslate();
			targetTransform  = decomposeTranslate(target->getTransformMatrix());
			break;
		case COPY_ROTATION:
			currentTransform = node.getFinalRotate();
			targetTransform  = decomposeRotateRzRyRx(target->getTransformMatrix());
			break;
		case COPY_SCALE:
			currentTransform = node.getFinalScale();
			targetTransform  = decomposeScale(target->getTransformMatrix());
			break;
	}

	//

	glm::vec3 constraintTransform = currentTransform;

	for (int32_t i = 0; i < 3; i++)
	{
		if (getUse()[i])
		{
			float value = targetTransform[i];

			if (getOffset())
			{
				constraintTransform[i] += value;
			}
			else
			{
				constraintTransform[i] = value;
			}

			if (getInvert()[i])
			{
				constraintTransform[i] = -constraintTransform[i];
			}
		}
	}

	//

	switch (type)
	{
		case COPY_LOCATION:
			node.setFinalTranslate(glm::mix(currentTransform, constraintTransform, influence));
			break;
		case COPY_ROTATION:
			node.setFinalRotate(glm::mix(currentTransform, constraintTransform, influence));
			break;
		case COPY_SCALE:
			node.setFinalScale(glm::mix(currentTransform, constraintTransform, influence));
			break;
	}

	return VK_TRUE;
}

//
// ICloneable
//

IConstraintSP CopyConstraint::clone() const
{
    return IConstraintSP(new CopyConstraint(*this));
}

} /* namespace vkts */
