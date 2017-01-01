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

#include "BinaryBuffer.hpp"

namespace vkts
{

BinaryBuffer::BinaryBuffer() :
    IBinaryBuffer(), data(0), pos(0)
{
}

BinaryBuffer::BinaryBuffer(const uint32_t size) :
    IBinaryBuffer(), data(size), pos(0)
{
}

BinaryBuffer::BinaryBuffer(const uint8_t* data, const uint32_t size) :
    IBinaryBuffer(), data(size), pos(0)
{
    memcpy(&(this->data[0]), data, size);
}

BinaryBuffer::BinaryBuffer(const std::vector<uint8_t>& data) :
	IBinaryBuffer(), data(std::move(data)), pos(0)
{
}

BinaryBuffer::~BinaryBuffer()
{
    reset();
}

//
// IBinaryBuffer
//

void BinaryBuffer::reset()
{
    data.clear();

    pos = 0;
}

const void* BinaryBuffer::getData() const
{
    return static_cast<const void*>(&data[0]);
}

const uint8_t* BinaryBuffer::getByteData() const
{
    return &data[0];
}

uint32_t BinaryBuffer::getSize() const
{
    return (uint32_t)data.size();
}

VkBool32 BinaryBuffer::seek(const int64_t offset, const VkTsSearch search)
{
    switch (search)
    {
        case VKTS_SEARCH_ABSOLUTE:
        {
            if (offset < 0 || offset > static_cast<int64_t>(getSize()))
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
                if (static_cast<int64_t>(getSize() - pos) < offset)
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

uint32_t BinaryBuffer::read(void* ptr, const uint32_t sizeElement, const uint32_t countElement)
{
    if (!ptr || sizeElement == 0 || countElement == 0)
    {
        return 0;
    }

    if (pos >= getSize())
    {
        return 0;
    }

    uint32_t bytesRead = sizeElement * countElement;

    bytesRead = glm::min(bytesRead, getSize() - pos);

    uint32_t countElementRead = bytesRead / sizeElement;

    bytesRead = sizeElement * countElementRead;

    memcpy(ptr, &data[pos], bytesRead);

    pos += bytesRead;

    return countElementRead;
}

uint32_t BinaryBuffer::write(const void* ptr, const uint32_t sizeElement, const uint32_t countElement)
{
    if (!ptr || sizeElement == 0 || countElement == 0)
    {
        return 0;
    }

    uint32_t bytesWrite = sizeElement * countElement;

    if (pos + bytesWrite > getSize())
    {
        data.resize(pos + bytesWrite, 0);
    }

    uint32_t countElementWrite = bytesWrite / sizeElement;

    bytesWrite = sizeElement * countElementWrite;

    memcpy(&data[pos], ptr, bytesWrite);

    pos += bytesWrite;

    return countElementWrite;
}

VkBool32 BinaryBuffer::copy(void* data, const uint32_t dataSize) const
{
    if (!data || !getData())
    {
        return VK_FALSE;
    }

    if (dataSize < getSize())
    {
    	return VK_FALSE;
    }

    memcpy(data, getData(), getSize());

    return VK_TRUE;
}

//
// ICloneable
//

IBinaryBufferSP BinaryBuffer::clone() const
{
	auto result = IBinaryBufferSP(new BinaryBuffer(getByteData(), getSize()));

	if (result.get() && result->getSize() != getSize())
	{
		return IBinaryBufferSP();
	}

    return result;
}

} /* namespace vkts */
