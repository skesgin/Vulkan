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
#include "ImageObject.hpp"

namespace vkts
{

static VkBool32 imageObjectUpload(const IDeviceMemorySP& deviceMemory, const IImageDataSP& imageData, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout)
{
    if (!imageData.get())
    {
        return VK_FALSE;
    }

    VkResult result;

    if (deviceMemory->getMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        result = deviceMemory->mapMemory(0, VK_WHOLE_SIZE, 0);

        if (result != VK_SUCCESS)
        {
            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not map memory.");

            return VK_FALSE;
        }

        imageData->copy(deviceMemory->getMemory(), mipLevel, arrayLayer, subresourceLayout);

		if (!(deviceMemory->getMemoryPropertyFlags() & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		{
			result = deviceMemory->flushMappedMemoryRanges(0, VK_WHOLE_SIZE);

			if (result != VK_SUCCESS)
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not flush memory.");

				return VK_FALSE;
			}
		}

		deviceMemory->unmapMemory();
    }
    else
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

static VkBool32 imageObjectPrepare(IImageSP& image, IDeviceMemorySP& deviceMemory, const IContextObjectSP& contextObject, const ICommandBuffersSP& cmdBuffer, const VkImageCreateInfo& imageCreateInfo, const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout, const VkImageSubresourceRange& subresourceRange, const VkMemoryPropertyFlags memoryPropertyFlags)
{
    if (!cmdBuffer.get())
    {
        return VK_FALSE;
    }

    VkResult result;

    //

    image = imageCreate(contextObject->getDevice()->getDevice(), imageCreateInfo.flags, imageCreateInfo.imageType, imageCreateInfo.format, imageCreateInfo.extent, imageCreateInfo.mipLevels, imageCreateInfo.arrayLayers, imageCreateInfo.samples, imageCreateInfo.tiling, imageCreateInfo.usage, imageCreateInfo.sharingMode, imageCreateInfo.queueFamilyIndexCount, imageCreateInfo.pQueueFamilyIndices, imageCreateInfo.initialLayout, srcAccessMask);

    if (!image.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image.");

        return VK_FALSE;
    }

    //

    VkMemoryRequirements memoryRequirements;

    image->getImageMemoryRequirements(memoryRequirements);

    //

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    contextObject->getPhysicalDevice()->getPhysicalDeviceMemoryProperties(physicalDeviceMemoryProperties);

    deviceMemory = deviceMemoryCreate(contextObject->getDevice()->getDevice(), memoryRequirements, physicalDeviceMemoryProperties.memoryTypeCount, physicalDeviceMemoryProperties.memoryTypes, memoryPropertyFlags);

    if (!deviceMemory.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not allocate memory.");

        return VK_FALSE;
    }

    //

    result = vkBindImageMemory(contextObject->getDevice()->getDevice(), image->getImage(), deviceMemory->getDeviceMemory(), 0);

    if (result != VK_SUCCESS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not bind image memory.");

        return VK_FALSE;
    }

    //

    image->cmdPipelineBarrier(cmdBuffer->getCommandBuffer(), dstAccessMask, newLayout, subresourceRange);

    return VK_TRUE;
}

static VkBool32 imageObjectPrepare(IBufferSP& buffer, IDeviceMemorySP& deviceMemory, const IContextObjectSP& contextObject, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags)
{

    VkResult result;

    //

    buffer = bufferCreate(contextObject->getDevice()->getDevice(), bufferCreateInfo.flags, bufferCreateInfo.size, bufferCreateInfo.usage, bufferCreateInfo.sharingMode, bufferCreateInfo.queueFamilyIndexCount, bufferCreateInfo.pQueueFamilyIndices);

    if (!buffer.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create buffer.");

        return VK_FALSE;
    }

    //

    VkMemoryRequirements memoryRequirements;

    buffer->getBufferMemoryRequirements(memoryRequirements);

    //

    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    contextObject->getPhysicalDevice()->getPhysicalDeviceMemoryProperties(physicalDeviceMemoryProperties);

    deviceMemory = deviceMemoryCreate(contextObject->getDevice()->getDevice(), memoryRequirements, physicalDeviceMemoryProperties.memoryTypeCount, physicalDeviceMemoryProperties.memoryTypes, memoryPropertyFlags);

    if (!deviceMemory.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not allocate memory.");

        return VK_FALSE;
    }

    //

    result = vkBindBufferMemory(contextObject->getDevice()->getDevice(), buffer->getBuffer(), deviceMemory->getDeviceMemory(), 0);

    if (result != VK_SUCCESS)
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not bind buffer memory.");

        return VK_FALSE;
    }

    return VK_TRUE;
}

IImageObjectSP VKTS_APIENTRY imageObjectCreate(IImageSP& stageImage, IBufferSP& stageBuffer, IDeviceMemorySP& stageDeviceMemory, const IContextObjectSP& contextObject, const ICommandBuffersSP& cmdBuffer, const std::string& name, const IImageDataSP& imageData, const VkImageCreateInfo& imageCreateInfo, const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout, const VkImageSubresourceRange& subresourceRange, const VkMemoryPropertyFlags memoryPropertyFlags)
{
    if (!cmdBuffer || !imageData.get())
    {
        return IImageObjectSP();
    }

    VkResult result;

    //

    IImageSP image;

    IDeviceMemorySP deviceMemory;

    //

    if (!imageObjectPrepare(image, deviceMemory, contextObject, cmdBuffer, imageCreateInfo, srcAccessMask, dstAccessMask, newLayout, subresourceRange, memoryPropertyFlags))
    {
        return IImageObjectSP();
    }

    //

    if (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        for (uint32_t arrayLayer = 0; arrayLayer < imageData->getArrayLayers(); arrayLayer++)
        {
			for (uint32_t mipLevel = 0; mipLevel < imageData->getMipLevels(); mipLevel++)
			{
				VkImageSubresource imageSubresource;

				imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageSubresource.mipLevel = mipLevel;
				imageSubresource.arrayLayer = arrayLayer;

				VkSubresourceLayout subresourceLayout;

				image->getImageSubresourceLayout(subresourceLayout, imageSubresource);

				if (!imageObjectUpload(deviceMemory, imageData, mipLevel, arrayLayer, subresourceLayout))
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload image data.");

					return IImageObjectSP();
				}
            }
        }
    }
    else
    {
        if (contextObject->getPhysicalDevice()->isImageTilingAvailable(VK_IMAGE_TILING_LINEAR, imageData->getFormat(), imageData->getImageType(), imageCreateInfo.flags, imageData->getExtent3D(), imageData->getMipLevels(), imageData->getArrayLayers(), imageCreateInfo.samples, imageData->getSize()))
        {
            VkImageCreateInfo stageImageCreateInfo(imageCreateInfo);

            stageImageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
            stageImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            stageImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

            if (!imageObjectPrepare(stageImage, stageDeviceMemory, contextObject, cmdBuffer, stageImageCreateInfo, VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, subresourceRange, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not prepare staging image.");

                return IImageObjectSP();
            }

            for (uint32_t arrayLayer = 0; arrayLayer < imageData->getArrayLayers(); arrayLayer++)
            {
				for (uint32_t mipLevel = 0; mipLevel < imageData->getMipLevels(); mipLevel++)
				{
					VkImageSubresource imageSubresource;
					imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					imageSubresource.mipLevel = mipLevel;
					imageSubresource.arrayLayer = arrayLayer;

					VkSubresourceLayout subresourceLayout;

					stageImage->getImageSubresourceLayout(subresourceLayout, imageSubresource);

					if (!imageObjectUpload(stageDeviceMemory, imageData, mipLevel, arrayLayer, subresourceLayout))
					{
						logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not upload image data.");

						return IImageObjectSP();
					}

					VkImageCopy imageCopy;

					imageCopy.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, mipLevel, arrayLayer, 1};
					imageCopy.srcOffset = {0, 0, 0};
					imageCopy.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, mipLevel, arrayLayer, 1};
					imageCopy.dstOffset = {0, 0, 0};
					imageCopy.extent = {glm::max(imageData->getWidth() >> mipLevel, 1u), glm::max(imageData->getHeight() >> mipLevel, 1u), glm::max(imageData->getDepth() >> mipLevel, 1u)};

					stageImage->copyImage(cmdBuffer->getCommandBuffer(), image, imageCopy);
				}
        	}
        }
        else
        {
            VkBufferCreateInfo bufferCreateInfo{};

            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = imageData->getSize();
            bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
            bufferCreateInfo.flags = 0;
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            bufferCreateInfo.queueFamilyIndexCount = 0;
            bufferCreateInfo.pQueueFamilyIndices = nullptr;

            if (!imageObjectPrepare(stageBuffer, stageDeviceMemory, contextObject, bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not prepare staging buffer.");

                return IImageObjectSP();
            }

            // Copy image data into buffer.

            result = stageDeviceMemory->upload(0, 0, imageData->getData(), imageData->getSize());

            if (result != VK_SUCCESS)
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not copy data to stage buffer.");

                return IImageObjectSP();
            }

            // Upload from buffer into image.

            for (uint32_t arrayLayer = 0; arrayLayer < imageData->getArrayLayers(); arrayLayer++)
            {
				for (uint32_t mipLevel = 0; mipLevel < imageData->getMipLevels(); mipLevel++)
				{
					VkExtent3D currentExtent;
					uint32_t currentOffset;
					if (!imageData->getExtentAndOffset(currentExtent, currentOffset, mipLevel, arrayLayer))
					{
						return IImageObjectSP();
					}

					VkBufferImageCopy bufferImageCopy;

					bufferImageCopy.bufferOffset = currentOffset;
					bufferImageCopy.bufferRowLength = 0;	// Zero means tightly packed.
					bufferImageCopy.bufferImageHeight = 0;
					bufferImageCopy.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, mipLevel, arrayLayer, 1};
					bufferImageCopy.imageOffset = {0, 0, 0};
					bufferImageCopy.imageExtent = {glm::max(imageData->getWidth() >> mipLevel, 1u), glm::max(imageData->getHeight() >> mipLevel, 1u), glm::max(imageData->getDepth() >> mipLevel, 1u)};

					stageBuffer->copyBufferToImage(cmdBuffer->getCommandBuffer(), image, bufferImageCopy);
				}
            }
        }
    }

    //

    VkImageViewCreateInfo imageViewCreateInfo{};

    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    imageViewCreateInfo.flags = 0;
    imageViewCreateInfo.image = image->getImage();
    imageViewCreateInfo.viewType = image->getArrayLayers() == 6 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = image->getFormat();
    imageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    imageViewCreateInfo.subresourceRange = subresourceRange;

    auto imageView = imageViewCreate(contextObject->getDevice()->getDevice(), imageViewCreateInfo.flags, imageViewCreateInfo.image, imageViewCreateInfo.viewType, imageViewCreateInfo.format, imageViewCreateInfo.components, imageViewCreateInfo.subresourceRange);

    if (!imageView.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image view.");

        return IImageObjectSP();
    }

    //

    auto newInstance = new ImageObject(contextObject, name, imageData, image, imageView, deviceMemory);

    if (!newInstance)
    {
        newInstance->destroy();

        return IImageObjectSP();
    }

    return IImageObjectSP(newInstance);
}

IImageObjectSP VKTS_APIENTRY imageObjectCreate(const IContextObjectSP& contextObject, const ICommandBuffersSP& cmdBuffer, const std::string& name, const VkImageCreateInfo& imageCreateInfo, const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout, const VkImageSubresourceRange& subresourceRange, const VkMemoryPropertyFlags memoryPropertyFlags)
{
    if (!cmdBuffer.get())
    {
        return IImageObjectSP();
    }

    IImageSP image;

    IDeviceMemorySP deviceMemory;

    //

    if (!imageObjectPrepare(image, deviceMemory, contextObject, cmdBuffer, imageCreateInfo, srcAccessMask, dstAccessMask, newLayout, subresourceRange, memoryPropertyFlags))
    {
        return IImageObjectSP();
    }

    //

    VkImageViewCreateInfo imageViewCreateInfo{};

    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    imageViewCreateInfo.flags = 0;
    imageViewCreateInfo.image = image->getImage();
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = image->getFormat();
    imageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    imageViewCreateInfo.subresourceRange = subresourceRange;

    auto imageView = imageViewCreate(contextObject->getDevice()->getDevice(), imageViewCreateInfo.flags, imageViewCreateInfo.image, imageViewCreateInfo.viewType, imageViewCreateInfo.format, imageViewCreateInfo.components, imageViewCreateInfo.subresourceRange);

    if (!imageView.get())
    {
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create image view.");

        return IImageObjectSP();
    }

    //

    auto newInstance = new ImageObject(contextObject, name, IImageDataSP(), image, imageView, deviceMemory);

    if (!newInstance)
    {
        newInstance->destroy();

        return IImageObjectSP();
    }

    return IImageObjectSP(newInstance);
}

}
