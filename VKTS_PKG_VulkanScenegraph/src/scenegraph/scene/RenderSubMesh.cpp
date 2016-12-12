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

#include "RenderSubMesh.hpp"

namespace vkts
{

RenderSubMesh::RenderSubMesh() :
    IRenderSubMesh()
{
}

RenderSubMesh::~RenderSubMesh()
{
}

void RenderSubMesh::draw(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite, const ISubMesh& subMesh, const std::string& nodeName)
{
	IGraphicsPipelineSP graphicsPipeline;

    if (subMesh.getBSDFMaterial().get())
    {
    	graphicsPipeline = subMesh.getGraphicsPipeline();
    }
    else if (subMesh.getPhongMaterial().get())
    {
		for (size_t i = 0; i < allGraphicsPipelines.size(); i++)
		{
			if (allGraphicsPipelines[i]->getVertexBufferType() == subMesh.getVertexBufferType())
			{
				graphicsPipeline = allGraphicsPipelines[i];

				break;
			}
		}
    }
    else
    {
        logPrint(VKTS_LOG_SEVERE, __FILE__, __LINE__, "No material");

        return;
    }

    //

    if (!cmdBuffer.get())
    {
        return;
    }

	if (!graphicsPipeline.get())
	{
		return;
	}

	vkCmdBindPipeline(cmdBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

	if (subMesh.getBSDFMaterial().get())
	{
		subMesh.getBSDFMaterial()->drawRecursive(cmdBuffer, graphicsPipeline, currentBuffer, dynamicOffsetMappings, renderOverwrite, nodeName);
	}

	if (subMesh.getPhongMaterial().get())
	{
		subMesh.getPhongMaterial()->drawRecursive(cmdBuffer, graphicsPipeline, currentBuffer, dynamicOffsetMappings, renderOverwrite, nodeName);
	}

    // Bind index buffer.

    if (!subMesh.getIndexBuffer().get())
    {
        return;
    }

    if (!subMesh.getIndexBuffer()->getBuffer().get())
    {
        return;
    }

    vkCmdBindIndexBuffer(cmdBuffer->getCommandBuffer(0), subMesh.getIndexBuffer()->getBuffer()->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

    // Bind vertex buffer.

    if (!subMesh.getVertexBuffer().get())
    {
        return;
    }

    if (!subMesh.getVertexBuffer()->getBuffer().get())
    {
        return;
    }

    VkDeviceSize offsets[1] = {0};

    VkBuffer buffers[1] = {subMesh.getVertexBuffer()->getBuffer()->getBuffer()};

    vkCmdBindVertexBuffers(cmdBuffer->getCommandBuffer(0), 0, 1, buffers, offsets);

    // Draw indexed.

    vkCmdDrawIndexed(cmdBuffer->getCommandBuffer(0), subMesh.getNumberIndices(), 1, 0, 0, 0);
}

IRenderSubMeshSP RenderSubMesh::create(const VkBool32 createData) const
{
	return IRenderSubMeshSP(new RenderSubMesh());
}

} /* namespace vkts */
