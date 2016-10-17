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

#ifndef VKTS_IPARTICLESYSTEM_HPP_
#define VKTS_IPARTICLESYSTEM_HPP_

#include <vkts/vkts.hpp>

namespace vkts
{

enum RenderType {BillboardRenderType, ObjectRenderType};

class IParticleSystem : public ICloneable<IParticleSystem>
{

public:

    IParticleSystem() :
        ICloneable<IParticleSystem>()
    {
    }

    virtual ~IParticleSystem()
    {
    }

    virtual const std::string& getName() const = 0;
    virtual void setName(const std::string& name) = 0;

    virtual uint32_t getEmissionNumber() const = 0;
    virtual void setEmissionNumber(const uint32_t number) = 0;

    virtual float getEmissionStart(const float start) const = 0;
    virtual void setEmissionStart(const float start) = 0;

    virtual float getEmissionEnd() const = 0;
    virtual void setEmissionEnd(const float end) = 0;

    virtual float getEmissionLifetime() const = 0;
    virtual void setEmissionLifetime(const float lifetime) = 0;

    virtual float getEmissionRandom() const = 0;
    virtual void setEmissionRandom(const float random) = 0;

    virtual float getEmissionEmitFrom() const = 0;
    virtual void setEmissionEmitFrom(const float emitfrom) = 0;

    virtual float getVelocityNormalFactor() const = 0;
    virtual void setVelocityNormalFactor(const float factor) = 0;

    virtual float getVelocityObjectAlignFactor() const = 0;
    virtual void setVelocityObjectAlignFactor(const float factor) = 0;

    virtual float getVelocityFactorRandom() const = 0;
    virtual void setVelocityFactorRandom(const float factor) = 0;

    virtual float getPhysicsParticleSize() const = 0;
    virtual void setPhysicsParticleSize(const float size) = 0;

    virtual float getPhysicsSizeRandom() const = 0;
    virtual void setPhysicsSizeRandom(const float size) = 0;

    virtual float getPhysicsMass() const = 0;
    virtual void setPhysicsMass(const float mass) = 0;

    virtual float getPhysicsMultiplySizeMass() const = 0;
    virtual void setPhysicsMultiplySizeMass(const float multiply) = 0;

    virtual enum RenderType getRenderType() const = 0;
    virtual void setRenderType(const enum RenderType renderType) = 0;

    virtual const std::string& getRenderObjectName() const = 0;
    virtual void setRenderObjectName(const std::string& objectName) = 0;

    virtual const IObjectSP& getRenderObject() const = 0;
    virtual void setRenderObject(const IObjectSP& object) = 0;

};

typedef std::shared_ptr<IParticleSystem> IParticleSystemSP;

} /* namespace vkts */

#endif /* VKTS_IPARTICLESYSTEM_HPP_ */
