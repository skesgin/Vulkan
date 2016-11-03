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

#include "Camera.hpp"

namespace vkts
{

void Camera::updateProjectionMatrix()
{
	if (cameraType == PerspectiveCamera)
	{
		projection = perspectiveMat4(fovy, aspect, zNear, zFar);
	}
	else
	{
		projection = orthoMat4(ortho.x, ortho.y, ortho.z, ortho.w, zNear, zFar);
	}
}

Camera::Camera() :
    ICamera(), name(""), cameraType(PerspectiveCamera), zNear(0.1f), zFar(100.0f), aspect(1.0f), fovy(45.0f), ortho(-1.0f, 1.0f, -1.0f, 1.0f)
{
	updateProjectionMatrix();
}

Camera::Camera(const Camera& other) :
	ICamera(), name(other.name + "_clone"), cameraType(other.cameraType), zNear(other.zNear), zFar(other.zFar), aspect(other.aspect), fovy(other.fovy), ortho(other.ortho), view(other.view), projection(other.projection)
{
}

Camera::~Camera()
{
}

//
// ICamera
//

const std::string& Camera::getName() const
{
    return name;
}

void Camera::setName(const std::string& name)
{
    this->name = name;
}

enum CameraType Camera::getCameraType() const
{
	return cameraType;
}

void Camera::setCameraType(const enum CameraType cameraType)
{
	this->cameraType = cameraType;

	updateProjectionMatrix();
}


float Camera::getZNear() const
{
	return zNear;
}

void Camera::setZNear(const float znear)
{
	this->zNear = zNear;
	updateProjectionMatrix();
}

float Camera::getZFar() const
{
	return zFar;
}

void Camera::setZFar(const float zfar)
{
	this->zFar = zFar;
	updateProjectionMatrix();
}


float Camera::getAspect() const
{
	return aspect;
}

void Camera::setAspect(const float aspect)
{
	this->aspect = aspect;
	updateProjectionMatrix();
}

float Camera::getFovY() const
{
	return fovy;
}

void Camera::setFovY(const float fovy)
{
	this->fovy = fovy;
	updateProjectionMatrix();
}


float Camera::getLeft() const
{
	return ortho.x;
}

void Camera::setLeft(const float left)
{
	this->ortho.x = left;
	updateProjectionMatrix();
}

float Camera::getRight() const
{
	return ortho.y;
}

void Camera::setRight(const float right)
{
	this->ortho.y = right;
	updateProjectionMatrix();
}

float Camera::getBottom() const
{
	return ortho.z;
}

void Camera::setBottom(const float bottom)
{
	this->ortho.z = bottom;
	updateProjectionMatrix();
}

float Camera::getTop() const
{
	return this->ortho.w;
}

void Camera::setTop(const float top)
{
	this->ortho.w = top;
	updateProjectionMatrix();
}


const glm::mat4& Camera::getViewMatrix() const
{
	return view;
}

void Camera::updateViewMatrix(const glm::mat4& transform)
{
	this->view = transform * lookAtMat4(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, -1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

const glm::mat4& Camera::getProjectionMatrix() const
{
	return projection;
}

//
// ICloneable
//

ICameraSP Camera::clone() const
{
    return ICameraSP(new Camera(*this));
}

} /* namespace vkts */
