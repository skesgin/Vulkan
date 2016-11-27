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

#include "AssetManager.hpp"

namespace vkts
{

AssetManager::AssetManager(const VkBool32 replace, const IContextObjectSP& contextObject, const ICommandObjectSP& commandObject, const IDescriptorSetLayoutSP& descriptorSetLayout, const IRenderPassSP& renderPass) :
    IAssetManager(), replace(replace), contextObject(contextObject), commandObject(commandObject), descriptorSetLayout(descriptorSetLayout), renderPass(renderPass), allObjects(), allCameras(), allLights(), allMeshes(), allSubMeshes(), allAnimations(), allChannels(), allBSDFMaterials(), allUsedBSDFMaterials(), allPhongMaterials(), allUsedPhongMaterials(), allTextureObjects(), allImageDatas(), allVertexShaderModules(), allFragmentShaderModules()
{
}

AssetManager::~AssetManager()
{
    destroy();
}

//
// IAssetManager
//

const IContextObjectSP& AssetManager::getContextObject() const
{
    return contextObject;
}

const ICommandObjectSP& AssetManager::getCommandObject() const
{
    return commandObject;
}

const IDescriptorSetLayoutSP& AssetManager::getDescriptorSetLayout() const
{
    return descriptorSetLayout;
}

const IRenderPassSP& AssetManager::getRenderPass() const
{
	return renderPass;
}

//

IObjectSP AssetManager::useObject(const std::string& name) const
{
	if (!contains(name, allObjects))
	{
		return IObjectSP();
	}

	//

    return get(name, allObjects);
}

VkBool32 AssetManager::addObject(const IObjectSP& object)
{
    if (!object.get())
    {
        return VK_FALSE;
    }

    return add(object->getName(), object, allObjects);
}

VkBool32 AssetManager::removeObject(const IObjectSP& object)
{
    if (!object.get())
    {
        return VK_FALSE;
    }

    return remove(object->getName(), allObjects);
}

const SmartPointerMap<std::string, IObjectSP>& AssetManager::getAllObjects() const
{
	return allObjects;
}

//

ICameraSP AssetManager::useCamera(const std::string& name) const
{
	if (!contains(name, allCameras))
	{
		return ICameraSP();
	}

	//

    return get(name, allCameras);
}

VkBool32 AssetManager::addCamera(const ICameraSP& camera)
{
    if (!camera.get())
    {
        return VK_FALSE;
    }

    return add(camera->getName(), camera, allCameras);
}

VkBool32 AssetManager::removeCamera(const ICameraSP& camera)
{
    if (!camera.get())
    {
        return VK_FALSE;
    }

    return remove(camera->getName(), allCameras);
}

const SmartPointerMap<std::string, ICameraSP>& AssetManager::getAllCameras() const
{
	return allCameras;
}

//

ILightSP AssetManager::useLight(const std::string& name) const
{
	if (!contains(name, allLights))
	{
		return ILightSP();
	}

	//

    return get(name, allLights);
}

VkBool32 AssetManager::addLight(const ILightSP& light)
{
    if (!light.get())
    {
        return VK_FALSE;
    }

    return add(light->getName(), light, allLights);
}

VkBool32 AssetManager::removeLight(const ILightSP& light)
{
    if (!light.get())
    {
        return VK_FALSE;
    }

    return remove(light->getName(), allLights);
}

const SmartPointerMap<std::string, ILightSP>& AssetManager::getAllLights() const
{
	return allLights;
}

//

IMeshSP AssetManager::useMesh(const std::string& name) const
{
	if (!contains(name, allMeshes))
	{
		return IMeshSP();
	}

	//

    return get(name, allMeshes);
}

VkBool32 AssetManager::addMesh(const IMeshSP& mesh)
{
    if (!mesh.get())
    {
        return VK_FALSE;
    }

    return add(mesh->getName(), mesh, allMeshes);
}

VkBool32 AssetManager::removeMesh(const IMeshSP& mesh)
{
    if (!mesh.get())
    {
        return VK_FALSE;
    }

    return remove(mesh->getName(), allMeshes);
}

ISubMeshSP AssetManager::useSubMesh(const std::string& name) const
{
	if (!contains(name, allSubMeshes))
	{
		return ISubMeshSP();
	}

	//

    return get(name, allSubMeshes);
}

VkBool32 AssetManager::addSubMesh(const ISubMeshSP& subMesh)
{
    if (!subMesh.get())
    {
        return VK_FALSE;
    }

    return add(subMesh->getName(), subMesh, allSubMeshes);
}

VkBool32 AssetManager::removeSubMesh(const ISubMeshSP& subMesh)
{
    if (!subMesh.get())
    {
        return VK_FALSE;
    }

    return remove(subMesh->getName(), allSubMeshes);
}

//

IAnimationSP AssetManager::useAnimation(const std::string& name) const
{
	if (!contains(name, allAnimations))
	{
		return IAnimationSP();
	}

	//

    return get(name, allAnimations);
}

VkBool32 AssetManager::addAnimation(const IAnimationSP& animation)
{
    if (!animation.get())
    {
        return VK_FALSE;
    }

    return add(animation->getName(), animation, allAnimations);
}

VkBool32 AssetManager::removeAnimation(const IAnimationSP& animation)
{
    if (!animation.get())
    {
        return VK_FALSE;
    }

    return remove(animation->getName(), allAnimations);
}

IChannelSP AssetManager::useChannel(const std::string& name) const
{
	if (!contains(name, allChannels))
	{
		return IChannelSP();
	}

	//

    return get(name, allChannels);
}

VkBool32 AssetManager::addChannel(const IChannelSP& channel)
{
    if (!channel.get())
    {
        return VK_FALSE;
    }

    return add(channel->getName(), channel, allChannels);
}

VkBool32 AssetManager::removeChannel(const IChannelSP& channel)
{
    if (!channel.get())
    {
        return VK_FALSE;
    }

    return remove(channel->getName(), allChannels);
}

//

IBSDFMaterialSP AssetManager::useBSDFMaterial(const std::string& name) const
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

VkBool32 AssetManager::addBSDFMaterial(const IBSDFMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    return add(material->getName(), material, allBSDFMaterials);
}

VkBool32 AssetManager::removeBSDFMaterial(const IBSDFMaterialSP& material)
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

IPhongMaterialSP AssetManager::usePhongMaterial(const std::string& name) const
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

VkBool32 AssetManager::addPhongMaterial(const IPhongMaterialSP& material)
{
    if (!material.get())
    {
        return VK_FALSE;
    }

    return add(material->getName(), material, allPhongMaterials);
}

VkBool32 AssetManager::removePhongMaterial(const IPhongMaterialSP& material)
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

ITextureObjectSP AssetManager::useTextureObject(const std::string& name) const
{
	if (!contains(name, allTextureObjects))
	{
		return ITextureObjectSP();
	}

	//

    return get(name, allTextureObjects);
}

VkBool32 AssetManager::addTextureObject(const ITextureObjectSP& textureObject)
{
    if (!textureObject.get())
    {
        return VK_FALSE;
    }

    return add(textureObject->getName(), textureObject, allTextureObjects);
}

VkBool32 AssetManager::removeTextureObject(const ITextureObjectSP& textureObject)
{
    if (!textureObject.get())
    {
        return VK_FALSE;
    }

    return remove(textureObject->getName(), allTextureObjects);
}

//

IImageObjectSP AssetManager::useImageObject(const std::string& name) const
{
	if (!contains(name, allImageObjects))
	{
		return IImageObjectSP();
	}

	//

    return get(name, allImageObjects);
}

VkBool32 AssetManager::addImageObject(const IImageObjectSP& imageObject)
{
    if (!imageObject.get())
    {
        return VK_FALSE;
    }

    return add(imageObject->getName(), imageObject, allImageObjects);
}

VkBool32 AssetManager::removeImageObject(const IImageObjectSP& imageObject)
{
    if (!imageObject.get())
    {
        return VK_FALSE;
    }

    return remove(imageObject->getName(), allImageObjects);
}

//

IImageDataSP AssetManager::useImageData(const std::string& name) const
{
	if (!contains(name, allImageDatas))
	{
		return IImageDataSP();
	}

	//

    return get(name, allImageDatas);
}

VkBool32 AssetManager::addImageData(const IImageDataSP& imageData)
{
    if (!imageData.get())
    {
        return VK_FALSE;
    }

    return add(imageData->getName(), imageData, allImageDatas);
}

VkBool32 AssetManager::removeImageData(const IImageDataSP& imageData)
{
    if (!imageData.get())
    {
        return VK_FALSE;
    }

    return remove(imageData->getName(), allImageDatas);
}


//

IShaderModuleSP AssetManager::useVertexShaderModule(const VkTsVertexBufferType vertexBufferType) const
{
	if (!contains(vertexBufferType, allVertexShaderModules))
	{
		return IShaderModuleSP();
	}

	//

    return get(vertexBufferType, allVertexShaderModules);
}

VkBool32 AssetManager::addVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule)
{
    if (!shaderModule.get())
    {
        return VK_FALSE;
    }

    return add(vertexBufferType, shaderModule, allVertexShaderModules);
}

VkBool32 AssetManager::removeVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule)
{
    if (!shaderModule.get())
    {
        return VK_FALSE;
    }

    return remove(vertexBufferType, allVertexShaderModules);
}

IShaderModuleSP AssetManager::useFragmentShaderModule(const std::string& name) const
{
	if (!contains(name, allFragmentShaderModules))
	{
		return IShaderModuleSP();
	}

	//

    return get(name, allFragmentShaderModules);
}

VkBool32 AssetManager::addFragmentShaderModule(const IShaderModuleSP& shaderModule)
{
    if (!shaderModule.get())
    {
        return VK_FALSE;
    }

    return add(shaderModule->getName(), shaderModule, allFragmentShaderModules);
}

VkBool32 AssetManager::removeFragmentShaderModule(const IShaderModuleSP& shaderModule)
{
    if (!shaderModule.get())
    {
        return VK_FALSE;
    }

    return remove(shaderModule->getName(), allFragmentShaderModules);
}

//
// IDestroyable
//

void AssetManager::destroy()
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

    // Stage resources have to be deleted.

    if (commandObject.get())
    {
    	commandObject->destroy();
    }
}

} /* namespace vkts */
