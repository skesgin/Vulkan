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

#include "Scene.hpp"

#include "Object.hpp"

namespace vkts
{

Scene::Scene() :
    IScene(), name(""), allObjects(), allCameras(), allLights(), environment(nullptr), diffuseEnvironment(nullptr), specularEnvironment(nullptr), lut(nullptr)
{
}

Scene::Scene(const Scene& other) :
    IScene(), name(other.name + "_clone")
{
    for (size_t i = 0; i < other.allObjects.size(); i++)
    {
        const auto& currentObject = other.allObjects[i];

        if (!currentObject.get())
        {
            name = "";

            allObjects.clear();

            break;
        }

        IObjectSP cloneObject = currentObject->clone();

        if (!cloneObject.get())
        {
            name = "";

            allObjects.clear();

            break;
        }

        allObjects.append(cloneObject);
    }

    for (size_t i = 0; i < other.allCameras.size(); i++)
    {
        const auto& currentCamera = other.allCameras[i];

        if (!currentCamera.get())
        {
            name = "";

            allCameras.clear();

            break;
        }

        ICameraSP cloneCamera = currentCamera->clone();

        if (!cloneCamera.get())
        {
            name = "";

            allCameras.clear();

            break;
        }

        allCameras.append(cloneCamera);
    }

    for (size_t i = 0; i < other.allLights.size(); i++)
    {
        const auto& currentLight = other.allLights[i];

        if (!currentLight.get())
        {
            name = "";

            allLights.clear();

            break;
        }

        ILightSP cloneLight = currentLight->clone();

        if (!cloneLight.get())
        {
            name = "";

            allLights.clear();

            break;
        }

        allLights.append(cloneLight);
    }

    environment = other.environment;
    diffuseEnvironment = other.diffuseEnvironment;
    specularEnvironment = other.specularEnvironment;
    lut = other.lut;
}

Scene::~Scene()
{
    destroy();
}

//
// IScene
//

const std::string& Scene::getName() const
{
    return name;
}

void Scene::setName(const std::string& name)
{
    this->name = name;
}


void Scene::addObject(const IObjectSP& object)
{
    allObjects.append(object);
}

VkBool32 Scene::removeObject(const IObjectSP& object)
{
    return allObjects.remove(object);
}

IObjectSP Scene::findObject(const std::string& name) const
{
    for (size_t i = 0; i < allObjects.size(); i++)
    {
    	if (allObjects[i]->getName() == name)
    	{
    		return allObjects[i];
    	}
    }

    return IObjectSP();
}

size_t Scene::getNumberObjects() const
{
    return allObjects.size();
}

const SmartPointerVector<IObjectSP>& Scene::getObjects() const
{
    return allObjects;
}


void Scene::addCamera(const ICameraSP& camera)
{
    allCameras.append(camera);
}

VkBool32 Scene::removeCamera(const ICameraSP& camera)
{
    return allCameras.remove(camera);
}

ICameraSP Scene::findCamera(const std::string& name) const
{
    for (size_t i = 0; i < allCameras.size(); i++)
    {
    	if (allCameras[i]->getName() == name)
    	{
    		return allCameras[i];
    	}
    }

    return ICameraSP();
}

size_t Scene::getNumberCameras() const
{
    return allCameras.size();
}

const SmartPointerVector<ICameraSP>& Scene::getCameras() const
{
    return allCameras;
}


void Scene::addLight(const ILightSP& light)
{
    allLights.append(light);
}

VkBool32 Scene::removeLight(const ILightSP& light)
{
    return allLights.remove(light);
}

ILightSP Scene::findLight(const std::string& name) const
{
    for (size_t i = 0; i < allLights.size(); i++)
    {
    	if (allLights[i]->getName() == name)
    	{
    		return allLights[i];
    	}
    }

    return ILightSP();
}

size_t Scene::getNumberLights() const
{
    return allLights.size();
}

const SmartPointerVector<ILightSP>& Scene::getLights() const
{
    return allLights;
}

void Scene::setEnvironment(const ITextureObjectSP& environment)
{
    this->environment = environment;
}

ITextureObjectSP Scene::getEnvironment() const
{
    return environment;
}

void Scene::setDiffuseEnvironment(const ITextureObjectSP& diffuseEnvironment)
{
    this->diffuseEnvironment = diffuseEnvironment;
}

ITextureObjectSP Scene::getDiffuseEnvironment() const
{
    return diffuseEnvironment;
}

void Scene::setSpecularEnvironment(const ITextureObjectSP& specularEnvironment)
{
    this->specularEnvironment = specularEnvironment;
}

ITextureObjectSP Scene::getSpecularEnvironment() const
{
    return specularEnvironment;
}

void Scene::setLut(const ITextureObjectSP& lut)
{
    this->lut = lut;
}

ITextureObjectSP Scene::getLut() const
{
    return lut;
}

void Scene::updateParameterRecursive(const Parameter* parameter, const uint32_t objectOffset, const uint32_t objectStep, const size_t objectLimit)
{
	if (parameter)
	{
		parameter->visit(*this);
	}

	//

    if (objectStep == 0)
    {
        return;
    }

    for (size_t i = (size_t) objectOffset; i < glm::min(allObjects.size(), objectLimit); i += (size_t) objectStep)
    {
        allObjects[i]->updateParameterRecursive(parameter);
    }
}

void Scene::updateDescriptorSetsRecursive(const uint32_t allWriteDescriptorSetsCount, VkWriteDescriptorSet* allWriteDescriptorSets, const uint32_t objectOffset, const uint32_t objectStep, const size_t objectLimit)
{
    if (objectStep == 0)
    {
        return;
    }

    for (size_t i = (size_t) objectOffset; i < glm::min(allObjects.size(), objectLimit); i += (size_t) objectStep)
    {
        allObjects[i]->updateDescriptorSetsRecursive(allWriteDescriptorSetsCount, allWriteDescriptorSets);
    }
}

void Scene::updateTransformRecursive(const double deltaTime, const uint64_t deltaTicks, const double tickTime, const uint32_t objectOffset, const uint32_t objectStep, const size_t objectLimit)
{
    if (objectStep == 0)
    {
        return;
    }

    for (size_t i = (size_t) objectOffset; i < glm::min(allObjects.size(), objectLimit); i += (size_t) objectStep)
    {
        allObjects[i]->updateTransformRecursive(deltaTime, deltaTicks, tickTime);
    }
}

void Scene::drawRecursive(const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const OverwriteDraw* renderOverwrite, const uint32_t dynamicOffsetCount, const uint32_t* dynamicOffsets, const uint32_t objectOffset, const uint32_t objectStep, const size_t objectLimit)
{
    const OverwriteDraw* currentOverwrite = renderOverwrite;
    while (currentOverwrite)
    {
    	if (!currentOverwrite->visit(*this, cmdBuffer, allGraphicsPipelines, dynamicOffsetCount, dynamicOffsets, objectOffset, objectStep, objectLimit))
    	{
    		return;
    	}

    	currentOverwrite = currentOverwrite->getNextOverwrite();
    }

    //

    if (objectStep == 0)
    {
        return;
    }

    for (size_t i = (size_t) objectOffset; i < glm::min(allObjects.size(), objectLimit); i += (size_t) objectStep)
    {
        allObjects[i]->drawRecursive(cmdBuffer, allGraphicsPipelines, renderOverwrite, dynamicOffsetCount, dynamicOffsets);
    }
}

//
// ICloneable
//

ISceneSP Scene::clone() const
{
	auto result = ISceneSP(new Scene(*this));

	if (result.get() && result->getNumberObjects() != getNumberObjects())
	{
		return ISceneSP();
	}

	if (result.get() && result->getNumberCameras() != getNumberCameras())
	{
		return ISceneSP();
	}


	if (result.get() && result->getNumberLights() != getNumberLights())
	{
		return ISceneSP();
	}

    return result;
}

//
// IDestroyable
//

void Scene::destroy()
{
	if (lut.get())
	{
		lut->destroy();

		lut.reset();
	}

	if (specularEnvironment.get())
	{
		specularEnvironment->destroy();

		specularEnvironment.reset();
	}

	if (diffuseEnvironment.get())
	{
		diffuseEnvironment->destroy();

		diffuseEnvironment.reset();
	}

	if (environment.get())
	{
		environment->destroy();

		environment.reset();
	}

    for (size_t i = 0; i < allObjects.size(); i++)
    {
        allObjects[i]->destroy();
    }
    allObjects.clear();

    allCameras.clear();

    allLights.clear();
}

} /* namespace vkts */
