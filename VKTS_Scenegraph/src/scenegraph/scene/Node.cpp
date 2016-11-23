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

#include "Node.hpp"

#include "Animation.hpp"
#include "Mesh.hpp"
#include "../visitor/SceneVisitor.hpp"

namespace vkts
{

void Node::reset()
{
    name = "";

    parentNode = INodeSP();

    translate = glm::vec3(0.0f, 0.0f, 0.0f);
    rotate = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);

    finalTranslate = translate;
    finalRotate = rotate;
    finalScale = scale;

    transformMatrix = glm::mat4(1.0f);
    transformMatrixDirty = VK_TRUE;

    jointIndex = -1;
    joints = 0;

    bindTranslate = glm::vec3(0.0f, 0.0f, 0.0f);
    bindRotate = glm::vec3(0.0f, 0.0f, 0.0f);
    bindScale = glm::vec3(1.0f, 1.0f, 1.0f);

    bindMatrix = glm::mat4(1.0f);
    inverseBindMatrix = glm::mat4(1.0f);

    bindMatrixDirty = VK_FALSE;

    allChildNodes.clear();

    allMeshes.clear();

    allCameras.clear();

    allLights.clear();

    allConstraints.clear();

    allAnimations.clear();

    currentAnimation = -1;

    transformUniformBuffer = IBufferObjectSP();

    memset(&transformDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
    memset(&transformWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));

    jointsUniformBuffer = IBufferObjectSP();
    memset(&jointDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
    memset(&jointWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));

    box = Aabb(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    layers = 0x01;
}

Node::Node() :
    INode(), name(""), parentNode(), translate(0.0f, 0.0f, 0.0f), rotate(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), finalTranslate(0.0f, 0.0f, 0.0f), finalRotate(0.0f, 0.0f, 0.0f), finalScale(1.0f, 1.0f, 1.0f), transformMatrix(1.0f), transformMatrixDirty(VK_TRUE), jointIndex(-1), joints(0), bindTranslate(0.0f, 0.0f, 0.0f), bindRotate(0.0f, 0.0f,0.0f), bindScale(1.0f, 1.0f, 1.0f), bindMatrix(1.0f), inverseBindMatrix(1.0f), bindMatrixDirty(VK_FALSE), allChildNodes(), allMeshes(), allCameras(), allLights(), allConstraints(), allAnimations(), currentAnimation(-1), transformUniformBuffer(), jointsUniformBuffer(), box(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), layers(0x01)
{
    reset();
}

Node::Node(const Node& other) :
    INode(), name(other.name + "_clone"), parentNode(other.parentNode), translate(other.translate), rotate(other.rotate), scale(other.scale), finalTranslate(other.finalTranslate), finalRotate(other.finalRotate), finalScale(other.finalScale), transformMatrix(other.transformMatrix), transformMatrixDirty(other.transformMatrixDirty), jointIndex(-1), joints(0), bindTranslate(other.bindTranslate), bindRotate(other.bindRotate), bindScale(other.bindScale), bindMatrix(other.bindMatrix), inverseBindMatrix(other.inverseBindMatrix), bindMatrixDirty(other.bindMatrixDirty), box(other.box), layers(other.layers)
{
    for (size_t i = 0; i < other.allChildNodes.size(); i++)
    {
        const auto& currentChildNode = other.allChildNodes[i];

        if (!currentChildNode.get())
        {
            reset();

            break;
        }

        INodeSP cloneChildNode = currentChildNode->clone();

        if (!cloneChildNode.get())
        {
            reset();

            break;
        }

        allChildNodes.append(cloneChildNode);
    }

    for (size_t i = 0; i < other.allMeshes.size(); i++)
    {
        const auto& currentMesh = other.allMeshes[i];

        if (!currentMesh.get())
        {
            reset();

            break;
        }

        IMeshSP cloneMesh = currentMesh->clone();

        if (!cloneMesh.get())
        {
            reset();

            break;
        }

        allMeshes.append(cloneMesh);
    }

    for (size_t i = 0; i < other.allCameras.size(); i++)
    {
        const auto& currentCamera = other.allCameras[i];

        if (!currentCamera.get())
        {
            reset();

            break;
        }

        ICameraSP cloneCamera = currentCamera->clone();

        if (!cloneCamera.get())
        {
            reset();

            break;
        }

        allCameras.append(cloneCamera);
    }

    for (size_t i = 0; i < other.allLights.size(); i++)
    {
        const auto& currentLight = other.allLights[i];

        if (!currentLight.get())
        {
            reset();

            break;
        }

        ILightSP cloneLight = currentLight->clone();

        if (!cloneLight.get())
        {
            reset();

            break;
        }

        allLights.append(cloneLight);
    }

    for (size_t i = 0; i < other.allConstraints.size(); i++)
    {
        const auto& currentConstraint = other.allConstraints[i];

        if (!currentConstraint.get())
        {
            reset();

            break;
        }

        IConstraintSP cloneConstraint = currentConstraint->clone();

        if (!cloneConstraint.get())
        {
            reset();

            break;
        }

        allConstraints.append(cloneConstraint);
    }

    for (size_t i = 0; i < other.allAnimations.size(); i++)
    {
        const auto& currentAnimation = other.allAnimations[i];

        if (!currentAnimation.get())
        {
            reset();

            break;
        }

        IAnimationSP cloneAnimation = currentAnimation->clone();

        if (!cloneAnimation.get())
        {
            reset();

            break;
        }

        allAnimations.append(cloneAnimation);
    }

    currentAnimation = other.currentAnimation;

    //

    if (other.transformUniformBuffer.get())
    {
		IBufferObjectSP transformUniformBuffer = bufferObjectCreate(other.transformUniformBuffer->getContextObject(), other.transformUniformBuffer->getBuffer()->getBufferCreateInfo(), other.transformUniformBuffer->getDeviceMemory()->getMemoryPropertyFlags());

		if (!transformUniformBuffer.get())
		{
			reset();

			return;
		}

		setTransformUniformBuffer(transformUniformBuffer);
    }

    //

    setJointIndex(other.jointIndex);

    //

    if (other.jointsUniformBuffer.get())
    {
        IBufferObjectSP jointsUniformBuffer = bufferObjectCreate(other.jointsUniformBuffer->getContextObject(), other.jointsUniformBuffer->getBuffer()->getBufferCreateInfo(), other.jointsUniformBuffer->getDeviceMemory()->getMemoryPropertyFlags());

        if (!jointsUniformBuffer.get())
        {
            reset();

            return;
        }

        setJointsUniformBuffer(other.joints, jointsUniformBuffer);
    }
}

Node::~Node()
{
    destroy();
}

void Node::updateTransformDescriptorBufferInfo(const VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range)
{
    memset(&transformDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));

    transformDescriptorBufferInfo.buffer = buffer;
    transformDescriptorBufferInfo.offset = offset;
    transformDescriptorBufferInfo.range = range;

    memset(&transformWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));

    transformWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    transformWriteDescriptorSet.dstSet = VK_NULL_HANDLE; // Defined later.
    transformWriteDescriptorSet.dstBinding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;
    transformWriteDescriptorSet.dstArrayElement = 0;
    transformWriteDescriptorSet.descriptorCount = 1;
    transformWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    transformWriteDescriptorSet.pImageInfo = nullptr;
    transformWriteDescriptorSet.pBufferInfo = &transformDescriptorBufferInfo;
    transformWriteDescriptorSet.pTexelBufferView = nullptr;
}

void Node::updateJointDescriptorBufferInfo(const VkBuffer buffer, const VkDeviceSize offset, const VkDeviceSize range)
{
    memset(&jointDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));

    jointDescriptorBufferInfo.buffer = buffer;
    jointDescriptorBufferInfo.offset = offset;
    jointDescriptorBufferInfo.range = range;

    memset(&jointWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));

    jointWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    jointWriteDescriptorSet.dstSet = VK_NULL_HANDLE; // Defined later.
    jointWriteDescriptorSet.dstBinding = VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM;
    jointWriteDescriptorSet.dstArrayElement = 0;
    jointWriteDescriptorSet.descriptorCount = 1;
    jointWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    jointWriteDescriptorSet.pImageInfo = nullptr;
    jointWriteDescriptorSet.pBufferInfo = &jointDescriptorBufferInfo;
    jointWriteDescriptorSet.pTexelBufferView = nullptr;
}

//
// INode
//

const std::string& Node::getName() const
{
    return name;
}

void Node::setName(const std::string& name)
{
    this->name = name;
}

const INodeSP& Node::getParentNode() const
{
    return parentNode;
}

void Node::setParentNode(const INodeSP& parentNode)
{
    this->parentNode = parentNode;
}

const glm::vec3& Node::getTranslate() const
{
    return translate;
}

void Node::setTranslate(const glm::vec3& translate)
{
    this->translate = translate;

    setDirty();
}

const glm::vec3& Node::getRotate() const
{
    return rotate;
}

void Node::setRotate(const glm::vec3& rotate)
{
    this->rotate = rotate;

    setDirty();
}

const glm::vec3& Node::getScale() const
{
    return scale;
}

void Node::setScale(const glm::vec3& scale)
{
    this->scale = scale;

    setDirty();
}

const glm::vec3& Node::getFinalTranslate() const
{
    return finalTranslate;
}

void Node::setFinalTranslate(const glm::vec3& translate)
{
    this->finalTranslate = translate;
}

const glm::vec3& Node::getFinalRotate() const
{
    return finalRotate;
}

void Node::setFinalRotate(const glm::vec3& rotate)
{
    this->finalRotate = rotate;
}

const glm::vec3& Node::getFinalScale() const
{
    return finalScale;
}

void Node::setFinalScale(const glm::vec3& scale)
{
    this->finalScale = scale;
}

int32_t Node::getJointIndex() const
{
    return jointIndex;
}

void Node::setJointIndex(const int32_t jointIndex)
{
    this->jointIndex = jointIndex;
}

int32_t Node::getNumberJoints() const
{
	return joints;
}

const glm::vec3& Node::getBindTranslate() const
{
    return bindTranslate;
}

void Node::setBindTranslate(const glm::vec3& translate)
{
    this->bindTranslate = translate;

    this->bindMatrixDirty = VK_TRUE;
}

const glm::vec3& Node::getBindRotate() const
{
    return bindRotate;
}

void Node::setBindRotate(const glm::vec3& rotate)
{
    this->bindRotate = rotate;

    this->bindMatrixDirty = VK_TRUE;
}

const glm::vec3& Node::getBindScale() const
{
    return bindScale;
}

void Node::setBindScale(const glm::vec3& scale)
{
    this->bindScale = scale;

    this->bindMatrixDirty = VK_TRUE;
}

void Node::addChildNode(const INodeSP& childNode)
{
    allChildNodes.append(childNode);

    //

    if (allChildNodes.size() == 1)
    {
    	this->box = childNode->getAABB();
    }
    else
    {
    	this->box += childNode->getAABB();
    }
}

VkBool32 Node::removeChildNode(const INodeSP& childNode)
{
    return allChildNodes.remove(childNode);
}

size_t Node::getNumberChildNodes() const
{
    return allChildNodes.size();
}

const SmartPointerVector<INodeSP>& Node::getChildNodes() const
{
    return allChildNodes;
}

void Node::addMesh(const IMeshSP& mesh)
{
    allMeshes.append(mesh);

    //

    if (allMeshes.size() == 1)
    {
    	this->box = mesh->getAABB();
    }
    else
    {
    	this->box += mesh->getAABB();
    }
}

VkBool32 Node::removeMesh(const IMeshSP& mesh)
{
    return allMeshes.remove(mesh);
}

size_t Node::getNumberMeshes() const
{
    return allMeshes.size();
}

const SmartPointerVector<IMeshSP>& Node::getMeshes() const
{
    return allMeshes;
}

void Node::addCamera(const ICameraSP& camera)
{
    allCameras.append(camera);
}

VkBool32 Node::removeCamera(const ICameraSP& camera)
{
    return allCameras.remove(camera);
}

size_t Node::getNumberCameras() const
{
    return allCameras.size();
}

const SmartPointerVector<ICameraSP>& Node::getCameras() const
{
    return allCameras;
}

void Node::addLight(const ILightSP& light)
{
    allLights.append(light);
}

VkBool32 Node::removeLight(const ILightSP& light)
{
    return allLights.remove(light);
}

size_t Node::getNumberLights() const
{
    return allLights.size();
}

const SmartPointerVector<ILightSP>& Node::getLights() const
{
    return allLights;
}

void Node::addConstraint(const IConstraintSP& constraint)
{
    allConstraints.append(constraint);
}

VkBool32 Node::removeConstraint(const IConstraintSP& constraint)
{
    return allConstraints.remove(constraint);
}

size_t Node::getNumberConstraints() const
{
    return allConstraints.size();
}

const SmartPointerVector<IConstraintSP>& Node::getConstraints() const
{
    return allConstraints;
}

void Node::addAnimation(const IAnimationSP& animation)
{
    allAnimations.append(animation);

    if (allAnimations.size() == 1)
    {
        currentAnimation = 0;
    }
}

VkBool32 Node::removeAnimation(const IAnimationSP& animation)
{
    VkBool32 result = allAnimations.remove(animation);

    if (currentAnimation >= (int32_t) allAnimations.size())
    {
        currentAnimation = (int32_t) allAnimations.size() - 1;
    }

    return result;
}

size_t Node::getNumberAnimations() const
{
    return allAnimations.size();
}

const SmartPointerVector<IAnimationSP>& Node::getAnimations() const
{
    return allAnimations;
}

int32_t Node::getCurrentAnimation() const
{
	return currentAnimation;
}

void Node::setCurrentAnimation(const int32_t currentAnimation)
{
	if (currentAnimation >= -1 && currentAnimation < (int32_t)allAnimations.size())
	{
		this->currentAnimation = currentAnimation;
	}
	else
	{
		this->currentAnimation = -1;
	}
}

VkBool32 Node::getDirty() const
{
    return transformMatrixDirty;
}

void Node::setDirty()
{
    transformMatrixDirty = VK_TRUE;
}

IBufferObjectSP Node::getTransformUniformBuffer() const
{
    return transformUniformBuffer;
}

void Node::setTransformUniformBuffer(const IBufferObjectSP& transformUniformBuffer)
{
    this->transformUniformBuffer = transformUniformBuffer;

    updateTransformDescriptorBufferInfo(this->transformUniformBuffer->getBuffer()->getBuffer(), 0, this->transformUniformBuffer->getBuffer()->getSize());

    setDirty();
}

IBufferObjectSP Node::getJointsUniformBuffer() const
{
	return jointsUniformBuffer;
}

void Node::setJointsUniformBuffer(const int32_t joints, const IBufferObjectSP& jointsUniformBuffer)
{
	this->joints = joints;
	this->jointsUniformBuffer = jointsUniformBuffer;

    updateJointDescriptorBufferInfo(this->jointsUniformBuffer->getBuffer()->getBuffer(), 0, this->jointsUniformBuffer->getBuffer()->getSize());

    this->bindMatrixDirty = VK_TRUE;
}

void Node::updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const glm::mat4& parentTransformMatrix, const VkBool32 parentTransformMatrixDirty, const glm::mat4& parentBindMatrix, const VkBool32 parentBindMatrixDirty, const INodeSP& armatureNode)
{
    transformMatrixDirty = transformMatrixDirty || parentTransformMatrixDirty;

    bindMatrixDirty = bindMatrixDirty || parentBindMatrixDirty;

    auto newArmatureNode = (joints != 0) ? INode::shared_from_this() : armatureNode;

    //

    finalTranslate = translate;
    finalRotate = rotate;
    finalScale = scale;

    //

    if (currentAnimation >= 0 && currentAnimation < (int32_t) allAnimations.size())
    {
    	float currentTime = allAnimations[currentAnimation]->update((float)deltaTime);

        const auto& currentChannels = allAnimations[currentAnimation]->getChannels();

        float value;

        //

        Quat quaternion;

        VkBool32 quaternionDirty = VK_FALSE;

        //

        for (size_t i = 0; i < currentChannels.size(); i++)
        {
            value = interpolate(currentTime, currentChannels[i]);

            if (currentChannels[i]->getTargetTransform() == VKTS_TARGET_TRANSFORM_TRANSLATE)
            {
                finalTranslate[currentChannels[i]->getTargetTransformElement()] = value;
            }
            else if (currentChannels[i]->getTargetTransform() == VKTS_TARGET_TRANSFORM_ROTATE)
            {
            	finalRotate[currentChannels[i]->getTargetTransformElement()] = value;
            }
            else if (currentChannels[i]->getTargetTransform() == VKTS_TARGET_TRANSFORM_QUATERNION_ROTATE)
            {
            	switch (currentChannels[i]->getTargetTransformElement())
            	{
            		case VKTS_TARGET_TRANSFORM_ELEMENT_X:
            			quaternion.x = value;
            			break;
            		case VKTS_TARGET_TRANSFORM_ELEMENT_Y:
            			quaternion.y = value;
            			break;
            		case VKTS_TARGET_TRANSFORM_ELEMENT_Z:
            			quaternion.z = value;
            			break;
            		case VKTS_TARGET_TRANSFORM_ELEMENT_W:
            			quaternion.w = value;
            			break;
            	}

                quaternionDirty = VK_TRUE;
            }
            else if (currentChannels[i]->getTargetTransform() == VKTS_TARGET_TRANSFORM_SCALE)
            {
            	finalScale[currentChannels[i]->getTargetTransformElement()] = value;
            }
        }

        if (quaternionDirty)
        {
        	finalRotate = quaternion.rotation();
        }

        //

        transformMatrixDirty = VK_TRUE;

        if (joints != 0)
        {
        	bindMatrixDirty = VK_TRUE;
        }
    }

    //

    if (allConstraints.size() > 0)
    {
    	for (size_t i = 0; i < allConstraints.size(); i++)
    	{
    		if (!allConstraints[i]->applyConstraint(*this))
    		{
    			return;
    		}
    	}

        //

        transformMatrixDirty = VK_TRUE;

        if (joints != 0)
        {
        	bindMatrixDirty = VK_TRUE;
        }
    }

    //

    if (bindMatrixDirty)
    {
    	// Only armature, having joints not zero and joints can enter here.

    	glm::mat4 localBindMatrix;

    	if (joints == 0)
    	{
    		localBindMatrix = translateMat4(bindTranslate.x, bindTranslate.y, bindTranslate.z) * rotateRzRyRxMat4(bindRotate.z, bindRotate.y, bindRotate.x) * scaleMat4(bindScale.x, bindScale.y, bindScale.z);
    	}
    	else
    	{
    		// Armature has no bind values, but transform is taken into account.

    		localBindMatrix = translateMat4(finalTranslate.x, finalTranslate.y, finalTranslate.z) * rotateRzRyRxMat4(finalRotate.z, finalRotate.y, finalRotate.x) * scaleMat4(finalScale.x, finalScale.y, finalScale.z);
    	}

		this->bindMatrix = parentBindMatrix * localBindMatrix;

        this->inverseBindMatrix = glm::inverse(this->bindMatrix);

        //

        transformMatrixDirty = VK_TRUE;
    }

    //

    if (transformMatrixDirty)
    {
        if (jointIndex == -1)
        {
        	this->transformMatrix = translateMat4(finalTranslate.x, finalTranslate.y, finalTranslate.z) * rotateRzRyRxMat4(finalRotate.z, finalRotate.y, finalRotate.x) * scaleMat4(finalScale.x, finalScale.y, finalScale.z);

        	// Only use parent transform, if this is not an armature.
        	if (joints == 0)
        	{
        		this->transformMatrix = parentTransformMatrix * this->transformMatrix;
        	}
        }
        else
        {
        	// Processing joints.

        	this->transformMatrix = bindMatrix * translateMat4(finalTranslate.x, finalTranslate.y, finalTranslate.z) * rotateRzRyRxMat4(finalRotate.z, finalRotate.y, finalRotate.x) * scaleMat4(finalScale.x, finalScale.y, finalScale.z) * this->inverseBindMatrix;

        	// Only use parent transform, if parent is not an armature.
        	if (parentNode.get() && parentNode->getNumberJoints() == 0)
        	{
        		this->transformMatrix = parentTransformMatrix * this->transformMatrix;
        	}
        }

        if (allCameras.size() > 0)
        {
        	for (size_t i = 0; i < allCameras.size(); i++)
        	{
        		allCameras[i]->updateViewMatrix(this->transformMatrix);
        	}
        }

        if (allLights.size() > 0)
        {
        	for (size_t i = 0; i < allLights.size(); i++)
        	{
        		allLights[i]->updateDirection(this->transformMatrix);
        	}
        }

        // Update buffer.
        if (allMeshes.size() > 0)
        {
            transformUniformBuffer->upload(0, 0, this->transformMatrix);

            auto transformNormalMatrix = glm::transpose(glm::inverse(glm::mat3(this->transformMatrix)));

            transformUniformBuffer->upload(sizeof(float) * 16, 0, transformNormalMatrix);
        }
        else if (joints != 0)
        {
        	// If this is an armature, store the parent matrix.
        	// This allows to modify the parent matrices without recalculating the bind matrices.

			jointsUniformBuffer->upload(0, 0, parentTransformMatrix);

            auto transformNormalMatrix = glm::transpose(glm::inverse(glm::mat3(parentTransformMatrix)));

            jointsUniformBuffer->upload(sizeof(float) * 16, 0, transformNormalMatrix);
        }
        else if (jointIndex >= 0 && jointIndex < VKTS_MAX_JOINTS)
        {
        	if (newArmatureNode.get())
        	{
        		auto currentJointsUniformBuffer = newArmatureNode->getJointsUniformBuffer();

        		if (currentJointsUniformBuffer.get())
        		{
        			// Upload the joint matrices to blend them on the GPU.

        			size_t offset = sizeof(float) * 16 + sizeof(float) * 12;

        			currentJointsUniformBuffer->upload(offset + jointIndex * sizeof(float) * 16, 0, this->transformMatrix);

                    auto transformNormalMatrix = glm::transpose(glm::inverse(glm::mat3(this->transformMatrix)));

        			currentJointsUniformBuffer->upload(offset + VKTS_MAX_JOINTS * sizeof(float) * 16 + jointIndex * sizeof(float) * 12, 0, transformNormalMatrix);
        		}
        	}
        }
        else if (jointIndex >= VKTS_MAX_JOINTS)
        {
        	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Too many joints: %d >= %d",  jointIndex, VKTS_MAX_JOINTS);

        	return;
        }
    }

    for (size_t i = 0; i < allChildNodes.size(); i++)
    {
        allChildNodes[i]->updateTransformRecursive(deltaTime, deltaTicks, tickTime, this->transformMatrix, this->transformMatrixDirty, this->bindMatrix, this->bindMatrixDirty, newArmatureNode);
    }

    //

    transformMatrixDirty = VK_FALSE;

    bindMatrixDirty = VK_FALSE;
}

const Aabb& Node::getAABB() const
{
	return box;
}

Sphere Node::getBoundingSphere() const
{
	Sphere boundingSphere = transformMatrix * box.getSphere();

	for (size_t i = 0; i < allChildNodes.size(); i++)
    {
		boundingSphere += allChildNodes[i]->getBoundingSphere();
    }

	return boundingSphere;
}

uint32_t Node::getLayers() const
{
	return layers;
}

void Node::setLayers(const uint32_t layers)
{
	this->layers = layers;
}

VkBool32 Node::isOnLayer(const uint8_t layer) const
{
	if (layer >= 20)
	{
		throw std::out_of_range("layer >= 20");
	}

	return layers & (1 << (uint32_t)layer);
}

void Node::setOnLayer(const uint8_t layer)
{
	if (layer >= 20)
	{
		throw std::out_of_range("layer >= 20");
	}

	layers |= 1 << (uint32_t)layer;
}

void Node::removeFromLayer(const uint8_t layer)
{
	if (layer >= 20)
	{
		throw std::out_of_range("layer >= 20");
	}

	layers &= ~(1 << (uint32_t)layer) & 0x000FFFFF;
}

const glm::mat4& Node::getTransformMatrix() const
{
	return transformMatrix;
}

INodeSP Node::findNodeRecursive(const std::string& searchName)
{
	if (name == searchName)
	{
		return INode::shared_from_this();
	}

    for (size_t i = 0; i < allChildNodes.size(); i++)
    {
    	auto result = allChildNodes[i]->findNodeRecursive(searchName);

    	if (result.get())
    	{
    		return result;
    	}
    }

    return INodeSP(nullptr);
}

INodeSP Node::findNodeRecursiveFromRoot(const std::string& searchName)
{
	auto currentNode = INode::shared_from_this();

	// Search, until parent node is hit.
	while (currentNode.get())
	{
		if (currentNode->getName() == searchName)
		{
			return currentNode;
		}

		if (currentNode->getParentNode().get())
		{
			currentNode = currentNode->getParentNode();
		}
		else
		{
			break;
		}
	}

	//

	// Now, search complete tree.
	return findNodeRecursive(searchName);
}


void Node::visitRecursive(SceneVisitor* sceneVisitor)
{
	SceneVisitor* currentSceneVisitor = sceneVisitor;

	while (currentSceneVisitor)
	{
		if (!currentSceneVisitor->visit(*this))
		{
			return;
		}

		currentSceneVisitor = currentSceneVisitor->getNextSceneVisitor();
	}

	for (size_t i = 0; i < allMeshes.size(); i++)
	{
		static_cast<Mesh*>(allMeshes[i].get())->visitRecursive(sceneVisitor);
	}

	for (size_t i = 0; i < allChildNodes.size(); i++)
	{
		static_cast<Node*>(allChildNodes[i].get())->visitRecursive(sceneVisitor);
	}
}

//
// ICloneable
//

INodeSP Node::clone() const
{
	auto result = INodeSP(new Node(*this));

	if (result.get() && result->getNumberCameras() != getNumberCameras())
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberLights() != getNumberLights())
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberChildNodes() != getNumberChildNodes())
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberMeshes() != getNumberMeshes())
	{
		return INodeSP();
	}

	if (result.get() && result->getNumberAnimations() != getNumberAnimations())
	{
		return INodeSP();
	}

	if (result.get() && getTransformUniformBuffer().get() && !result->getTransformUniformBuffer().get())
	{
		return INodeSP();
	}

	if (result.get() && getJointsUniformBuffer().get() && !result->getJointsUniformBuffer().get())
	{
		return INodeSP();
	}

    return result;
}

//
// IDestroyable
//

void Node::destroy()
{
    for (size_t i = 0; i < allChildNodes.size(); i++)
    {
        allChildNodes[i]->destroy();
    }
    allChildNodes.clear();

    for (size_t i = 0; i < allMeshes.size(); i++)
    {
        allMeshes[i]->destroy();
    }
    allMeshes.clear();

    allLights.clear();

    allConstraints.clear();

    for (size_t i = 0; i < allAnimations.size(); i++)
    {
        allAnimations[i]->destroy();
    }
    allAnimations.clear();
}

} /* namespace vkts */
