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

#ifndef VKTS_SCENEVISITOR_HPP_
#define VKTS_SCENEVISITOR_HPP_

#include <vkts/scenegraph.hpp>

#include "../scene/Scene.hpp"
#include "../scene/Object.hpp"
#include "../scene/Node.hpp"
#include "../scene/ParticleSystem.hpp"
#include "../scene/Mesh.hpp"
#include "../scene/SubMesh.hpp"
#include "../scene/PhongMaterial.hpp"
#include "../scene/BSDFMaterial.hpp"

namespace vkts
{

class SceneVisitor
{

private:

	SceneVisitor* nextSceneVisitor;

public:

	SceneVisitor() :
		nextSceneVisitor(nullptr)
    {
    }

	SceneVisitor(SceneVisitor* nextSceneVisitor) :
		nextSceneVisitor(nextSceneVisitor)
    {
    }

    virtual ~SceneVisitor()
    {
    }

    //

	SceneVisitor* getNextSceneVisitor() const
	{
		return nextSceneVisitor;
	}

	void setNextSceneVisitor(SceneVisitor* nextSceneVisitor)
	{
		this->nextSceneVisitor = nextSceneVisitor;
	}

    //

    virtual VkBool32 visit(Scene& scene, const uint32_t objectOffset, const uint32_t objectStep, const size_t objectLimit)
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(Object& object)
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(Node& node)
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(ParticleSystem& particleSystem)
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(Mesh& mesh)
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(SubMesh& subMesh)
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(PhongMaterial& material)
    {
    	return VK_TRUE;
    }

    virtual VkBool32 visit(BSDFMaterial& material)
    {
    	return VK_TRUE;
    }
};

} /* namespace vkts */

#endif /* VKTS_SCENEVISITOR_HPP_ */
