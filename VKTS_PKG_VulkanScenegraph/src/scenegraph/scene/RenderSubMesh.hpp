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

#ifndef VKTS_RENDERSUBMESH_HPP_
#define VKTS_RENDERSUBMESH_HPP_

#include <vkts/vulkan/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class RenderSubMesh: public IRenderSubMesh
{

public:

    RenderSubMesh();
    RenderSubMesh(const RenderSubMesh& other) = delete;
    RenderSubMesh(RenderSubMesh&& other) = delete;
    virtual ~RenderSubMesh();

    RenderSubMesh& operator =(const RenderSubMesh& other) = delete;
    RenderSubMesh& operator =(RenderSubMesh && other) = delete;

    virtual IRenderSubMeshSP create(const VkBool32 createData = VK_TRUE) const override;

    virtual void draw(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite, const ISubMesh& subMesh, const std::string& nodeName) override;

};

} /* namespace vkts */

#endif /* VKTS_RENDERSUBMESH_HPP_ */
