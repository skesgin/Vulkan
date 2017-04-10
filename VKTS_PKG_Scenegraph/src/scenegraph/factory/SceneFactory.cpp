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

#include "SceneFactory.hpp"

#include "../scene/Animation.hpp"
#include "../scene/BSDFMaterial.hpp"
#include "../scene/Camera.hpp"
#include "../scene/Channel.hpp"
#include "../scene/CopyConstraint.hpp"
#include "../scene/Light.hpp"
#include "../scene/LimitConstraint.hpp"
#include "../scene/Marker.hpp"
#include "../scene/Mesh.hpp"
#include "../scene/Node.hpp"
#include "../scene/Object.hpp"
#include "../scene/PhongMaterial.hpp"
#include "../scene/ParticleSystem.hpp"
#include "../scene/Scene.hpp"
#include "../scene/SubMesh.hpp"



namespace vkts
{

SceneFactory::SceneFactory(const ISceneRenderFactorySP& sceneRenderFactory, const VkBool32 gpu) :
	ISceneFactory(), sceneRenderFactory(sceneRenderFactory), gpu(gpu)
{
}

SceneFactory::~SceneFactory()
{
}

//
// ISceneFactory
//


ISceneRenderFactorySP SceneFactory::getSceneRenderFactory() const
{
	return sceneRenderFactory;
}

IPhongMaterialSP SceneFactory::createPhongMaterial(const ISceneManagerSP& sceneManager, const VkBool32 forwardRendering)
{
	auto material = IPhongMaterialSP(new PhongMaterial(forwardRendering));

	if (!material.get())
	{
		return IPhongMaterialSP();
	}

	for (VkDeviceSize i = 0; i < sceneRenderFactory->getBufferCount(); i++)
	{
		auto renderMaterial = sceneRenderFactory->createRenderMaterial(sceneManager);

		if (!renderMaterial.get())
		{
			return IPhongMaterialSP();
		}

		material->addRenderMaterial(renderMaterial);
	}

	return material;
}

//

IBSDFMaterialSP SceneFactory::createBSDFMaterial(const ISceneManagerSP& sceneManager, const VkBool32 forwardRendering)
{
	auto material = IBSDFMaterialSP(new BSDFMaterial(forwardRendering));

	if (!material.get())
	{
		return IBSDFMaterialSP();
	}

	for (VkDeviceSize i = 0; i < sceneRenderFactory->getBufferCount(); i++)
	{
		auto renderMaterial = sceneRenderFactory->createRenderMaterial(sceneManager);

		if (!renderMaterial.get())
		{
			return IBSDFMaterialSP();
		}

		material->addRenderMaterial(renderMaterial);
	}

	return material;
}

//

ISubMeshSP SceneFactory::createSubMesh(const ISceneManagerSP& sceneManager)
{
	auto subMesh = ISubMeshSP(new SubMesh());

	if (!subMesh.get())
	{
		return ISubMeshSP();
	}

	auto subMeshData = sceneRenderFactory->createRenderSubMesh(sceneManager);

	if (!subMeshData.get())
	{
		return ISubMeshSP();
	}

	subMesh->setRenderSubMesh(subMeshData);

	return subMesh;
}

IMeshSP SceneFactory::createMesh(const ISceneManagerSP& sceneManager)
{
	return IMeshSP(new Mesh());
}

//

IChannelSP SceneFactory::createChannel(const ISceneManagerSP& sceneManager)
{
	return IChannelSP(new Channel());
}

IMarkerSP SceneFactory::createMarker(const ISceneManagerSP& sceneManager)
{
	return IMarkerSP(new Marker());
}

IAnimationSP SceneFactory::createAnimation(const ISceneManagerSP& sceneManager)
{
	return IAnimationSP(new Animation());
}

//

ICameraSP SceneFactory::createCamera(const ISceneManagerSP& sceneManager)
{
	return ICameraSP(new Camera());
}

ICameraSP SceneFactory::createCamera(const ISceneManagerSP& sceneManager, const std::string& name, const CameraType cameraType = CameraType::PerspectiveCamera, const float zNear = 0.1f, const float zFar = 100.f, const float fovy = 45.0f, const float orthoScale = 1.0f, const glm::ivec2& windowDimension = glm::ivec2(1))
{
	return ICameraSP(new Camera(name, cameraType, zNear, zFar, fovy, orthoScale, windowDimension));
}

ILightSP SceneFactory::createLight(const ISceneManagerSP& sceneManager)
{
	return ILightSP(new Light());
}
ILightSP SceneFactory::createLight(const ISceneManagerSP& sceneManager, const std::string& name = "Light", const enum LightType lightType = LightType::DirectionalLight, const float stength = 1.0f, const glm::vec3& color = glm::vec3(1), const glm::vec4& direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
{
	return ILightSP(new Light(name, lightType, stength, color, direction));
}

IParticleSystemSP SceneFactory::createParticleSystem(const ISceneManagerSP& sceneManager)
{
	return IParticleSystemSP(new ParticleSystem());
}

//

IConstraintSP SceneFactory::createCopyConstraint(const ISceneManagerSP& sceneManager, const enum CopyConstraintType copyConstraintType)
{
	return IConstraintSP(new CopyConstraint(copyConstraintType));
}

IConstraintSP SceneFactory::createLimitConstraint(const ISceneManagerSP& sceneManager, const enum LimitConstraintType limitConstraintType)
{
	return IConstraintSP(new LimitConstraint(limitConstraintType));
}

//

INodeSP SceneFactory::createNode(const ISceneManagerSP& sceneManager)
{
	auto node = INodeSP(new Node());

	if (!node.get())
	{
		return INodeSP();
	}

	for (VkDeviceSize i = 0; i < sceneRenderFactory->getBufferCount(); i++)
	{
		auto renderNode = sceneRenderFactory->createRenderNode(sceneManager);

		if (!renderNode.get())
		{
			return INodeSP();
		}

		node->addRenderNode(renderNode);
	}

	return node;
}

INodeSP SceneFactory::createNode(const ISceneManagerSP& sceneManager, const std::string& name, const glm::vec3& translate = glm::vec3(0), const glm::vec3& rotate = glm::vec3(0), const glm::vec3& scale = glm::vec3(1))
{
	auto node = INodeSP(new Node(name, translate, rotate, scale));

	if (!node.get())
	{
		return INodeSP();
	}

	for (VkDeviceSize i = 0; i < sceneRenderFactory->getBufferCount(); i++)
	{
		auto renderNode = sceneRenderFactory->createRenderNode(sceneManager);

		if (!renderNode.get())
		{
			return INodeSP();
		}

		node->addRenderNode(renderNode);
	}

	return node;
}

//

IObjectSP SceneFactory::createObject(const ISceneManagerSP& sceneManager)
{
	return IObjectSP(new Object());
}

IObjectSP SceneFactory::createObject(const ISceneManagerSP& sceneManager, const std::string& name, const glm::vec3& translate = glm::vec3(0), const glm::vec3& rotate = glm::vec3(0), const glm::vec3& scale = glm::vec3(1))
{
	auto object = new Object();
	object->setRootNode(createNode(sceneManager, name + "_RootNode", translate, rotate, scale));
	return IObjectSP(object);
}

//

ISceneSP SceneFactory::createScene(const ISceneManagerSP& sceneManager)
{
	return ISceneSP(new Scene());
}

//

VkBool32 SceneFactory::useGPU() const
{
	return gpu;
}

} /* namespace vkts */
