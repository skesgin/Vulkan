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

#include <vkts/vkts.hpp>

namespace vkts
{

SmartPointerVector<IImageDataSP> VKTS_APIENTRY imageDataPrefilter(const IImageDataSP& sourceImage, const std::string& name)
{
    if (name.size() == 0 || !sourceImage.get() || sourceImage->getArrayLayers() != 6 || sourceImage->getDepth() != 1)
    {
        return SmartPointerVector<IImageDataSP>();
    }

    std::string sourceImageFilename = name;

    auto dotIndex = sourceImageFilename.rfind(".");

    if (dotIndex == sourceImageFilename.npos)
    {
        return SmartPointerVector<IImageDataSP>();
    }

    auto sourceImageName = sourceImageFilename.substr(0, dotIndex);
    auto sourceImageExtension = sourceImageFilename.substr(dotIndex);

    IImageDataSP currentTargetImage;
    std::string targetImageFilename;

    SmartPointerVector<IImageDataSP> result;

    // Create mip maps for all six layers.
    for (uint32_t layer = 0; layer < 6; layer++)
    {
        int32_t level = 0;

        int32_t width = sourceImage->getWidth();
        int32_t height = sourceImage->getHeight();

		while (width > 1 || height > 1)
		{
			targetImageFilename = sourceImageName + "_LEVEL" + std::to_string(level++) + "_LAYER" + std::to_string(layer) + sourceImageExtension;

			currentTargetImage = imageDataCreate(targetImageFilename, width, height, 1, 0.0f, 0.0f, 0.0f, 0.0f, sourceImage->getImageType(), sourceImage->getFormat());

			if (!currentTargetImage.get())
			{
				return SmartPointerVector<IImageDataSP>();
			}

			result.append(currentTargetImage);

			//

			width = glm::max(width / 2, 1);
			height = glm::max(height / 2, 1);
		}
    }

    //

    // TODO: Prefilter.

    return result;
}


}
