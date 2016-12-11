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

#ifndef VKTS_SCENE_HPP_
#define VKTS_SCENE_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class Scene: public IScene
{

private:

    std::string name;

    SmartPointerVector<IObjectSP> allObjects;

    SmartPointerVector<ICameraSP> allCameras;

    SmartPointerVector<ILightSP> allLights;

    ITextureObjectSP environment;

    ITextureObjectSP diffuseEnvironment;

    ITextureObjectSP specularEnvironment;

    ITextureObjectSP lut;

public:

    Scene();
    Scene(const Scene& other);
    Scene(Scene&& other) = delete;
    virtual ~Scene();

    Scene& operator =(const Scene& other) = delete;

    Scene& operator =(Scene && other) = delete;

    //
    // IScene
    //

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;


    virtual void addObject(const IObjectSP& object) override;

    virtual VkBool32 removeObject(const IObjectSP& object) override;

    virtual IObjectSP findObject(const std::string& name) const override;

    virtual size_t getNumberObjects() const override;

    virtual const SmartPointerVector<IObjectSP>& getObjects() const override;


    virtual void addCamera(const ICameraSP& camera) override;

    virtual VkBool32 removeCamera(const ICameraSP& camera) override;

    virtual ICameraSP findCamera(const std::string& name) const override;

    virtual size_t getNumberCameras() const override;

    virtual const SmartPointerVector<ICameraSP>& getCameras() const override;


    virtual void addLight(const ILightSP& light) override;

    virtual VkBool32 removeLight(const ILightSP& light) override;

    virtual ILightSP findLight(const std::string& name) const override;

    virtual size_t getNumberLights() const override;

    virtual const SmartPointerVector<ILightSP>& getLights() const override;


    virtual void setEnvironment(const ITextureObjectSP& environment) override;

    virtual ITextureObjectSP getEnvironment() const override;

    virtual void setDiffuseEnvironment(const ITextureObjectSP& diffuseEnvironment) override;

    virtual ITextureObjectSP getDiffuseEnvironment() const override;

    virtual void setSpecularEnvironment(const ITextureObjectSP& cookTorranceEnvironment) override;

    virtual ITextureObjectSP getSpecularEnvironment() const override;

    virtual void setLut(const ITextureObjectSP& environment) override;

    virtual ITextureObjectSP getLut() const override;

    //

    virtual void updateParameterRecursive(const Parameter* parameter, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) override;

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) override;

    virtual void updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t dynamicOffsetIndex = 0, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) override;

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets, const OverwriteDraw* renderOverwrite, const uint32_t objectOffset = 0, const uint32_t objectStep = 1, const size_t objectLimit = SIZE_MAX) override;

    //
    // ICloneable
    //

    virtual ISceneSP clone() const override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_SCENE_HPP_ */
