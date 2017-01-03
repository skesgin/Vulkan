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

#include <vkts/math/vkts_math.hpp>

namespace vkts
{

void Quat::multiply(Quat& result, const Quat& q0, const Quat& q1) const
{
    Quat temp;

    temp.x = q0.w * q1.x + q0.x * q1.w + q0.y * q1.z - q0.z * q1.y;
    temp.y = q0.w * q1.y - q0.x * q1.z + q0.y * q1.w + q0.z * q1.x;
    temp.z = q0.w * q1.z + q0.x * q1.y - q0.y * q1.x + q0.z * q1.w;
    temp.w = q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z;

    result = temp;
}

Quat::Quat() :
    x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{
}

Quat::Quat(const float x, const float y, const float z, const float w) :
    x(x), y(y), z(z), w(w)
{
}

Quat::Quat(const Quat& other) :
    x(other.x), y(other.y), z(other.z), w(other.w)
{
}

Quat::Quat(const glm::vec3& other) :
    x(other.x), y(other.y), z(other.z), w(0.0f)
{
}

Quat::Quat(const glm::vec4& other) :
    x(other.x), y(other.y), z(other.z), w(0.0f)
{
}

Quat::~Quat()
{
}

bool Quat::operator ==(const Quat& other) const
{
    return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
}

bool Quat::operator !=(const Quat& other) const
{
    return !(*this == other);
}

Quat Quat::operator +() const
{
	return *this;
}

float& Quat::operator[](const uint32_t index)
{
	if (index >= 4)
	{
		throw std::out_of_range(std::to_string(index) + " >= 4");
	}

	if (index == 0)
	{
		return x;
	}
	else if (index == 1)
	{
		return y;
	}
	else if (index == 2)
	{
		return z;
	}

	return w;
}

const float& Quat::operator[](const uint32_t index) const
{
	if (index >= 4)
	{
		throw std::out_of_range(std::to_string(index) + " >= 4");
	}

	if (index == 0)
	{
		return x;
	}
	else if (index == 1)
	{
		return y;
	}
	else if (index == 2)
	{
		return z;
	}

	return w;
}

Quat Quat::operator -() const
{
	return Quat(-x, -y, -z, -w);
}

Quat Quat::operator +(const Quat& other) const
{
    return Quat(x + other.x, y + other.y, z + other.z, w + other.w);
}

Quat& Quat::operator +=(const Quat& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;

    return *this;
}

Quat Quat::operator -(const Quat& other) const
{
    return Quat(x - other.x, y - other.y, z - other.z, w - other.w);
}

Quat& Quat::operator -=(const Quat& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;

    return *this;
}

Quat Quat::operator *(const Quat& other) const
{
    Quat result;

    multiply(result, *this, other);

    return result;
}

Quat& Quat::operator *=(const Quat& other)
{
    multiply(*this, *this, other);

    return *this;
}

Quat Quat::operator *(const float other) const
{
    return Quat(x * other, y * other, z * other, w * other);
}

Quat& Quat::operator *=(const float other)
{
    x *= other;
    y *= other;
    z *= other;
    w *= other;

    return *this;
}

Quat Quat::operator /(const float other) const
{
    return Quat(x / other, y / other, z / other, w / other);
}

Quat& Quat::operator /=(const float other)
{
    x /= other;
    y /= other;
    z /= other;
    w /= other;

    return *this;
}

Quat& Quat::operator =(const Quat& other)
{
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->w = other.w;

    return *this;
}

Quat& Quat::operator =(const Quat&& other)
{
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    this->w = other.w;

    return *this;
}

glm::vec3 Quat::operator *(const glm::vec3& other) const
{
    auto result = *this * Quat(other) * conjugate(*this);

    return glm::vec3(result.x, result.y, result.z);
}

glm::vec4 Quat::operator *(const glm::vec4& other) const
{
    auto result = *this * Quat(other) * conjugate(*this);

    return glm::vec4(result.x, result.y, result.z, 1.0f);
}

glm::mat3 Quat::mat3() const
{
    glm::mat3 matrix;

    matrix[0][0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
    matrix[0][1] = 2.0f * x * y + 2.0f * w * z;
    matrix[0][2] = 2.0f * x * z - 2.0f * w * y;

    matrix[1][0] = 2.0f * x * y - 2.0f * w * z;
    matrix[1][1] = 1.0f - 2.0f * x * x - 2.0f * z * z;
    matrix[1][2] = 2.0f * y * z + 2.0f * w * x;

    matrix[2][0] = 2.0f * x * z + 2.0f * w * y;
    matrix[2][1] = 2.0f * y * z - 2.0f * w * x;
    matrix[2][2] = 1.0f - 2.0f * x * x - 2.0f * y * y;

    return matrix;
}

glm::mat4 Quat::mat4() const
{
    return glm::mat4(mat3());
}

//

glm::mat4 operator* (const glm::mat4& matrix, const Quat& q)
{
	return matrix * q.mat4();
}

glm::mat4 operator* (const Quat& q, const glm::mat4& matrix)
{
	return q.mat4() * matrix;
}

} /* namespace vkts */
