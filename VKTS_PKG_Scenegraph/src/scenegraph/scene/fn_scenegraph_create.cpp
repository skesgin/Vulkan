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

#include <vkts/scenegraph.hpp>

#include "fn_scenegraph_internal.hpp"

namespace vkts
{

ITextureObjectSP VKTS_APIENTRY scenegraphCreateTextureObject(const IContextSP& context, const glm::vec4& color, const VkFormat format)
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

    auto imageData = context->useImageData(imageDataName);

    if (!imageData.get())
    {
        imageData = imageDataCreate(imageDataName, 1, 1, 1, color, VK_IMAGE_TYPE_2D, format);

        logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Create image '%s'", imageDataName);

        if (!imageData.get())
        {
            return ITextureObjectSP();
        }

        context->addImageData(imageData);
    }

    //

    VkImageTiling imageTiling;
    VkMemoryPropertyFlags memoryPropertyFlags;

    if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
    {
        if (format == VK_FORMAT_R8G8B8_UNORM)
        {
            context->removeImageData(imageData);

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

            auto convertedImageData = context->useImageData(imageDataName);

            if (!convertedImageData.get())
            {
                convertedImageData = imageDataConvert(imageData, VK_FORMAT_R8G8B8A8_UNORM, imageDataName);

                if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, convertedImageData->getFormat(), convertedImageData->getImageType(), 0, convertedImageData->getExtent3D(), convertedImageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, convertedImageData->getSize()))
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                    return ITextureObjectSP();
                }

                context->addImageData(convertedImageData);
            }
            else
            {
                if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, convertedImageData->getFormat(),convertedImageData->getImageType(), 0, convertedImageData->getExtent3D(), convertedImageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, convertedImageData->getSize()))
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

    auto imageObject = imageObjectCreate(stageImage, stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), imageObjectName, imageData, imageCreateInfo, srcAccessMask, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange, memoryPropertyFlags);

    context->addStageImage(stageImage);
    context->addStageBuffer(stageBuffer);
    context->addStageDeviceMemory(stageDeviceMemory);

    if (!imageObject.get())
    {
        return ITextureObjectSP();
    }

    context->addImageObject(imageObject);

    //

    auto textureObject = textureObjectCreate(context->getContextObject(), textureObjectName, VK_FALSE, imageObject, context->getSamplerCreateInfo());

    if (!textureObject.get())
    {
        return ITextureObjectSP();
    }

    context->addTextureObject(textureObject);

    return textureObject;
}

IBufferObjectSP VKTS_APIENTRY scenegraphCreateUniformBufferObject(const IContextSP& context, const VkDeviceSize size)
{
    VkBufferCreateInfo bufferCreateInfo{};

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    return bufferObjectCreate(context->getContextObject(), bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

IBufferObjectSP VKTS_APIENTRY scenegraphCreateIndexBufferObject(const IContextSP& context, const IBinaryBufferSP& binaryBuffer)
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

    auto indexBufferObject = bufferObjectCreate(stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), binaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    context->addStageBuffer(stageBuffer);
    context->addStageDeviceMemory(stageDeviceMemory);

    return indexBufferObject;
}

IBufferObjectSP VKTS_APIENTRY scenegraphCreateVertexBufferObject(const IContextSP& context, const IBinaryBufferSP& binaryBuffer)
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

    auto vertexBufferObject = bufferObjectCreate(stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), binaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    context->addStageBuffer(stageBuffer);
    context->addStageDeviceMemory(stageDeviceMemory);

    return vertexBufferObject;
}

}
