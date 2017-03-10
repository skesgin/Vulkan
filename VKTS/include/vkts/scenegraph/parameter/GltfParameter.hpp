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

#ifndef VKTS_GLTFPARAMETER_HPP_
#define VKTS_GLTFPARAMETER_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class GltfParameter : public Parameter
{

private:

	JSONobjectSP glTF;

	JSONarraySP scenes_nodes;
	int32_t rootNodeCounter;

	JSONarraySP nodes;
	SmartPointerMap<std::string, JSONarraySP> nodeNameToChildren;
	int32_t nodeCounter;

public:

	GltfParameter() :
		Parameter(), glTF(), scenes_nodes(), rootNodeCounter(0), nodes(), nodeCounter(0)
    {
    }

    virtual ~GltfParameter()
    {
    }

    //

	const JSONobjectSP& getGlTf() const
	{
		return glTF;
	}

    //

    virtual void visit(IScene& scene)
    {
    	glTF = JSONobjectSP(new JSONobject());

    	if (!glTF.get())
    	{
    		return;
    	}

    	//
    	// asset
    	//

    	auto assetValue = JSONobjectSP(new JSONobject());

    	auto generatorValue = JSONstringSP(new JSONstring("VKTS glTF 2.0 exporter"));

    	auto versionValue = JSONstringSP(new JSONstring("2.0"));

    	if (!assetValue.get() || !generatorValue.get() || !versionValue.get())
    	{
    		return;
    	}

    	assetValue->addKeyValue("generator", generatorValue);
    	assetValue->addKeyValue("version", versionValue);

    	//

    	glTF->addKeyValue("asset", assetValue);

    	//
    	// scene
    	//

    	auto sceneValue = JSONintegerSP(new JSONinteger(0));

    	if (!sceneValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("scene", sceneValue);

    	//
    	// scenes
    	//

    	auto scenesValue = JSONarraySP(new JSONarray());

    	auto defaultScene = JSONobjectSP(new JSONobject());

    	auto scene_nodes = JSONarraySP(new JSONarray());

    	if (!scenesValue.get() || !defaultScene.get() || !scene_nodes.get())
    	{
    		return;
    	}

    	scenesValue->addValue(defaultScene);

    	defaultScene->addKeyValue("nodes", scene_nodes);

    	//

    	glTF->addKeyValue("scenes", scenesValue);

    	//

    	this->scenes_nodes = scene_nodes;

    	//
    	// nodes
    	//

    	auto nodesValue = JSONarraySP(new JSONarray());

    	if (!nodesValue.get())
    	{
    		return;
    	}

    	//

    	glTF->addKeyValue("nodes", nodesValue);

    	//

    	this->nodes = nodesValue;
    }

    virtual void visit(IObject& object)
    {
    	if (!scenes_nodes.get())
    	{
    		return;
    	}

    	//

    	auto rootNodeIndex = JSONintegerSP(new JSONinteger(rootNodeCounter));

    	if (!rootNodeIndex.get())
    	{
    		return;
    	}

    	//

    	scenes_nodes->addValue(rootNodeIndex);

    	//

    	rootNodeCounter++;
    }

    virtual void visit(INode& node)
    {
    	if (!nodes.get())
    	{
    		return;
    	}

    	//

    	auto currentNode = JSONobjectSP(new JSONobject());

    	if (!currentNode.get())
    	{
    		return;
    	}

    	nodes->addValue(currentNode);

    	//
    	//

    	auto childrenValue = JSONarraySP(new JSONarray());

    	if (!childrenValue.get())
    	{
    		return;
    	}

    	currentNode->addKeyValue("children", childrenValue);

    	nodeNameToChildren[node.getName()] = childrenValue;

    	//

    	// TODO: Mesh.

    	//

    	if (node.getRotate().x != 0.0f || node.getRotate().y != 0.0f || node.getRotate().z != 0.0f)
    	{
    		Quat rotation;

    		VkTsRotationMode rotationMode = node.getNodeRotationMode();

    		if (!(node.isNode() || node.isArmature()))
    		{
    			rotationMode = node.getBindRotationMode();
    		}

        	switch (rotationMode)
        	{
        		case VKTS_EULER_YXZ:
        			rotation = rotateRzRxRy(node.getRotate().z, node.getRotate().x, node.getRotate().y);
        			break;
        		case VKTS_EULER_XYZ:
        			rotation = rotateRzRyRx(node.getRotate().z, node.getRotate().y, node.getRotate().x);
        			break;
        		case VKTS_EULER_XZY:
        			rotation = rotateRyRzRx(node.getRotate().y, node.getRotate().z, node.getRotate().x);
        			break;
        	}

    		//

        	auto rotationValue = JSONarraySP(new JSONarray());

        	auto rotationX = JSONfloatSP(new JSONfloat(rotation.x));
        	auto rotationY = JSONfloatSP(new JSONfloat(rotation.y));
        	auto rotationZ = JSONfloatSP(new JSONfloat(rotation.z));
        	auto rotationW = JSONfloatSP(new JSONfloat(rotation.w));

        	if (!rotationValue.get() || !rotationX.get() || !rotationY.get() || !rotationZ.get() || !rotationW.get())
        	{
        		return;
        	}

        	rotationValue->addValue(rotationX);
        	rotationValue->addValue(rotationY);
        	rotationValue->addValue(rotationZ);
        	rotationValue->addValue(rotationW);

        	//

        	currentNode->addKeyValue("rotation", rotationValue);
    	}

    	if (node.getScale().x != 1.0f || node.getScale().y != 1.0f || node.getScale().z != 1.0f)
    	{
        	auto scaleValue = JSONarraySP(new JSONarray());

        	auto scaleX = JSONfloatSP(new JSONfloat(node.getScale().x));
        	auto scaleY = JSONfloatSP(new JSONfloat(node.getScale().y));
        	auto scaleZ = JSONfloatSP(new JSONfloat(node.getScale().z));

        	if (!scaleValue.get() || !scaleX.get() || !scaleY.get() || !scaleZ.get())
        	{
        		return;
        	}

        	scaleValue->addValue(scaleX);
        	scaleValue->addValue(scaleY);
        	scaleValue->addValue(scaleZ);

        	//

        	currentNode->addKeyValue("scale", scaleValue);
    	}

    	if (node.getTranslate().x != 0.0f || node.getTranslate().y != 0.0f || node.getTranslate().z != 0.0f)
    	{
        	auto translationValue = JSONarraySP(new JSONarray());

        	auto translationX = JSONfloatSP(new JSONfloat(node.getTranslate().x));
        	auto translationY = JSONfloatSP(new JSONfloat(node.getTranslate().y));
        	auto translationZ = JSONfloatSP(new JSONfloat(node.getTranslate().z));

        	if (!translationValue.get() || !translationX.get() || !translationY.get() || !translationZ.get())
        	{
        		return;
        	}

        	translationValue->addValue(translationX);
        	translationValue->addValue(translationY);
        	translationValue->addValue(translationZ);

        	//

        	currentNode->addKeyValue("translation", translationValue);
    	}

    	//

    	auto nameValue = JSONstringSP(new JSONstring(node.getName()));

    	if (!nameValue.get())
    	{
    		return;
    	}

    	currentNode->addKeyValue("name", nameValue);

    	//
    	// Process children index.
    	//

    	if (node.getParentNode().get())
    	{
    		if (!nodeNameToChildren.contains(node.getParentNode()->getName()))
    		{
    			return;
    		}

    		auto parentChildren = nodeNameToChildren[node.getParentNode()->getName()];

    		//

        	auto nodeIndex = JSONintegerSP(new JSONinteger(nodeCounter));

        	if (!nodeIndex.get())
        	{
        		return;
        	}

        	parentChildren->addValue(nodeIndex);
    	}

    	//

    	nodeCounter++;
    }

};

} /* namespace vkts */

#endif /* VKTS_GLTFPARAMETER_HPP_ */
