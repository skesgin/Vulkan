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

#include "Context.hpp"

namespace vkts
{

Context::Context(const VkBool32 replace, const IContextObjectSP& contextObject, const ICommandBuffersSP& cmdBuffer, const VkSamplerCreateInfo& samplerCreateInfo, const VkImageViewCreateInfo& imageViewCreateInfo, const IDescriptorSetLayoutSP& descriptorSetLayout) :
    IContext(), replace(replace), contextObject(contextObject), cmdBuffer(cmdBuffer), samplerCreateInfo(samplerCreateInfo), imageViewCreateInfo(imageViewCreateInfo), descriptorSetLayout(descriptorSetLayout), allObjects(), allCameras(), allLights(), allMeshes(), allSubMeshes(), allAnimations(), allChannels(), allBSDFMaterials(), allUsedBSDFMaterials(), allPhongMaterials(), allUsedPhongMaterials(), allTextureObjects(), allImageDatas(), allVertexShaderModules(), allFragmentShaderModules(), renderPass()
{
}

Context::~Context()
{
    destroy();
}

//
// IContext
//

const IContextObjectSP& Context::getContextObject() const
{
    return contextObject;
}

const ICommandBuffersSP& Context::getCommandBuffer() const
{
    return cmdBuffer;
}

const VkSamplerCreateInfo& Context::getSamplerCreateInfo() const
{
    return samplerCreateInfo;
}

const VkImageViewCreateInfo& Context::getImageViewCreateInfo() const
{
    return imageViewCreateInfo;
}

const IDescriptorSetLayoutSP& Context::getDescriptorSetLayout() const
{
    return descriptorSetLayout;
}

//

IObjectSP Context::useObject(const std::string& name) const
{
	if (!contains(name, allObjects))
	{
		return IObjectSP();
	}

	//

    return get(name, allObjects);
}

VkBool32 Context::addObject(const IObjectSP& object)
{
    if (!object.get())
    {
        return VK_FALSE;
    }

    return add(object->getName(), object, allObjects);
}

VkBool32 Context::removeObject(const IObjectSP& object)
{
    if (!object.get())
    {
        return VK_FALSE;
    }

    return remove(object->getName(), allObjects);
}

const SmartPointerMap<std::string, IObjectSP>& Context::getAllObjects() const
{
	return allObjects;
}

//

ICameraSP Context::useCamera(const std::string& name) const
{
	if (!contains(name, allCameras))
	{
		return ICameraSP();
	}

	//

    return get(name, allCameras);
}

VkBool32 Context::addCamera(const ICameraSP& camera)
{
    if (!camera.get())
    {
        return VK_FALSE;
    }

    return add(camera->getName(), camera, allCameras);
}

VkBool32 Context::removeCamera(const ICameraSP& camera)
{
    if (!camera.get())
    {
        return VK_FALSE;
    }

    return remove(camera->getName(), allCameras);
}

const SmartPointerMap<std::string, ICameraSP>& Context::getAllCameras() const
{
	return allCameras;
}

//

ILightSP Context::useLight(const std::string& name) const
{
	if (!contains(name, allLights))
	{
		return ILightSP();
	}

	//

    return get(name, allLights);
}

VkBool32 Context::addLight(const ILightSP& light)
{
    if (!light.get())
    {
        return VK_FALSE;
    }

    return add(light->getName(), light, allLights);
}

VkBool32 Context::removeLight(const ILightSP& light)
{
    if (!light.get())
    {
        return VK_FALSE;
    }

    return remove(light->getName(), allLights);
}

const SmartPointerMap<std::string, ILightSP>& Context::getAllLights() const
{
	return allLights;
}

//

IMeshSP Context::useMesh(const std::string& name) const
{
	if (!contains(name, allMeshes))
	{
		return IMeshSP();
	}

	//

    return get(name, allMeshes);
}

VkBool32 Context::addMesh(const IMeshSP& mesh)
{
    if (!mesh.get())
    {
        return VK_FALSE;
    }

    return add(mesh->getName(), mesh, allMeshes);
}

VkBool32 Context::removeMesh(const IMeshSP& mesh)
{
    if (!mesh.get())
    {
        return VK_FALSE;
    }

    return remove(mesh->getName(), allMeshes);
}

ISubMeshSP Context::useSubMesh(const std::string& name) const
{
	if (!contains(name, allSubMeshes))
	{
		return ISubMeshSP();
	}

	//

    return get(name, allSubMeshes);
}

VkBool32 Context::addSubMesh(const ISubMeshSP& subMesh)
{
    if (!subMesh.get())
    {
        return VK_FALSE;
    }

    return add(subMesh->getName(), subMesh, allSubMeshes);
}

VkBool32 Context::removeSubMesh(const ISubMeshSP& subMesh)
{
    if (!subMesh.get())
    {
        return VK_FALSE;
    }

    return remove(subMesh->getName(), allSubMeshes);
}

//

IAnimationSP Context::useAnimation(const std::string& name) const
{
	if (!contains(name, allAnimations))
	{
		return IAnimationSP();
	}

	//

    return get(name, allAnimations);
}

VkBool32 Context::addAnimation(const IAnimationSP& animation)
{
    if (!animation.get())
    {
        return VK_FALSE;
    }

    return add(animation->getName(), animation, allAnimations);
}

VkBool32 Context::removeAnimation(const IAnimationSP& animation)
{
    if (!animation.get())
    {
        return VK_FALSE;
    }

    return remove(animation->getName(), allAnimations);
}

IChannelSP Context::useChannel(const std::string& name) const
{
	if (!contains(name, allChannels))
	{
		return IChannelSP();
	}

	//

    return get(name, allChannels);
}

VkBool32 Context::addChannel(const IChannelSP& channel)
{
    if (!channel.get())
    {
        return VK_FALSE;
    }

    return add(channel->getName(), channel, allChannels);
}

VkBool32 Context::removeChannel(const IChannelSP& channel)
{
    if (!channel.get())
    {
        return VK_FALSE;
    }

    return remove(channel->getName(), allChannels);
}

//

IBSDFMaterialSP Context::useBSDFMaterial(const std::string& name) const
{
	if (!contains(name, allBSDFMaterials))
	{
		return IBSDFMaterialSP();
	}

	//

	IBSDFMaterialSP result = get(name, allBSDFMaterials);

	if (std::find(allUsedBSDFMaterials.begin(), allUsedBSDFMaterials.end(), name) != allUsedBSDFMaterials.end())
	{
		result = result->clone();
	}
	else
	{
		allUsedBSDFMaterials.push_back(name);
	}

    return result;
}

VkBool32 Context::addBSDFMaterial(const IBSDFMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    return add(material->getName(), material, allBSDFMaterials);
}

VkBool32 Context::removeBSDFMaterial(const IBSDFMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    auto removeElement = std::find(allUsedBSDFMaterials.begin(), allUsedBSDFMaterials.end(), material->getName());
    if (removeElement != allUsedBSDFMaterials.end())
    {
    	allUsedBSDFMaterials.erase(removeElement);
    }

    return remove(material->getName(), allBSDFMaterials);
}

IPhongMaterialSP Context::usePhongMaterial(const std::string& name) const
{
	if (!contains(name, allPhongMaterials))
	{
		return IPhongMaterialSP();
	}

	//

	IPhongMaterialSP result = get(name, allPhongMaterials);

	if (std::find(allUsedPhongMaterials.begin(), allUsedPhongMaterials.end(), name) != allUsedPhongMaterials.end())
	{
		result = result->clone();
	}
	else
	{
		allUsedPhongMaterials.push_back(name);
	}

    return result;
}

VkBool32 Context::addPhongMaterial(const IPhongMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    return add(material->getName(), material, allPhongMaterials);
}

VkBool32 Context::removePhongMaterial(const IPhongMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    auto removeElement = std::find(allUsedPhongMaterials.begin(), allUsedPhongMaterials.end(), material->getName());
    if (removeElement != allUsedPhongMaterials.end())
    {
    	allUsedPhongMaterials.erase(removeElement);
    }

    return remove(material->getName(), allPhongMaterials);
}

//

ITextureObjectSP Context::useTextureObject(const std::string& name) const
{
	if (!contains(name, allTextureObjects))
	{
		return ITextureObjectSP();
	}

	//

    return get(name, allTextureObjects);
}

VkBool32 Context::addTextureObject(const ITextureObjectSP& textureObject)
{
    if (!textureObject.get())
    {
        return VK_FALSE;
    }

    return add(textureObject->getName(), textureObject, allTextureObjects);
}

VkBool32 Context::removeTextureObject(const ITextureObjectSP& textureObject)
{
    if (!textureObject.get())
    {
        return VK_FALSE;
    }

    return remove(textureObject->getName(), allTextureObjects);
}

//

IImageObjectSP Context::useImageObject(const std::string& name) const
{
	if (!contains(name, allImageObjects))
	{
		return IImageObjectSP();
	}

	//

    return get(name, allImageObjects);
}

VkBool32 Context::addImageObject(const IImageObjectSP& imageObject)
{
    if (!imageObject.get())
    {
        return VK_FALSE;
    }

    return add(imageObject->getName(), imageObject, allImageObjects);
}

VkBool32 Context::removeImageObject(const IImageObjectSP& imageObject)
{
    if (!imageObject.get())
    {
        return VK_FALSE;
    }

    return remove(imageObject->getName(), allImageObjects);
}

//

IImageDataSP Context::useImageData(const std::string& name) const
{
	if (!contains(name, allImageDatas))
	{
		return IImageDataSP();
	}

	//

    return get(name, allImageDatas);
}

VkBool32 Context::addImageData(const IImageDataSP& imageData)
{
    if (!imageData.get())
    {
        return VK_FALSE;
    }

    return add(imageData->getName(), imageData, allImageDatas);
}

VkBool32 Context::removeImageData(const IImageDataSP& imageData)
{
    if (!imageData.get())
    {
        return VK_FALSE;
    }

    return remove(imageData->getName(), allImageDatas);
}


//

IShaderModuleSP Context::useVertexShaderModule(const VkTsVertexBufferType vertexBufferType) const
{
	if (!contains(vertexBufferType, allVertexShaderModules))
	{
		return IShaderModuleSP();
	}

	//

    return get(vertexBufferType, allVertexShaderModules);
}

VkBool32 Context::addVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule)
{
    if (!shaderModule.get())
    {
        return VK_FALSE;
    }

    return add(vertexBufferType, shaderModule, allVertexShaderModules);
}

VkBool32 Context::removeVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule)
{
    if (!shaderModule.get())
    {
        return VK_FALSE;
    }

    return remove(vertexBufferType, allVertexShaderModules);
}

IShaderModuleSP Context::useFragmentShaderModule(const std::string& name) const
{
	if (!contains(name, allFragmentShaderModules))
	{
		return IShaderModuleSP();
	}

	//

    return get(name, allFragmentShaderModules);
}

VkBool32 Context::addFragmentShaderModule(const IShaderModuleSP& shaderModule)
{
    if (!shaderModule.get())
    {
        return VK_FALSE;
    }

    return add(shaderModule->getName(), shaderModule, allFragmentShaderModules);
}

VkBool32 Context::removeFragmentShaderModule(const IShaderModuleSP& shaderModule)
{
    if (!shaderModule.get())
    {
        return VK_FALSE;
    }

    return remove(shaderModule->getName(), allFragmentShaderModules);
}

//

IRenderPassSP Context::getRenderPass() const
{
	return renderPass;
}

void Context::setRenderPass(const IRenderPassSP& renderPass)
{
	this->renderPass = renderPass;
}

//

void Context::addStageImage(const IImageSP& stageImage)
{
    if (stageImage.get())
    {
        allStageImages.append(stageImage);
    }
}

void Context::addStageBuffer(const IBufferSP& stageBuffer)
{
    if (stageBuffer.get())
    {
        allStageBuffers.append(stageBuffer);
    }
}

void Context::addStageDeviceMemory(const IDeviceMemorySP& stageDeviceMemory)
{
    if (stageDeviceMemory.get())
    {
        allStageDeviceMemories.append(stageDeviceMemory);
    }
}

//
// IDestroyable
//

void Context::destroy()
{
    // Only free resources, but do not destroy them.

    allObjects.clear();

    allCameras.clear();

    allLights.clear();

    allMeshes.clear();

    allSubMeshes.clear();

    allAnimations.clear();

    allChannels.clear();

    allBSDFMaterials.clear();

    allPhongMaterials.clear();

    allTextureObjects.clear();

    allImageDatas.clear();

    allVertexShaderModules.clear();

    allFragmentShaderModules.clear();

    renderPass.reset();

    // Stage resources have to be deleted.

    for (uint32_t i = 0; i < allStageImages.size(); i++)
    {
        if (allStageImages[i].get())
        {
            allStageImages[i]->destroy();
        }
    }
    allStageImages.clear();

    for (uint32_t i = 0; i < allStageBuffers.size(); i++)
    {
        if (allStageBuffers[i].get())
        {
            allStageBuffers[i]->destroy();
        }
    }
    allStageBuffers.clear();

    for (uint32_t i = 0; i < allStageDeviceMemories.size(); i++)
    {
        if (allStageDeviceMemories[i].get())
        {
            allStageDeviceMemories[i]->destroy();
        }
    }
    allStageDeviceMemories.clear();
}

} /* namespace vkts */
