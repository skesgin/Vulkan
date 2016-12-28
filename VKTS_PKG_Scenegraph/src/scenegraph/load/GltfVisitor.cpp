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
	JsonVisitor(), directory(directory), state(), gltfString(), gltfInteger(0), gltfFloat(0.0f), gltfIntegerArray{}, gltfFloatArray{}, arrayIndex(0), arraySize(0), numberArray(VK_FALSE), objectArray(VK_FALSE), gltfBuffer{}, gltfBufferView{}, gltfAccessor{}, gltfMesh{}, gltfNode{}, gltfScene{}, gltfPrimitive{}, allGltfBuffers(), allGltfBufferViews(), allGltfAccessors(), allGltfMeshes(), allGltfNodes(), allGltfScenes()
{
}

GltfVisitor::~GltfVisitor()
{
}

//

void GltfVisitor::visit(JSONnull& jsonNull)
{
	auto gltfState = state.top();

	if (gltfState == GltfState_Error)
	{
		return;
	}

	// No used, so always error.
	state.push(GltfState_Error);
}

void GltfVisitor::visit(JSONfalse& jsonFalse)
{
	auto gltfState = state.top();

	if (gltfState == GltfState_Error)
	{
		return;
	}

	// No used, so always error.
	state.push(GltfState_Error);
}

void GltfVisitor::visit(JSONtrue& jsonTrue)
{
	auto gltfState = state.top();

	if (gltfState == GltfState_Error)
	{
		return;
	}

	// No used, so always error.
	state.push(GltfState_Error);
}

void GltfVisitor::visit(JSONfloat& jsonFloat)
{
	auto gltfState = state.top();

	if (gltfState == GltfState_Error)
	{
		return;
	}

	gltfInteger = (int32_t)jsonFloat.getValue();
	gltfFloat = jsonFloat.getValue();
}

void GltfVisitor::visit(JSONinteger& jsonInteger)
{
	auto gltfState = state.top();

	if (gltfState == GltfState_Error)
	{
		return;
	}

	gltfInteger = jsonInteger.getValue();
	gltfFloat = (float)jsonInteger.getValue();
}

void GltfVisitor::visit(JSONstring& jsonString)
{
	auto gltfState = state.top();

	if (gltfState == GltfState_Error)
	{
		return;
	}

	gltfString = jsonString.getValue();
}

void GltfVisitor::visit(JSONarray& jsonArray)
{
	auto gltfState = state.top();

	if (gltfState == GltfState_Error)
	{
		return;
	}

	if (numberArray)
	{
		if (arrayIndex >= arraySize || jsonArray.size() != arraySize)
		{
			state.push(GltfState_Error);
			return;
		}

		for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
		{
			jsonArray.getValueAt(i)->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			gltfIntegerArray[arrayIndex] = gltfInteger;
			gltfFloatArray[arrayIndex] = gltfFloat;

			arrayIndex++;
		}
	}
	else if (objectArray)
	{
		if (gltfState == GltfState_Primitive)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				memset(&gltfPrimitive, 0, sizeof(GltfPrimitive));

				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				gltfMesh.primitives.append(gltfPrimitive);
			}
		}
		else
		{
			state.push(GltfState_Error);
			return;
		}
	}
	else
	{
		state.push(GltfState_Error);
		return;
	}
}

void GltfVisitor::visit(JSONobject& jsonObject)
{
	if (state.size() == 0)
	{
		state.push(GltfState_Start);
	}

	auto gltfState = state.top();

	//

	if (gltfState == GltfState_Error)
	{
		return;
	}
	else if (gltfState == GltfState_Start)
	{
		if (!jsonObject.hasKey("asset"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto asset = jsonObject.getValue("asset");

		state.push(GltfState_Asset);
		asset->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

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
	else if (gltfState == GltfState_Asset)
	{
		if (!jsonObject.hasKey("version"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto version = jsonObject.getValue("version");

		version->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		auto dotIndex = gltfString.find(".");

		if (dotIndex == gltfString.npos)
		{
			state.push(GltfState_Error);
			return;
		}

		auto majorName = gltfString.substr(0, dotIndex);
		auto minorName = gltfString.substr(dotIndex + 1);

		int32_t majorVersion = (int32_t)atoi(majorName.c_str());
		int32_t minorVersion = (int32_t)atoi(minorName.c_str());

		if (majorVersion != 1 || minorVersion != 1)
		{
			state.push(GltfState_Error);
			return;
		}
	}
	else if (gltfState == GltfState_Buffers)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (size_t i = 0; i < allKeys.size(); i++)
		{
			memset(&gltfBuffer, 0, sizeof(GltfBuffer));

			//

			auto currentBuffer = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Buffer);
			currentBuffer->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfBuffers[allKeys[i]] = gltfBuffer;
		}
	}
	else if (gltfState == GltfState_BufferViews)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (size_t i = 0; i < allKeys.size(); i++)
		{
			memset(&gltfBufferView, 0, sizeof(GltfBufferView));

			//

			auto currentBufferView = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_BufferView);
			currentBufferView->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfBufferViews[allKeys[i]] = gltfBufferView;
		}
	}
	else if (gltfState == GltfState_Accessors)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (size_t i = 0; i < allKeys.size(); i++)
		{
			memset(&gltfAccessor, 0, sizeof(GltfAccessor));

			//

			auto currentAccessor = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Accessor);
			currentAccessor->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfAccessors[allKeys[i]] = gltfAccessor;
		}
	}
	else if (gltfState == GltfState_Meshes)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (size_t i = 0; i < allKeys.size(); i++)
		{
			memset(&gltfMesh, 0, sizeof(GltfMesh));

			//

			auto currentMesh = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Mesh);
			currentMesh->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfMeshes[allKeys[i]] = gltfMesh;
		}
	}
	else if (gltfState == GltfState_Nodes)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (size_t i = 0; i < allKeys.size(); i++)
		{
			memset(&gltfNode, 0, sizeof(GltfNode));

			//

			auto currentNode = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Node);
			currentNode->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfNodes[allKeys[i]] = gltfNode;
		}
	}
	else if (gltfState == GltfState_Scenes)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (size_t i = 0; i < allKeys.size(); i++)
		{
			memset(&gltfScene, 0, sizeof(GltfNode));

			//

			auto currentScene = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Scene);
			currentScene->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfScenes[allKeys[i]] = gltfScene;
		}
	}
	else if (gltfState == GltfState_Buffer)
	{
		if (!jsonObject.hasKey("uri") ||  !jsonObject.hasKey("byteLength"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto uri = jsonObject.getValue("uri");

		uri->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		std::string finalFilename = directory + gltfString;

		auto binaryBuffer = fileLoadBinary(finalFilename.c_str());

		if (!binaryBuffer.get())
		{
			binaryBuffer = fileLoadBinary(gltfString.c_str());

			if (!binaryBuffer.get())
			{
				state.push(GltfState_Error);
				return;
			}
		}

		gltfBuffer.binaryBuffer = binaryBuffer;

		//

		auto byteLength = jsonObject.getValue("byteLength");

		byteLength->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfBuffer.byteLength = (int32_t)gltfInteger;

		if (gltfBuffer.byteLength != gltfBuffer.binaryBuffer->getSize())
		{
			state.push(GltfState_Error);
			return;
		}
	}
	else if (gltfState == GltfState_BufferView)
	{
		if (!jsonObject.hasKey("buffer") || !jsonObject.hasKey("byteOffset"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto buffer = jsonObject.getValue("buffer");

		buffer->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfBuffers.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfBufferView.buffer = &(allGltfBuffers[gltfString]);

		//

		auto byteOffset = jsonObject.getValue("byteOffset");

		byteOffset->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfBufferView.byteOffset = (size_t)gltfInteger;

		//

		auto byteLength = jsonObject.getValue("byteLength");

		byteLength->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfBufferView.byteLength = (size_t)gltfInteger;
	}
	else if (gltfState == GltfState_Accessor)
	{
		if (!jsonObject.hasKey("bufferView") || !jsonObject.hasKey("byteOffset") || !jsonObject.hasKey("componentType") || !jsonObject.hasKey("count") || !jsonObject.hasKey("type") || !jsonObject.hasKey("min") || !jsonObject.hasKey("max"))
		{
			state.push(GltfState_Error);
			return;
		}

		auto bufferView = jsonObject.getValue("bufferView");

		bufferView->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfBufferViews.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfAccessor.bufferView = &(allGltfBufferViews[gltfString]);

		//

		auto byteOffset = jsonObject.getValue("byteOffset");

		byteOffset->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfAccessor.byteOffset = (size_t)gltfInteger;

		//

		auto componentType = jsonObject.getValue("componentType");

		componentType->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (gltfInteger == 5120 ||
			gltfInteger == 5121 ||
			gltfInteger == 5122 ||
			gltfInteger == 5123 ||
			gltfInteger == 5125 ||
			gltfInteger == 5126)
		{
			gltfAccessor.componentType = gltfInteger;
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

		gltfAccessor.count = gltfInteger;

		//

		auto type = jsonObject.getValue("type");

		type->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (gltfString == "SCALAR")
		{
			arraySize = 1;
		}
		else if (gltfString == "VEC2")
		{
			arraySize = 2;
		}
		else if (gltfString == "VEC3")
		{
			arraySize = 3;
		}
		else if (gltfString == "VEC4" || gltfString == "MAT2")
		{
			arraySize = 4;
		}
		else if (gltfString == "MAT3")
		{
			arraySize = 9;
		}
		else if (gltfString == "MAT4")
		{
			arraySize = 16;
		}
		else
		{
			state.push(GltfState_Error);
			return;
		}

		numberArray = VK_TRUE;

		gltfAccessor.type = gltfString;

		//

		auto min = jsonObject.getValue("min");

		min->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (arrayIndex != arraySize)
		{
			state.push(GltfState_Error);
			return;
		}

		for (size_t i = 0; i < arraySize; i++)
		{
			switch (gltfAccessor.componentType)
			{
				case 5120:
					gltfAccessor.minByte.append((int8_t)gltfIntegerArray[i]);
					break;
				case 5121:
					gltfAccessor.minUnsignedByte.append((uint8_t)gltfIntegerArray[i]);
					break;
				case 5122:
					gltfAccessor.minShort.append((int16_t)gltfIntegerArray[i]);
					break;
				case 5123:
					gltfAccessor.minUnsignedShort.append((uint16_t)gltfIntegerArray[i]);
					break;
				case 5125:
					gltfAccessor.minUnsignedInteger.append(gltfIntegerArray[i]);
					break;
				case 5126:
					gltfAccessor.minFloat.append(gltfFloatArray[i]);
					break;
			}
		}

		arrayIndex = 0;

		//

		auto max = jsonObject.getValue("max");

		max->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (arrayIndex != arraySize)
		{
			state.push(GltfState_Error);
			return;
		}

		for (size_t i = 0; i < arraySize; i++)
		{
			switch (gltfAccessor.componentType)
			{
				case 5120:
					gltfAccessor.maxByte.append((int8_t)gltfIntegerArray[i]);
					break;
				case 5121:
					gltfAccessor.maxUnsignedByte.append((uint8_t)gltfIntegerArray[i]);
					break;
				case 5122:
					gltfAccessor.maxShort.append((int16_t)gltfIntegerArray[i]);
					break;
				case 5123:
					gltfAccessor.maxUnsignedShort.append((uint16_t)gltfIntegerArray[i]);
					break;
				case 5125:
					gltfAccessor.maxUnsignedInteger.append(gltfIntegerArray[i]);
					break;
				case 5126:
					gltfAccessor.maxFloat.append(gltfFloatArray[i]);
					break;
			}
		}

		arrayIndex = 0;

		arraySize = 0;

		numberArray = VK_FALSE;
	}
	else if (gltfState == GltfState_Mesh)
	{
		if (!jsonObject.hasKey("primitives"))
		{
			state.push(GltfState_Error);
			return;
		}

		objectArray = VK_TRUE;

		auto primitives = jsonObject.getValue("primitives");

		state.push(GltfState_Primitive);
		primitives->visit(*this);

		objectArray = VK_FALSE;

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		// TODO: Create and assign sub mesh.
	}
	else if (gltfState == GltfState_Node)
	{
		// TODO: Implement.
	}
	else if (gltfState == GltfState_Scene)
	{
		// TODO: Implement.
	}
	else if (gltfState == GltfState_Primitive)
	{
		// TODO: Implement next.
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
