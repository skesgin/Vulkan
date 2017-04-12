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

#ifndef VKTS_SUBMESH_HPP_
#define VKTS_SUBMESH_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class SubMesh: public ISubMesh
{

private:

    std::string name;

    IBufferObjectSP vertexBuffer;
    IBinaryBufferSP vertexBinaryBuffer;

    VkTsVertexBufferType vertexBufferType;


    int32_t numberVertices;

    IBufferObjectSP indicesVertexBuffer;
    IBinaryBufferSP indicesBinaryBuffer;

    int32_t numberIndices;

    VkPrimitiveTopology primitiveTopology;

    IBSDFMaterialSP bsdfMaterial;

    IDescriptorSetLayoutSP descriptorSetLayout;
    IPipelineLayoutSP pipelineLayout;
    IGraphicsPipelineSP graphicsPipeline;

    IPhongMaterialSP phongMaterial;

    int32_t vertexOffset;

    int32_t normalOffset;

    int32_t bitangentOffset;
    int32_t tangentOffset;

	int32_t texcoordOffset;
	int32_t colorOffset;

    int32_t boneIndices0Offset;
    int32_t boneIndices1Offset;
    int32_t boneWeights0Offset;
    int32_t boneWeights1Offset;
    int32_t numberBonesOffset;

    uint32_t strideInBytes;

    Aabb box;

    VkBool32 doubleSided;

    IRenderSubMeshSP subMeshData;

public:

    SubMesh();
    SubMesh(const SubMesh& other);
    SubMesh(SubMesh&& other) = delete;
    virtual ~SubMesh();

    SubMesh& operator =(const SubMesh& other) = delete;

    SubMesh& operator =(SubMesh && other) = delete;

    //
    // ISubMesh
    //

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;


    virtual IRenderSubMeshSP getRenderSubMesh() const override;

    virtual void setRenderSubMesh(const IRenderSubMeshSP& subMeshData) override;


    virtual const IBufferObjectSP& getVertexBuffer() const override;

    virtual const IBinaryBufferSP& getVertexBinaryBuffer() const override;

    virtual VkTsVertexBufferType getVertexBufferType() const override;

    virtual void setVertexBuffer(const IBufferObjectSP& vertexBuffer, const VkTsVertexBufferType vertexBufferType, const Aabb& verticesAABB, const IBinaryBufferSP& vertexBinaryBuffer) override;

    virtual int32_t getNumberVertices() const override;

    virtual void setNumberVertices(const int32_t numberVertices) override;

    virtual const IBufferObjectSP& getIndexBuffer() const override;

    virtual const IBinaryBufferSP& getIndicesBinaryBuffer() const override;

    virtual void setIndexBuffer(const IBufferObjectSP& indicesVertexBuffer, const IBinaryBufferSP& indicesBinaryBuffer) override;

    virtual int32_t getNumberIndices() const override;

    virtual void setNumberIndices(const int32_t numberIndices) override;

    virtual VkIndexType getIndexType() const override;

    virtual VkPrimitiveTopology getPrimitiveTopology() const override;

    virtual void setPrimitiveTopology(const VkPrimitiveTopology primitiveTopology) override;

    virtual const IBSDFMaterialSP& getBSDFMaterial() const override;

    virtual void setBSDFMaterial(const IBSDFMaterialSP& bsdfMaterial) override;

    virtual IDescriptorSetLayoutSP getDescriptorSetLayout() const override;

    virtual void setDescriptorSetLayout(const IDescriptorSetLayoutSP& descriptorSetLayout) override;

    virtual IPipelineLayoutSP getPipelineLayout() const override;

    virtual void setPipelineLayout(const IPipelineLayoutSP& pipelineLayout) override;

    virtual const IGraphicsPipelineSP& getGraphicsPipeline() const override;

    virtual void setGraphicsPipeline(const IGraphicsPipelineSP& graphicsPipeline) override;

    virtual const IPhongMaterialSP& getPhongMaterial() const override;

    virtual void setPhongMaterial(const IPhongMaterialSP& phongMaterial) override;

    virtual int32_t getVertexOffset() const override;

    virtual void setVertexOffset(const int32_t vertexOffset) override;

    virtual int32_t getNormalOffset() const override;

    virtual void setNormalOffset(const int32_t normalOffset) override;

    virtual int32_t getBitangentOffset() const override;

    virtual void setBitangentOffset(const int32_t bitangentOffset) override;

    virtual int32_t getTangentOffset() const override;

    virtual void setTangentOffset(const int32_t tangentOffset) override;

    virtual int32_t getTexcoordOffset() const override;

    virtual void setTexcoordOffset(const int32_t texcoordOffset) override;

	virtual int32_t getColorOffset() const override;

	virtual void setColorOffset(const int32_t texcoordOffset) override;

    virtual int32_t getBoneIndices0Offset() const override;

    virtual void setBoneIndices0Offset(const int32_t boneIndices0Offset) override;

    virtual int32_t getBoneIndices1Offset() const override;

    virtual void setBoneIndices1Offset(const int32_t boneIndices1Offset) override;

    virtual int32_t getBoneWeights0Offset() const override;

    virtual void setBoneWeights0Offset(const int32_t boneWeights0Offset) override;

    virtual int32_t getBoneWeights1Offset() const override;

    virtual void setBoneWeights1Offset(const int32_t boneWeights1Offset) override;

    virtual int32_t getNumberBonesOffset() const override;

    virtual void setNumberBonesOffset(const int32_t numberBonesOffset) override;

    virtual uint32_t getStrideInBytes() const override;

    virtual void setStrideInBytes(const uint32_t strideInBytes) override;

    virtual VkBool32 hasBones() const override;

    virtual const Aabb& getAABB() const override;

    virtual VkBool32 getDoubleSided() const override;

    virtual void setDoubleSided(const VkBool32 doubleSided) override;

    virtual void updateParameterRecursive(Parameter* parameter) override;

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer, const std::string& nodeName) override;


    virtual void freeHostMemory() override;

    //

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite, const std::string& nodeName) override;

    //
    // ICloneable
    //

    virtual ISubMeshSP clone() const override;

    //
    // IDestroyable
    //

    virtual void destroy() override;
};

} /* namespace vkts */

#endif /* VKTS_SUBMESH_HPP_ */
