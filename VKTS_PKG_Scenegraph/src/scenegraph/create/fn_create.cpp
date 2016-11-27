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
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

    //

    return textureObjectCreate(assetManager->getContextObject(), textureObjectName, mipmap, imageObject, samplerCreateInfo);
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

        logPrint(VKTS_LOG_DEBUG, __FILE__, __LINE__, "Create image '%s'", imageDataName);

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

    //

    auto textureObject = textureObjectCreate(assetManager->getContextObject(), textureObjectName, VK_FALSE, imageObject, samplerCreateInfo);

    if (!textureObject.get())
    {
        return ITextureObjectSP();
    }

    assetManager->addTextureObject(textureObject);

    return textureObject;
}

IBufferObjectSP VKTS_APIENTRY createUniformBufferObject(const IAssetManagerSP& assetManager, const VkDeviceSize size)
{
    VkBufferCreateInfo bufferCreateInfo{};

    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;

    return bufferObjectCreate(assetManager->getContextObject(), bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
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

VkBool32 VKTS_APIENTRY createSubMeshDescriptorSetLayout(const IAssetManagerSP& assetManager, const ISubMeshSP& subMesh)
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[VKTS_BINDING_UNIFORM_MATERIAL_TOTAL_BINDING_COUNT]{};

	uint32_t bindingCount = 0;

	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION;
	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

	bindingCount++;

	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM;
	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

	bindingCount++;

	if ((subMesh->getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_BONES) == VKTS_VERTEX_BUFFER_TYPE_BONES)
	{
    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;
	}


	// For forward rendering, add more buffers and textureObjects.
	if (subMesh->getBSDFMaterial()->getForwardRendering())
	{
    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_LIGHT;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;

    	descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_BUFFER_BSDF_INVERSE;
    	descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    	descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
    	descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    	descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

    	bindingCount++;

    	//

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_DIFFUSE;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_SPECULAR;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;

		descriptorSetLayoutBinding[bindingCount].binding = VKTS_BINDING_UNIFORM_SAMPLER_BSDF_LUT;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;
	}

	if (subMesh->getBSDFMaterial()->getNumberTextureObjects() > VKTS_BINDING_UNIFORM_BSDF_BINDING_COUNT)
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Too many textureObjects.");

		return VK_FALSE;
	}

    for (size_t i = 0; i < subMesh->getBSDFMaterial()->getNumberTextureObjects(); i++)
    {
    	uint32_t bindingStart = subMesh->getBSDFMaterial()->getForwardRendering() ? VKTS_BINDING_UNIFORM_SAMPLER_BSDF_FORWARD_FIRST : VKTS_BINDING_UNIFORM_SAMPLER_BSDF_DEFERRED_FIRST;

		descriptorSetLayoutBinding[bindingCount].binding = bindingStart + (uint32_t)i;
		descriptorSetLayoutBinding[bindingCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding[bindingCount].descriptorCount = 1;
		descriptorSetLayoutBinding[bindingCount].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptorSetLayoutBinding[bindingCount].pImmutableSamplers = nullptr;

		bindingCount++;
    }

    //

    auto descriptorSetLayout = descriptorSetLayoutCreate(assetManager->getContextObject()->getDevice()->getDevice(), 0, bindingCount, descriptorSetLayoutBinding);

	if (!descriptorSetLayout.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

		return VK_FALSE;
	}

	subMesh->setDescriptorSetLayout(descriptorSetLayout);

	return VK_TRUE;
}

VkBool32 VKTS_APIENTRY createPhongMaterialDescriptorSets(const IAssetManagerSP& assetManager, const IPhongMaterialSP& phongMaterial)
{
    // Create all possibilities, even when not used.

    VkDescriptorPoolSize descriptorPoolSize[3]{};

	descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSize[0].descriptorCount = VKTS_BINDING_UNIFORM_BUFFER_COUNT;

	descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize[1].descriptorCount = VKTS_BINDING_UNIFORM_PHONG_BINDING_COUNT + 2;

	descriptorPoolSize[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	descriptorPoolSize[2].descriptorCount = VKTS_BINDING_UNIFORM_VOXEL_COUNT;

    auto descriptorPool = descriptorPoolCreate(assetManager->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 3, descriptorPoolSize);

    if (!descriptorPool.get())
    {
    	return VK_FALSE;
    }

    phongMaterial->setDescriptorPool(descriptorPool);

    //

    auto allDescriptorSetLayouts = assetManager->getDescriptorSetLayout()->getDescriptorSetLayout();

    auto descriptorSets = descriptorSetsCreate(assetManager->getContextObject()->getDevice()->getDevice(), descriptorPool->getDescriptorPool(), 1, &allDescriptorSetLayouts);

    if (!descriptorSets.get())
    {
    	return VK_FALSE;
    }

    phongMaterial->setDescriptorSets(descriptorSets);

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY createBSDFMaterialDescriptorSets(const IAssetManagerSP& assetManager, const IBSDFMaterialSP& bsdfMaterial, const IDescriptorSetLayoutSP& descriptorSetLayout)
{
    // Create all possibilities, even when not used.

    VkDescriptorPoolSize descriptorPoolSize[2]{};

	descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorPoolSize[0].descriptorCount = 5;

	descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorPoolSize[1].descriptorCount = 18;

    auto descriptorPool = descriptorPoolCreate(assetManager->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 2, descriptorPoolSize);

    if (!descriptorPool.get())
    {
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor pool.");

        return VK_FALSE;
    }

    bsdfMaterial->setDescriptorPool(descriptorPool);

	//

	const auto allDescriptorSetLayouts = descriptorSetLayout->getDescriptorSetLayout();

    auto descriptorSets = descriptorSetsCreate(assetManager->getContextObject()->getDevice()->getDevice(), bsdfMaterial->getDescriptorPool()->getDescriptorPool(), 1, &allDescriptorSetLayouts);

    if (!descriptorSets.get())
    {
    	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor sets.");

        return VK_FALSE;
    }

    bsdfMaterial->setDescriptorSets(descriptorSets);

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY createSubMeshGraphicsPipeline(const IAssetManagerSP& assetManager, const ISubMeshSP& subMesh)
{
	VkDescriptorSetLayout setLayouts[1];

	setLayouts[0] = subMesh->getDescriptorSetLayout()->getDescriptorSetLayout();

	auto pipelineLayout = pipelineCreateLayout(assetManager->getContextObject()->getDevice()->getDevice(), 0, 1, setLayouts, 0, nullptr);

	if (!pipelineLayout.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

		return VK_FALSE;
	}

	subMesh->setPipelineLayout(pipelineLayout);

	// Create graphics pipeline for this sub mesh.

	auto currentRenderPass = assetManager->getRenderPass();

	if (!currentRenderPass.get())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No renderpass found");

        return VK_FALSE;
	}

	auto currentPipelineLayout = subMesh->getPipelineLayout();

	if (!currentPipelineLayout.get())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No pipeline layout found");

        return VK_FALSE;
	}


	auto vertexBufferType = subMesh->getVertexBufferType() & subMesh->getBSDFMaterial()->getAttributes();

	if (vertexBufferType != subMesh->getBSDFMaterial()->getAttributes())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh vertex buffer type does not match with material");

        return VK_FALSE;
	}

	if ((subMesh->getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_BONES) == VKTS_VERTEX_BUFFER_TYPE_BONES)
	{
		vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_BONES;
	}

	auto currentVertexShaderModule = assetManager->useVertexShaderModule(vertexBufferType);

	if (!currentVertexShaderModule.get())
	{
        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No shader module found");

		return VK_FALSE;
	}

	DefaultGraphicsPipeline gp;

	gp.getPipelineShaderStageCreateInfo(0).stage = VK_SHADER_STAGE_VERTEX_BIT;
	gp.getPipelineShaderStageCreateInfo(0).module = currentVertexShaderModule->getShaderModule();

	gp.getPipelineShaderStageCreateInfo(1).stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	gp.getPipelineShaderStageCreateInfo(1).module = subMesh->getBSDFMaterial()->getFragmentShader()->getShaderModule();


	gp.getVertexInputBindingDescription(0).binding = 0;
	gp.getVertexInputBindingDescription(0).stride = alignmentGetStrideInBytes(subMesh->getVertexBufferType());
	gp.getVertexInputBindingDescription(0).inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


	uint32_t location = 0;

	gp.getVertexInputAttributeDescription(location).location = location;
	gp.getVertexInputAttributeDescription(location).binding = 0;
	gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
	gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_VERTEX, subMesh->getVertexBufferType());

	if ((vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_NORMAL) == VKTS_VERTEX_BUFFER_TYPE_NORMAL)
	{
		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_NORMAL, subMesh->getVertexBufferType());

		if ((vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TANGENTS) == VKTS_VERTEX_BUFFER_TYPE_TANGENTS)
		{
			location++;

			gp.getVertexInputAttributeDescription(location).location = location;
			gp.getVertexInputAttributeDescription(location).binding = 0;
			gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32_SFLOAT;
			gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BITANGENT, subMesh->getVertexBufferType());

			location++;

			gp.getVertexInputAttributeDescription(location).location = location;
			gp.getVertexInputAttributeDescription(location).binding = 0;
			gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32_SFLOAT;
			gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_TANGENT, subMesh->getVertexBufferType());
		}
	}

	if ((vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD) == VKTS_VERTEX_BUFFER_TYPE_TEXCOORD)
	{
		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_TEXCOORD, subMesh->getVertexBufferType());
	}


	if ((vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONES) == VKTS_VERTEX_BUFFER_TYPE_BONES)
	{
		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0, subMesh->getVertexBufferType());

		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1, subMesh->getVertexBufferType());

		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0, subMesh->getVertexBufferType());

		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32G32B32A32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1, subMesh->getVertexBufferType());

		location++;

		gp.getVertexInputAttributeDescription(location).location = location;
		gp.getVertexInputAttributeDescription(location).binding = 0;
		gp.getVertexInputAttributeDescription(location).format = VK_FORMAT_R32_SFLOAT;
		gp.getVertexInputAttributeDescription(location).offset = alignmentGetOffsetInBytes(VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS, subMesh->getVertexBufferType());
	}

	//

	gp.getPipelineInputAssemblyStateCreateInfo().topology = subMesh->getPrimitiveTopology();

	gp.getViewports(0).x = 0.0f;
	gp.getViewports(0).y = 0.0f;
	gp.getViewports(0).width = 1.0f;
	gp.getViewports(0).height = 1.0f;
	gp.getViewports(0).minDepth = 0.0f;
	gp.getViewports(0).maxDepth = 1.0f;


	gp.getScissors(0).offset.x = 0;
	gp.getScissors(0).offset.y = 0;
	gp.getScissors(0).extent = {1, 1};


	gp.getPipelineMultisampleStateCreateInfo();

	gp.getPipelineDepthStencilStateCreateInfo().depthTestEnable = VK_TRUE;
	gp.getPipelineDepthStencilStateCreateInfo().depthWriteEnable = VK_TRUE;
	gp.getPipelineDepthStencilStateCreateInfo().depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

	for (uint32_t i = 0; i < 3; i++)
	{
		gp.getPipelineColorBlendAttachmentState(i).blendEnable = VK_FALSE;
		gp.getPipelineColorBlendAttachmentState(i).colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    	// For forward rendering, only one color buffer is attached.
    	if (subMesh->getBSDFMaterial()->getForwardRendering())
    	{
    		break;
    	}
	}

	gp.getDynamicState(0) = VK_DYNAMIC_STATE_VIEWPORT;
	gp.getDynamicState(1) = VK_DYNAMIC_STATE_SCISSOR;


	gp.getGraphicsPipelineCreateInfo().layout = currentPipelineLayout->getPipelineLayout();
	gp.getGraphicsPipelineCreateInfo().renderPass = currentRenderPass->getRenderPass();


	auto pipeline = pipelineCreateGraphics(assetManager->getContextObject()->getDevice()->getDevice(), VK_NULL_HANDLE, gp.getGraphicsPipelineCreateInfo(), vertexBufferType);

	if (!pipeline.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create graphics pipeline.");

		return VK_FALSE;
	}

	subMesh->setGraphicsPipeline(pipeline);

	return VK_TRUE;
}

}
