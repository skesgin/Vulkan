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

#ifndef VKTS_CAMERA_HPP_
#define VKTS_CAMERA_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class Camera : public ICamera
{

private:

    std::string name;

    enum CameraType cameraType;

    float zNear;
    float zFar;

    float fovy;

    float orthoScale;

    glm::ivec2 windowDimension;

    glm::mat4 view;

    glm::mat4 projection;

    void updateProjectionMatrix();

public:

	explicit Camera();
	Camera(const std::string& name, CameraType cameraType = CameraType::PerspectiveCamera, float zNear = 0.1f, float zFar = 100.f, float fovy = 45.0f, float orthoScale = 1.0f, const glm::ivec2& windowDimension = glm::ivec2(1));
    Camera(const Camera& other);
    Camera(Camera&& other) = delete;
    virtual ~Camera();

    Camera& operator =(const Camera& other) = delete;
    Camera& operator =(Camera && other) = delete;

    //
    // ICamera
    //

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;

    virtual enum CameraType getCameraType() const override;

    virtual void setCameraType(const enum CameraType lightType) override;

    virtual float getZNear() const override;

    virtual void setZNear(const float zNear) override;

    virtual float getZFar() const override;

    virtual void setZFar(const float zFar) override;


    virtual float getFovY() const override;

    virtual void setFovY(const float fovy) override;


    virtual float getOrthoScale() const override;

    virtual void setOrthoScale(const float orthoScale) override;


    virtual const glm::ivec2& getWindowDimension() const override;

    virtual void setWindowDimension(const glm::ivec2& windowDimension) override;


    virtual const glm::mat4& getViewMatrix() const override;

    virtual void updateViewMatrix(const glm::mat4& transform) override;

    virtual const glm::mat4& getProjectionMatrix() const override;

    //
    // ICloneable
    //

    virtual ICameraSP clone() const override;

};

} /* namespace vkts */

#endif /* VKTS_CAMERA_HPP_ */
