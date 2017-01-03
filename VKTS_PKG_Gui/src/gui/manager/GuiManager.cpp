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

#include "GuiManager.hpp"

namespace vkts
{

GuiManager::GuiManager(const IAssetManagerSP& assetManager) :
    IGuiManager(), assetManager(assetManager)
{
}

GuiManager::~GuiManager()
{
    destroy();
}

//
// IGuiManager
//

const IAssetManagerSP& GuiManager::getAssetManager() const
{
    return assetManager;
}

const IContextObjectSP& GuiManager::getContextObject() const
{
    return assetManager->getContextObject();
}

const ICommandObjectSP& GuiManager::getCommandObject() const
{
    return assetManager->getCommandObject();
}

//

ITextureObjectSP GuiManager::useTextureObject(const std::string& name) const
{
	return assetManager->useTextureObject(name);
}

VkBool32 GuiManager::addTextureObject(const ITextureObjectSP& textureObject)
{
	return assetManager->addTextureObject(textureObject);
}

VkBool32 GuiManager::removeTextureObject(const ITextureObjectSP& textureObject)
{
	return assetManager->removeTextureObject(textureObject);
}

//

IImageObjectSP GuiManager::useImageObject(const std::string& name) const
{
	return assetManager->useImageObject(name);
}

VkBool32 GuiManager::addImageObject(const IImageObjectSP& imageObject)
{
	return assetManager->addImageObject(imageObject);
}

VkBool32 GuiManager::removeImageObject(const IImageObjectSP& imageObject)
{
	return assetManager->removeImageObject(imageObject);
}

//

ISamplerSP GuiManager::useSampler(const std::string& name) const
{
	return assetManager->useSampler(name);
}

VkBool32 GuiManager::addSampler(const ISamplerSP& sampler)
{
	return assetManager->addSampler(sampler);
}

VkBool32 GuiManager::removeSampler(const ISamplerSP& sampler)
{
	return assetManager->removeSampler(sampler);
}

//

IImageDataSP GuiManager::useImageData(const std::string& name) const
{
	return assetManager->useImageData(name);
}

VkBool32 GuiManager::addImageData(const IImageDataSP& imageData)
{
	return assetManager->addImageData(imageData);
}

VkBool32 GuiManager::removeImageData(const IImageDataSP& imageData)
{
    return assetManager->removeImageData(imageData);
}

//

IShaderModuleSP GuiManager::useVertexShaderModule(const VkTsVertexBufferType vertexBufferType) const
{
	return assetManager->useVertexShaderModule(vertexBufferType);
}

VkBool32 GuiManager::addVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule)
{
	return assetManager->addVertexShaderModule(vertexBufferType, shaderModule);
}

VkBool32 GuiManager::removeVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule)
{
	return assetManager->removeVertexShaderModule(vertexBufferType, shaderModule);
}

IShaderModuleSP GuiManager::useFragmentShaderModule(const std::string& name) const
{
	return assetManager->useFragmentShaderModule(name);
}

VkBool32 GuiManager::addFragmentShaderModule(const IShaderModuleSP& shaderModule)
{
	return assetManager->addFragmentShaderModule(shaderModule);
}

VkBool32 GuiManager::removeFragmentShaderModule(const IShaderModuleSP& shaderModule)
{
	return assetManager->removeFragmentShaderModule(shaderModule);
}

//
// IDestroyable
//

void GuiManager::destroy()
{
	assetManager.reset();
}

} /* namespace vkts */
