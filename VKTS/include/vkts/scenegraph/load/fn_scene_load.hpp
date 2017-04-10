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

#ifndef VKTS_FN_SCENE_LOAD_HPP_
#define VKTS_FN_SCENE_LOAD_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

/**
 *
 * @ThreadSafe
 */
VKTS_APICALL ISceneSP VKTS_APIENTRY sceneLoad(const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory, const VkBool32 freeHostMemory = VK_FALSE);


VKTS_APICALL VkBool32 VKTS_APIENTRY populateSubMesh(const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory, ISubMeshSP& subMesh, std::vector<float>& vertex, std::vector<int32_t>& indices,
	std::vector<float>& normal, std::vector<float>& bitangent, std::vector<float>& tangent, std::vector<float>& texcoord, std::vector<float>& boneIndices0,
	std::vector<float>& boneIndices1, std::vector<float>& boneWeights0, std::vector<float>& boneWeights1, std::vector<float>& numberBones);

VKTS_APICALL IImageDataSP VKTS_APIENTRY loadImageData(const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory, VkBool32 mipMap, VkBool32 environment, VkTsEnvironmentType environmentType,
	VkBool32 preFiltered, const std::string& imageDataFilename, const std::string& imageDataFilePath, const std::string& imageObjectName);
}

#endif /* VKTS_FN_SCENE_LOAD_HPP_ */
