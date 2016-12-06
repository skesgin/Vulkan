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

#ifndef VKTS_MATH_HPP_
#define VKTS_MATH_HPP_

/**
 *
 * Depends on VKTS core.
 *
 */

#include <vkts/core/vkts_core.hpp>

/**
 *
 * VKTS Start.
 *
 */

/**
 * Defines.
 */

#define VKTS_MATH_PI                     3.1415926535897932384626433832795f

/**
 * Random.
 */

#include <vkts/math/random/fn_random.hpp>

/**
 * Matrix.
 */

#include <vkts/math/matrix/fn_matrix.hpp>
#include <vkts/math/matrix/fn_matrix_viewprojection.hpp>

/**
 * Quaternion.
 */

#include <vkts/math/quaternion/Quat.hpp>

#include <vkts/math/quaternion/fn_quat.hpp>

/**
 * Geometry.
 */

#include <vkts/math/geometry/Plane.hpp>
#include <vkts/math/geometry/Sphere.hpp>

/**
 * Bounding volume.
 */

#include <vkts/math/bounding_volume/Obb.hpp>
#include <vkts/math/bounding_volume/Aabb.hpp>

/**
 * Culling.
 */

#include <vkts/math/culling/Frustum.hpp>

#endif /* VKTS_MATH_HPP_ */
