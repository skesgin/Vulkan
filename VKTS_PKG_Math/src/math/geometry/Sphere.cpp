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

Sphere::Sphere() :
	c(0.0f, 0.0f, 0.0f, 1.0f), r(1.0f)
{
}

Sphere::Sphere(const float x, const float y, const float z, const float radius) :
	c(x, y, z, 1.0f), r(glm::abs(radius))
{
}

Sphere::Sphere(const glm::vec4& center, const float radius) :
	c(center), r(glm::abs(radius))
{
}

Sphere::~Sphere()
{
}

const glm::vec4& Sphere::getCenter() const
{
	return c;
}

float Sphere::getRadius() const
{
	return r;
}

float Sphere::distance(const glm::vec4& point) const
{
	return glm::abs(glm::distance(c, point)) - r;
}

float Sphere::distance(const Plane& plane) const
{
	return plane.distance(*this);
}

float Sphere::distance(const Sphere& sphere) const
{
	return glm::abs(glm::distance(c, sphere.getCenter())) - r - sphere.getRadius();
}

Sphere Sphere::operator +(const Sphere& other) const
{
	glm::vec4 center = (c + other.getCenter()) * 0.5f;

	float radius = glm::distance(c, center) + glm::max(r, other.getRadius());

	return Sphere(center, radius);
}

Sphere& Sphere::operator +=(const Sphere& other)
{
	glm::vec4 center = (c + other.getCenter()) * 0.5f;

	float radius = glm::distance(c, center) + glm::max(r, other.getRadius());

	//

	c = center;

	r = radius;

	return *this;
}

//

Sphere operator *(const glm::mat4& transform, const Sphere& s)
{
	float sx = glm::length(glm::vec3(glm::column(transform, 0)));
	float sy = glm::length(glm::vec3(glm::column(transform, 1)));
	float sz = glm::length(glm::vec3(glm::column(transform, 2)));

	float scale = glm::max(sx, glm::max(sy, sz));

	return Sphere(transform * s.getCenter(), s.getRadius() * scale);
}

} /* namespace vkts */
