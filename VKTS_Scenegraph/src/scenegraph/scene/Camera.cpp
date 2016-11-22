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

#include "../scene/Camera.hpp"

namespace vkts
{

void Camera::updateProjectionMatrix()
{
	if (cameraType == PerspectiveCamera)
	{
		projection = perspectiveMat4(fovy, (float)windowDimension.x / (float)windowDimension.y, zNear, zFar);
	}
	else
	{
		projection = orthoMat4(-(float)windowDimension.x * 0.5f, (float)windowDimension.x * 0.5f, -(float)windowDimension.y * 0.5f, (float)windowDimension.y * 0.5f, zNear, zFar);
	}
}

Camera::Camera() :
    ICamera(), name(""), cameraType(PerspectiveCamera), zNear(0.1f), zFar(100.0f), fovy(45.0f), orthoScale(1.0f), windowDimension(1, 1)
{
	updateProjectionMatrix();
}

Camera::Camera(const Camera& other) :
	ICamera(), name(other.name + "_clone"), cameraType(other.cameraType), zNear(other.zNear), zFar(other.zFar), fovy(other.fovy), orthoScale(other.orthoScale), windowDimension(other.windowDimension), view(other.view), projection(other.projection)
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

void Camera::setZNear(const float zNear)
{
	this->zNear = zNear;
	updateProjectionMatrix();
}

float Camera::getZFar() const
{
	return zFar;
}

void Camera::setZFar(const float zFar)
{
	this->zFar = zFar;
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


float Camera::getOrthoScale() const
{
	return orthoScale;
}

void Camera::setOrthoScale(const float orthoScale)
{
	this->orthoScale = orthoScale;
	updateProjectionMatrix();
}


const glm::ivec2& Camera::getWindowDimension() const
{
	return windowDimension;
}

void Camera::setWindowDimension(const glm::ivec2& windowDimension)
{
	this->windowDimension = windowDimension;
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
