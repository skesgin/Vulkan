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

#include "Light.hpp"

namespace vkts
{

Light::Light() :
    ILight(), name(""), index(0), lightType(PointLight), falloffType(QuadraticFalloff), strength(1.0f), outerAngle(0.0f), innerAngle(0.0f), color(0.0f, 0.0f, 0.0f), direction(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Light::Light(const std::string& name = "Light", const enum LightType lightType = LightType::DirectionalLight, const float stength = 1.0f , const glm::vec3& color = glm::vec3(1), const glm::vec4& direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) :
    ILight(), name(std::move(name)), index(0), lightType(lightType), falloffType(QuadraticFalloff), strength(stength), outerAngle(0.0f), innerAngle(0.0f), color(std::move(color)), direction(std::move(direction))
{
}

Light::Light(const Light& other) :
	ILight(), name(other.name + "_clone"), index(other.index), lightType(other.lightType), falloffType(other.falloffType), strength(other.strength), outerAngle(other.outerAngle), innerAngle(other.innerAngle), color(other.color), direction(other.direction)
{
}

Light::~Light()
{
}

//
// ILight
//

const std::string& Light::getName() const
{
    return name;
}

void Light::setName(const std::string& name)
{
    this->name = name;
}

uint32_t Light::getIndex() const
{
	return index;
}

void Light::setIndex(const uint32_t index)
{
	this->index = index;
}

enum LightType Light::getLightType() const
{
	return lightType;
}

void Light::setLightType(const enum LightType lightType)
{
	this->lightType = lightType;

	if (this->lightType == DirectionalLight)
	{
		this->falloffType = ConstantFalloff;
	}
}

enum FalloffType Light::getFalloffType() const
{
	return falloffType;
}

void Light::setFalloffType(const enum FalloffType falloffType)
{
	this->falloffType = falloffType;

	if (this->falloffType != ConstantFalloff && this->lightType == DirectionalLight)
	{
		throw std::invalid_argument("Invalid fall off and light type combination!");
	}
}

float Light::getStrength() const
{
	return strength;
}

void Light::setStrength(const float stength)
{
	this->strength = stength;
}

float Light::getOuterAngle() const
{
	return outerAngle;
}

void Light::setOuterAngle(const float angle)
{
	this->outerAngle = angle;
}

float Light::getInnerAngle() const
{
	return innerAngle;
}

void Light::setInnerAngle(const float angle)
{
	this->innerAngle = angle;
}

const glm::vec3& Light::getColor() const
{
	return color;
}

void Light::setColor(const glm::vec3& color)
{
	this->color = color;
}

const glm::vec4& Light::getDirection() const
{
	return direction;
}

void Light::updateDirection(const glm::mat4& transform)
{
	if (lightType == DirectionalLight)
	{
		direction = transform * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	}
	else
	{
		direction = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

//
// ICloneable
//

ILightSP Light::clone() const
{
    return ILightSP(new Light(*this));
}

} /* namespace vkts */
