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
 * IMPLIED, INCLUDING BUT NOT COPYED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef VKTS_ICOPYCONSTRAINT_HPP_
#define VKTS_ICOPYCONSTRAINT_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

enum CopyConstraintType {COPY_LOCATION, COPY_ROTATION, COPY_SCALE};

class ICopyConstraint : public IConstraint
{

public:

    ICopyConstraint() :
        IConstraint()
    {
    }

    virtual ~ICopyConstraint()
    {
    }

    virtual enum CopyConstraintType getType() const = 0;

    virtual const INodeSP& getTarget() const = 0;
    virtual void setTarget(const INodeSP& target) = 0;

    virtual const std::array<VkBool32, 3>& getUse() const = 0;
    virtual void setUse(const std::array<VkBool32, 3>& use) = 0;

    virtual const std::array<VkBool32, 3>& getInvert() const = 0;
    virtual void setInvert(const std::array<VkBool32, 3>& invert) = 0;

    virtual VkBool32 getOffset() const = 0;
    virtual void setOffset(const VkBool32 offset) = 0;

    virtual float getInfluence() const = 0;
    virtual void setInfluence(const float influence) = 0;

};

} /* namespace vkts */

#endif /* VKTS_ICOPYCONSTRAINT_HPP_ */
