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

#include "Material.hpp"

namespace vkts
{

void Material::destroyMaterial()
{
	try
	{
		for (uint32_t i = 0; i < materialData.size(); i++)
		{
			if (materialData[i].get())
			{
				materialData[i]->destroy();
			}
		}
		materialData.clear();
	}
	catch(const std::exception& e)
	{
    	logPrint(VKTS_LOG_ERROR, __FILE__, __LINE__, "Catched exception '%s'", e.what());
	}
}

Material::Material(const VkBool32 forwardRendering, const VkBool32 createData, const std::string& name) :
    forwardRendering(forwardRendering), createData(createData), name(name), materialData()
{
}

Material::Material(const Material& other) :
    forwardRendering(other.forwardRendering), createData(other.createData), name(other.name + "_clone"), materialData()
{
	for (uint32_t i = 0; i < other.materialData.size(); i++)
	{
		if (other.materialData[i].get())
		{
			auto currentMaterialData = other.materialData[i]->create(createData);

			if (!currentMaterialData.get())
			{
				return;
			}

			materialData.append(currentMaterialData);
		}
	}
}

Material::~Material()
{

}

} /* namespace vkts */
