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

#ifndef VKTS_ASSETMANAGER_HPP_
#define VKTS_ASSETMANAGER_HPP_

#include <vkts/vulkan/composition/vkts_composition.hpp>

namespace vkts
{

class AssetManager: public IAssetManager
{

protected:

    VkBool32 replace;

    const IContextObjectSP contextObject;

    const ICommandObjectSP commandObject;


    SmartPointerMap<std::string, ITextureObjectSP> allTextureObjects;

    SmartPointerMap<std::string, IImageObjectSP> allImageObjects;

    SmartPointerMap<std::string, IImageDataSP> allImageDatas;

    SmartPointerMap<VkTsVertexBufferType, IShaderModuleSP> allVertexShaderModules;

    SmartPointerMap<std::string, IShaderModuleSP> allFragmentShaderModules;

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
        if (replace || map.find(key) == map.size())
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

    AssetManager() = delete;
    AssetManager(const VkBool32 replace, const IContextObjectSP& contextObject, const ICommandObjectSP& commandObject);
    AssetManager(const AssetManager& other) = delete;
    AssetManager(AssetManager&& other) = delete;
    virtual ~AssetManager();

    AssetManager& operator =(const AssetManager& other) = delete;

    AssetManager& operator =(AssetManager && other) = delete;

    //
    // IAssetManager
    //

    virtual const IContextObjectSP& getContextObject() const override;

    virtual const ICommandObjectSP& getCommandObject() const override;

    virtual VkBool32 isReplace() const override;

    //

    virtual ITextureObjectSP useTextureObject(const std::string& name) const override;

    virtual VkBool32 addTextureObject(const ITextureObjectSP& textureObject) override;

    virtual VkBool32 removeTextureObject(const ITextureObjectSP& textureObject) override;

    //

    virtual IImageObjectSP useImageObject(const std::string& name) const override;

    virtual VkBool32 addImageObject(const IImageObjectSP& imageObject) override;

    virtual VkBool32 removeImageObject(const IImageObjectSP& imageObject) override;

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

#endif /* VKTS_ASSETMANAGER_HPP_ */
