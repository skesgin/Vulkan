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

#ifndef VKTS_QUAT_HPP_
#define VKTS_QUAT_HPP_

#include <vkts/math/vkts_math.hpp>

namespace vkts
{

class Quat
{

private:

    void multiply(Quat& result, const Quat& q0, const Quat& q1) const;

public:

    float x;
    float y;
    float z;
    float w;

    Quat();
    Quat(const float x, const float y, const float z, const float w);
    Quat(const Quat& other);
    Quat(const glm::vec3& other);
    Quat(const glm::vec4& other);
    ~Quat();

    bool operator ==(const Quat& other) const;

    bool operator !=(const Quat& other) const;


    Quat operator +() const;
    Quat operator -() const;


    float& operator[](const uint32_t index);
    const float& operator[](const uint32_t index) const;


    Quat operator +(const Quat& other) const;

    Quat& operator +=(const Quat& other);

    Quat operator -(const Quat& other) const;

    Quat& operator -=(const Quat& other);

    Quat operator *(const Quat& other) const;

    Quat& operator *=(const Quat& other);

    Quat operator *(const float other) const;

    Quat& operator *=(const float other);

    Quat operator /(const float other) const;

    Quat& operator /=(const float other);

    Quat& operator =(const Quat& other);


    glm::vec3 operator *(const glm::vec3& other) const;

    glm::vec4 operator *(const glm::vec4& other) const;

    glm::mat3 mat3() const;

    glm::mat4 mat4() const;

};

glm::mat4 operator* (const glm::mat4& matrix, const Quat& q);

glm::mat4 operator* (const Quat& q, const glm::mat4& matrix);

} /* namespace vkts */

#endif /* VKTS_QUAT_HPP_ */
