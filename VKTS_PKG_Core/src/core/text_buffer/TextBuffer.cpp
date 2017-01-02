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

#include "TextBuffer.hpp"

namespace vkts
{

TextBuffer::TextBuffer() :
    ITextBuffer(), text(""), pos(0)
{
}

TextBuffer::TextBuffer(const char* text) :
    ITextBuffer(), text(""), pos(0)
{
    if (text)
    {
        this->text = std::string(text);
    }
}

TextBuffer::TextBuffer(const std::string& text) :
    ITextBuffer(), text(std::move(text)), pos(0)
{
}

TextBuffer::~TextBuffer()
{
}

const char* TextBuffer::getString() const
{
    return static_cast<const char*>(text.c_str());
}

uint32_t TextBuffer::getLength() const
{
    return (uint32_t)text.length();
}

VkBool32 TextBuffer::seek(const int64_t offset, const VkTsSearch search)
{
    switch (search)
    {
        case VKTS_SEARCH_ABSOLUTE:
        {
            if (offset < 0 || offset > static_cast<int64_t>(text.length()))
            {
                return VK_FALSE;
            }

            pos = static_cast<uint32_t>(offset);

            return VK_TRUE;
        }
        break;
        case VKTS_SEARCH_RELATVE:
        {
            if (offset < 0)
            {
                if (static_cast<int64_t>(pos) < -offset)
                {
                    return VK_FALSE;
                }

                pos -= static_cast<uint32_t>(-offset);
            }
            else if (offset > 0)
            {
                if (static_cast<int64_t>(text.length() - pos) < offset)
                {
                    return VK_FALSE;
                }

                pos += static_cast<uint32_t>(offset);
            }

            return VK_TRUE;
        }
        break;
    }

    return VK_FALSE;
}

const char* TextBuffer::gets(char* str, const uint32_t num)
{
    if (!str || num == 0)
    {
        return nullptr;
    }

    if (pos >= (uint32_t)text.length())
    {
        return nullptr;
    }

    uint32_t strIndex = 0;

    while (strIndex < num)
    {
        str[strIndex] = text.c_str()[pos];

        pos++;

        // End of line.
        if (str[strIndex] == '\r')
        {
            str[strIndex] = '\0';

            if (pos < (uint32_t)text.length() && text.c_str()[pos] == '\n')
            {
            	pos++;
            }

            return str;
        }
        else if (str[strIndex] == '\n')
        {
            str[strIndex] = '\0';

            return str;
        }

        strIndex++;

        // Not enough space in target buffer.
        if (strIndex == num)
        {
            str[strIndex - 1] = '\0';

            return str;
        }

        // End of buffer.
        if (pos == (uint32_t)text.length())
        {
			str[strIndex] = '\0';

			return str;
        }
    }

    return str;
}

VkBool32 TextBuffer::puts(const char* str)
{
    if (!str)
    {
        return VK_FALSE;
    }

    if (pos >= (uint32_t)text.length())
    {
        text.append(str);

        pos = (uint32_t)text.length();

        return VK_TRUE;
    }

    uint32_t strLen = (uint32_t)strlen(str);

    uint32_t strIndex = 0;

    while (strIndex < strLen && pos < (uint32_t)text.length())
    {
        text[pos] = str[strIndex];

        pos++;
        strIndex++;
    }

    if (strIndex == strLen)
    {
    	return VK_TRUE;
    }

    while (strIndex < strLen)
    {
        text.push_back(str[strIndex]);

        strIndex++;
    }

    pos = (uint32_t)text.length();

    return VK_TRUE;
}

//
// ICloneable
//

ITextBufferSP TextBuffer::clone() const
{
	// If character pointer is passed, the text is copied.
    return ITextBufferSP(new TextBuffer(text.c_str()));
}

} /* namespace vkts */
