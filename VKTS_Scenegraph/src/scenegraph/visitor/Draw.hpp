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

#ifndef VKTS_DRAW_HPP_
#define VKTS_DRAW_HPP_

#include <vkts/scenegraph.hpp>

#include "SceneVisitor.hpp"

namespace vkts
{

class Draw : public SceneVisitor
{

private:

	const ICommandBuffersSP& cmdBuffer;
	const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines;
	const OverwriteDraw* renderOverwrite;
	const uint32_t bufferIndex;

	std::string nodeName;
	IGraphicsPipelineSP graphicsPipeline;

	VkBool32 updateMaterial(Material& material)
	{
    	if (!graphicsPipeline.get())
    	{
    		return VK_FALSE;
    	}

    	vkCmdBindPipeline(cmdBuffer->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipeline());

        material.bindDescriptorSets(nodeName, cmdBuffer, graphicsPipeline->getLayout(), bufferIndex);

        return VK_TRUE;
	}

public:

	Draw() = delete;

	Draw(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const OverwriteDraw* renderOverwrite, const uint32_t bufferIndex) :
		SceneVisitor(), cmdBuffer(cmdBuffer), allGraphicsPipelines(allGraphicsPipelines), renderOverwrite(renderOverwrite), bufferIndex(bufferIndex), nodeName(""), graphicsPipeline()
    {
    }

    virtual ~Draw()
    {
    }

    //

    virtual VkBool32 visit(Scene& scene, const uint32_t objectOffset, const uint32_t objectStep, const size_t objectLimit)
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

    virtual VkBool32 visit(Object& object)
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

    virtual VkBool32 visit(Node& node)
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
			node.allMeshes[i]->visitRecursive(this);
		}

        for (size_t i = 0; i < node.allChildNodes.size(); i++)
        {
        	node.allChildNodes[i]->visitRecursive(this);
        }

    	return VK_FALSE;
    }

    virtual VkBool32 visit(Mesh& mesh)
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

    virtual VkBool32 visit(SubMesh& subMesh)
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
        	subMesh.bsdfMaterial->visitRecursive(this);
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

    		subMesh.phongMaterial->visitRecursive(this);
        }
        else
        {
            logPrint(VKTS_LOG_SEVERE, __FILE__, __LINE__, "No material");

            return VK_FALSE;
        }

        subMesh.bindIndexBuffer(cmdBuffer, bufferIndex);
        subMesh.bindVertexBuffers(cmdBuffer, bufferIndex);
        subMesh.drawIndexed(cmdBuffer, bufferIndex);

    	return VK_FALSE;
    }

    virtual VkBool32 visit(PhongMaterial& material)
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

    virtual VkBool32 visit(BSDFMaterial& material)
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
};

} /* namespace vkts */

#endif /* VKTS_DRAW_HPP_ */
