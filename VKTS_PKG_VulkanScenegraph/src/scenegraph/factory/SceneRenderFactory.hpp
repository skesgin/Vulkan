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

#ifndef VKTS_SCENERENDERFACTORY_HPP_
#define VKTS_SCENERENDERFACTORY_HPP_

#include <vkts/vulkan/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class SceneRenderFactory : public ISceneRenderFactory
{

private:

	const IDescriptorSetLayoutSP descriptorSetLayout;

    const IRenderPassSP renderPass;

    const IPipelineCacheSP pipelineCache;

    const VkDeviceSize bufferCount;

public:

	SceneRenderFactory() = delete;

	SceneRenderFactory(const IDescriptorSetLayoutSP& descriptorSetLayout, const IRenderPassSP& renderPass, const IPipelineCacheSP& pipelineCache, const VkDeviceSize bufferCount);

    virtual ~SceneRenderFactory();

    //

    VkDeviceSize getBufferCount() const override;

    virtual IRenderNodeSP createRenderNode(const ISceneManagerSP& sceneManager) override;
    virtual IRenderSubMeshSP createRenderSubMesh(const ISceneManagerSP& sceneManager) override;
    virtual IRenderMaterialSP createRenderMaterial(const ISceneManagerSP& sceneManager) override;

    virtual VkBool32 preparePhongMaterial(const ISceneManagerSP& sceneManager, const IPhongMaterialSP& phongMaterial) override;

    virtual VkBool32 prepareBSDFMaterial(const ISceneManagerSP& sceneManager, const ISubMeshSP& subMesh) override;

    virtual VkBool32 prepareTransformUniformBuffer(const ISceneManagerSP& sceneManager, const INodeSP& node) override;
    virtual VkDeviceSize getTransformUniformBufferAlignmentSize(const ISceneManagerSP& sceneManager) const override;
    virtual VkBool32 prepareJointsUniformBuffer(const ISceneManagerSP& sceneManager, const INodeSP& node, const int32_t joints) override;
    virtual VkDeviceSize getJointsUniformBufferAlignmentSize(const ISceneManagerSP& sceneManager) const override;
};

} /* namespace vkts */

#endif /* VKTS_SCENERENDERFACTORY_HPP_ */
