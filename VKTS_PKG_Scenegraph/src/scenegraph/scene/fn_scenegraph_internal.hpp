/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT);
 *
 * Copyright (c); since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software");, to deal
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

#ifndef VKTS_FN_SCENEGRAPH_INTERNAL_HPP_
#define VKTS_FN_SCENEGRAPH_INTERNAL_HPP_

#include <vkts/scenegraph.hpp>

namespace vkts
{

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseSkipBuffer(const char* buffer);

VKTS_APICALL void VKTS_APIENTRY scenegraphParseUnknownBuffer(const char* buffer);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseIsToken(const char* buffer, const char* token);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseString(const char* buffer, char* string);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseStringTuple(const char* buffer, char* string0, char* string1);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseStringFloat(const char* buffer, char* string, float* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseStringBool(const char* buffer, char* string, VkBool32* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseBool(const char* buffer, VkBool32* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseBoolTriple(const char* buffer, VkBool32* scalar0, VkBool32* scalar1, VkBool32* scalar2);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseFloat(const char* buffer, float* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseVec2(const char* buffer, float vec2[2]);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseVec3(const char* buffer, float vec3[3]);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseVec4(const char* buffer, float vec4[4]);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseVec6(const char* buffer, float vec6[6]);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseVec8(const char* buffer, float vec8[8]);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseInt(const char* buffer, int32_t* scalar);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseIVec3(const char* buffer, int32_t ivec3[3]);

VKTS_APICALL VkBool32 VKTS_APIENTRY scenegraphParseUIntHex(const char* buffer, uint32_t* scalar);

//

VKTS_APICALL ITextureObjectSP VKTS_APIENTRY scenegraphCreateTextureObject(const float red, const float green, const float blue, const VkFormat format, const IContextSP& context);

}

#endif /* VKTS_FN_SCENEGRAPH_INTERNAL_HPP_ */
