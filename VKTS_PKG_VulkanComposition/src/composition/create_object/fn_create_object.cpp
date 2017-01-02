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

#include <vkts/vulkan/composition/vkts_composition.hpp>

namespace vkts
{

IImageDataSP VKTS_APIENTRY createDeviceImageData(const IAssetManagerSP& assetManager, IImageDataSP& imageData)
{
	// Check, if image data can be used on the device.

	VkImageTiling imageTiling;
	VkMemoryPropertyFlags memoryPropertyFlags;

	if (!assetManager->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
	{
		if (imageData->getFormat() == VK_FORMAT_R8G8B8_UNORM)
		{
			imageData = imageDataConvert(imageData, VK_FORMAT_R8G8B8A8_UNORM, imageData->getName());

			if (!assetManager->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

				return IImageDataSP();
			}
		}
		else if (imageData->getFormat() == VK_FORMAT_R32G32B32_SFLOAT)
		{
			imageData = imageDataConvert(imageData, VK_FORMAT_R32G32B32A32_SFLOAT, imageData->getName());

			if (!assetManager->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

				return IImageDataSP();
			}
		}
		else if (imageData->getFormat() == VK_FORMAT_R32G32_SFLOAT)
		{
			imageData = imageDataConvert(imageData, VK_FORMAT_R32G32B32A32_SFLOAT, imageData->getName());

			if (!assetManager->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

				return IImageDataSP();
			}
		}
		else
		{
			logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

			return IImageDataSP();
		}
	}

	return imageData;
}

IImageObjectSP VKTS_APIENTRY createImageObject(const IAssetManagerSP& assetManager, const std::string& imageObjectName, const IImageDataSP& imageData, const VkBool32 environment)
{
    if (!imageData.get())
    {
    	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No image data");

    	return IImageObjectSP();
    }

    //

	VkImageTiling imageTiling;
	VkMemoryPropertyFlags memoryPropertyFlags;

	if (!assetManager->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

		return IImageObjectSP();
	}

	VkImageLayout initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	VkAccessFlags srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;

	if (imageTiling == VK_IMAGE_TILING_OPTIMAL)
	{
		initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		srcAccessMask = 0;
	}

	//

	VkImageCreateInfo imageCreateInfo{};

	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	imageCreateInfo.flags = environment ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	imageCreateInfo.imageType = imageData->getImageType();
	imageCreateInfo.format = imageData->getFormat();
	imageCreateInfo.extent = imageData->getExtent3D();
	imageCreateInfo.mipLevels = imageData->getMipLevels();
	imageCreateInfo.arrayLayers = imageData->getArrayLayers();
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = imageTiling;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = initialLayout;

	VkImageSubresourceRange subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, imageData->getMipLevels(), 0, imageData->getArrayLayers()};

	//

	IDeviceMemorySP stageDeviceMemory;
	IImageSP stageImage;
	IBufferSP stageBuffer;

	auto imageObject = imageObjectCreate(stageImage, stageBuffer, stageDeviceMemory, assetManager->getContextObject(), assetManager->getCommandObject()->getCommandBuffer(), imageObjectName, imageData, imageCreateInfo, srcAccessMask, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange, memoryPropertyFlags);

	assetManager->getCommandObject()->addStageImage(stageImage);
	assetManager->getCommandObject()->addStageBuffer(stageBuffer);
	assetManager->getCommandObject()->addStageDeviceMemory(stageDeviceMemory);

	return imageObject;
}

ITextureObjectSP VKTS_APIENTRY createTextureObject(const IAssetManagerSP& assetManager, const std::string& textureObjectName, const VkBool32 mipmap, const VkFilter filter, const VkSamplerAddressMode samplerAddressMode, const IImageObjectSP& imageObject)
{
    if (textureObjectName == "")
    {
        return ITextureObjectSP();
    }

    //

	VkSamplerCreateInfo samplerCreateInfo{};

	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerCreateInfo.flags = 0;
	samplerCreateInfo.magFilter = filter;
	samplerCreateInfo.minFilter = filter;
	samplerCreateInfo.mipmapMode = mipmap && imageObject->getImage()->getMipLevels() > 1 ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.addressModeU = samplerAddressMode;
	samplerCreateInfo.addressModeV = samplerAddressMode;
	samplerCreateInfo.addressModeW = samplerAddressMode;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = mipmap ? (float)(imageObject->getImageData()->getMipLevels() - 1) : 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	//

	auto sampler = assetManager->useSampler(samplerGetName(samplerCreateInfo));

	if (!sampler.get())
	{
		sampler = samplerCreate(assetManager->getContextObject()->getDevice()->getDevice(), samplerCreateInfo);

		if (!sampler.get())
		{
			return ITextureObjectSP();
		}

		assetManager->addSampler(sampler);
	}

    //

    return textureObjectCreate(assetManager->getContextObject(), textureObjectName, imageObject, sampler);
}

ITextureObjectSP VKTS_APIENTRY createTextureObject(const IAssetManagerSP& assetManager, const glm::vec4& color, const VkFormat format)
{
	std::string colorName = imageDataGetColorName(format, color);

    if (colorName == "")
    {
        return ITextureObjectSP();
    }

	std::string fileExtension = ".tga";

	if (imageDataIsSFLOAT(format))
	{
		fileExtension = ".hdr";
	}

    std::string imageDataName = "IMAGEDATA_" + colorName + fileExtension;
    std::string imageObjectName = "IMAGEOBJECT_" + colorName;
    std::string textureObjectName = "TEXTUREOBJECT_" + colorName;

    //

    auto imageData = assetManager->useImageData(imageDataName);

    if (!imageData.get())
    {
        imageData = imageDataCreate(imageDataName, 1, 1, 1, color, VK_IMAGE_TYPE_2D, format);

        logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Create image '%s'", imageDataName.c_str());

        if (!imageData.get())
        {
            return ITextureObjectSP();
        }

        assetManager->addImageData(imageData);
    }

    //

    VkImageTiling imageTiling;
    VkMemoryPropertyFlags memoryPropertyFlags;

    if (!assetManager->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
    {
        if (format == VK_FORMAT_R8G8B8_UNORM)
        {
            assetManager->removeImageData(imageData);

            //

        	colorName = imageDataGetColorName(format, color);

            if (colorName == "")
            {
                return ITextureObjectSP();
            }

            imageDataName = "IMAGEDATA_" + colorName + fileExtension;
            imageObjectName = "IMAGEOBJECT_" + colorName;
            textureObjectName = "TEXTUREOBJECT_" + colorName;

            //

            auto convertedImageData = assetManager->useImageData(imageDataName);

            if (!convertedImageData.get())
            {
                convertedImageData = imageDataConvert(imageData, VK_FORMAT_R8G8B8A8_UNORM, imageDataName);

                if (!assetManager->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, convertedImageData->getFormat(), convertedImageData->getImageType(), 0, convertedImageData->getExtent3D(), convertedImageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, convertedImageData->getSize()))
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                    return ITextureObjectSP();
                }

                assetManager->addImageData(convertedImageData);
            }
            else
            {
                if (!assetManager->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, convertedImageData->getFormat(),convertedImageData->getImageType(), 0, convertedImageData->getExtent3D(), convertedImageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, convertedImageData->getSize()))
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                    return ITextureObjectSP();
                }
            }

            imageData = convertedImageData;
        }
        else
        {
            return ITextureObjectSP();
        }
    }

    //

    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
    VkAccessFlags srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;

    if (imageTiling == VK_IMAGE_TILING_OPTIMAL)
    {
    	initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    	srcAccessMask = 0;
    }

    //

    VkImageCreateInfo imageCreateInfo{};

    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = imageData->getImageType();
    imageCreateInfo.format = imageData->getFormat();
    imageCreateInfo.extent = imageData->getExtent3D();
    imageCreateInfo.mipLevels = imageData->getMipLevels();
    imageCreateInfo.arrayLayers = imageData->getArrayLayers();
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = imageTiling;
    imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = nullptr;
    imageCreateInfo.initialLayout = initialLayout;

    VkImageSubresourceRange subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, imageData->getMipLevels(), 0, imageData->getArrayLayers()};

    IDeviceMemorySP stageDeviceMemory;
    IBufferSP stageBuffer;
    IImageSP stageImage;

    auto imageObject = imageObjectCreate(stageImage, stageBuffer, stageDeviceMemory, assetManager->getContextObject(), assetManager->getCommandObject()->getCommandBuffer(), imageObjectName, imageData, imageCreateInfo, srcAccessMask, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange, memoryPropertyFlags);

    assetManager->getCommandObject()->addStageImage(stageImage);
    assetManager->getCommandObject()->addStageBuffer(stageBuffer);
    assetManager->getCommandObject()->addStageDeviceMemory(stageDeviceMemory);

    if (!imageObject.get())
    {
        return ITextureObjectSP();
    }

    assetManager->addImageObject(imageObject);

    //

	VkSamplerCreateInfo samplerCreateInfo{};

	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

	samplerCreateInfo.flags = 0;
	samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.maxAnisotropy = 1.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	auto sampler = assetManager->useSampler(samplerGetName(samplerCreateInfo));

	if (!sampler.get())
	{
		sampler = samplerCreate(assetManager->getContextObject()->getDevice()->getDevice(), samplerCreateInfo);

		if (!sampler.get())
		{
			return ITextureObjectSP();
		}

		assetManager->addSampler(sampler);
	}

    //

    auto textureObject = textureObjectCreate(assetManager->getContextObject(), textureObjectName, imageObject, sampler);

    if (!textureObject.get())
    {
        return ITextureObjectSP();
    }

    assetManager->addTextureObject(textureObject);

    return textureObject;
}

IBufferObjectSP VKTS_APIENTRY createUniformBufferObject(const IAssetManagerSP& assetManager, const VkDeviceSize size, const VkDeviceSize bufferCount)
{
    VkBufferCreateInfo bufferCreateInfo{};

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    return bufferObjectCreate(assetManager->getContextObject(), bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferCount);
}

IBufferObjectSP VKTS_APIENTRY createIndexBufferObject(const IAssetManagerSP& assetManager, const IBinaryBufferSP& binaryBuffer)
{
    VkBufferCreateInfo bufferCreateInfo{};

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = (VkDeviceSize)binaryBuffer->getSize();
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    IDeviceMemorySP stageDeviceMemory;
    IBufferSP stageBuffer;

    auto indexBufferObject = bufferObjectCreate(stageBuffer, stageDeviceMemory, assetManager->getContextObject(), assetManager->getCommandObject()->getCommandBuffer(), binaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    assetManager->getCommandObject()->addStageBuffer(stageBuffer);
    assetManager->getCommandObject()->addStageDeviceMemory(stageDeviceMemory);

    return indexBufferObject;
}

IBufferObjectSP VKTS_APIENTRY createVertexBufferObject(const IAssetManagerSP& assetManager, const IBinaryBufferSP& binaryBuffer)
{
    VkBufferCreateInfo bufferCreateInfo{};

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = (VkDeviceSize)binaryBuffer->getSize();
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    IDeviceMemorySP stageDeviceMemory;
    IBufferSP stageBuffer;

    auto vertexBufferObject = bufferObjectCreate(stageBuffer, stageDeviceMemory, assetManager->getContextObject(), assetManager->getCommandObject()->getCommandBuffer(), binaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    assetManager->getCommandObject()->addStageBuffer(stageBuffer);
    assetManager->getCommandObject()->addStageDeviceMemory(stageDeviceMemory);

    return vertexBufferObject;
}

IShaderModuleSP VKTS_APIENTRY createShaderModule(const IAssetManagerSP& assetManager, const std::string& shaderModuleName, const IBinaryBufferSP& binaryBuffer)
{
	return shaderModuleCreate(shaderModuleName, assetManager->getContextObject()->getDevice()->getDevice(), 0, binaryBuffer->getSize(), (uint32_t*)binaryBuffer->getData());
}

}
