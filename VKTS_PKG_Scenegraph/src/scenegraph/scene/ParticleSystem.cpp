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

#include "ParticleSystem.hpp"

namespace vkts
{

ParticleSystem::ParticleSystem() :
    IParticleSystem(), name(""), emissionNumber(0), emissionStart(0.0f), emissionEnd(0.0f), emissionLifetime(0.0f), emissionRandom(0.0f), emissionEmitFrom(0.0f), velocityNormalFactor(0.0f), velocityObjectAlignFactor(0.0f), velocityFactorRandom(0.0f), physicsParticleSize(0.0f), physicsSizeRandom(0.0f), physicsMass(0.0f), physicsMultiplySizeMass(0.0f), renderType(BillboardRenderType), renderObjectName(""), renderObject()
{
}

ParticleSystem::ParticleSystem(const ParticleSystem& other) :
	IParticleSystem(), name(other.name + "_clone"), emissionNumber(other.emissionNumber), emissionStart(other.emissionStart), emissionEnd(other.emissionEnd), emissionLifetime(other.emissionLifetime), emissionRandom(other.emissionRandom), emissionEmitFrom(other.emissionEmitFrom), velocityNormalFactor(other.velocityNormalFactor), velocityObjectAlignFactor(other.velocityObjectAlignFactor), velocityFactorRandom(other.velocityFactorRandom), physicsParticleSize(other.physicsParticleSize), physicsSizeRandom(other.physicsSizeRandom), physicsMass(other.physicsMass), physicsMultiplySizeMass(other.physicsMultiplySizeMass), renderType(other.renderType), renderObjectName(other.renderObjectName), renderObject(other.renderObject)
{
}

ParticleSystem::~ParticleSystem()
{
}

//
// IParticleSystem
//

const std::string& ParticleSystem::getName() const
{
    return name;
}

void ParticleSystem::setName(const std::string& name)
{
    this->name = name;
}

uint32_t ParticleSystem::getEmissionNumber() const
{
    return emissionNumber;
}

void ParticleSystem::setEmissionNumber(const uint32_t number)
{
    this->emissionNumber = number;
}

float ParticleSystem::getEmissionStart(const float start) const
{
    return emissionStart;
}

void ParticleSystem::setEmissionStart(const float start)
{
    this->emissionStart = start;
}

float ParticleSystem::getEmissionEnd() const
{
    return emissionEnd;
}

void ParticleSystem::setEmissionEnd(const float end)
{
    this->emissionEnd = end;
}

float ParticleSystem::getEmissionLifetime() const
{
    return emissionLifetime;
}

void ParticleSystem::setEmissionLifetime(const float lifetime)
{
    this->emissionLifetime = lifetime;
}

float ParticleSystem::getEmissionRandom() const
{
    return emissionRandom;
}

void ParticleSystem::setEmissionRandom(const float random)
{
    this->emissionRandom = random;
}

float ParticleSystem::getEmissionEmitFrom() const
{
    return emissionEmitFrom;
}

void ParticleSystem::setEmissionEmitFrom(const float emitfrom)
{
    this->emissionEmitFrom = emitfrom;
}

float ParticleSystem::getVelocityNormalFactor() const
{
    return velocityNormalFactor;
}

void ParticleSystem::setVelocityNormalFactor(const float factor)
{
    this->velocityNormalFactor = factor;
}

float ParticleSystem::getVelocityObjectAlignFactor() const
{
    return velocityObjectAlignFactor;
}

void ParticleSystem::setVelocityObjectAlignFactor(const float factor)
{
    this->velocityObjectAlignFactor = factor;
}

float ParticleSystem::getVelocityFactorRandom() const
{
    return velocityFactorRandom;
}

void ParticleSystem::setVelocityFactorRandom(const float factor)
{
    this->velocityFactorRandom = factor;
}

float ParticleSystem::getPhysicsParticleSize() const
{
    return physicsParticleSize;
}

void ParticleSystem::setPhysicsParticleSize(const float size)
{
    this->physicsParticleSize = size;
}

float ParticleSystem::getPhysicsSizeRandom() const
{
    return physicsSizeRandom;
}

void ParticleSystem::setPhysicsSizeRandom(const float size)
{
    this->physicsSizeRandom = size;
}

float ParticleSystem::getPhysicsMass() const
{
    return physicsMass;
}

void ParticleSystem::setPhysicsMass(const float mass)
{
    this->physicsMass = mass;
}

float ParticleSystem::getPhysicsMultiplySizeMass() const
{
    return physicsMultiplySizeMass;
}

void ParticleSystem::setPhysicsMultiplySizeMass(const float multiply)
{
    this->physicsMultiplySizeMass = multiply;
}

enum RenderType ParticleSystem::getRenderType() const
{
    return renderType;
}

void ParticleSystem::setRenderType(const enum RenderType renderType)
{
    this->renderType = renderType;
}

const std::string& ParticleSystem::getRenderObjectName() const
{
    return renderObjectName;
}

void ParticleSystem::setRenderObjectName(const std::string& objectName)
{
	this->renderObjectName = objectName;

	renderObject = IObjectSP();
}

const IObjectSP& ParticleSystem::getRenderObject() const
{
	return renderObject;
}

void ParticleSystem::setRenderObject(const IObjectSP& object)
{
	this->renderObject = object;

	if (object.get())
	{
		renderObjectName = renderObject->getName();
	}
	else
	{
		renderObjectName = "";
	}
}

/*void ParticleSystem::visitRecursive(ISceneVisitor* sceneVisitor)
{
	ISceneVisitor* currentSceneVisitor = sceneVisitor;

	while (currentSceneVisitor)
	{
		if (!currentSceneVisitor->visit(*this))
		{
			return;
		}

		currentSceneVisitor = currentSceneVisitor->getNextSceneVisitor();
	}
}*/

//
// ICloneable
//

IParticleSystemSP ParticleSystem::clone() const
{
    return IParticleSystemSP(new ParticleSystem(*this));
}

} /* namespace vkts */
