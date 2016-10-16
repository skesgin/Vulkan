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

#ifndef VKTS_ILIGHT_HPP_
#define VKTS_ILIGHT_HPP_

#include <vkts/vkts.hpp>

namespace vkts
{

enum LightType {PointLight, DirectionalLight, SpotLight};

enum FalloffType {QuadraticFalloff, LinearFalloff, ConstantFalloff};

class ILight : public ICloneable<ILight>
{

public:

    ILight() :
        ICloneable<ILight>()
    {
    }

    virtual ~ILight()
    {
    }

    virtual const std::string& getName() const = 0;

    virtual void setName(const std::string& name) = 0;

    virtual uint32_t getIndex() const = 0;

    virtual void setIndex(const uint32_t index) = 0;

    virtual enum LightType getLightType() const = 0;

    virtual void setLightType(const enum LightType lightType) = 0;

    virtual enum FalloffType getFalloffType() const = 0;

    virtual void setFalloffType(const enum FalloffType falloffType) = 0;

    virtual float getOuterAngle() const = 0;

    virtual void setOuterAngle(const float angle) = 0;

    virtual float getInnerAngle() const = 0;

    virtual void setInnerAngle(const float angle) = 0;

    virtual const glm::vec4& getColor() const = 0;

    virtual void setColor(const glm::vec4& color) = 0;

    virtual const glm::vec4& getDirection() const = 0;

    virtual void updateDirection(const glm::mat4& transform) = 0;

};

typedef std::shared_ptr<ILight> ILightSP;

} /* namespace vkts */

#endif /* VKTS_ILIGHT_HPP_ */
