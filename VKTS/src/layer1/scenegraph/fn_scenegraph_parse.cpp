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

#include <vkts/vkts.hpp>

#include "../../fn_internal.hpp"

namespace vkts
{

VkBool32 VKTS_APIENTRY scenegraphParseSkipBuffer(const char* buffer)
{
    if (!buffer)
    {
        return VK_TRUE;
    }

    if (strncmp(buffer, "#", 1) == 0)
    {
        // Comment, just skip.

        return VK_TRUE;
    }
    else if (strncmp(buffer, " ", 1) == 0 || strncmp(buffer, "\t", 1) == 0 || strncmp(buffer, "\r", 1) == 0 || strncmp(buffer, "\n", 1) == 0 || strlen(buffer) == 0)
    {
        // Empty line, just skip.

        return VK_TRUE;
    }

    return VK_FALSE;
}

void VKTS_APIENTRY scenegraphParseUnknownBuffer(const char* buffer)
{
    if (!buffer)
    {
        return;
    }

    std::string unknown(buffer);

    if (unknown.length() > 0 && (unknown[unknown.length() - 1] == '\r' || unknown[unknown.length() - 1] == '\n'))
    {
        logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not parse line '%s'", unknown.substr(0, unknown.length() - 1).c_str());
    }
    else
    {
        logPrint(VKTS_LOG_WARNING, __FILE__, __LINE__, "Could not parse line '%s'", unknown.c_str());
    }
}

VkBool32 VKTS_APIENTRY scenegraphParseIsToken(const char* buffer, const char* token)
{
    if (!buffer || !token)
    {
        return VK_FALSE;
    }

    if (!(buffer[strlen(token)] == ' ' || buffer[strlen(token)] == '\t' || buffer[strlen(token)] == '\n' || buffer[strlen(token)] == '\r' || buffer[strlen(token)] == '\0'))
    {
        return VK_FALSE;
    }

    if (strncmp(buffer, token, strlen(token)) == 0)
    {
        return VK_TRUE;
    }

    return VK_FALSE;
}

VkBool32 VKTS_APIENTRY scenegraphParseString(const char* buffer, char* string)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %s", token, string) != 2)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseStringTuple(const char* buffer, char* string0, char* string1)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %s %s", token, string0, string1) != 3)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseStringFloat(const char* buffer, char* string, float* scalar)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %s %f", token, string, scalar) != 3)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseStringBool(const char* buffer, char* string, VkBool32* scalar)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    char value[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %s %s", token, string, value) != 3)
    {
        return VK_FALSE;
    }

    if (strcmp(value, "true") == 0)
    {
    	*scalar = VK_TRUE;
    }
    else if (strcmp(value, "false") == 0)
    {
    	*scalar = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseBool(const char* buffer, VkBool32* scalar)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    char value[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %s", token, value) != 2)
    {
        return VK_FALSE;
    }

    if (strcmp(value, "true") == 0)
    {
    	*scalar = VK_TRUE;
    }
    else if (strcmp(value, "false") == 0)
    {
    	*scalar = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseBoolTriple(const char* buffer, VkBool32* scalar0, VkBool32* scalar1, VkBool32* scalar2)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    char value0[VKTS_MAX_TOKEN_CHARS + 1];
    char value1[VKTS_MAX_TOKEN_CHARS + 1];
    char value2[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %s %s %s", token, value0, value1, value2) != 4)
    {
        return VK_FALSE;
    }

    if (strcmp(value0, "true") == 0)
    {
    	*scalar0 = VK_TRUE;
    }
    else if (strcmp(value0, "false") == 0)
    {
    	*scalar0 = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    if (strcmp(value1, "true") == 0)
    {
    	*scalar1 = VK_TRUE;
    }
    else if (strcmp(value1, "false") == 0)
    {
    	*scalar1 = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    if (strcmp(value2, "true") == 0)
    {
    	*scalar2 = VK_TRUE;
    }
    else if (strcmp(value2, "false") == 0)
    {
    	*scalar2 = VK_FALSE;
    }
    else
    {
    	return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseFloat(const char* buffer, float* scalar)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %f", token, scalar) != 2)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseVec2(const char* buffer, float vec2[2])
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %f %f", token, &vec2[0], &vec2[1]) != 3)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseVec3(const char* buffer, float vec3[3])
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %f %f %f", token, &vec3[0], &vec3[1], &vec3[2]) != 4)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseVec4(const char* buffer, float vec4[4])
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %f %f %f %f", token, &vec4[0], &vec4[1], &vec4[2], &vec4[3]) != 5)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseVec6(const char* buffer, float vec6[6])
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %f %f %f %f %f %f", token, &vec6[0], &vec6[1], &vec6[2], &vec6[3], &vec6[4], &vec6[5]) != 7)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseVec8(const char* buffer, float vec8[8])
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %f %f %f %f %f %f %f %f", token, &vec8[0], &vec8[1], &vec8[2], &vec8[3], &vec8[4], &vec8[5], &vec8[6], &vec8[7]) != 9)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseInt(const char* buffer, int32_t* scalar)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %d", token, scalar) != 2)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseIVec3(const char* buffer, int32_t ivec3[3])
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %d %d %d", token, &ivec3[0], &ivec3[1], &ivec3[2]) != 4)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

VkBool32 VKTS_APIENTRY scenegraphParseUIntHex(const char* buffer, uint32_t* scalar)
{
    if (!buffer)
    {
        return VK_FALSE;
    }

    char token[VKTS_MAX_TOKEN_CHARS + 1];

    if (sscanf(buffer, "%s %x", token, scalar) != 2)
    {
        return VK_FALSE;
    }

    return VK_TRUE;
}

}
