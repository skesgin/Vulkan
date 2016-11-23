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

#include "Draw.hpp"

namespace vkts
{

VkBool32 Draw::updateMaterial(Material& material)
{
	if (!graphicsPipeline.get())
	{
		return VK_FALSE;
	}

	vkCmdBindPipeline(cmdBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

    material.bindDescriptorSets(nodeName, cmdBuffer, graphicsPipeline->getLayout(), bufferIndex);

    return VK_TRUE;
}

Draw::Draw(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const OverwriteDraw* renderOverwrite, const uint32_t bufferIndex) :
	SceneVisitor(), cmdBuffer(cmdBuffer), allGraphicsPipelines(allGraphicsPipelines), renderOverwrite(renderOverwrite), bufferIndex(bufferIndex), nodeName(""), graphicsPipeline()
{
}

Draw::~Draw()
{
}

//

 VkBool32 Draw::visit(Scene& scene, const uint32_t objectOffset, const uint32_t objectStep, const size_t objectLimit)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(scene, cmdBuffer, allGraphicsPipelines, bufferIndex, objectOffset, objectStep, objectLimit))
    	{
    		return VK_FALSE;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	return VK_TRUE;
}

 VkBool32 Draw::visit(Object& object)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(object, cmdBuffer, allGraphicsPipelines, bufferIndex))
    	{
    		return VK_FALSE;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	return VK_TRUE;
}

 VkBool32 Draw::visit(Node& node)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(node, cmdBuffer, allGraphicsPipelines, bufferIndex))
    	{
    		return VK_FALSE;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	nodeName = node.name;

	for (size_t i = 0; i < node.allMeshes.size(); i++)
	{
		static_cast<Mesh*>(node.allMeshes[i].get())->visitRecursive(this);
	}

    for (size_t i = 0; i < node.allChildNodes.size(); i++)
    {
    	static_cast<Node*>(node.allChildNodes[i].get())->visitRecursive(this);
    }

	return VK_FALSE;
}

 VkBool32 Draw::visit(Mesh& mesh)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(mesh, cmdBuffer, allGraphicsPipelines, bufferIndex))
    	{
    		return VK_FALSE;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	return VK_TRUE;
}

 VkBool32 Draw::visit(SubMesh& subMesh)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(subMesh, cmdBuffer, allGraphicsPipelines, bufferIndex))
    	{
    		return VK_FALSE;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

    if (subMesh.bsdfMaterial.get())
    {
    	graphicsPipeline = subMesh.graphicsPipeline;

    	// Pipeline is bound inside the material.
    	static_cast<BSDFMaterial*>(subMesh.bsdfMaterial.get())->visitRecursive(this);
    }
    else if (subMesh.phongMaterial.get())
    {
		graphicsPipeline = IGraphicsPipelineSP();

		for (size_t i = 0; i < allGraphicsPipelines.size(); i++)
		{
			if (allGraphicsPipelines[i]->getVertexBufferType() == subMesh.vertexBufferType)
			{
				graphicsPipeline = allGraphicsPipelines[i];

				break;
			}
		}

		static_cast<PhongMaterial*>(subMesh.phongMaterial.get())->visitRecursive(this);
    }
    else
    {
        logPrint(VKTS_LOG_SEVERE, __FILE__, __LINE__, "No material");

        return VK_FALSE;
    }

    if (!cmdBuffer.get())
    {
        return VK_FALSE;
    }

    // Bind index buffer.

    if (!subMesh.indicesVertexBuffer.get())
    {
        return VK_FALSE;
    }

    if (!subMesh.indicesVertexBuffer->getBuffer().get())
    {
        return VK_FALSE;
    }

    vkCmdBindIndexBuffer(cmdBuffer->getCommandBuffer(bufferIndex), subMesh.indicesVertexBuffer->getBuffer()->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

    // Bind vertex buffer.

    if (!subMesh.vertexBuffer.get())
    {
        return VK_FALSE;
    }

    if (!subMesh.vertexBuffer->getBuffer().get())
    {
        return VK_FALSE;
    }

    VkDeviceSize offsets[1] = {0};

    VkBuffer buffers[1] = {subMesh.vertexBuffer->getBuffer()->getBuffer()};

    vkCmdBindVertexBuffers(cmdBuffer->getCommandBuffer(bufferIndex), 0, 1, buffers, offsets);

    // Draw indexed.

    vkCmdDrawIndexed(cmdBuffer->getCommandBuffer(bufferIndex), subMesh.getNumberIndices(), 1, 0, 0, 0);

	return VK_FALSE;
}

 VkBool32 Draw::visit(PhongMaterial& material)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(material, cmdBuffer, graphicsPipeline, bufferIndex))
    	{
    		return VK_FALSE;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	return updateMaterial(material);
}

VkBool32 Draw::visit(BSDFMaterial& material)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(material, cmdBuffer, graphicsPipeline, bufferIndex))
    	{
    		return VK_FALSE;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

	return updateMaterial(material);
}

} /* namespace vkts */
