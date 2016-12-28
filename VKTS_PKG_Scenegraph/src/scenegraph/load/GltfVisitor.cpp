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

#include "GltfVisitor.hpp"

namespace vkts
{

GltfVisitor::GltfVisitor(const std::string& directory) :
	JsonVisitor(), directory(directory), state(), currentString(), currentInteger(0), currentFloat(0.0f), currentBufferView{}, currentAccessor{}, allBuffers(), allBufferViews(), allAccessors()
{
}

GltfVisitor::~GltfVisitor()
{
}

//

void GltfVisitor::visit(JSONnull& jsonNull)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}

	// No used, so always error.
	state.push(GltfState_Error);
}

void GltfVisitor::visit(JSONfalse& jsonFalse)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}

	// No used, so always error.
	state.push(GltfState_Error);
}

void GltfVisitor::visit(JSONtrue& jsonTrue)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}

	// No used, so always error.
	state.push(GltfState_Error);
}

void GltfVisitor::visit(JSONfloat& jsonFloat)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}

	currentInteger = (int32_t)jsonFloat.getValue();
	currentFloat = jsonFloat.getValue();
}

void GltfVisitor::visit(JSONinteger& jsonInteger)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}

	currentInteger = jsonInteger.getValue();
	currentFloat = (float)jsonInteger.getValue();
}

void GltfVisitor::visit(JSONstring& jsonString)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}

	currentString = jsonString.getValue();
}

void GltfVisitor::visit(JSONarray& jsonArray)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}
	else
	{
		state.push(GltfState_Error);
		return;
	}

	state.pop();
}

void GltfVisitor::visit(JSONobject& jsonObject)
{
	if (state.size() == 0)
	{
		state.push(GltfState_Start);
	}

	auto currentState = state.top();

	//

	if (currentState == GltfState_Error)
	{
		return;
	}
	else if (currentState == GltfState_Start)
	{
		if (!jsonObject.hasKey("buffers"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto buffers = jsonObject.getValue("buffers");

		state.push(GltfState_Buffers);
		buffers->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		if (!jsonObject.hasKey("bufferViews"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto bufferViews = jsonObject.getValue("bufferViews");

		state.push(GltfState_BufferViews);
		bufferViews->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		if (!jsonObject.hasKey("accessors"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto accessors = jsonObject.getValue("accessors");

		state.push(GltfState_Accessors);
		accessors->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		if (!jsonObject.hasKey("meshes"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto meshes = jsonObject.getValue("meshes");

		state.push(GltfState_Meshes);
		meshes->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		if (!jsonObject.hasKey("nodes"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto nodes = jsonObject.getValue("nodes");

		state.push(GltfState_Nodes);
		nodes->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		if (!jsonObject.hasKey("scenes"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto scenes = jsonObject.getValue("scenes");

		state.push(GltfState_Scenes);
		scenes->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		state.push(GltfState_End);

		return;
	}
	else if (currentState == GltfState_Buffers)
	{
		const auto& allBuffers = jsonObject.getAllKeys();

		for (size_t i = 0; i < allBuffers.size(); i++)
		{
			auto currentBuffer = jsonObject.getValue(allBuffers[i]);

			state.push(GltfState_Buffer);
			currentBuffer->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			std::string finalFilename = directory + currentString;

			auto binaryBuffer = fileLoadBinary(finalFilename.c_str());

			if (!binaryBuffer.get())
			{
				binaryBuffer = fileLoadBinary(currentString.c_str());

				if (!binaryBuffer.get())
				{
					state.push(GltfState_Error);
					return;
				}
			}

			this->allBuffers[allBuffers[i]] = binaryBuffer;
		}
	}
	else if (currentState == GltfState_BufferViews)
	{
		const auto& allBufferViews = jsonObject.getAllKeys();

		for (size_t i = 0; i < allBufferViews.size(); i++)
		{
			memset(&currentBufferView, 0, sizeof(GltfBufferView));

			//

			auto currentBufferView = jsonObject.getValue(allBufferViews[i]);

			state.push(GltfState_BufferView);
			currentBufferView->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			this->allBufferViews[allBufferViews[i]] = this->currentBufferView;
		}
	}
	else if (currentState == GltfState_Accessors)
	{
		const auto& allAccessors = jsonObject.getAllKeys();

		for (size_t i = 0; i < allAccessors.size(); i++)
		{
			memset(&currentAccessor, 0, sizeof(GltfAccessor));

			//

			auto currentAccessor = jsonObject.getValue(allAccessors[i]);

			state.push(GltfState_Accessor);
			currentAccessor->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			this->allAccessors[allAccessors[i]] = this->currentAccessor;
		}
	}
	else if (currentState == GltfState_Meshes)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_Nodes)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_Scenes)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_Buffer)
	{
		if (!jsonObject.hasKey("uri"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto currentUri = jsonObject.getValue("uri");

		currentUri->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}
	else if (currentState == GltfState_BufferView)
	{
		if (!jsonObject.hasKey("buffer") || !jsonObject.hasKey("byteOffset"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto currentBuffer = jsonObject.getValue("buffer");

		currentBuffer->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allBuffers.contains(currentString))
		{
			state.push(GltfState_Error);
			return;
		}

		currentBufferView.buffer = allBuffers[currentString];

		//

		auto byteOffset = jsonObject.getValue("byteOffset");

		byteOffset->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		currentBufferView.byteOffset = (size_t)currentInteger;
	}
	else if (currentState == GltfState_Accessor)
	{
		if (!jsonObject.hasKey("bufferView") || !jsonObject.hasKey("byteOffset") || !jsonObject.hasKey("componentType") || !jsonObject.hasKey("count") || !jsonObject.hasKey("type"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto currentBufferView = jsonObject.getValue("bufferView");

		currentBufferView->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allBufferViews.contains(currentString))
		{
			state.push(GltfState_Error);
			return;
		}

		currentAccessor.bufferView = &(allBufferViews[currentString]);

		//

		auto byteOffset = jsonObject.getValue("byteOffset");

		byteOffset->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		currentAccessor.byteOffset = (size_t)currentInteger;

		//

		auto componentType = jsonObject.getValue("componentType");

		componentType->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (currentInteger == 5120 ||
			currentInteger == 5121 ||
			currentInteger == 5122 ||
			currentInteger == 5123 ||
			currentInteger == 5125 ||
			currentInteger == 5126)
		{
			currentAccessor.componentType = currentInteger;
		}
		else
		{
			state.push(GltfState_Error);
			return;
		}

		//

		auto count = jsonObject.getValue("count");

		count->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		currentAccessor.count = currentInteger;

		//

		auto type = jsonObject.getValue("type");

		type->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (currentString == "SCALAR" ||
			currentString == "VEC2" ||
			currentString == "VEC3" ||
			currentString == "VEC4" ||
			currentString == "MAT2" ||
			currentString == "MAT3" ||
			currentString == "MAT4")
		{
			currentAccessor.type = currentString;
		}
		else
		{
			state.push(GltfState_Error);
			return;
		}
	}
	else if (currentState == GltfState_Mesh)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_Node)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_Scene)
	{
		// TODO: Implement.
	}
	else
	{
		state.push(GltfState_Error);
		return;
	}

	state.pop();
}

enum GltfState GltfVisitor::getState() const
{
	if (state.size() > 0)
	{
		return state.top();
	}

	return GltfState_Error;
}

}
