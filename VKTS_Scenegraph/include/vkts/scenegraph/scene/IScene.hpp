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

#ifndef VKTS_ISCENE_HPP_
#define VKTS_ISCENE_HPP_

#include <vkts/scenegraph.hpp>

namespace vkts
{

class SceneVisitor;
class Parameter;
class OverwriteDraw;

class IScene: public ICloneable<IScene>, public IDestroyable
{

public:

    IScene() :
        ICloneable<IScene>(), IDestroyable()
    {
    }

    virtual ~IScene()
    {
    }

    virtual const std::string& getName() const = 0;

    virtual void setName(const std::string& name) = 0;


    virtual void addObject(const IObjectSP& object) = 0;

    virtual VkBool32 removeObject(const IObjectSP& object) = 0;

    virtual IObjectSP findObject(const std::string& name) const = 0;

    virtual size_t getNumberObjects() const = 0;

    virtual const SmartPointerVector<IObjectSP>& getObjects() const = 0;


    virtual void addCamera(const ICameraSP& camera) = 0;

    virtual VkBool32 removeCamera(const ICameraSP& camera) = 0;

    virtual ICameraSP findCamera(const std::string& name) const = 0;

    virtual size_t getNumberCameras() const = 0;

    virtual const SmartPointerVector<ICameraSP>& getCameras() const = 0;


    virtual void addLight(const ILightSP& light) = 0;

    virtual VkBool32 removeLight(const ILightSP& light) = 0;

    virtual ILightSP findLight(const std::string& name) const = 0;

    virtual size_t getNumberLights() const = 0;

    virtual const SmartPointerVector<ILightSP>& getLights() const = 0;


    virtual void setEnvironment(const ITextureObjectSP& environment) = 0;

    virtual ITextureObjectSP getEnvironment() const = 0;

    virtual void setDiffuseEnvironment(const ITextureObjectSP& diffuseEnvironment) = 0;

    virtual ITextureObjectSP getDiffuseEnvironment() const = 0;

    virtual void setSpecularEnvironment(const ITextureObjectSP& specularEnvironment) = 0;

    virtual ITextureObjectSP getSpecularEnvironment() const = 0;

    virtual void setLut(const ITextureObjectSP& environment) = 0;

    virtual ITextureObjectSP getLut() const = 0;

    //

    virtual void updateParameterRecursive(const Parameter* parameter, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) = 0;

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) = 0;

    virtual void updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) = 0;

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const OverwriteDraw* renderOverwrite, const uint32_t bufferIndex, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) = 0;

    //

    virtual void visitRecursive(SceneVisitor* sceneVisitor, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) = 0;

};

typedef std::shared_ptr<IScene> ISceneSP;

} /* namespace vkts */

#endif /* VKTS_ISCENE_HPP_ */
