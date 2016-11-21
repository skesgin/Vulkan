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

#include "UserCamera.hpp"

namespace vkts
{

void UserCamera::updateViewMatrix()
{
    viewMatrix = lookAtMat4(glm::vec4(translate, 1.0f), glm::vec4(translate + forward, 1.0f), up);
}

void UserCamera::update()
{
    updateViewMatrix();
}

UserCamera::UserCamera() :
    IUserCamera()
{
    updateViewMatrix();
}

UserCamera::UserCamera(const glm::vec4& eye, const glm::vec3& rotation) :
    IUserCamera()
{
    setTranslateRotate(glm::vec3(eye), rotation);
}

UserCamera::UserCamera(const glm::vec4& eye, const Quat& rotationZ, const Quat& rotationY, const Quat& rotationX) :
    IUserCamera()
{
    setTranslateRotate(glm::vec3(eye), rotationZ, rotationY, rotationX);
}

UserCamera::UserCamera(const UserCamera& other) :
    IUserCamera(), viewMatrix(other.viewMatrix)
{
}

UserCamera::~UserCamera()
{
}

UserCamera& UserCamera::operator =(const UserCamera& other)
{
    this->translate = other.translate;
    this->rotateZ = other.rotateZ;
    this->rotateY = other.rotateY;
    this->rotateX = other.rotateX;

    this->targetTranslate = other.targetTranslate;
    this->targetRotateZ = other.targetRotateZ;
    this->targetRotateY = other.targetRotateY;
    this->targetRotateX = other.targetRotateX;

    this->forward = other.forward;
    this->left = other.left;
    this->up = other.up;

    this->viewMatrix = other.viewMatrix;

    return *this;
}

const glm::mat4& UserCamera::getViewMatrix() const
{
    return viewMatrix;
}

//
// ICloneable
//

IUserCameraSP UserCamera::clone() const
{
    return IUserCameraSP(new UserCamera(*this));
}

} /* namespace vkts */
