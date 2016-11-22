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

#ifndef VKTS_PARTICLESYSTEM_HPP_
#define VKTS_PARTICLESYSTEM_HPP_

#include <vkts/scenegraph.hpp>

namespace vkts
{

class ParticleSystem : public IParticleSystem
{

private:

    std::string name;

    uint32_t emissionNumber;
    float emissionStart;
	float emissionEnd;
	float emissionLifetime;
	float emissionRandom;
	float emissionEmitFrom;

	float velocityNormalFactor;
	float velocityObjectAlignFactor;
	float velocityFactorRandom;

	float physicsParticleSize;
	float physicsSizeRandom;
	float physicsMass;
	float physicsMultiplySizeMass;

    enum RenderType renderType;
    std::string renderObjectName;
    IObjectSP renderObject;

public:

    ParticleSystem();
    ParticleSystem(const ParticleSystem& other);
    ParticleSystem(ParticleSystem&& other) = delete;
    virtual ~ParticleSystem();

    ParticleSystem& operator =(const ParticleSystem& other) = delete;
    ParticleSystem& operator =(ParticleSystem && other) = delete;

    //
    // IParticleSystem
    //

    virtual const std::string& getName() const override;
    virtual void setName(const std::string& name) override;

    virtual uint32_t getEmissionNumber() const override;
    virtual void setEmissionNumber(const uint32_t number) override;

    virtual float getEmissionStart(const float start) const override;
    virtual void setEmissionStart(const float start) override;

    virtual float getEmissionEnd() const override;
    virtual void setEmissionEnd(const float end) override;

    virtual float getEmissionLifetime() const override;
    virtual void setEmissionLifetime(const float lifetime) override;

    virtual float getEmissionRandom() const override;
    virtual void setEmissionRandom(const float random) override;

    virtual float getEmissionEmitFrom() const override;
    virtual void setEmissionEmitFrom(const float emitfrom) override;

    virtual float getVelocityNormalFactor() const override;
    virtual void setVelocityNormalFactor(const float factor) override;

    virtual float getVelocityObjectAlignFactor() const override;
    virtual void setVelocityObjectAlignFactor(const float factor) override;

    virtual float getVelocityFactorRandom() const override;
    virtual void setVelocityFactorRandom(const float factor) override;

    virtual float getPhysicsParticleSize() const override;
    virtual void setPhysicsParticleSize(const float size) override;

    virtual float getPhysicsSizeRandom() const override;
    virtual void setPhysicsSizeRandom(const float size) override;

    virtual float getPhysicsMass() const override;
    virtual void setPhysicsMass(const float mass) override;

    virtual float getPhysicsMultiplySizeMass() const override;
    virtual void setPhysicsMultiplySizeMass(const float multiply) override;

    virtual enum RenderType getRenderType() const override;
    virtual void setRenderType(const enum RenderType renderType) override;

    virtual const std::string& getRenderObjectName() const override;
    virtual void setRenderObjectName(const std::string& objectName) override;

    virtual const IObjectSP& getRenderObject() const override;
    virtual void setRenderObject(const IObjectSP& object) override;

    //
    // ICloneable
    //

    virtual IParticleSystemSP clone() const override;

};

} /* namespace vkts */

#endif /* VKTS_PARTICLESYSTEM_HPP_ */
