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
	JsonVisitor(), directory(directory), state(), currentString(), currentInteger(0), currentFloat(0.0f), currentBool(VK_FALSE), currentBufferView{}, allBuffers(), allBufferViews()
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

	state.push(GltfState_Error);
}

void GltfVisitor::visit(JSONfalse& jsonFalse)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}

	currentBool = VK_FALSE;
}

void GltfVisitor::visit(JSONtrue& jsonTrue)
{
	auto currentState = state.top();

	if (currentState == GltfState_Error)
	{
		return;
	}

	currentBool = VK_TRUE;
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

		state.push(GltfState_GltfBuffers);
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

		state.push(GltfState_GltfBufferViews);
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

		state.push(GltfState_GltfAccessors);
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

		state.push(GltfState_GltfMeshes);
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

		state.push(GltfState_GltfNodes);
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

		state.push(GltfState_GltfScenes);
		scenes->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		state.push(GltfState_Success);

		return;
	}
	else if (currentState == GltfState_GltfBuffers)
	{
		const auto& allBuffers = jsonObject.getAllKeys();

		for (size_t i = 0; i < allBuffers.size(); i++)
		{
			auto currentBuffer = jsonObject.getValue(allBuffers[i]);

			state.push(GltfState_GltfBuffer);
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
	else if (currentState == GltfState_GltfBufferViews)
	{
		const auto& allBufferViews = jsonObject.getAllKeys();

		for (size_t i = 0; i < allBufferViews.size(); i++)
		{
			memset(&currentBufferView, 0, sizeof(GltfBufferView));

			//

			auto currentBufferView = jsonObject.getValue(allBufferViews[i]);

			state.push(GltfState_GltfBufferView);
			currentBufferView->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			this->allBufferViews[allBufferViews[i]] = this->currentBufferView;
		}
	}
	else if (currentState == GltfState_GltfAccessors)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_GltfMeshes)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_GltfNodes)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_GltfScenes)
	{
		// TODO: Implement.
	}
	else if (currentState == GltfState_GltfBuffer)
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
	else if (currentState == GltfState_GltfBufferView)
	{
		if (!jsonObject.hasKey("buffer") || !jsonObject.hasKey("byteOffset") || !jsonObject.hasKey("byteLength"))
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

		//

		auto byteLength = jsonObject.getValue("byteLength");

		byteLength->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		currentBufferView.byteLength = (size_t)currentInteger;
	}
	else
	{
		state.push(GltfState_Error);
		return;
	}

	state.pop();
}

}
