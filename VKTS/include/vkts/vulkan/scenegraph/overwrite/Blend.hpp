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

#ifndef VKTS_BLEND_HPP_
#define VKTS_BLEND_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

class Blend : public OverwriteDraw
{

private:

	VkBool32 passTransparent;

public:

	Blend() :
		OverwriteDraw(), passTransparent(VK_FALSE)
    {
    }

    virtual ~Blend()
    {
    }

    virtual VkBool32 visit(const ISubMesh& subMesh, const ICommandBuffersSP& cmdBuffer, const SmartPointerVector<IGraphicsPipelineSP>& allGraphicsPipelines, const uint32_t currentBuffer, const std::map<uint32_t, VkTsDynamicOffset>& dynamicOffsetMappings) const
    {
    	VkBool32 transparent = VK_FALSE;

        if (subMesh.getPhongMaterial().get())
        {
        	transparent = subMesh.getPhongMaterial()->isTransparent();
        }
        else if (subMesh.getBSDFMaterial().get())
        {
            transparent = subMesh.getBSDFMaterial()->isTransparent();
        }

    	return (transparent && passTransparent) || (!transparent && !passTransparent);
    }

	VkBool32 getPassTransparent() const
	{
		return passTransparent;
	}

	void setPassTransparent(const VkBool32 passTransparent)
	{
		this->passTransparent = passTransparent;
	}
};

} /* namespace vkts */

#endif /* VKTS_BLEND_HPP_ */
