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

#ifndef VKTS_LIGHT_HPP_
#define VKTS_LIGHT_HPP_

#include <vkts/scenegraph.hpp>

namespace vkts
{

class Light : public ILight
{

private:

    std::string name;

    uint32_t index;

    enum LightType lightType;

    enum FalloffType falloffType;

    float strength;

    float outerAngle;

    float innerAngle;

    glm::vec3 color;

    glm::vec4 direction;

public:

    Light();
    Light(const Light& other);
    Light(Light&& other) = delete;
    virtual ~Light();

    Light& operator =(const Light& other) = delete;
    Light& operator =(Light && other) = delete;

    //
    // ILight
    //

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;

    virtual uint32_t getIndex() const override;

    virtual void setIndex(const uint32_t index) override;

    virtual enum LightType getLightType() const override;

    virtual void setLightType(const enum LightType lightType) override;

    virtual enum FalloffType getFalloffType() const override;

    virtual void setFalloffType(const enum FalloffType falloffType) override;

    virtual float getStrength() const override;

    virtual void setStrength(const float stength) override;

    virtual float getOuterAngle() const override;

    virtual void setOuterAngle(const float angle) override;

    virtual float getInnerAngle() const override;

    virtual void setInnerAngle(const float angle) override;

    virtual const glm::vec3& getColor() const override;

    virtual void setColor(const glm::vec3& color) override;

    virtual const glm::vec4& getDirection() const override;

    virtual void updateDirection(const glm::mat4& transform) override;

    //
    // ICloneable
    //

    virtual ILightSP clone() const override;

};

} /* namespace vkts */

#endif /* VKTS_LIGHT_HPP_ */
