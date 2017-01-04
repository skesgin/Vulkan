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

AssetManager::AssetManager(const VkBool32 replace, const IContextObjectSP& contextObject, const ICommandObjectSP& commandObject) :
    IAssetManager(), replace(replace), contextObject(contextObject), commandObject(commandObject), allTextureObjects(), allImageObjects(), allSamplers(), allImageDatas(), allVertexShaderModules(), allFragmentShaderModules()
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

VkBool32 AssetManager::isReplace() const
{
    return replace;
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

ISamplerSP AssetManager::useSampler(const std::string& name) const
{
	if (!contains(name, allSamplers))
	{
		return ISamplerSP();
	}

	//

    return get(name, allSamplers);
}

VkBool32 AssetManager::addSampler(const ISamplerSP& sampler)
{
    if (!sampler.get())
    {
        return VK_FALSE;
    }

    return add(sampler->getName(), sampler, allSamplers);
}

VkBool32 AssetManager::removeSampler(const ISamplerSP& sampler)
{
    if (!sampler.get())
    {
        return VK_FALSE;
    }

    return remove(sampler->getName(), allSamplers);
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

const SmartPointerMap<std::string, IImageDataSP>& AssetManager::getAllImageDatas() const
{
	return allImageDatas;
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
    allTextureObjects.clear();

    allImageObjects.clear();

    allSamplers.clear();

    allImageDatas.clear();

    allVertexShaderModules.clear();

    allFragmentShaderModules.clear();
}

} /* namespace vkts */
