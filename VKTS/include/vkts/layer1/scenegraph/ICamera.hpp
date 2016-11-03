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

#ifndef VKTS_ICAMERA_HPP_
#define VKTS_ICAMERA_HPP_

#include <vkts/vkts.hpp>

namespace vkts
{

enum CameraType {PerspectiveCamera, OrthogonalCamera};

class ICamera : public ICloneable<ICamera>
{

public:

    ICamera() :
        ICloneable<ICamera>()
    {
    }

    virtual ~ICamera()
    {
    }

    virtual const std::string& getName() const = 0;

    virtual void setName(const std::string& name) = 0;

    virtual enum CameraType getCameraType() const = 0;

    virtual void setCameraType(const enum CameraType lightType) = 0;

    virtual float getZNear() const = 0;

    virtual void setZNear(const float zNear) = 0;

    virtual float getZFar() const = 0;

    virtual void setZFar(const float zFar) = 0;


    virtual float getFovY() const = 0;

    virtual void setFovY(const float fovy) = 0;


    virtual float getOrthoScale() const = 0;

    virtual void setOrthoScale(const float orthoScale) = 0;


    virtual const glm::ivec2& getWindowDimension() const = 0;

    virtual void setWindowDimension(const glm::ivec2& windowDimension) = 0;


    virtual const glm::mat4& getViewMatrix() const = 0;

    virtual void updateViewMatrix(const glm::mat4& transform) = 0;

    virtual const glm::mat4& getProjectionMatrix() const = 0;

};

typedef std::shared_ptr<ICamera> ICameraSP;

} /* namespace vkts */

#endif /* VKTS_ICAMERA_HPP_ */
