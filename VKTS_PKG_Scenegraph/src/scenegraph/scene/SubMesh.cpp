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

#include "SubMesh.hpp"

namespace vkts
{

SubMesh::SubMesh() :
    ISubMesh(), name(""), vertexBuffer(), vertexBinaryBuffer(), vertexBufferType(0), numberVertices(0), indicesVertexBuffer(), indicesBinaryBuffer(), numberIndices(0), primitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST), bsdfMaterial(), descriptorSetLayout(), pipelineLayout(), graphicsPipeline(), phongMaterial(), vertexOffset(-1), normalOffset(-1), bitangentOffset(-1), tangentOffset(-1), texcoord0Offset(-1), texcoord1Offset(-1), boneIndices0Offset(-1), boneIndices1Offset(-1), boneWeights0Offset(-1), boneWeights1Offset(-1), numberBonesOffset(-1), strideInBytes(0), box(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), doubleSided(VK_FALSE), subMeshData()
{
}

SubMesh::SubMesh(const SubMesh& other) :
    ISubMesh(), name(other.name + "_clone"), vertexBuffer(other.vertexBuffer), vertexBinaryBuffer(other.vertexBinaryBuffer), vertexBufferType(other.vertexBufferType), numberVertices(other.numberVertices), indicesVertexBuffer(other.indicesVertexBuffer), indicesBinaryBuffer(other.indicesBinaryBuffer), numberIndices(other.numberIndices), primitiveTopology(other.primitiveTopology), bsdfMaterial(), descriptorSetLayout(other.descriptorSetLayout), pipelineLayout(other.pipelineLayout), graphicsPipeline(other.graphicsPipeline), phongMaterial(), vertexOffset(other.vertexOffset), normalOffset(other.normalOffset), bitangentOffset(other.bitangentOffset), tangentOffset(other.tangentOffset), texcoord0Offset(other.texcoord0Offset), texcoord1Offset(other.texcoord1Offset), boneIndices0Offset(other.boneIndices0Offset), boneIndices1Offset(other.boneIndices1Offset), boneWeights0Offset(other.boneWeights0Offset), boneWeights1Offset(other.boneWeights1Offset), numberBonesOffset(other.numberBonesOffset), strideInBytes(other.strideInBytes), box(other.box), doubleSided(other.doubleSided), subMeshData(other.subMeshData)
{
    if (other.bsdfMaterial.get())
    {
        bsdfMaterial = other.bsdfMaterial;
    }
    else if (other.phongMaterial.get())
    {
        phongMaterial = other.phongMaterial->clone();
    }
}

SubMesh::~SubMesh()
{
    destroy();
}

//
// ISubMesh
//

const std::string& SubMesh::getName() const
{
    return name;
}

void SubMesh::setName(const std::string& name)
{
    this->name = name;
}

IRenderSubMeshSP SubMesh::getRenderSubMesh() const
{
	return subMeshData;
}

void SubMesh::setRenderSubMesh(const IRenderSubMeshSP& subMeshData)
{
    this->subMeshData = subMeshData;
}

const IBufferObjectSP& SubMesh::getVertexBuffer() const
{
    return vertexBuffer;
}

const IBinaryBufferSP& SubMesh::getVertexBinaryBuffer() const
{
	return vertexBinaryBuffer;
}

VkTsVertexBufferType SubMesh::getVertexBufferType() const
{
    return vertexBufferType;
}

void SubMesh::setVertexBuffer(const IBufferObjectSP& vertexBuffer, const VkTsVertexBufferType vertexBufferType, const Aabb& verticesAABB, const IBinaryBufferSP& vertexBinaryBuffer)
{
    this->vertexBuffer = vertexBuffer;
    this->vertexBufferType = vertexBufferType;
    this->vertexBinaryBuffer = vertexBinaryBuffer;

    //

    this->box = verticesAABB;
}

int32_t SubMesh::getNumberVertices() const
{
    return numberVertices;
}

void SubMesh::setNumberVertices(const int32_t numberVertices)
{
    this->numberVertices = numberVertices;
}

const IBufferObjectSP& SubMesh::getIndexBuffer() const
{
    return indicesVertexBuffer;
}

const IBinaryBufferSP& SubMesh::getIndicesBinaryBuffer() const
{
	return indicesBinaryBuffer;
}

void SubMesh::setIndexBuffer(const IBufferObjectSP& indicesVertexBuffer, const IBinaryBufferSP& indicesBinaryBuffer)
{
    this->indicesVertexBuffer = indicesVertexBuffer;
    this->indicesBinaryBuffer = indicesBinaryBuffer;
}

int32_t SubMesh::getNumberIndices() const
{
    return numberIndices;
}

void SubMesh::setNumberIndices(const int32_t numberIndices)
{
    this->numberIndices = numberIndices;
}

VkIndexType SubMesh::getIndexType() const
{
    return VK_INDEX_TYPE_UINT32;
}

VkPrimitiveTopology SubMesh::getPrimitiveTopology() const
{
    return primitiveTopology;
}

void SubMesh::setPrimitiveTopology(const VkPrimitiveTopology primitiveTopology)
{
    this->primitiveTopology = primitiveTopology;
}

const IBSDFMaterialSP& SubMesh::getBSDFMaterial() const
{
    return bsdfMaterial;
}

void SubMesh::setBSDFMaterial(const IBSDFMaterialSP& bsdfMaterial)
{
    this->bsdfMaterial = bsdfMaterial;
}

IDescriptorSetLayoutSP SubMesh::getDescriptorSetLayout() const
{
	return descriptorSetLayout;
}

void SubMesh::setDescriptorSetLayout(const IDescriptorSetLayoutSP& descriptorSetLayout)
{
    this->descriptorSetLayout = descriptorSetLayout;
}

IPipelineLayoutSP SubMesh::getPipelineLayout() const
{
    return pipelineLayout;
}

void SubMesh::setPipelineLayout(const IPipelineLayoutSP& pipelineLayout)
{
    this->pipelineLayout = pipelineLayout;
}

const IGraphicsPipelineSP& SubMesh::getGraphicsPipeline() const
{
    return graphicsPipeline;
}

void SubMesh::setGraphicsPipeline(const IGraphicsPipelineSP& graphicsPipeline)
{
    this->graphicsPipeline = graphicsPipeline;
}

const IPhongMaterialSP& SubMesh::getPhongMaterial() const
{
    return phongMaterial;
}

void SubMesh::setPhongMaterial(const IPhongMaterialSP& phongMaterial)
{
    this->phongMaterial = phongMaterial;
}

int32_t SubMesh::getVertexOffset() const
{
    return vertexOffset;
}

void SubMesh::setVertexOffset(const int32_t vertexOffset)
{
    this->vertexOffset = vertexOffset;
}

int32_t SubMesh::getNormalOffset() const
{
    return normalOffset;
}

void SubMesh::setNormalOffset(const int32_t normalOffset)
{
    this->normalOffset = normalOffset;
}

int32_t SubMesh::getBitangentOffset() const
{
    return bitangentOffset;
}

void SubMesh::setBitangentOffset(const int32_t bitangentOffset)
{
    this->bitangentOffset = bitangentOffset;
}

int32_t SubMesh::getTangentOffset() const
{
    return tangentOffset;
}

void SubMesh::setTangentOffset(const int32_t tangentOffset)
{
    this->tangentOffset = tangentOffset;
}

int32_t SubMesh::getTexcoord0Offset() const
{
    return texcoord0Offset;
}

void SubMesh::setTexcoord0Offset(const int32_t texcoordOffset)
{
    this->texcoord0Offset = texcoordOffset;
}

int32_t SubMesh::getTexcoord1Offset() const
{
    return texcoord1Offset;
}

void SubMesh::setTexcoord1Offset(const int32_t texcoordOffset)
{
    this->texcoord1Offset = texcoordOffset;
}

int32_t SubMesh::getBoneIndices0Offset() const
{
    return boneIndices0Offset;
}

void SubMesh::setBoneIndices0Offset(const int32_t boneIndices0Offset)
{
    this->boneIndices0Offset = boneIndices0Offset;
}

int32_t SubMesh::getBoneIndices1Offset() const
{
    return boneIndices1Offset;
}

void SubMesh::setBoneIndices1Offset(const int32_t boneIndices1Offset)
{
    this->boneIndices1Offset = boneIndices1Offset;
}

int32_t SubMesh::getBoneWeights0Offset() const
{
    return boneWeights0Offset;
}

void SubMesh::setBoneWeights0Offset(const int32_t boneWeights0Offset)
{
    this->boneWeights0Offset = boneWeights0Offset;
}

int32_t SubMesh::getBoneWeights1Offset() const
{
    return boneWeights1Offset;
}

void SubMesh::setBoneWeights1Offset(const int32_t boneWeights1Offset)
{
    this->boneWeights1Offset = boneWeights1Offset;
}

int32_t SubMesh::getNumberBonesOffset() const
{
    return numberBonesOffset;
}

void SubMesh::setNumberBonesOffset(const int32_t numberBonesOffset)
{
    this->numberBonesOffset = numberBonesOffset;
}

uint32_t SubMesh::getStrideInBytes() const
{
    return strideInBytes;
}

void SubMesh::setStrideInBytes(const uint32_t strideInBytes)
{
    this->strideInBytes = strideInBytes;
}

VkBool32 SubMesh::hasBones() const
{
    return numberBonesOffset >= 0;
}

const Aabb& SubMesh::getAABB() const
{
	return box;
}

VkBool32 SubMesh::getDoubleSided() const
{
	return doubleSided;
}

void SubMesh::setDoubleSided(const VkBool32 doubleSided)
{
	this->doubleSided = doubleSided;
}

void SubMesh::updateParameterRecursive(Parameter* parameter)
{
	if (parameter)
	{
		parameter->visit(*this);
	}

	if (bsdfMaterial.get())
	{
		bsdfMaterial->updateParameterRecursive(parameter);
	}
	else if (phongMaterial.get())
	{
		phongMaterial->updateParameterRecursive(parameter);
	}
}

void SubMesh::updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer, const std::string& nodeName)
{
	if (bsdfMaterial.get())
	{
		bsdfMaterial->updateDescriptorSetsRecursive(allWriteDescriptorSetsCount, allWriteDescriptorSets, currentBuffer, nodeName);
	}
	else if (phongMaterial.get())
	{
		phongMaterial->updateDescriptorSetsRecursive(allWriteDescriptorSetsCount, allWriteDescriptorSets, currentBuffer, nodeName);
	}
}

void SubMesh::freeHostMemory()
{
	vertexBinaryBuffer = IBinaryBufferSP();
	indicesBinaryBuffer = IBinaryBufferSP();
}

//

void SubMesh::drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite, const std::string& nodeName)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(*this, cmdBuffer, allGraphicsPipelines, currentBuffer, dynamicOffsetMappings))
    	{
    		return;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

    //

    if (subMeshData.get())
    {
    	subMeshData->draw(cmdBuffer, allGraphicsPipelines, currentBuffer, dynamicOffsetMappings, renderOverwrite, *this, nodeName);
    }
}

//
// ICloneable
//

ISubMeshSP SubMesh::clone() const
{
	auto result = ISubMeshSP(new SubMesh(*this));

	if (result.get() && getRenderSubMesh().get() && !result->getRenderSubMesh().get())
	{
		return ISubMeshSP();
	}

	if (result.get() && getBSDFMaterial().get() && !result->getBSDFMaterial().get())
	{
		return ISubMeshSP();
	}

	if (result.get() && getPhongMaterial().get() && !result->getPhongMaterial().get())
	{
		return ISubMeshSP();
	}

    return result;
}

//
// IDestroyable
//

void SubMesh::destroy()
{
    if (bsdfMaterial.get())
    {
        bsdfMaterial->destroy();
    }

    if (phongMaterial.get())
    {
        phongMaterial->destroy();
    }

    // Not destroying the index and vertex buffer by purpose.

    // Not destroying the graphics pipeline by purpose.
}

} /* namespace vkts */
