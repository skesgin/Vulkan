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

#include "../../composition/texture_object/TextureObject.hpp"

namespace vkts
{

TextureObject::TextureObject(const IContextObjectSP& contextObject, const std::string& name, const IImageObjectSP& imageObject, const ISamplerSP& sampler) :
    ITextureObject(), contextObject(contextObject), name(name), imageObject(imageObject), sampler(sampler)
{
}

TextureObject::~TextureObject()
{
    destroy();
}

//
// ITextureObject
//

const IContextObjectSP& TextureObject::getContextObject() const
{
    return contextObject;
}

const std::string& TextureObject::getName() const
{
    return name;
}

const IImageObjectSP& TextureObject::getImageObject() const
{
    return imageObject;
}

const ISamplerSP& TextureObject::getSampler() const
{
    return sampler;
}

//
// IDestroyable
//

void TextureObject::destroy()
{
	// Sampler can be used by several texture objects, so just reset.
	sampler.reset();

    if (imageObject.get())
    {
    	imageObject->destroy();
    }
}

} /* namespace vkts */
