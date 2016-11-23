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

#ifndef VKTS_SCENEGRAPH_HPP_
#define VKTS_SCENEGRAPH_HPP_

/**
 *
 * Depends on VKTS runtime, entity and Vulkan object.
 *
 */

#include <vkts/vk_object.hpp>

#include <vkts/entity.hpp>

#include <vkts/runtime.hpp>

/**
 *
 * VKTS Start.
 *
 */

/**
 * Defines.
 */

#include <vkts/fn_bindings.hpp>

#define VKTS_BSDF_LENGTH 512
#define VKTS_BSDF_M 8
#define VKTS_BSDF_M_CUBE_MAP 10

/**
 * Types.
 */

typedef enum VkTsTargetTransform_
{
    VKTS_TARGET_TRANSFORM_TRANSLATE = 0,
    VKTS_TARGET_TRANSFORM_ROTATE = 1,
    VKTS_TARGET_TRANSFORM_QUATERNION_ROTATE = 2,
    VKTS_TARGET_TRANSFORM_SCALE = 3
} VkTsTargetTransform;

typedef enum VkTsTargetTransformElement_
{
    VKTS_TARGET_TRANSFORM_ELEMENT_X = 0,
    VKTS_TARGET_TRANSFORM_ELEMENT_Y = 1,
    VKTS_TARGET_TRANSFORM_ELEMENT_Z = 2,
	VKTS_TARGET_TRANSFORM_ELEMENT_W = 3
} VkTsTargetTransformElement;

typedef enum VkTsInterpolator_
{
    VKTS_INTERPOLATOR_CONSTANT = 0,
    VKTS_INTERPOLATOR_LINEAR = 1,
    VKTS_INTERPOLATOR_BEZIER = 2
} VkTsInterpolator;

/**
 * Scene.
 */

#include <vkts/scenegraph/scene/IChannel.hpp>

#include <vkts/scenegraph/scene/IMarker.hpp>

#include <vkts/scenegraph/scene/IBSDFMaterial.hpp>

#include <vkts/scenegraph/scene/IPhongMaterial.hpp>

#include <vkts/scenegraph/scene/ISubMesh.hpp>

#include <vkts/scenegraph/scene/IAnimation.hpp>

#include <vkts/scenegraph/scene/IConstraint.hpp>

#include <vkts/scenegraph/scene/ICamera.hpp>

#include <vkts/scenegraph/scene/ILight.hpp>

#include <vkts/scenegraph/scene/IMesh.hpp>

#include <vkts/scenegraph/scene/INode.hpp>

#include <vkts/scenegraph/scene/IObject.hpp>

#include <vkts/scenegraph/scene/IParticleSystem.hpp>

#include <vkts/scenegraph/scene/IScene.hpp>

#include <vkts/scenegraph/scene/IContext.hpp>

#include <vkts/scenegraph/scene/fn_scenegraph.hpp>

/**
 * Render overwrites.
 */

#include <vkts/scenegraph/overwrite/Overwrite.hpp>

#include <vkts/scenegraph/overwrite/Blend.hpp>
#include <vkts/scenegraph/overwrite/Cull.hpp>
#include <vkts/scenegraph/overwrite/Displace.hpp>

/**
 * Interpolator.
 */

#include <vkts/scenegraph/interpolator/fn_interpolator.hpp>

#endif /* VKTS_SCENEGRAPH_HPP_ */
