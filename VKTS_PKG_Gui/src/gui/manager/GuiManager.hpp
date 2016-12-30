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

#ifndef VKTS_GUIMANAGER_HPP_
#define VKTS_GUIMANAGER_HPP_

#include <vkts/gui/vkts_gui.hpp>

namespace vkts
{

class GuiManager: public IGuiManager
{

protected:

    const IAssetManagerSP assetManager;


    template<class S, class T>
    VkBool32 contains(const S& key, const SmartPointerMap<S, T>& map) const
    {
    	return map.contains(key);
    }

    template<class S, class T>
    const T& get(const S& key, const SmartPointerMap<S, T>& map) const
    {
    	return map[key];
    }

    template<class S, class T>
    VkBool32 add(const S& key, const T& value, SmartPointerMap<S, T>& map)
    {
        if (assetManager->isReplace() || map.find(key) == map.size())
        {
            map[key] = value;

            return VK_TRUE;
        }

        return VK_FALSE;
    }

    template<class S, class T>
    VkBool32 remove(const S& key, SmartPointerMap<S, T>& map)
    {
        auto index = map.find(key);

        if (index != map.size())
        {
            map.removeAt(index);

            return VK_TRUE;
        }

        return VK_FALSE;
    }

public:

    GuiManager() = delete;
    explicit GuiManager(const IAssetManagerSP& assetManager);
    GuiManager(const GuiManager& other) = delete;
    GuiManager(GuiManager&& other) = delete;
    virtual ~GuiManager();

    GuiManager& operator =(const GuiManager& other) = delete;

    GuiManager& operator =(GuiManager && other) = delete;

    //
    // IGuiManager
    //

    virtual const IAssetManagerSP& getAssetManager() const override;

    virtual const IContextObjectSP& getContextObject() const override;

    virtual const ICommandObjectSP& getCommandObject() const override;

    //

    virtual ITextureObjectSP useTextureObject(const std::string& name) const override;

    virtual VkBool32 addTextureObject(const ITextureObjectSP& textureObject) override;

    virtual VkBool32 removeTextureObject(const ITextureObjectSP& textureObject) override;

    //

    virtual IImageObjectSP useImageObject(const std::string& name) const override;

    virtual VkBool32 addImageObject(const IImageObjectSP& imageObject) override;

    virtual VkBool32 removeImageObject(const IImageObjectSP& imageObject) override;

    //

    virtual ISamplerSP useSampler(const std::string& name) const override;

    virtual VkBool32 addSampler(const ISamplerSP& sampler) override;

    virtual VkBool32 removeSampler(const ISamplerSP& sampler) override;

    //

    virtual IImageDataSP useImageData(const std::string& name) const override;

    virtual VkBool32 addImageData(const IImageDataSP& imageData) override;

    virtual VkBool32 removeImageData(const IImageDataSP& imageData) override;

    //

    virtual IShaderModuleSP useVertexShaderModule(const VkTsVertexBufferType vertexBufferType) const override;

    virtual VkBool32 addVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule) override;

    virtual VkBool32 removeVertexShaderModule(const VkTsVertexBufferType vertexBufferType, const IShaderModuleSP& shaderModule) override;


    virtual IShaderModuleSP useFragmentShaderModule(const std::string& name) const override;

    virtual VkBool32 addFragmentShaderModule(const IShaderModuleSP& shaderModule) override;

    virtual VkBool32 removeFragmentShaderModule(const IShaderModuleSP& shaderModule) override;

    //
    // IDestroyable
    //

    virtual void destroy() override;

};

} /* namespace vkts */

#endif /* VKTS_GUIMANAGER_HPP_ */
