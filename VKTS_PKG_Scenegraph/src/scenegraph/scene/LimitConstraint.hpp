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

#ifndef VKTS_LIMITCONSTRAINT_HPP_
#define VKTS_LIMITCONSTRAINT_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class LimitConstraint : public ILimitConstraint
{

private:

	enum LimitConstraintType type;

	std::array<VkBool32, 3> useMin;
	std::array<VkBool32, 3> useMax;
	glm::vec3 min;
	glm::vec3 max;
	float influence;

public:

	LimitConstraint() = delete;
	explicit LimitConstraint(const enum LimitConstraintType type);
	LimitConstraint(const LimitConstraint& other);
	LimitConstraint(LimitConstraint&& other) = delete;
    virtual ~LimitConstraint();

    LimitConstraint& operator =(const LimitConstraint& other) = delete;
    LimitConstraint& operator =(LimitConstraint && other) = delete;

    //
    // ILimitConstraint
    //

    virtual enum LimitConstraintType getType() const override;

    virtual const std::array<VkBool32, 3>& getUseMin() const override;
    virtual void setUseMin(const std::array<VkBool32, 3>& use) override;

    virtual const std::array<VkBool32, 3>& getUseMax() const override;
    virtual void setUseMax(const std::array<VkBool32, 3>& use) override;

    virtual const glm::vec3& getMin() const override;
    virtual void setMin(const glm::vec3& min) override;

    virtual const glm::vec3& getMax() const override;
    virtual void setMax(const glm::vec3& max) override;

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

#endif /* VKTS_LIMITCONSTRAINT_HPP_ */
