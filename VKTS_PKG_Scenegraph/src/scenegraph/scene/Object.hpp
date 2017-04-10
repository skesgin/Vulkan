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

#ifndef VKTS_OBJECT_HPP_
#define VKTS_OBJECT_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class Object: public IObject
{

private:

    std::string name;

    glm::vec3 scale;

    glm::mat4 transformMatrix;
    VkBool32 dirty;

    INodeSP rootNode;

protected:

    //
    // IMoveable
    //

    virtual void update() override;

public:

	Object();
	Object(std::string name, glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale);
    Object(const Object& other);
    Object(Object&& other) = delete;
    virtual ~Object();

    Object& operator =(const Object& other) = delete;

    Object& operator =(Object && other) = delete;

    //
    // IObject
    //

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;

    virtual const glm::vec3& getScale() const override;

    virtual void setScale(const glm::vec3& scale) override;

    virtual const INodeSP& getRootNode() const override;

    virtual void setRootNode(const INodeSP& rootNode) override;

    virtual void setDirty() override;

    virtual void updateParameterRecursive(Parameter* parameter) override;

    virtual void updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t currentBuffer) override;

    virtual void updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t currentBuffer = 0, const OverwriteUpdate* updateOverwrite = nullptr) override;

    virtual void drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings, const OverwriteDraw* renderOverwrite = nullptr) override;

    //
    // ICloneable
    //

    virtual IObjectSP clone() const override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_OBJECT_HPP_ */
