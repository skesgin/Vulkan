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

#include <vkts/scenegraph/vkts_scenegraph.hpp>

#include "GltfVisitor.hpp"

namespace vkts
{

static VkBool32 gltfProcessNode(INodeSP& node, const GltfVisitor& visitor, const GltfNode& gltfNode, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
	// TODO: Implement glTF scene load.

	return VK_TRUE;
}

static VkBool32 gltfProcessObject(IObjectSP& object, const GltfVisitor& visitor, const GltfScene& gltfScene, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
	// Process root node.

    for (uint32_t i = 0; i < gltfScene.nodes.size(); i++)
    {
    	auto* gltfNode = gltfScene.nodes[i];

    	if (gltfNode->name == object->getName())
    	{
            auto node = sceneFactory->createNode(sceneManager);

            if (!node.get())
            {
                return VK_FALSE;
            }

            node->setName(gltfNode->name);

            //

            if (!gltfProcessNode(node, visitor, *gltfNode, sceneManager, sceneFactory))
            {
            	return VK_FALSE;
            }

            //

            object->setRootNode(node);
    	}
    }

	return VK_TRUE;
}

ISceneSP VKTS_APIENTRY gltfLoad(const char* filename, const ISceneManagerSP& sceneManager, const ISceneFactorySP& sceneFactory)
{
    if (!filename || !sceneManager.get() || !sceneFactory.get())
    {
        return ISceneSP();
    }

	auto textFile = fileLoadText(filename);

	if (!textFile.get())
	{
		return ISceneSP();
	}

	auto json = jsonDecode(textFile->getString());

	if (!json.get())
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Parsing JSON failed");

		return ISceneSP();
	}

    char directory[VKTS_MAX_BUFFER_CHARS] = "";

    fileGetDirectory(directory, filename);

	GltfVisitor visitor(directory);

	json->visit(visitor);

	if (visitor.getState() != GltfState_End)
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Processing glTF failed");

		return ISceneSP();
	}

	logPrint(VKTS_LOG_INFO, __FILE__, __LINE__, "Processing glTF succeeded");

	//
	// Scene.
	//

	const GltfScene* gltfScene = visitor.getDefaultScene();

	if (!gltfScene && visitor.getAllGltfScenes().size() > 0)
	{
		gltfScene = &(visitor.getAllGltfScenes().valueAt(0));
	}

	if (!gltfScene)
	{
		logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "No glTF scene found");

		return ISceneSP();
	}

    auto scene = sceneFactory->createScene(sceneManager);

    if (!scene.get())
    {
        return ISceneSP();
    }

    scene->setName(gltfScene->name);

    //
    // Objects: Create out of every root node an object, that it can be moved around independently.
    //

    for (uint32_t i = 0; i < gltfScene->nodes.size(); i++)
    {
    	auto* gltfNode = gltfScene->nodes[i];

    	VkBool32 isRoot = VK_TRUE;

        for (uint32_t k = 0; k < gltfScene->nodes.size(); k++)
        {
        	if (k == i)
        	{
        		continue;
        	}

        	auto* testGltfNode = gltfScene->nodes[k];

            for (uint32_t m = 0; m < testGltfNode->childrenPointer.size(); m++)
            {
            	if (gltfNode == testGltfNode->childrenPointer[m])
            	{
            		isRoot = VK_FALSE;

            		break;
            	}
            }

            if (!isRoot)
            {
            	break;
            }
        }

        if (!isRoot)
        {
        	continue;
        }

    	//

        auto object = sceneFactory->createObject(sceneManager);

        if (!object.get())
        {
        	return ISceneSP();
        }

        object->setName(gltfNode->name);

        sceneManager->addObject(object);

        //

        if (!gltfProcessObject(object, visitor, *gltfScene, sceneManager, sceneFactory))
        {
        	return ISceneSP();
        }

        //

        auto currentObject = sceneManager->useObject(gltfNode->name);

        if (!currentObject.get())
        {
            return ISceneSP();
        }

        scene->addObject(currentObject);
    }

    return scene;
}

}
