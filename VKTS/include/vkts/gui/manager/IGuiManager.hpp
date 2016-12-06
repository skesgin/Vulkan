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

#ifndef VKTS_IGUIMANAGER_HPP_
#define VKTS_IGUIMANAGER_HPP_

#include <vkts/gui/vkts_gui.hpp>

namespace vkts
{

class IGuiManager: public IDestroyable
{

public:

    IGuiManager() :
        IDestroyable()
    {
    }

    virtual ~IGuiManager()
    {
    }

    virtual const IAssetManagerSP& getAssetManager() const = 0;

    virtual const IContextObjectSP& getContextObject() const = 0;

    virtual const ICommandObjectSP& getCommandObject() const = 0;

    //


    virtual ITextureObjectSP useTextureObject(const std::string& name) const = 0;

    virtual VkBool32 addTextureObject(const ITextureObjectSP& textureObject) = 0;

    virtual VkBool32 removeTextureObject(const ITextureObjectSP& textureObject) = 0;

    //

    virtual IImageObjectSP useImageObject(const std::string& name) const = 0;

    virtual VkBool32 addImageObject(const IImageObjectSP& imageObject) = 0;

    virtual VkBool32 removeImageObject(const IImageObjectSP& imageObject) = 0;

    //

    virtual IImageDataSP useImageData(const std::string& name) const = 0;

    virtual VkBool32 addImageData(const IImageDataSP& imageData) = 0;

    virtual VkBool32 removeImageData(const IImageDataSP& imageData) = 0;

    //

    virtual IShaderModuleSP useVertexShaderModule(const VkTsVertexBufferType vertexBufferType) const = 0;

    virtual VkBool32 addVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule) = 0;

    virtual VkBool32 removeVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule) = 0;


    virtual IShaderModuleSP useFragmentShaderModule(const std::string& name) const = 0;

    virtual VkBool32 addFragmentShaderModule(const IShaderModuleSP& shaderModule) = 0;

    virtual VkBool32 removeFragmentShaderModule(const IShaderModuleSP& shaderModule) = 0;

};

typedef std::shared_ptr<IGuiManager> IGuiManagerSP;

} /* namespace vkts */

#endif /* VKTS_IGUIMANAGER_HPP_ */
