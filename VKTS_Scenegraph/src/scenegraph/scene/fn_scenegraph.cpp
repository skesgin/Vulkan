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

#include "Animation.hpp"
#include "BSDFMaterial.hpp"
#include "Camera.hpp"
#include "Channel.hpp"
#include "Context.hpp"
#include "CopyConstraint.hpp"
#include "Light.hpp"
#include "LimitConstraint.hpp"
#include "Marker.hpp"
#include "Mesh.hpp"
#include "Node.hpp"
#include "Object.hpp"
#include "ParticleSystem.hpp"
#include "PhongMaterial.hpp"
#include "Scene.hpp"
#include "SubMesh.hpp"

#include "fn_scenegraph_internal.hpp"

namespace vkts
{

static ITextureObjectSP scenegraphCreateTextureObject(const float red, const float green, const float blue, const VkFormat format, const IContextSP& context)
{
    char imageDataName[VKTS_MAX_BUFFER_CHARS + 1];
    char textureObjectName[VKTS_MAX_BUFFER_CHARS + 1];

    //

    if (format == VK_FORMAT_R8_UNORM)
    {
        sprintf(imageDataName, "image_%1.3f_unorm.tga", red);

        sprintf(textureObjectName, "textureObject_%1.3f_unorm", red);
    }
    else if (format == VK_FORMAT_R8G8_UNORM)
    {
        sprintf(imageDataName, "image_%1.3f_%1.3f_unorm.tga", red, green);

        sprintf(textureObjectName, "textureObject_%1.3f_%1.3f_unorm", red, green);
    }
    else if (format == VK_FORMAT_R8G8B8_UNORM)
    {
        sprintf(imageDataName, "image_%1.3f_%1.3f_%1.3f_unorm.tga", red, green, blue);

        sprintf(textureObjectName, "textureObject_%1.3f_%1.3f_%1.3f_unorm", red, green, blue);
    }
    else if (format == VK_FORMAT_R8G8B8A8_UNORM)
    {
        sprintf(imageDataName, "image_%1.3f_%1.3f_%1.3f_unorm.tga", red, green, blue);

        sprintf(textureObjectName, "textureObject_%1.3f_%1.3f_%1.3f_unorm", red, green, blue);
    }
    else
    {
        return ITextureObjectSP();
    }

    //

    auto imageData = context->useImageData(imageDataName);

    if (!imageData.get())
    {
        imageData = imageDataCreate(imageDataName, 1, 1, 1, red, green, blue, 0.0f, VK_IMAGE_TYPE_2D, format);

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

            sprintf(imageDataName, "image_%1.3f_%1.3f_%1.3f_1.0_unorm.tga", red, green, blue);

            sprintf(textureObjectName, "textureObject_%1.3f_%1.3f_%1.3f_1.0_unorm", red, green, blue);

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

    auto imageObject = imageObjectCreate(stageImage, stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), textureObjectName, imageData, imageCreateInfo, srcAccessMask, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange, memoryPropertyFlags);

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

static VkBool32 scenegraphLoadImages(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    VkBool32 bdata;

    std::string imageObjectName;
    VkBool32 mipMap = VK_FALSE;
    VkBool32 environment = VK_FALSE;
    VkBool32 preFiltered = VK_FALSE;
    IImageDataSP imageData;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }
        if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid name");

                return VK_FALSE;
            }

            imageObjectName = sdata;

            mipMap = VK_FALSE;
            environment = VK_FALSE;
            preFiltered = VK_FALSE;
        }
        else if (scenegraphParseIsToken(buffer, "mipmap"))
        {
            if (!scenegraphParseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            mipMap = bdata;
        }
        else if (scenegraphParseIsToken(buffer, "environment"))
        {
            if (!scenegraphParseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            environment = bdata;
        }
        else if (scenegraphParseIsToken(buffer, "pre_filtered"))
        {
            if (!scenegraphParseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            preFiltered = bdata;
        }
        else if (scenegraphParseIsToken(buffer, "image_data"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (mipMap && environment)
            {
            	return VK_FALSE;
            }

            if (preFiltered && !environment)
            {
            	return VK_FALSE;
            }

            //

            std::string finalImageDataFilename = std::string(directory) + std::string(sdata);

            imageData = context->useImageData(finalImageDataFilename.c_str());

            if (!imageData.get())
            {
                imageData = context->useImageData(sdata);

                if (!imageData.get())
                {
                    imageData = imageDataLoad(finalImageDataFilename.c_str());

                    if (!imageData.get())
                    {
                        imageData = imageDataLoad(sdata);

                        if (!imageData.get())
                        {
                        	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load image data '%s'", finalImageDataFilename.c_str());

                            return VK_FALSE;
                        }
                    }

                    //

                    VkImageTiling imageTiling;
                    VkMemoryPropertyFlags memoryPropertyFlags;

                    if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
                    {
                        if (imageData->getFormat() == VK_FORMAT_R8G8B8_UNORM)
                        {
                            imageData = imageDataConvert(imageData, VK_FORMAT_R8G8B8A8_UNORM, imageData->getName());

                            if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
                            {
                                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                                return VK_FALSE;
                            }
                        }
                        else
                        {
                            logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                            return VK_FALSE;
                        }
                    }

                    if (mipMap && imageData->getMipLevels() == 1 && (imageData->getExtent3D().width > 1 || imageData->getExtent3D().height > 1 || imageData->getExtent3D().depth > 1))
                    {
                        // Mipmaping image creation.

                        auto dotIndex = finalImageDataFilename.rfind(".");

                        if (dotIndex == finalImageDataFilename.npos)
                        {
                        	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No valid image filename '%s'", finalImageDataFilename.c_str());

                            return VK_FALSE;
                        }

                        auto sourceImageName = finalImageDataFilename.substr(0, dotIndex);
                        auto sourceImageExtension = finalImageDataFilename.substr(dotIndex);

                        int32_t width = imageData->getWidth();
                        int32_t height = imageData->getHeight();
                        int32_t depth = imageData->getDepth();

                        SmartPointerVector<IImageDataSP> allMipMaps;

                        if (cacheGetEnabled())
                        {
                            allMipMaps.append(imageData);

                            int32_t level = 1;

                            while (width > 1 || height > 1 || depth > 1)
							{
								width = glm::max(width / 2, 1);
								height = glm::max(height / 2, 1);
								depth = glm::max(depth / 2, 1);

								auto targetImageFilename = sourceImageName + "_LEVEL" + std::to_string(level++) + sourceImageExtension;

								auto targetImage = cacheLoadImageData(targetImageFilename.c_str());

								if (!targetImage.get())
								{
									allMipMaps.clear();

									break;
								}

								allMipMaps.append(targetImage);

								//

								width = targetImage->getWidth();
								height = targetImage->getHeight();
								depth = targetImage->getDepth();
							}
                        }

                        //

                        if (allMipMaps.size() == 0)
                        {
                        	allMipMaps = imageDataMipmap(imageData, VK_FALSE, finalImageDataFilename);

                            if (allMipMaps.size() == 0)
                            {
                            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create mip maps for '%s'", finalImageDataFilename.c_str());

                                return VK_FALSE;
                            }

                            if (cacheGetEnabled())
                            {
								logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Storing cached data for '%s'", finalImageDataFilename.c_str());

								// Only cache mip maps sub levels.
								for (size_t i = 1; i < allMipMaps.size(); i++)
								{
									cacheSaveImageData(allMipMaps[i]);
								}
                            }
                        }
                        else
                        {
                        	logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Using cached data for '%s'", finalImageDataFilename.c_str());
                        }

                        imageData = imageDataMerge(allMipMaps, finalImageDataFilename, (uint32_t)allMipMaps.size(), 1);

                        if (!imageData.get())
                        {
                        	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No merged image for '%s'", finalImageDataFilename.c_str());

                            return VK_FALSE;
                        }
                    }
                    else if (environment)
                    {
                		// Cube map image creation.

						auto dotIndex = finalImageDataFilename.rfind(".");

						if (dotIndex == finalImageDataFilename.npos)
						{
							logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No valid image filename '%s'", finalImageDataFilename.c_str());

							return VK_FALSE;
						}

						auto sourceImageName = finalImageDataFilename.substr(0, dotIndex);
						auto sourceImageExtension = finalImageDataFilename.substr(dotIndex);

                    	if (imageData->getArrayLayers() != 6)
                    	{
							SmartPointerVector<IImageDataSP> allCubeMaps;

							if (cacheGetEnabled())
							{
								for (uint32_t layer = 0; layer < 6; layer++)
								{
									auto targetImageFilename = sourceImageName + "_LAYER" + std::to_string(layer) + sourceImageExtension;

									auto targetImage = cacheLoadImageData(targetImageFilename.c_str());

									if (!targetImage.get())
									{
										allCubeMaps.clear();

										break;
									}

									allCubeMaps.append(targetImage);
								}
							}

							//

							if (allCubeMaps.size() == 0)
							{
								allCubeMaps = imageDataCubemap(imageData, (uint32_t)imageData->getHeight() / 2, finalImageDataFilename);

								if (allCubeMaps.size() == 0)
								{
									logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create cube maps for '%s'", finalImageDataFilename.c_str());

									return VK_FALSE;
								}

								if (cacheGetEnabled())
								{
									logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Storing cached data for '%s'", finalImageDataFilename.c_str());

									for (size_t i = 0; i < allCubeMaps.size(); i++)
									{
										cacheSaveImageData(allCubeMaps[i]);
									}
								}
							}
							else
							{
								logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Using cached data for '%s'", finalImageDataFilename.c_str());
							}

							imageData = imageDataMerge(allCubeMaps, finalImageDataFilename, 1, (uint32_t)allCubeMaps.size());

							if (!imageData.get())
							{
								logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No merged image for '%s'", finalImageDataFilename.c_str());

								return VK_FALSE;
							}
                    	}

                        if (preFiltered)
                        {
                            if (imageObjectName == "")
                            {
                                return VK_FALSE;
                            }

                        	// Pre-filtered diffuse cube map.

                        	SmartPointerVector<IImageDataSP> allDiffuseCubeMaps;

                            if (cacheGetEnabled())
                            {
                                for (uint32_t layer = 0; layer < 6; layer++)
    							{
    								auto targetImageFilename = sourceImageName + "_LEVEL0_LAYER" + std::to_string(layer) + "_LAMBERT" + sourceImageExtension;

    								auto targetImage = cacheLoadImageData(targetImageFilename.c_str());

    								if (!targetImage.get())
    								{
    									allDiffuseCubeMaps.clear();

    									break;
    								}

    								allDiffuseCubeMaps.append(targetImage);
    							}
                            }

                            if (allDiffuseCubeMaps.size() == 0)
                            {
                            	allDiffuseCubeMaps = imageDataPrefilterLambert(imageData, VKTS_BSDF_M_CUBE_MAP, finalImageDataFilename);

                                if (allDiffuseCubeMaps.size() == 0)
                                {
                                	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create diffuse cube maps for '%s'", finalImageDataFilename.c_str());

                                    return VK_FALSE;
                                }

                                if (cacheGetEnabled())
                                {
    								logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Storing cached data for '%s'", finalImageDataFilename.c_str());

    								for (size_t i = 0; i < allDiffuseCubeMaps.size(); i++)
    								{
    									cacheSaveImageData(allDiffuseCubeMaps[i]);
    								}
                                }
                            }
                            else
                            {
                            	logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Using cached data for '%s'", finalImageDataFilename.c_str());
                            }

                            auto diffuseImageData = imageDataMerge(allDiffuseCubeMaps, finalImageDataFilename, 1, (uint32_t)allDiffuseCubeMaps.size());

                            if (!diffuseImageData.get())
                            {
                            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No merged image for '%s'", finalImageDataFilename.c_str());

                                return VK_FALSE;
                            }

                            context->addImageData(diffuseImageData);

                            //

                            VkImageTiling imageTiling;
                            VkMemoryPropertyFlags memoryPropertyFlags;

                            if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, diffuseImageData->getFormat(), diffuseImageData->getImageType(), 0, diffuseImageData->getExtent3D(), diffuseImageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, diffuseImageData->getSize()))
                            {
                                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                                return VK_FALSE;
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
                            imageCreateInfo.imageType = diffuseImageData->getImageType();
                            imageCreateInfo.format = diffuseImageData->getFormat();
                            imageCreateInfo.extent = diffuseImageData->getExtent3D();
                            imageCreateInfo.mipLevels = diffuseImageData->getMipLevels();
                            imageCreateInfo.arrayLayers = diffuseImageData->getArrayLayers();
                            imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                            imageCreateInfo.tiling = imageTiling;
                            imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                            imageCreateInfo.queueFamilyIndexCount = 0;
                            imageCreateInfo.pQueueFamilyIndices = nullptr;
                            imageCreateInfo.initialLayout = initialLayout;

                            VkImageSubresourceRange subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, diffuseImageData->getMipLevels(), 0, diffuseImageData->getArrayLayers()};

                            IDeviceMemorySP stageDeviceMemory;
                            IImageSP stageImage;
                            IBufferSP stageBuffer;

                            auto imageObject = imageObjectCreate(stageImage, stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), imageObjectName + "_LAMBERT", diffuseImageData, imageCreateInfo, srcAccessMask, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange, memoryPropertyFlags);

                            context->addStageImage(stageImage);
                            context->addStageBuffer(stageBuffer);
                            context->addStageDeviceMemory(stageDeviceMemory);

                            if (!imageObject.get())
                            {
                            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No memory image for '%s'", finalImageDataFilename.c_str());

                                return VK_FALSE;
                            }

                            context->addImageObject(imageObject);

                            //
                            //

                        	// Pre-filtered cook torrance cube map.

                        	SmartPointerVector<IImageDataSP> allCookTorranceCubeMaps;

                        	uint32_t levelCount = 1;

                            if (cacheGetEnabled())
                            {
                                for (uint32_t layer = 0; layer < 6; layer++)
    							{
                                    for (uint32_t level = 0; level < levelCount; level++)
        							{
										auto targetImageFilename = sourceImageName + "_LEVEL" + std::to_string(level) + "_LAYER" + std::to_string(layer) + "_COOKTORRANCE" + sourceImageExtension;

										auto targetImage = cacheLoadImageData(targetImageFilename.c_str());

										if (!targetImage.get())
										{
											allCookTorranceCubeMaps.clear();

											break;
										}

										// Gather level count iterations by first image.
										if (layer == 0 && level == 0)
										{
											uint32_t testWidth = targetImage->getWidth();

											while (testWidth > 1)
											{
												testWidth /= 2;

												levelCount++;
											}
										}

										allCookTorranceCubeMaps.append(targetImage);
    								}
    							}
                            }

                            if (allCookTorranceCubeMaps.size() == 0)
                            {
                            	allCookTorranceCubeMaps = imageDataPrefilterCookTorrance(imageData, VKTS_BSDF_M_CUBE_MAP, finalImageDataFilename);

                                if (allCookTorranceCubeMaps.size() == 0)
                                {
                                	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create cook torrance cube maps for '%s'", finalImageDataFilename.c_str());

                                    return VK_FALSE;
                                }

                                levelCount = (uint32_t)allCookTorranceCubeMaps.size() / 6;

                                if (cacheGetEnabled())
                                {
    								logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Storing cached data for '%s'", finalImageDataFilename.c_str());

    								for (size_t i = 0; i < allCookTorranceCubeMaps.size(); i++)
    								{
    									cacheSaveImageData(allCookTorranceCubeMaps[i]);
    								}
                                }
                            }
                            else
                            {
                            	logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Using cached data for '%s'", finalImageDataFilename.c_str());
                            }

                            auto cookTorranceImageData = imageDataMerge(allCookTorranceCubeMaps, finalImageDataFilename, levelCount, 6);

                            if (!cookTorranceImageData.get())
                            {
                            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No merged image for '%s'", finalImageDataFilename.c_str());

                                return VK_FALSE;
                            }

                            context->addImageData(cookTorranceImageData);

                            //

                            if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, cookTorranceImageData->getFormat(), cookTorranceImageData->getImageType(), 0, cookTorranceImageData->getExtent3D(), cookTorranceImageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, cookTorranceImageData->getSize()))
                            {
                                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                                return VK_FALSE;
                            }


                            initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                            srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;

                            if (imageTiling == VK_IMAGE_TILING_OPTIMAL)
                            {
                            	initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                            	srcAccessMask = 0;
                            }

                            //

                            memset(&imageCreateInfo, 0, sizeof(VkImageCreateInfo));

                            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

                            imageCreateInfo.flags = environment ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
                            imageCreateInfo.imageType = cookTorranceImageData->getImageType();
                            imageCreateInfo.format = cookTorranceImageData->getFormat();
                            imageCreateInfo.extent = cookTorranceImageData->getExtent3D();
                            imageCreateInfo.mipLevels = cookTorranceImageData->getMipLevels();
                            imageCreateInfo.arrayLayers = cookTorranceImageData->getArrayLayers();
                            imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                            imageCreateInfo.tiling = imageTiling;
                            imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                            imageCreateInfo.queueFamilyIndexCount = 0;
                            imageCreateInfo.pQueueFamilyIndices = nullptr;
                            imageCreateInfo.initialLayout = initialLayout;

                            subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, cookTorranceImageData->getMipLevels(), 0, cookTorranceImageData->getArrayLayers()};

                            imageObject = imageObjectCreate(stageImage, stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), imageObjectName + "_COOKTORRANCE", cookTorranceImageData, imageCreateInfo, srcAccessMask, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange, memoryPropertyFlags);

                            context->addStageImage(stageImage);
                            context->addStageBuffer(stageBuffer);
                            context->addStageDeviceMemory(stageDeviceMemory);

                            if (!imageObject.get())
                            {
                            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No memory image for '%s'", finalImageDataFilename.c_str());

                                return VK_FALSE;
                            }

                            context->addImageObject(imageObject);

                            //
                            //

                        	// Generate BSDF environment look up table.

                            auto lutName = "BSDF_LUT_" + std::to_string(VKTS_BSDF_LENGTH) + "_" + std::to_string(VKTS_BSDF_M);

                        	auto targetImageFilename = "textureObject/" + lutName + ".data";

                        	IImageDataSP lut = imageDataLoadRaw(targetImageFilename.c_str(), VKTS_BSDF_LENGTH, VKTS_BSDF_LENGTH, VK_FORMAT_R32G32_SFLOAT);

                            if (!lut.get())
                            {
                            	lut = imageDataEnvironmentBRDF(VKTS_BSDF_LENGTH, VKTS_BSDF_M, "BSDF_LUT.data");

                                if (!lut.get())
                                {
                                	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not generate BSDF lut");

                                    return VK_FALSE;
                                }

                            	if (!imageDataSave(targetImageFilename.c_str(), lut))
                            	{
									logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not save BSDF lut");

									return VK_FALSE;
                                }

                                context->addImageData(lut);
                            }

                            if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, lut->getFormat(), lut->getImageType(), 0, lut->getExtent3D(), lut->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, lut->getSize()))
                            {
                                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                                return VK_FALSE;
                            }

                            initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                            srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;

                            if (imageTiling == VK_IMAGE_TILING_OPTIMAL)
                            {
                            	initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                            	srcAccessMask = 0;
                            }

                            //

                            memset(&imageCreateInfo, 0, sizeof(VkImageCreateInfo));

                            imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

                            imageCreateInfo.flags = 0;
                            imageCreateInfo.imageType = lut->getImageType();
                            imageCreateInfo.format = lut->getFormat();
                            imageCreateInfo.extent = lut->getExtent3D();
                            imageCreateInfo.mipLevels = lut->getMipLevels();
                            imageCreateInfo.arrayLayers = lut->getArrayLayers();
                            imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                            imageCreateInfo.tiling = imageTiling;
                            imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                            imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                            imageCreateInfo.queueFamilyIndexCount = 0;
                            imageCreateInfo.pQueueFamilyIndices = nullptr;
                            imageCreateInfo.initialLayout = initialLayout;

                            subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, lut->getMipLevels(), 0, lut->getArrayLayers()};

                            imageObject = imageObjectCreate(stageImage, stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), lutName, lut, imageCreateInfo, srcAccessMask, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange, memoryPropertyFlags);

                            context->addStageImage(stageImage);
                            context->addStageBuffer(stageBuffer);
                            context->addStageDeviceMemory(stageDeviceMemory);

                            if (!imageObject.get())
                            {
                                return VK_FALSE;
                            }

                            context->addImageObject(imageObject);
                        }
                    }

                    //

                    context->addImageData(imageData);
                }
            }

            //
            // TextureObject creation.
            //

            if (imageObjectName == "" || !imageData.get())
            {
            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No memory image name or image data for '%s'", finalImageDataFilename.c_str());

                return VK_FALSE;
            }

            //

            VkImageTiling imageTiling;
            VkMemoryPropertyFlags memoryPropertyFlags;

            if (!context->getContextObject()->getPhysicalDevice()->getGetImageTilingAndMemoryProperty(imageTiling, memoryPropertyFlags, imageData->getFormat(), imageData->getImageType(), 0, imageData->getExtent3D(), imageData->getMipLevels(), 1, VK_SAMPLE_COUNT_1_BIT, imageData->getSize()))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Format not supported.");

                return VK_FALSE;
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

            IDeviceMemorySP stageDeviceMemory;
            IImageSP stageImage;
            IBufferSP stageBuffer;

            auto imageObject = imageObjectCreate(stageImage, stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), imageObjectName, imageData, imageCreateInfo, srcAccessMask, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange, memoryPropertyFlags);

            context->addStageImage(stageImage);
            context->addStageBuffer(stageBuffer);
            context->addStageDeviceMemory(stageDeviceMemory);

            if (!imageObject.get())
            {
            	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No memory image for '%s'", finalImageDataFilename.c_str());

                return VK_FALSE;
            }

            context->addImageObject(imageObject);
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadTextureObjects(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    VkBool32 bdata;

    std::string textureObjectName;
    VkBool32 mipMap = VK_FALSE;
    VkBool32 environment = VK_FALSE;
    VkBool32 preFiltered = VK_FALSE;
    IImageObjectSP imageObject;

    ITextureObjectSP textureObject;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "image_library"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadImages(directory, sdata, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load images: '%s'", sdata);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObjectName = sdata;

            mipMap = VK_FALSE;
            environment = VK_FALSE;
            preFiltered = VK_FALSE;
        }
        else if (scenegraphParseIsToken(buffer, "mipmap"))
        {
            if (!scenegraphParseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            mipMap = bdata;
        }
        else if (scenegraphParseIsToken(buffer, "environment"))
        {
            if (!scenegraphParseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            environment = bdata;
        }
        else if (scenegraphParseIsToken(buffer, "pre_filtered"))
        {
            if (!scenegraphParseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            preFiltered = bdata;
        }
        else if (scenegraphParseIsToken(buffer, "image"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (mipMap && environment)
            {
            	return VK_FALSE;
            }

            if (!preFiltered && environment)
            {
            	return VK_FALSE;
            }

            //

            if (preFiltered && environment)
            {
                // The pre-filtered images are stored in the mip map layers.

            	mipMap = VK_TRUE;
            }

            imageObject = context->useImageObject(sdata);

			if (!imageObject.get())
			{
				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Memory image not found: '%s'", sdata);

				return VK_FALSE;
			}

            // TextureObject creation.

            if (textureObjectName == "")
            {
                return VK_FALSE;
            }

            auto textureObject = textureObjectCreate(context->getContextObject(), textureObjectName, mipMap, imageObject, context->getSamplerCreateInfo());

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            context->addTextureObject(textureObject);

            //

            if (preFiltered)
            {
            	auto lambertName = std::string(sdata) + "_LAMBERT";

                imageObject = context->useImageObject(lambertName);

    			if (!imageObject.get())
    			{
    				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Memory image not found: '%s'", lambertName.c_str());

    				return VK_FALSE;
    			}

                textureObject = textureObjectCreate(context->getContextObject(), textureObjectName + "_LAMBERT", mipMap, imageObject, context->getSamplerCreateInfo());

                if (!textureObject.get())
                {
                    return VK_FALSE;
                }

                context->addTextureObject(textureObject);

                //

            	auto cookTorranceName = std::string(sdata) + "_COOKTORRANCE";

                imageObject = context->useImageObject(cookTorranceName);

    			if (!imageObject.get())
    			{
    				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Memory image not found: '%s'", cookTorranceName.c_str());

    				return VK_FALSE;
    			}

                textureObject = textureObjectCreate(context->getContextObject(), textureObjectName + "_COOKTORRANCE", mipMap, imageObject, context->getSamplerCreateInfo());

                if (!textureObject.get())
                {
                    return VK_FALSE;
                }

                context->addTextureObject(textureObject);

                //

            	auto lutName = "BSDF_LUT_" + std::to_string(VKTS_BSDF_LENGTH) + "_" + std::to_string(VKTS_BSDF_M);

                imageObject = context->useImageObject(lutName);

    			if (!imageObject.get())
    			{
    				logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Memory image not found: '%s'", lutName.c_str());

    				return VK_FALSE;
    			}

    			//

    			VkSamplerCreateInfo lutSamplerCreateInfo = context->getSamplerCreateInfo();

    			lutSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    			lutSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    			lutSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    			lutSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

                textureObject = textureObjectCreate(context->getContextObject(), lutName, VK_FALSE, imageObject, lutSamplerCreateInfo);

                if (!textureObject.get())
                {
                    return VK_FALSE;
                }

                context->addTextureObject(textureObject);
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadMaterials(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[3];
    uint32_t uidata;
    VkBool32 bdata;

    auto bsdfMaterial = IBSDFMaterialSP();
    auto phongMaterial = IPhongMaterialSP();

    auto textureObject = ITextureObjectSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "textureObject_library"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadTextureObjects(directory, sdata, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load textureObjects: '%s'", sdata);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "shading"))
        {
            if (!scenegraphParseStringBool(buffer, sdata, &bdata))
            {
                return VK_FALSE;
            }

            if (strncmp(sdata, "BSDF", 4) == 0)
            {
                bsdfMaterial = IBSDFMaterialSP(new BSDFMaterial(bdata));
                phongMaterial = IPhongMaterialSP();
            }
            else if (strncmp(sdata, "Phong", 5) == 0)
            {
                bsdfMaterial = IBSDFMaterialSP();
                phongMaterial = IPhongMaterialSP(new PhongMaterial(bdata));

                // Create all possibilities, even when not used.

                VkDescriptorPoolSize descriptorPoolSize[3]{};

				descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorPoolSize[0].descriptorCount = VKTS_BINDING_UNIFORM_BUFFER_COUNT;

				descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorPoolSize[1].descriptorCount = VKTS_BINDING_UNIFORM_PHONG_BINDING_COUNT + 2;

				descriptorPoolSize[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				descriptorPoolSize[2].descriptorCount = VKTS_BINDING_UNIFORM_VOXEL_COUNT;

                auto descriptorPool = descriptorPoolCreate(context->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 3, descriptorPoolSize);

                if (!descriptorPool.get())
                {
                    return VK_FALSE;
                }

                phongMaterial->setDescriptorPool(descriptorPool);

                //

                auto allDescriptorSetLayouts = context->getDescriptorSetLayout()->getDescriptorSetLayout();

                auto descriptorSets = descriptorSetsCreate(context->getContextObject()->getDevice()->getDevice(), descriptorPool->getDescriptorPool(), 1, &allDescriptorSetLayouts);

                if (!descriptorSets.get())
                {
                    return VK_FALSE;
                }

                phongMaterial->setDescriptorSets(descriptorSets);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown shading: '%s'", sdata);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->setName(sdata);

                context->addBSDFMaterial(bsdfMaterial);
            }
            else if (phongMaterial.get())
            {
                phongMaterial->setName(sdata);

                context->addPhongMaterial(phongMaterial);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "transparent"))
        {
            if (!scenegraphParseBool(buffer, &bdata))
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setTransparent(bdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "emissive_color"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], fdata[1], fdata[2], VK_FORMAT_R8G8B8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setEmissive(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "alpha_value"))
        {
            if (!scenegraphParseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], 0.0f, 0.0f, VK_FORMAT_R8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setAlpha(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "displacement_value"))
        {
            if (!scenegraphParseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], 0.0f, 0.0f, VK_FORMAT_R8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setDisplacement(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "normal_vector"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0] * 0.5f + 0.5f, fdata[1] * 0.5f + 0.5f, fdata[2] * 0.5f + 0.5f, VK_FORMAT_R8G8B8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setNormal(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "emissive_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setEmissive(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "alpha_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setAlpha(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "displacement_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setDisplacement(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "normal_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
            	phongMaterial->setNormal(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_ambient_color"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], fdata[1], fdata[2], VK_FORMAT_R8G8B8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setAmbient(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_diffuse_color"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], fdata[1], fdata[2], VK_FORMAT_R8G8B8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setDiffuse(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_specular_color"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], fdata[1], fdata[2], VK_FORMAT_R8G8B8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setSpecular(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_specular_shininess_value"))
        {
            if (!scenegraphParseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], 0.0f, 0.0f, VK_FORMAT_R8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setSpecularShininess(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_mirror_color"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], fdata[1], fdata[2], VK_FORMAT_R8G8B8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setMirror(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_mirror_reflectivity_value"))
        {
            if (!scenegraphParseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            textureObject = scenegraphCreateTextureObject(fdata[0], 0.0f, 0.0f, VK_FORMAT_R8_UNORM, context);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setMirrorReflectivity(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_ambient_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setAmbient(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_diffuse_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setDiffuse(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_specular_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setSpecular(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_specular_shininess_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setSpecularShininess(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_mirror_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setMirror(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "phong_mirror_reflectivity_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (phongMaterial.get())
            {
                phongMaterial->setMirrorReflectivity(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "fragment_shader"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            auto shaderModule = context->useFragmentShaderModule(sdata);

            if (!shaderModule.get())
            {
                std::string finalFilename = std::string(directory) + std::string(sdata);

                auto shaderBinary = fileLoadBinary(finalFilename.c_str());

				if (!shaderBinary.get())
				{
					shaderBinary = fileLoadBinary(filename);

					if (!shaderBinary.get())
					{
						logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load fragment shader: '%s'", sdata);

						return VK_FALSE;
					}
				}

				//

				shaderModule = shaderModuleCreate(sdata, context->getContextObject()->getDevice()->getDevice(), 0, shaderBinary->getSize(), (uint32_t*)shaderBinary->getData());

				if (!shaderModule.get())
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create fragment shader module.");

					return VK_FALSE;
				}

				context->addFragmentShaderModule(shaderModule);
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->setFragmentShader(shaderModule);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "attributes"))
        {
            if (!scenegraphParseUIntHex(buffer, &uidata))
            {
                return VK_FALSE;
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->setAttributes((VkTsVertexBufferType)uidata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "add_textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return VK_FALSE;
            }

            if (bsdfMaterial.get())
            {
            	bsdfMaterial->addTextureObject(textureObject);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No material");

                return VK_FALSE;
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadSubMeshes(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[8];
    int32_t idata[3];

    auto subMesh = ISubMeshSP();

    std::vector<float> vertex;
    std::vector<float> normal;
    std::vector<float> bitangent;
    std::vector<float> tangent;
    std::vector<float> texcoord;

    std::vector<float> boneIndices0;
    std::vector<float> boneIndices1;
    std::vector<float> boneWeights0;
    std::vector<float> boneWeights1;
    std::vector<float> numberBones;

    std::vector<int32_t> indices;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "material_library"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadMaterials(directory, sdata, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load materials: '%s'", sdata);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            subMesh = ISubMeshSP(new SubMesh());

            if (!subMesh.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh not created: '%s'", sdata);

                return VK_FALSE;
            }

            subMesh->setName(sdata);
        }
        else if (scenegraphParseIsToken(buffer, "vertex"))
        {
            if (!scenegraphParseVec4(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 4; i++)
                {
                    vertex.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "normal"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 3; i++)
                {
                    normal.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "bitangent"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 3; i++)
                {
                    bitangent.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "tangent"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 3; i++)
                {
                    tangent.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "texcoord"))
        {
            if (!scenegraphParseVec2(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 2; i++)
                {
                    texcoord.push_back(fdata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "boneIndex"))
        {
            if (!scenegraphParseVec8(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 8; i++)
                {
                    if (i < 4)
                    {
                        boneIndices0.push_back(fdata[i]);
                    }
                    else
                    {
                        boneIndices1.push_back(fdata[i]);
                    }
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "boneWeight"))
        {
            if (!scenegraphParseVec8(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 8; i++)
                {
                    if (i < 4)
                    {
                        boneWeights0.push_back(fdata[i]);
                    }
                    else
                    {
                        boneWeights1.push_back(fdata[i]);
                    }
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "numberBones"))
        {
            if (!scenegraphParseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                numberBones.push_back(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "face"))
        {
            if (!scenegraphParseIVec3(buffer, idata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                for (int32_t i = 0; i < 3; i++)
                {
                    indices.push_back(idata[i]);
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "material"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (subMesh.get())
            {
                const auto phongMaterial = context->usePhongMaterial(sdata);

                if (phongMaterial.get())
                {
                    subMesh->setPhongMaterial(phongMaterial);
                }
                else
                {
                    const auto bsdfMaterial = context->useBSDFMaterial(sdata);

                    if (bsdfMaterial.get())
                    {
                        subMesh->setBSDFMaterial(bsdfMaterial);
                    }
                    else
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Material not found: '%s'", sdata);

                        return VK_FALSE;
                    }
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }

            //
            // Sub mesh creation.
            //

            if (subMesh.get())
            {
                subMesh->setNumberVertices((int32_t)(vertex.size() / 4));
                subMesh->setNumberIndices((int32_t) indices.size());

                int32_t totalSize = 0;
                uint32_t strideInBytes = 0;

                VkTsVertexBufferType vertexBufferType = 0;

                if (vertex.size() > 0)
                {
                    subMesh->setVertexOffset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_VERTEX;
                }

                if (normal.size() > 0)
                {
                    if (normal.size() / 3 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Normal has different size");

                        return VK_FALSE;
                    }

                    subMesh->setNormalOffset(strideInBytes);
                    strideInBytes += 3 * sizeof(float);

                    totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_NORMAL;
                }

                if (normal.size() > 0 && bitangent.size() > 0 && tangent.size() > 0)
                {
                    if (bitangent.size() / 3 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bitangent has different size");

                        return VK_FALSE;
                    }

                    subMesh->setBitangentOffset(strideInBytes);
                    strideInBytes += 3 * sizeof(float);

                    totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

                    //

                    if (tangent.size() / 3 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Tangent has different size");

                        return VK_FALSE;
                    }

                    subMesh->setTangentOffset(strideInBytes);
                    strideInBytes += 3 * sizeof(float);

                    totalSize += 3 * sizeof(float) * subMesh->getNumberVertices();

                    //

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TANGENTS;
                }

                if (texcoord.size() > 0)
                {
                    if (texcoord.size() / 2 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "TextureObject coordinate has different size");

                        return VK_FALSE;
                    }

                    subMesh->setTexcoordOffset(strideInBytes);
                    strideInBytes += 2 * sizeof(float);

                    totalSize += 2 * sizeof(float) * subMesh->getNumberVertices();

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_TEXCOORD;
                }

                if (boneIndices0.size() > 0 && boneIndices1.size() > 0 && boneWeights0.size() > 0 && boneWeights1.size() > 0 && numberBones.size() > 0)
                {
                    if (boneIndices0.size() / 4 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bone indices0 has different size: %u != %u", boneIndices0.size(), vertex.size());

                        return VK_FALSE;
                    }

                    subMesh->setBoneIndices0Offset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    if (boneIndices1.size() / 4 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bone indices1 has different size %u != %u", boneIndices1.size(), vertex.size());

                        return VK_FALSE;
                    }

                    subMesh->setBoneIndices1Offset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    if (boneWeights0.size() / 4 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bone weights0 has different size");

                        return VK_FALSE;
                    }

                    subMesh->setBoneWeights0Offset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    if (boneWeights1.size() / 4 != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Bone weights has different size");

                        return VK_FALSE;
                    }

                    subMesh->setBoneWeights1Offset(strideInBytes);
                    strideInBytes += 4 * sizeof(float);

                    totalSize += 4 * sizeof(float) * subMesh->getNumberVertices();

                    if (numberBones.size() != vertex.size() / 4)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Number bones has different size");

                        return VK_FALSE;
                    }

                    subMesh->setNumberBonesOffset(strideInBytes);
                    strideInBytes += 1 * sizeof(float);

                    totalSize += 1 * sizeof(float) * subMesh->getNumberVertices();

                    vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_BONES;
                }

                subMesh->setStrideInBytes(strideInBytes);

                if (totalSize > 0)
                {
                    auto vertexBinaryBuffer = binaryBufferCreate((size_t)totalSize);

                    if (!vertexBinaryBuffer.get() || vertexBinaryBuffer->getSize() != (size_t)totalSize)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex binary buffer");

                        return VK_FALSE;
                    }

                    for (int32_t currentVertexElement = 0; currentVertexElement < subMesh->getNumberVertices(); currentVertexElement++)
                    {
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_VERTEX)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&vertex[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_NORMAL)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&normal[currentVertexElement * 3]), 1, 3 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BITANGENT)
                        {
                            vertexBinaryBuffer->write( reinterpret_cast<const uint8_t*>(&bitangent[currentVertexElement * 3]), 1, 3 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TANGENT)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&tangent[currentVertexElement * 3]), 1, 3 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_TEXCOORD)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&texcoord[currentVertexElement * 2]), 1, 2 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES0)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&boneIndices0[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_INDICES1)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&boneIndices1[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS0)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&boneWeights0[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_WEIGHTS1)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&boneWeights1[currentVertexElement * 4]), 1, 4 * sizeof(float));
                        }
                        if (vertexBufferType & VKTS_VERTEX_BUFFER_TYPE_BONE_NUMBERS)
                        {
                            vertexBinaryBuffer->write(reinterpret_cast<const uint8_t*>(&numberBones[currentVertexElement * 1]), 1, 1 * sizeof(float));
                        }
                    }

                    //

                    VkBufferCreateInfo bufferCreateInfo{};

                    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

                    bufferCreateInfo.size = (size_t) totalSize;
                    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                    bufferCreateInfo.flags = 0;
                    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    bufferCreateInfo.queueFamilyIndexCount = 0;
                    bufferCreateInfo.pQueueFamilyIndices = nullptr;

                    IDeviceMemorySP stageDeviceMemory;
                    IBufferSP stageBuffer;

                    auto vertexBuffer = bufferObjectCreate(stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), vertexBinaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    context->addStageBuffer(stageBuffer);
                    context->addStageDeviceMemory(stageDeviceMemory);

                    if (!vertexBuffer.get())
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create vertex buffer");

                        return VK_FALSE;
                    }

                    //

                    subMesh->setVertexBuffer(vertexBuffer, vertexBufferType, Aabb((const float*)vertexBinaryBuffer->getData(), subMesh->getNumberVertices(), subMesh->getStrideInBytes()));
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh incomplete");

                    return VK_FALSE;
                }

                if (indices.size() > 0)
                {
                	size_t size = sizeof(int32_t) * subMesh->getNumberIndices();

                    auto indicesBinaryBuffer = binaryBufferCreate(reinterpret_cast<const uint8_t*>(&indices[0]), size);

                    if (!indicesBinaryBuffer.get() || indicesBinaryBuffer->getSize() != size)
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create indices binary buffer");

                        return VK_FALSE;
                    }

                    //

                    VkBufferCreateInfo bufferCreateInfo{};

                    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                    bufferCreateInfo.size = indicesBinaryBuffer->getSize();
                    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                    bufferCreateInfo.flags = 0;

                    IDeviceMemorySP stageDeviceMemory;
                    IBufferSP stageBuffer;

                    auto indicesVertexBuffer = bufferObjectCreate(stageBuffer, stageDeviceMemory, context->getContextObject(), context->getCommandBuffer(), indicesBinaryBuffer, bufferCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    context->addStageBuffer(stageBuffer);
                    context->addStageDeviceMemory(stageDeviceMemory);

                    if (!indicesVertexBuffer.get())
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create indices vertex buffer");

                        return VK_FALSE;
                    }

                    //

                    subMesh->setIndicesVertexBuffer(indicesVertexBuffer);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh incomplete");

                    return VK_FALSE;
                }

                //

                if (subMesh->getBSDFMaterial().get())
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

                    auto descriptorSetLayout = descriptorSetLayoutCreate(context->getContextObject()->getDevice()->getDevice(), 0, bindingCount, descriptorSetLayoutBinding);

                	if (!descriptorSetLayout.get())
                	{
                		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor set layout.");

                		return VK_FALSE;
                	}

                	subMesh->setDescriptorSetLayout(descriptorSetLayout);

                    // Create all possibilities, even when not used.

                    VkDescriptorPoolSize descriptorPoolSize[2]{};

    				descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    				descriptorPoolSize[0].descriptorCount = 5;

    				descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    				descriptorPoolSize[1].descriptorCount = 18;

                    auto descriptorPool = descriptorPoolCreate(context->getContextObject()->getDevice()->getDevice(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, 2, descriptorPoolSize);

                    if (!descriptorPool.get())
                    {
                		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor pool.");

                        return VK_FALSE;
                    }

            		subMesh->getBSDFMaterial()->setDescriptorPool(descriptorPool);

                	//

                	const auto allDescriptorSetLayouts = descriptorSetLayout->getDescriptorSetLayout();

                    auto descriptorSets = descriptorSetsCreate(context->getContextObject()->getDevice()->getDevice(), subMesh->getBSDFMaterial()->getDescriptorPool()->getDescriptorPool(), 1, &allDescriptorSetLayouts);

                    if (!descriptorSets.get())
                    {
                    	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create descriptor sets.");

                        return VK_FALSE;
                    }

                    subMesh->getBSDFMaterial()->setDescriptorSets(descriptorSets);

                    //

                	VkDescriptorSetLayout setLayouts[1];

                	setLayouts[0] = descriptorSetLayout->getDescriptorSetLayout();

                	auto pipelineLayout = pipelineCreateLayout(context->getContextObject()->getDevice()->getDevice(), 0, 1, setLayouts, 0, nullptr);

                	if (!pipelineLayout.get())
                	{
                		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create pipeline layout.");

                		return VK_FALSE;
                	}

                	subMesh->setPipelineLayout(pipelineLayout);

					// Create graphics pipeline for this sub mesh.

                	auto currentRenderPass = context->getRenderPass();

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


					vertexBufferType = subMesh->getVertexBufferType() & subMesh->getBSDFMaterial()->getAttributes();

					if (vertexBufferType != subMesh->getBSDFMaterial()->getAttributes())
					{
	                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh vertex buffer type does not match with material");

	                    return VK_FALSE;
					}

					if ((subMesh->getVertexBufferType() & VKTS_VERTEX_BUFFER_TYPE_BONES) == VKTS_VERTEX_BUFFER_TYPE_BONES)
					{
						vertexBufferType |= VKTS_VERTEX_BUFFER_TYPE_BONES;
					}

					auto currentVertexShaderModule = context->useVertexShaderModule(vertexBufferType);

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


					auto pipeline = pipelineCreateGraphics(context->getContextObject()->getDevice()->getDevice(), VK_NULL_HANDLE, gp.getGraphicsPipelineCreateInfo(), vertexBufferType);

					if (!pipeline.get())
					{
						logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create graphics pipeline.");

						return VK_FALSE;
					}

					subMesh->setGraphicsPipeline(pipeline);
                }

                //

                context->addSubMesh(subMesh);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No sub mesh");

                return VK_FALSE;
            }

            vertex.clear();
            normal.clear();
            bitangent.clear();
            tangent.clear();
            texcoord.clear();

            boneIndices0.clear();
            boneIndices1.clear();
            boneWeights0.clear();
            boneWeights1.clear();
            numberBones.clear();

            indices.clear();
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadMeshes(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[6];

    auto mesh = IMeshSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "submesh_library"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadSubMeshes(directory, sdata, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load sub meshes: '%s'", sdata);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            mesh = IMeshSP(new Mesh());

            if (!mesh.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Mesh not created: '%s'", sdata);

                return VK_FALSE;
            }

            mesh->setName(sdata);

            context->addMesh(mesh);
        }
        else if (scenegraphParseIsToken(buffer, "submesh"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (mesh.get())
            {
                const auto& subMesh = context->useSubMesh(sdata);

                if (!subMesh.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Sub mesh not found: '%s'", sdata);

                    return VK_FALSE;
                }

                mesh->addSubMesh(subMesh);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "displace"))
        {
            if (!scenegraphParseVec2(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (mesh.get())
            {
                mesh->setDisplace(glm::vec2(fdata[0], fdata[1]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No mesh");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "aabb"))
        {
            if (!scenegraphParseVec6(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (mesh.get())
            {
                mesh->setAABB(Aabb(glm::vec3(fdata[0], fdata[1], fdata[2]), glm::vec3(fdata[3], fdata[4], fdata[5])));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No mesh");

                return VK_FALSE;
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadChannels(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[6];

    auto channel = IChannelSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            channel = IChannelSP(new Channel());

            if (!channel.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Channel not created: '%s'", sdata);

                return VK_FALSE;
            }

            channel->setName(sdata);

            context->addChannel(channel);
        }
        else if (scenegraphParseIsToken(buffer, "target_transform"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (channel.get())
            {
                if (strncmp(sdata, "TRANSLATE", 9) == 0)
                {
                    channel->setTargetTransform(VKTS_TARGET_TRANSFORM_TRANSLATE);
                }
                else if (strncmp(sdata, "ROTATE", 6) == 0)
                {
                    channel->setTargetTransform(VKTS_TARGET_TRANSFORM_ROTATE);
                }
                else if (strncmp(sdata, "QUATERNION_ROTATE", 17) == 0)
                {
                    channel->setTargetTransform(VKTS_TARGET_TRANSFORM_QUATERNION_ROTATE);
                }
                else if (strncmp(sdata, "SCALE", 5) == 0)
                {
                    channel->setTargetTransform(VKTS_TARGET_TRANSFORM_SCALE);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown target transform: '%s'", sdata);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No channel");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "target_element"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (channel.get())
            {
                if (strncmp(sdata, "X", 1) == 0)
                {
                    channel->setTargetTransformElement(VKTS_TARGET_TRANSFORM_ELEMENT_X);
                }
                else if (strncmp(sdata, "Y", 1) == 0)
                {
                    channel->setTargetTransformElement(VKTS_TARGET_TRANSFORM_ELEMENT_Y);
                }
                else if (strncmp(sdata, "Z", 1) == 0)
                {
                    channel->setTargetTransformElement(VKTS_TARGET_TRANSFORM_ELEMENT_Z);
                }
                else if (strncmp(sdata, "W", 1) == 0)
                {
                    channel->setTargetTransformElement(VKTS_TARGET_TRANSFORM_ELEMENT_W);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown target transform element: '%s'", sdata);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No channel");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "keyframe"))
        {
            char token[VKTS_MAX_TOKEN_CHARS + 1];

            auto numberItems = sscanf(buffer, "%s %f %f %s %f %f %f %f", token, &fdata[0], &fdata[1], sdata, &fdata[2], &fdata[3], &fdata[4], &fdata[5]);

            if (numberItems != 4 && numberItems != 8)
            {
                return VK_FALSE;
            }

            if (numberItems == 4)
            {
                fdata[2] = fdata[0] - 0.1f;
                fdata[3] = fdata[1];
                fdata[4] = fdata[0] + 0.1f;
                fdata[5] = fdata[1];
            }

            if (channel.get())
            {
                VkTsInterpolator interpolator = VKTS_INTERPOLATOR_CONSTANT;

                if (strncmp(sdata, "CONSTANT", 8) == 0)
                {
                    // Do nothing.
                }
                else if (strncmp(sdata, "LINEAR", 6) == 0)
                {
                    interpolator = VKTS_INTERPOLATOR_LINEAR;
                }
                else if (strncmp(sdata, "BEZIER", 6) == 0)
                {
                    interpolator = VKTS_INTERPOLATOR_BEZIER;
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unknown interpolator: '%s'", sdata);

                    return VK_FALSE;
                }

                channel->addEntry(fdata[0], fdata[1], glm::vec4(fdata[2], fdata[3], fdata[4], fdata[5]), interpolator);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No channel");

                return VK_FALSE;
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadAnimations(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[1];

    auto animation = IAnimationSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "channel_library"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadChannels(directory, sdata, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load channels: '%s'", sdata);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            animation = IAnimationSP(new Animation());

            if (!animation.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation not created: '%s'", sdata);

                return VK_FALSE;
            }

            animation->setName(sdata);

            context->addAnimation(animation);
        }
        else if (scenegraphParseIsToken(buffer, "start"))
        {
            if (!scenegraphParseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (animation.get())
            {
                animation->setStart(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No animation");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "stop"))
        {
            if (!scenegraphParseFloat(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (animation.get())
            {
                animation->setStop(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No animation");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "marker"))
        {
            if (!scenegraphParseStringFloat(buffer, sdata, fdata))
            {
                return VK_FALSE;
            }

            if (animation.get())
            {
                auto currentMarker = IMarkerSP(new Marker());

                if (!currentMarker.get())
                {
                	return VK_FALSE;
                }

                currentMarker->setName(sdata);
                currentMarker->setTime(fdata[0]);

                animation->addMarker(currentMarker);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No animation");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "channel"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (animation.get())
            {
                const auto& channel = context->useChannel(sdata);

                if (!channel.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Channel not found: '%s'", sdata);

                    return VK_FALSE;
                }

                animation->addChannel(channel);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No animation");

                return VK_FALSE;
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadParticleSystems(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        // TODO: Load particle systems and its data.
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadCameras(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata;

    auto camera = ICameraSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            camera = ICameraSP(new Camera());

            if (!camera.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Camera not created: '%s'", sdata);

                return VK_FALSE;
            }

            camera->setName(sdata);

            //

            context->addCamera(camera);
        }
        else if (scenegraphParseIsToken(buffer, "type"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (camera.get())
            {
                if (strcmp(sdata, "Perspective") == 0)
                {
                	camera->setCameraType(PerspectiveCamera);
                }
                else if (strcmp(sdata, "Orhtogonal") == 0)
                {
                	camera->setCameraType(OrthogonalCamera);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid camera type '%s'", sdata);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No Camera");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "znear"))
        {
            if (!scenegraphParseFloat(buffer, &fdata))
            {
                return VK_FALSE;
            }

            if (camera.get())
            {
            	camera->setZNear(fdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No camera");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "zfar"))
        {
            if (!scenegraphParseFloat(buffer, &fdata))
            {
                return VK_FALSE;
            }

            if (camera.get())
            {
            	camera->setZFar(fdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No camera");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "fovy"))
        {
            if (!scenegraphParseFloat(buffer, &fdata))
            {
                return VK_FALSE;
            }

            if (camera.get() && camera->getCameraType() == PerspectiveCamera)
            {
            	camera->setFovY(fdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No camera or invalid type");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "ortho_scale"))
        {
            if (!scenegraphParseFloat(buffer, &fdata))
            {
                return VK_FALSE;
            }

            if (camera.get() && camera->getCameraType() == OrthogonalCamera)
            {
            	camera->setOrthoScale(fdata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No camera or invalid type");

                return VK_FALSE;
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}


static VkBool32 scenegraphLoadLights(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[3];

    auto light = ILightSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            light = ILightSP(new Light());

            if (!light.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Light not created: '%s'", sdata);

                return VK_FALSE;
            }

            light->setName(sdata);

            context->addLight(light);
        }
        else if (scenegraphParseIsToken(buffer, "type"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (light.get())
            {
                if (strcmp(sdata, "Point") == 0)
                {
                	light->setLightType(PointLight);
                }
                else if (strcmp(sdata, "Directional") == 0)
                {
                	light->setLightType(DirectionalLight);
                }
                else if (strcmp(sdata, "Spot") == 0)
                {
                	light->setLightType(SpotLight);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid  light type '%s'", sdata);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No light");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "falloff"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return VK_FALSE;
            }

            if (light.get())
            {
                if (strcmp(sdata, "Quadratic") == 0 && light->getLightType() != DirectionalLight)
                {
                	light->setFalloffType(QuadraticFalloff);
                }
                else if (strcmp(sdata, "Linear") == 0 && light->getLightType() != DirectionalLight)
                {
                	light->setFalloffType(LinearFalloff);
                }
                else if (strcmp(sdata, "Constant") == 0)
                {
                	light->setFalloffType(ConstantFalloff);
                }
                else
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Invalid fall off type '%s'", sdata);

                    return VK_FALSE;
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No light");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "strength"))
        {
            if (!scenegraphParseFloat(buffer, &fdata[0]))
            {
                return VK_FALSE;
            }

            if (light.get())
            {
            	light->setStrength(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No light");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "outer_angle"))
        {
            if (!scenegraphParseFloat(buffer, &fdata[0]))
            {
                return VK_FALSE;
            }

            if (light.get() && light->getLightType() == SpotLight)
            {
            	light->setOuterAngle(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No or invalid light");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "inner_angle"))
        {
            if (!scenegraphParseFloat(buffer, &fdata[0]))
            {
                return VK_FALSE;
            }

            if (light.get() && light->getLightType() == SpotLight)
            {
            	light->setInnerAngle(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No or invalid light");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "color"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (light.get())
            {
            	light->setColor(glm::vec3(fdata[0], fdata[1], fdata[2]));

            	//

            	context->addLight(light);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No light");

                return VK_FALSE;
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

static VkBool32 scenegraphLoadObjects(const char* directory, const char* filename, const IContextSP& context)
{
    if (!directory || !filename || !context.get())
    {
        return VK_FALSE;
    }

    std::string finalFilename = std::string(directory) + std::string(filename);

    auto textBuffer = fileLoadText(finalFilename.c_str());

    if (!textBuffer.get())
    {
        textBuffer = fileLoadText(filename);

        if (!textBuffer.get())
        {
            return VK_FALSE;
        }
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata0[VKTS_MAX_TOKEN_CHARS + 1];
    char sdata1[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[3];
    VkBool32 bdata[3];
    int32_t idata;
    uint32_t uidata;
    glm::mat4 mat4;

    auto object = IObjectSP();

    auto node = INodeSP();

    auto constraint = IConstraintSP();
    CopyConstraint* copyConstraint = nullptr;
    LimitConstraint* limitConstraint = nullptr;

    SmartPointerMap<std::string, INodeSP> allNodes;

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "mesh_library"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadMeshes(directory, sdata0, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load meshes: '%s'", sdata0);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "animation_library"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadAnimations(directory, sdata0, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load animations: '%s'", sdata0);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "camera_library"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadCameras(directory, sdata0, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load cameras: '%s'", sdata0);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "light_library"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadLights(directory, sdata0, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load lights: '%s'", sdata0);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "particle_system_library"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (!scenegraphLoadParticleSystems(directory, sdata0, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load particles: '%s'", sdata0);

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            object = IObjectSP(new Object());

            if (!object.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Object not created: '%s'", sdata0);

                return VK_FALSE;
            }

            object->setName(sdata0);

            context->addObject(object);
        }
        else if (scenegraphParseIsToken(buffer, "node"))
        {
            if (!scenegraphParseStringTuple(buffer, sdata0, sdata1))
            {
                return VK_FALSE;
            }

            if (object.get())
            {
                node = INodeSP(new Node());

                if (!node.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Node not created: '%s'", sdata0);

                    return VK_FALSE;
                }

                node->setName(sdata0);

                //

                allNodes[sdata0] = node;

                if (strncmp(sdata1, "-", 1) == 0)
                {
                    object->setRootNode(node);
                }
                else
                {
                    auto parentNodeIndex = allNodes.find(sdata1);

                    if (parentNodeIndex != allNodes.size() && allNodes.valueAt(parentNodeIndex).get())
                    {
                        node->setParentNode(allNodes.valueAt(parentNodeIndex));

                        allNodes.valueAt(parentNodeIndex)->addChildNode(node);
                    }
                    else
                    {
                        logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Parent node not found: '%s'", sdata1);

                        return VK_FALSE;
                    }
                }
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "layers"))
        {
            if (!scenegraphParseUIntHex(buffer, &uidata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setLayers(uidata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "particle_system"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                // TODO: Append particle system.
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "seed"))
        {
            if (!scenegraphParseInt(buffer, &idata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                // TODO: Append particle system seed.
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "constraint"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
            	if (strcmp(sdata0, "COPY_LOCATION") == 0)
            	{
            		copyConstraint = new CopyConstraint(COPY_LOCATION);
                    node->addConstraint(IConstraintSP(copyConstraint));
            	}
            	else if (strcmp(sdata0, "COPY_ROTATION") == 0)
            	{
            		copyConstraint = new CopyConstraint(COPY_ROTATION);
                    node->addConstraint(IConstraintSP(copyConstraint));
            	}
            	else if (strcmp(sdata0, "COPY_SCALE") == 0)
            	{
            		copyConstraint = new CopyConstraint(COPY_SCALE);
                    node->addConstraint(IConstraintSP(copyConstraint));
            	}
            	else if (strcmp(sdata0, "LIMIT_LOCATION") == 0)
            	{
            		limitConstraint = new LimitConstraint(LIMIT_LOCATION);
                    node->addConstraint(IConstraintSP(limitConstraint));
            	}
            	else if (strcmp(sdata0, "LIMIT_ROTATION") == 0)
            	{
            		limitConstraint = new LimitConstraint(LIMIT_ROTATION);
                    node->addConstraint(IConstraintSP(limitConstraint));
            	}
            	else if (strcmp(sdata0, "LIMIT_SCALE") == 0)
            	{
            		limitConstraint = new LimitConstraint(LIMIT_SCALE);
                    node->addConstraint(IConstraintSP(limitConstraint));
            	}
				else
				{
					logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Unsupported constraint");

					return VK_FALSE;
				}
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "target"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	INodeSP targetNode;

            	for (size_t i = 0; i < context->getAllObjects().values().size(); i++)
            	{
            		if (!context->getAllObjects().values()[i]->getRootNode().get())
            		{
            			continue;
            		}

            		targetNode = context->getAllObjects().values()[i]->getRootNode()->findNodeRecursiveFromRoot(sdata0);

            		if (targetNode.get())
            		{
            			break;
            		}
            	}

                if (!targetNode.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No target node found");

                    return VK_FALSE;
                }

                copyConstraint->setTarget(targetNode);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or copy constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "use"))
        {
            if (!scenegraphParseBoolTriple(buffer, &bdata[0], &bdata[1], &bdata[2]))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	copyConstraint->setUse(std::array<VkBool32, 3>{bdata[0], bdata[1], bdata[2]});
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or copy constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "invert"))
        {
            if (!scenegraphParseBoolTriple(buffer, &bdata[0], &bdata[1], &bdata[2]))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	copyConstraint->setInvert(std::array<VkBool32, 3>{bdata[0], bdata[1], bdata[2]});
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or copy constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "use_offset"))
        {
            if (!scenegraphParseBool(buffer, &bdata[0]))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	copyConstraint->setOffset(bdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or copy constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "use_min"))
        {
            if (!scenegraphParseBoolTriple(buffer, &bdata[0], &bdata[1], &bdata[2]))
            {
                return VK_FALSE;
            }

            if (node.get() && limitConstraint)
            {
            	limitConstraint->setUseMin(std::array<VkBool32, 3>{bdata[0], bdata[1], bdata[2]});
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or limit constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "use_max"))
        {
            if (!scenegraphParseBoolTriple(buffer, &bdata[0], &bdata[1], &bdata[2]))
            {
                return VK_FALSE;
            }

            if (node.get() && limitConstraint)
            {
            	limitConstraint->setUseMax(std::array<VkBool32, 3>{bdata[0], bdata[1], bdata[2]});
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or limit constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "min"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get() && limitConstraint)
            {
            	limitConstraint->setMin(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or limit constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "max"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get() && limitConstraint)
            {
            	limitConstraint->setMax(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or limit constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "influence"))
        {
            if (!scenegraphParseFloat(buffer, &fdata[0]))
            {
                return VK_FALSE;
            }

            if (node.get() && copyConstraint)
            {
            	copyConstraint->setInfluence(fdata[0]);
            }
            else if (node.get() && limitConstraint)
            {
            	limitConstraint->setInfluence(fdata[0]);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node or constraint");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "translate"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setTranslate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "rotate"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setRotate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "scale"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setScale(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "jointIndex"))
        {
            if (!scenegraphParseInt(buffer, &idata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setJointIndex(idata);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "joints"))
        {
            if (!scenegraphParseInt(buffer, &idata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                VkBufferCreateInfo bufferCreateInfo{};

                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                // mat3 in std140 consumes three vec4 columns.
                bufferCreateInfo.size = alignmentGetSizeInBytes(16 * sizeof(float) * (VKTS_MAX_JOINTS + 1) + 12 * sizeof(float) * (VKTS_MAX_JOINTS + 1), 16);
                bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                bufferCreateInfo.flags = 0;

                auto jointsUniformBuffer = bufferObjectCreate(context->getContextObject(), bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

                if (!jointsUniformBuffer.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create joint uniform buffer for node: '%s'", sdata0);

                    return VK_FALSE;
                }

                node->setJointsUniformBuffer(idata, jointsUniformBuffer);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "bind_translate"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setBindTranslate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "bind_rotate"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setBindRotate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "bind_scale"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                node->setBindScale(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "mesh"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                VkBufferCreateInfo bufferCreateInfo{};

                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                // mat3 in std140 consumes three vec4 columns.
                bufferCreateInfo.size = alignmentGetSizeInBytes(16 * sizeof(float) + 12 * sizeof(float), 16);
                bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                bufferCreateInfo.flags = 0;

                auto transformUniformBuffer = bufferObjectCreate(context->getContextObject(), bufferCreateInfo, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

                if (!transformUniformBuffer.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not create uniform buffer for node: '%s'", sdata0);

                    return VK_FALSE;
                }

                //

                node->setTransformUniformBuffer(transformUniformBuffer);

            	//

                const auto& mesh = context->useMesh(sdata0);

                if (!mesh.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Mesh not found: '%s'", sdata0);

                    return VK_FALSE;
                }

                node->addMesh(mesh);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "camera"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                const auto& camera = context->useCamera(sdata0);

                if (!camera.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Camera not found: '%s'", sdata0);

                    return VK_FALSE;
                }

                node->addCamera(camera);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "light"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                const auto& light = context->useLight(sdata0);

                if (!light.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Light not found: '%s'", sdata0);

                    return VK_FALSE;
                }

                node->addLight(light);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else if (scenegraphParseIsToken(buffer, "animation"))
        {
            if (!scenegraphParseString(buffer, sdata0))
            {
                return VK_FALSE;
            }

            if (node.get())
            {
                const auto& animation = context->useAnimation(sdata0);

                if (!animation.get())
                {
                    logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Animation not found: '%s'", sdata0);

                    return VK_FALSE;
                }

                node->addAnimation(animation);
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No node");

                return VK_FALSE;
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    return VK_TRUE;
}

ISceneSP VKTS_APIENTRY scenegraphLoadScene(const char* filename, const IContextSP& context)
{
    if (!filename || !context.get())
    {
        return ISceneSP();
    }

    auto textBuffer = fileLoadText(filename);

    if (!textBuffer.get())
    {
        return ISceneSP();
    }

    char directory[VKTS_MAX_BUFFER_CHARS] = "";

    fileGetDirectory(directory, filename);

    auto scene = ISceneSP(new Scene());

    if (!scene.get())
    {
        return ISceneSP();
    }

    char buffer[VKTS_MAX_BUFFER_CHARS + 1];
    char sdata[VKTS_MAX_TOKEN_CHARS + 1];
    float fdata[3];

    auto object = IObjectSP();

    while (textBuffer->gets(buffer, VKTS_MAX_BUFFER_CHARS))
    {
        if (scenegraphParseSkipBuffer(buffer))
        {
            continue;
        }

        if (scenegraphParseIsToken(buffer, "scene_name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return ISceneSP();
            }

            scene->setName(std::string(sdata));
        }
        else if (scenegraphParseIsToken(buffer, "object_library"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return ISceneSP();
            }

            if (!scenegraphLoadObjects(directory, sdata, context))
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Could not load objects: '%s'", sdata);

                return ISceneSP();
            }
        }
        else if (scenegraphParseIsToken(buffer, "object"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return ISceneSP();
            }

            object = context->useObject(sdata);

            if (!object.get())
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Object not found: '%s'", sdata);

                return ISceneSP();
            }

            scene->addObject(object);
        }
        else if (scenegraphParseIsToken(buffer, "name"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return ISceneSP();
            }

            if (object.get())
            {
                object->setName(std::string(sdata));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return ISceneSP();
            }
        }
        else if (scenegraphParseIsToken(buffer, "translate"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return ISceneSP();
            }

            if (object.get())
            {
                object->setTranslate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return ISceneSP();
            }
        }
        else if (scenegraphParseIsToken(buffer, "rotate"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return ISceneSP();
            }

            if (object.get())
            {
                object->setRotate(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return ISceneSP();
            }
        }
        else if (scenegraphParseIsToken(buffer, "scale"))
        {
            if (!scenegraphParseVec3(buffer, fdata))
            {
                return ISceneSP();
            }

            if (object.get())
            {
                object->setScale(glm::vec3(fdata[0], fdata[1], fdata[2]));
            }
            else
            {
                logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No object");

                return ISceneSP();
            }
        }
        else if (scenegraphParseIsToken(buffer, "environment"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return ISceneSP();
            }

            // Nothing for now.
        }
        else if (scenegraphParseIsToken(buffer, "textureObject"))
        {
            if (!scenegraphParseString(buffer, sdata))
            {
                return ISceneSP();
            }

            auto textureObject = context->useTextureObject(sdata);

            if (!textureObject.get())
            {
                return ISceneSP();
            }

            if (textureObject->getImageObject()->getImageView()->getViewType() != VK_IMAGE_VIEW_TYPE_CUBE || textureObject->getImageObject()->getImageView()->getLayerCount() != 6)
            {
            	return ISceneSP();
            }

            scene->setEnvironment(textureObject);

            //

            textureObject = context->useTextureObject(std::string(sdata) + "_LAMBERT");

            if (textureObject.get())
            {
                scene->setDiffuseEnvironment(textureObject);

                //

                textureObject = context->useTextureObject(std::string(sdata) + "_COOKTORRANCE");

                if (!textureObject.get())
                {
                	return ISceneSP();
                }

                scene->setSpecularEnvironment(textureObject);

                //

                textureObject = context->useTextureObject("BSDF_LUT_" + std::to_string(VKTS_BSDF_LENGTH) + "_" + std::to_string(VKTS_BSDF_M));

                if (!textureObject.get())
                {
                	return ISceneSP();
                }

                scene->setLut(textureObject);
            }
        }
        else
        {
            scenegraphParseUnknownBuffer(buffer);
        }
    }

    // Gather all cameras and add too scene.

    for (size_t i = 0; i < context->getAllCameras().values().size(); i++)
    {
    	scene->addCamera(context->getAllCameras().valueAt(i));
    }

    // Gather all lights and add too scene.

    for (size_t i = 0; i < context->getAllLights().values().size(); i++)
    {
    	scene->addLight(context->getAllLights().valueAt(i));
    }

    return scene;
}

IContextSP VKTS_APIENTRY scenegraphCreateContext(const VkBool32 replace, const IContextObjectSP& contextObject, const ICommandBuffersSP& cmdBuffer, const VkSamplerCreateInfo& samplerCreateInfo, const VkImageViewCreateInfo& imageViewCreateInfo, const IDescriptorSetLayoutSP& descriptorSetLayout)
{
    if (!contextObject.get())
    {
        return IContextSP();
    }

    return IContextSP(new Context(replace, contextObject, cmdBuffer, samplerCreateInfo, imageViewCreateInfo, descriptorSetLayout));
}

}
