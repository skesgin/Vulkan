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

#ifndef VKTS_MARKER_HPP_
#define VKTS_MARKER_HPP_

#include <vkts/vkts.hpp>

namespace vkts
{

class Marker: public IMarker
{

private:

    std::string name;

    float t;

public:

    Marker();
    Marker(const Marker& other);
    Marker(Marker&& other) = delete;
    virtual ~Marker();

    Marker& operator =(const Marker& other) = delete;

    Marker& operator =(Marker && other) = delete;

    //
    // IMarker
    //

    virtual const std::string& getName() const override;

    virtual void setName(const std::string& name) override;

    virtual float getTime() const override;

    virtual void setTime(const float t) override;

    //
    // ICloneable
    //

    virtual IMarkerSP clone() const override;

};

} /* namespace vkts */

#endif /* VKTS_MARKER_HPP_ */
