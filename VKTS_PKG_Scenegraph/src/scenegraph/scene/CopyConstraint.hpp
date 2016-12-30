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

#ifndef VKTS_COPYCONSTRAINT_HPP_
#define VKTS_COPYCONSTRAINT_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class CopyConstraint : public ICopyConstraint
{

private:

	enum CopyConstraintType type;

	INodeSP target;
	std::array<VkBool32, 3> use;
	std::array<VkBool32, 3> invert;
	VkBool32 offset;
	float influence;

public:

	CopyConstraint() = delete;
	explicit CopyConstraint(const enum CopyConstraintType type);
	CopyConstraint(const CopyConstraint& other);
	CopyConstraint(CopyConstraint&& other) = delete;
    virtual ~CopyConstraint();

    CopyConstraint& operator =(const CopyConstraint& other) = delete;
    CopyConstraint& operator =(CopyConstraint && other) = delete;

    //
    // ICopyConstriant
    //

    virtual enum CopyConstraintType getType() const override;

    virtual const INodeSP& getTarget() const override;
	virtual void setTarget(const INodeSP& target) override;

	virtual const std::array<VkBool32, 3>& getUse() const override;
	virtual void setUse(const std::array<VkBool32, 3>& use) override;

	virtual const std::array<VkBool32, 3>& getInvert() const override;
	virtual void setInvert(const std::array<VkBool32, 3>& invert) override;

	virtual VkBool32 getOffset() const override;
	virtual void setOffset(const VkBool32 offset) override;

	virtual float getInfluence() const override;
	virtual void setInfluence(const float influence) override;

    //
    // IConstraint
    //

    virtual VkBool32 applyConstraint(INode& node) override;

    //
    // ICloneable
    //

    virtual IConstraintSP clone() const override;

};

} /* namespace vkts */

#endif /* VKTS_COPYCONSTRAINT_HPP_ */
