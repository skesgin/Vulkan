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

#ifndef VKTS_TOUCHPADINPUT_HPP_
#define VKTS_TOUCHPADINPUT_HPP_

#include <vkts/window.hpp>

namespace vkts
{

class TouchpadInput
{

private:

    VkBool32 pressed[VKTS_MAX_TOUCHPAD_SLOTS];

    mutable std::mutex mutexPressed;

    glm::ivec2 location[VKTS_MAX_TOUCHPAD_SLOTS];

    mutable std::mutex mutexLocation;

public:

    TouchpadInput();
    TouchpadInput(const TouchpadInput& other) = delete;
    TouchpadInput(TouchpadInput&& other) = delete;
    virtual ~TouchpadInput();

    TouchpadInput& operator =(const TouchpadInput& other) = delete;

    TouchpadInput& operator =(TouchpadInput && other) = delete;

    //

    VkBool32 getPressed(const int32_t slotIndex) const;

    void setPressed(const int32_t slotIndex, const VkBool32 pressed);

    //

    const glm::ivec2& getLocation(const int32_t slotIndex) const;

    void setLocation(const int32_t slotIndex, const glm::ivec2& location);

    void setLocationX(const int32_t slotIndex, const int32_t x);

    void setLocationY(const int32_t slotIndex, const int32_t y);

    //

    void resetTouchpad();

};

} /* namespace vkts */

#endif /* VKTS_TOUCHPADINPUT_HPP_ */
