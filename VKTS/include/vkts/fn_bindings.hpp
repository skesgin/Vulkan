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

#ifndef VKTS_BINDINGS_HPP_
#define VKTS_BINDINGS_HPP_

#include <vkts/vkts.hpp>

#define VKTS_MAX_JOINTS 96

#define VKTS_MAX_JOINTS_BUFFERSIZE (VKTS_MAX_JOINTS * sizeof(float) * ((16 + 1) + (12 + 1)))

// Shader bindings.

#define VKTS_BINDING_UNIFORM_BUFFER_VIEWPROJECTION 				0
#define VKTS_BINDING_UNIFORM_BUFFER_TRANSFORM 					1

#define VKTS_BINDING_UNIFORM_BUFFER_LIGHT 						2

#define VKTS_BINDING_UNIFORM_SAMPLER_DISPLACEMENT 				3
#define VKTS_BINDING_UNIFORM_SAMPLER_EMISSIVE 					4
#define VKTS_BINDING_UNIFORM_SAMPLER_ALPHA 						5
#define VKTS_BINDING_UNIFORM_SAMPLER_NORMAL 					6

#define VKTS_BINDING_UNIFORM_SAMPLER_PHONG_AMBIENT 				7
#define VKTS_BINDING_UNIFORM_SAMPLER_PHONG_DIFFUSE 				8
#define VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR 			9
#define VKTS_BINDING_UNIFORM_SAMPLER_PHONG_SPECULAR_SHININESS 	10

#define VKTS_BINDING_UNIFORM_BUFFER_BONE_TRANSFORM 				11

#define VKTS_BINDING_UNIFORM_SAMPLER_SHADOW 					12

#define VKTS_BINDING_UNIFORM_BUFFER_SHADOW			 			13

// Amount of bindings stored in phong material.

#define VKTS_BINDING_UNIFORM_TRANSFORM_BINDING_COUNT				3

#define VKTS_BINDING_UNIFORM_PHONG_BINDING_COUNT					8
#define VKTS_BINDING_UNIFORM_PHONG_NO_DISPLACEMENT_BINDING_COUNT	7

#define VKTS_BINDING_UNIFORM_BONES_BINDING_COUNT					1

#define VKTS_BINDING_UNIFORM_SHADOW_BINDING_COUNT					2

// Amount of maximum bindings.

#define VKTS_BINDING_UNIFORM_BINDING_COUNT 							14

#endif /* VKTS_BINDINGS_HPP_ */
