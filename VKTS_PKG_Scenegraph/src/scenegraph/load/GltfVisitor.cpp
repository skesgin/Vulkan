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
	JsonVisitor(), directory(directory), state(), gltfBool(VK_FALSE), gltfString(), gltfInteger(0), gltfFloat(0.0f), gltfIntegerArray{}, gltfFloatArray{}, arrayIndex(0), arraySize(0), numberArray(VK_FALSE), objectArray(VK_FALSE), gltfBuffer{}, gltfBufferView{}, gltfAccessor{}, gltfPrimitive{}, gltfImage{}, gltfSampler{}, gltfTexture{}, gltfMaterial{}, gltfMesh{}, gltfSkin{}, gltfNode{}, gltfAnimation_Sampler{}, gltfChannel{}, gltfAnimation{}, gltfScene{}, allGltfBuffers(), allGltfBufferViews(), allGltfAccessors(), allGltfImages(), allGltfSamplers(), allGltfTextures(), allGltfMaterials(), allGltfMeshes(), allGltfSkins(), allGltfNodes(), allGltfAnimations(), allGltfScenes(), defaultScene(nullptr)
{
}

GltfVisitor::~GltfVisitor()
{
}

//

void GltfVisitor::visitBuffer(JSONobject& jsonObject)
{
	// Not processing extensions, extras.

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

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfBuffer.name = gltfString;
	}
}

void GltfVisitor::visitBufferView(JSONobject& jsonObject)
{
	// Not processing target, extensions, extras.

	//

	if (!jsonObject.hasKey("buffer") || !jsonObject.hasKey("byteOffset") || !jsonObject.hasKey("byteLength"))
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

	gltfBufferView.byteOffset = (uint32_t)gltfInteger;

	//

	auto byteLength = jsonObject.getValue("byteLength");

	byteLength->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	gltfBufferView.byteLength = (uint32_t)gltfInteger;

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfBufferView.name = gltfString;
	}
}

void GltfVisitor::visitAccessor(JSONobject& jsonObject)
{
	// Not processing extensions, extras.

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

	gltfAccessor.byteOffset = (uint32_t)gltfInteger;

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

	if (gltfInteger < 1)
	{
		state.push(GltfState_Error);
		return;
	}

	gltfAccessor.count = (uint32_t)gltfInteger;

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

	for (uint32_t i = 0; i < arraySize; i++)
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

	for (uint32_t i = 0; i < arraySize; i++)
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


	//

	// Optional

	if (jsonObject.hasKey("normalized"))
	{
		auto normalized = jsonObject.getValue("normalized");

		normalized->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfAccessor.normalized = gltfBool;
	}

	//

	if (jsonObject.hasKey("byteStride"))
	{
		auto byteStride = jsonObject.getValue("byteStride");

		byteStride->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfAccessor.byteStride = (uint32_t)gltfInteger;
	}

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfAccessor.name = gltfString;
	}
}

void GltfVisitor::visitImage(JSONobject& jsonObject)
{
	// Nor processing extensions, extras.

	if (!jsonObject.hasKey("uri"))
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

	auto imageData = imageDataLoad(finalFilename.c_str());

	if (!imageData.get())
	{
		imageData = imageDataLoad(gltfString.c_str());

		if (!imageData.get())
		{
			state.push(GltfState_Error);
			return;
		}
	}

	gltfImage.imageData = imageData;

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfImage.name = gltfString;
	}
}

void GltfVisitor::visitSampler(JSONobject& jsonObject)
{
	// Nor processing extensions, extras.

	if (jsonObject.hasKey("magFilter"))
	{
		auto magFilter = jsonObject.getValue("magFilter");

		magFilter->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		switch (gltfInteger)
		{
			case 9728:
			case 9729:
				gltfSampler.magFilter = gltfInteger;
			break;
			default:
				state.push(GltfState_Error);
				return;
		}
	}

	if (jsonObject.hasKey("minFilter"))
	{
		auto minFilter = jsonObject.getValue("minFilter");

		minFilter->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		switch (gltfInteger)
		{
			case 9728:
			case 9729:
			case 9984:
			case 9985:
			case 9986:
			case 9987:
				gltfSampler.minFilter = gltfInteger;
				return;
			break;
			default:
				state.push(GltfState_Error);
				return;
		}
	}

	if (jsonObject.hasKey("wrapS"))
	{
		auto wrapS = jsonObject.getValue("wrapS");

		wrapS->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		switch (gltfInteger)
		{
			case 33071:
			case 33648:
			case 10497:
				gltfSampler.wrapS = gltfInteger;
				return;
			break;
			default:
				state.push(GltfState_Error);
				return;
		}
	}

	if (jsonObject.hasKey("wrapT"))
	{
		auto wrapT = jsonObject.getValue("wrapT");

		wrapT->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		switch (gltfInteger)
		{
			case 33071:
			case 33648:
			case 10497:
				gltfSampler.wrapT = gltfInteger;
				return;
			break;
			default:
				state.push(GltfState_Error);
				return;
		}
	}

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfSampler.name = gltfString;
	}
}

void GltfVisitor::visitTexture(JSONobject& jsonObject)
{
	// Not processing extensions, extras.

	if (!jsonObject.hasKey("sampler") || !jsonObject.hasKey("source"))
	{
		state.push(GltfState_Error);
		return;
	}

	auto sampler = jsonObject.getValue("sampler");

	sampler->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	if (!allGltfSamplers.contains(gltfString))
	{
		state.push(GltfState_Error);
		return;
	}

	gltfTexture.sampler = &(allGltfSamplers[gltfString]);

	//

	auto source = jsonObject.getValue("source");

	source->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	if (!allGltfImages.contains(gltfString))
	{
		state.push(GltfState_Error);
		return;
	}

	gltfTexture.source = &(allGltfImages[gltfString]);

	//

	if (jsonObject.hasKey("format"))
	{
		auto format = jsonObject.getValue("format");

		format->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		switch (gltfInteger)
		{
			case 6406:
			case 6407:
			case 6408:
			case 6409:
			case 6410:
				gltfTexture.format = gltfInteger;
				return;
			break;
			default:
				state.push(GltfState_Error);
				return;
		}
	}

	if (jsonObject.hasKey("internalFormat"))
	{
		auto internalFormat = jsonObject.getValue("internalFormat");

		internalFormat->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		switch (gltfInteger)
		{
			case 6406:
			case 6407:
			case 6408:
			case 6409:
			case 6410:
				gltfTexture.internalFormat = gltfInteger;
				return;
			break;
			default:
				state.push(GltfState_Error);
				return;
		}
	}

	if (jsonObject.hasKey("target"))
	{
		auto target = jsonObject.getValue("target");

		target->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		switch (gltfInteger)
		{
			case 3553:
				gltfTexture.target = gltfInteger;
				return;
			break;
			default:
				state.push(GltfState_Error);
				return;
		}
	}

	if (jsonObject.hasKey("type"))
	{
		auto type = jsonObject.getValue("type");

		type->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		switch (gltfInteger)
		{
			case 5121:
			case 33635:
			case 32819:
			case 32820:
			gltfTexture.type = gltfInteger;
				return;
			break;
			default:
				state.push(GltfState_Error);
				return;
		}
	}


	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfTexture.name = gltfString;
	}
}

void GltfVisitor::visitMaterial(JSONobject& jsonObject)
{
	// Not processing technique, values, extras.

	if (jsonObject.hasKey("values") && !jsonObject.hasKey("technique"))
	{
		state.push(GltfState_Error);
		return;
	}

	if (jsonObject.hasKey("extensions"))
	{
		auto extensions = jsonObject.getValue("extensions");

		state.push(GltfState_Material_Extensions);
		extensions->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfMaterial.name = gltfString;
	}
}

void GltfVisitor::visitMesh(JSONobject& jsonObject)
{
	// Not processing extensions, extras.

	if (!jsonObject.hasKey("primitives"))
	{
		state.push(GltfState_Error);
		return;
	}

	objectArray = VK_TRUE;

	auto primitives = jsonObject.getValue("primitives");

	state.push(GltfState_Mesh_Primitive);
	primitives->visit(*this);

	objectArray = VK_FALSE;

	if (state.top() == GltfState_Error)
	{
		return;
	}

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfMesh.name = gltfString;
	}
}

void GltfVisitor::visitSkin(JSONobject& jsonObject)
{
	// Not processing extensions, extras.

	if (!jsonObject.hasKey("jointNames"))
	{
		state.push(GltfState_Error);
		return;
	}

	//

	objectArray = VK_TRUE;

	auto jointNames = jsonObject.getValue("jointNames");

	state.push(GltfState_Skin_JointNames);
	jointNames->visit(*this);

	objectArray = VK_FALSE;

	if (state.top() == GltfState_Error)
	{
		return;
	}

	//

	if (jsonObject.hasKey("bindShapeMatrix"))
	{
		numberArray = VK_TRUE;

		arrayIndex = 0;
		arraySize = 16;

		auto bindShapeMatrix = jsonObject.getValue("bindShapeMatrix");

		bindShapeMatrix->visit(*this);

		numberArray = VK_FALSE;

		arrayIndex = 0;
		arraySize = 0;

		if (state.top() == GltfState_Error)
		{
			return;
		}

		for (int32_t i = 0; i < 16; i++)
		{
			gltfSkin.bindShapeMatrix[i] = gltfFloatArray[i];
		}
	}

	//

	if (jsonObject.hasKey("inverseBindMatrices"))
	{
		objectArray = VK_TRUE;

		auto inverseBindMatrices = jsonObject.getValue("inverseBindMatrices");

		state.push(GltfState_Skin_InverseBindMatrices);
		inverseBindMatrices->visit(*this);

		objectArray = VK_FALSE;

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfSkin.name = gltfString;
	}
}

void GltfVisitor::visitNode(JSONobject& jsonObject)
{
	// Not processing camera, extensions, extras.

	if (jsonObject.hasKey("children"))
	{
		objectArray = VK_TRUE;

		auto children = jsonObject.getValue("children");

		state.push(GltfState_Node_Children);
		children->visit(*this);

		objectArray = VK_FALSE;

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}

	if (jsonObject.hasKey("skeletons"))
	{
		if (!jsonObject.hasKey("skin") || !jsonObject.hasKey("meshes"))
		{
			state.push(GltfState_Error);
			return;
		}

		//

		objectArray = VK_TRUE;

		auto skeletons = jsonObject.getValue("skeletons");

		state.push(GltfState_Node_Skeletons);
		skeletons->visit(*this);

		objectArray = VK_FALSE;

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}

	if (jsonObject.hasKey("skin"))
	{
		if (!jsonObject.hasKey("skeletons") || !jsonObject.hasKey("meshes"))
		{
			state.push(GltfState_Error);
			return;
		}

		//

		auto skin = jsonObject.getValue("skin");

		skin->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfSkins.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfNode.skin = &(allGltfSkins[gltfString]);
	}

	if (jsonObject.hasKey("jointName"))
	{
		auto jointName = jsonObject.getValue("jointName");

		jointName->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfNode.jointName = gltfString;
	}

	if (jsonObject.hasKey("matrix"))
	{
		numberArray = VK_TRUE;

		arrayIndex = 0;
		arraySize = 16;

		auto matrix = jsonObject.getValue("matrix");

		matrix->visit(*this);

		numberArray = VK_FALSE;

		arrayIndex = 0;
		arraySize = 0;

		if (state.top() == GltfState_Error)
		{
			return;
		}

		for (int32_t i = 0; i < 16; i++)
		{
			gltfNode.matrix[i] = gltfFloatArray[i];
		}

		gltfNode.useMatrix = VK_TRUE;
	}

	if (jsonObject.hasKey("meshes"))
	{
		objectArray = VK_TRUE;

		auto meshes = jsonObject.getValue("meshes");

		state.push(GltfState_Node_Mesh);
		meshes->visit(*this);

		objectArray = VK_FALSE;

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}

	if (jsonObject.hasKey("rotation"))
	{
		if (gltfNode.useMatrix)
		{
			state.push(GltfState_Error);
			return;
		}

		//

		numberArray = VK_TRUE;

		arrayIndex = 0;
		arraySize = 4;

		auto rotation = jsonObject.getValue("rotation");

		rotation->visit(*this);

		numberArray = VK_FALSE;

		arrayIndex = 0;
		arraySize = 0;

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfNode.rotation[0] = gltfFloatArray[0];
		gltfNode.rotation[1] = gltfFloatArray[1];
		gltfNode.rotation[2] = gltfFloatArray[2];
		gltfNode.rotation[3] = gltfFloatArray[3];

		gltfNode.useRotation = VK_TRUE;
	}

	if (jsonObject.hasKey("scale"))
	{
		if (gltfNode.useMatrix)
		{
			state.push(GltfState_Error);
			return;
		}

		//

		numberArray = VK_TRUE;

		arrayIndex = 0;
		arraySize = 3;

		auto scale = jsonObject.getValue("scale");

		scale->visit(*this);

		numberArray = VK_FALSE;

		arrayIndex = 0;
		arraySize = 0;

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfNode.scale[0] = gltfFloatArray[0];
		gltfNode.scale[1] = gltfFloatArray[1];
		gltfNode.scale[2] = gltfFloatArray[2];

		gltfNode.useScale = VK_TRUE;
	}

	if (jsonObject.hasKey("translation"))
	{
		if (gltfNode.useMatrix)
		{
			state.push(GltfState_Error);
			return;
		}

		//

		numberArray = VK_TRUE;

		arrayIndex = 0;
		arraySize = 3;

		auto translation = jsonObject.getValue("translation");

		translation->visit(*this);

		numberArray = VK_FALSE;

		arrayIndex = 0;
		arraySize = 0;

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfNode.translation[0] = gltfFloatArray[0];
		gltfNode.translation[1] = gltfFloatArray[1];
		gltfNode.translation[2] = gltfFloatArray[2];

		gltfNode.useTranslation = VK_TRUE;
	}

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfNode.name = gltfString;
	}
}

void GltfVisitor::visitAnimation(JSONobject& jsonObject)
{
	// Not processing extensions, extras.

	if (jsonObject.hasKey("samplers"))
	{
		auto samplers = jsonObject.getValue("samplers");

		state.push(GltfState_Animation_Sampler);
		samplers->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}

	//

	if (jsonObject.hasKey("channels"))
	{
		if (!jsonObject.hasKey("samplers"))
		{
			state.push(GltfState_Error);
			return;
		}

		//

		objectArray = VK_TRUE;

		auto channels = jsonObject.getValue("channels");

		state.push(GltfState_Animation_Channel);
		channels->visit(*this);

		objectArray = VK_FALSE;

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfAnimation.name = gltfString;
	}
}

void GltfVisitor::visitScene(JSONobject& jsonObject)
{
	// Not processing extensions, extras.

	if (jsonObject.hasKey("nodes"))
	{
		objectArray = VK_TRUE;

		auto nodes = jsonObject.getValue("nodes");

		state.push(GltfState_Scene_Node);
		nodes->visit(*this);

		objectArray = VK_FALSE;

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfScene.name = gltfString;
	}
}


void GltfVisitor::visitMaterial_Extensions(JSONobject& jsonObject)
{
	// Not processing other extensions.

	if (jsonObject.hasKey("FRAUNHOFER_materials_pbr"))
	{
		auto pbr = jsonObject.getValue("FRAUNHOFER_materials_pbr");

		state.push(GltfState_Material_Extensions_Pbr);
		pbr->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}
	}
}

void GltfVisitor::visitMaterial_Extensions_Pbr(JSONobject& jsonObject)
{
	// Not processing name, extensions, extras.

	if (!jsonObject.hasKey("materialModel") || !jsonObject.hasKey("values"))
	{
		state.push(GltfState_Error);
		return;
	}

	auto materialModel = jsonObject.getValue("materialModel");

	materialModel->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	if (gltfString != "PBR_metal_roughness")
	{
		state.push(GltfState_Error);
		return;
	}

	gltfMaterial.name = gltfString;

	//

	auto values = jsonObject.getValue("values");

	state.push(GltfState_Material_Extensions_Pbr_Values);
	values->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}
}

void GltfVisitor::visitMaterial_Extensions_Pbr_Values(JSONobject& jsonObject)
{
	if (jsonObject.hasKey("baseColorFactor"))
	{
		numberArray = VK_TRUE;

		arrayIndex = 0;
		arraySize = 4;

		auto baseColorFactor = jsonObject.getValue("baseColorFactor");

		baseColorFactor->visit(*this);

		numberArray = VK_FALSE;

		arrayIndex = 0;
		arraySize = 0;

		if (state.top() == GltfState_Error)
		{
			return;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			gltfMaterial.baseColorFactor[i] = gltfFloatArray[i];
		}
	}

	if (jsonObject.hasKey("baseColorTexture"))
	{
		auto baseColorTexture = jsonObject.getValue("baseColorTexture");

		baseColorTexture->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfTextures.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfMaterial.baseColorTexture = &(allGltfTextures[gltfString]);
	}

	if (jsonObject.hasKey("metallicFactor"))
	{
		auto metallicFactor = jsonObject.getValue("metallicFactor");

		metallicFactor->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfMaterial.metallicFactor = gltfFloat;
	}

	if (jsonObject.hasKey("metallicTexture"))
	{
		auto metallicTexture = jsonObject.getValue("metallicTexture");

		metallicTexture->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfTextures.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfMaterial.metallicTexture = &(allGltfTextures[gltfString]);
	}

	if (jsonObject.hasKey("roughnessFactor"))
	{
		auto roughnessFactor = jsonObject.getValue("roughnessFactor");

		roughnessFactor->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfMaterial.roughnessFactor = gltfFloat;
	}

	if (jsonObject.hasKey("roughnessTexture"))
	{
		auto roughnessTexture = jsonObject.getValue("roughnessTexture");

		roughnessTexture->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfTextures.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfMaterial.roughnessTexture = &(allGltfTextures[gltfString]);
	}

	if (jsonObject.hasKey("normalFactor"))
	{
		auto normalFactor = jsonObject.getValue("normalFactor");

		normalFactor->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfMaterial.normalFactor = gltfFloat;
	}

	if (jsonObject.hasKey("normalTexture"))
	{
		auto normalTexture = jsonObject.getValue("normalTexture");

		normalTexture->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfTextures.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfMaterial.normalTexture = &(allGltfTextures[gltfString]);
	}

	if (jsonObject.hasKey("aoFactor"))
	{
		auto aoFactor = jsonObject.getValue("aoFactor");

		aoFactor->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfMaterial.aoFactor = gltfFloat;
	}

	if (jsonObject.hasKey("aoTexture"))
	{
		auto aoTexture = jsonObject.getValue("aoTexture");

		aoTexture->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfTextures.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfMaterial.aoTexture = &(allGltfTextures[gltfString]);
	}

	if (jsonObject.hasKey("emissiveFactor"))
	{
		numberArray = VK_TRUE;

		arrayIndex = 0;
		arraySize = 4;

		auto emissiveFactor = jsonObject.getValue("emissiveFactor");

		emissiveFactor->visit(*this);

		numberArray = VK_FALSE;

		arrayIndex = 0;
		arraySize = 0;

		if (state.top() == GltfState_Error)
		{
			return;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			gltfMaterial.emissiveFactor[i] = gltfFloatArray[i];
		}
	}

	if (jsonObject.hasKey("emissiveTexture"))
	{
		auto emissiveTexture = jsonObject.getValue("emissiveTexture");

		emissiveTexture->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfTextures.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfMaterial.emissiveTexture = &(allGltfTextures[gltfString]);
	}
}

void GltfVisitor::visitMesh_Primitive(JSONobject& jsonObject)
{
	// Not processing extensions, extras.

	if (!jsonObject.hasKey("attributes"))
	{
		state.push(GltfState_Error);
		return;
	}

	auto attributes = jsonObject.getValue("attributes");

	state.push(GltfState_Mesh_Primitive_Attributes);
	attributes->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	// Optional

	if (jsonObject.hasKey("indices"))
	{
		auto indices = jsonObject.getValue("indices");

		indices->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfAccessors.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.indices = &(allGltfAccessors[gltfString]);
	}

	//

	if (jsonObject.hasKey("mode"))
	{
		auto mode = jsonObject.getValue("mode");

		mode->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (gltfInteger < 0 || gltfInteger > 5)
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.mode = gltfInteger;
	}

	if (jsonObject.hasKey("material"))
	{
		auto material = jsonObject.getValue("material");

		material->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfMaterials.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.material = &(allGltfMaterials[gltfString]);
	}

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfPrimitive.name = gltfString;
	}
}

void GltfVisitor::visitMesh_Primitive_Attributes(JSONobject& jsonObject)
{
	// Not processing TEXCOORD_x (x >= 1) and COLOR_x (x >= 0)

	//

	if (!jsonObject.hasKey("POSITION"))
	{
		state.push(GltfState_Error);
		return;
	}

	auto position = jsonObject.getValue("POSITION");

	position->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	gltfPrimitive.position = &(allGltfAccessors[gltfString]);

	// Optional

	if (jsonObject.hasKey("NORMAL"))
	{
		auto normal = jsonObject.getValue("NORMAL");

		normal->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfAccessors.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.normal = &(allGltfAccessors[gltfString]);
	}

	if (jsonObject.hasKey("BINORMAL"))
	{
		auto binormal = jsonObject.getValue("BINORMAL");

		binormal->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfAccessors.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.binormal = &(allGltfAccessors[gltfString]);
	}

	if (jsonObject.hasKey("TANGENT"))
	{
		auto tangent = jsonObject.getValue("TANGENT");

		tangent->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfAccessors.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.tangent = &(allGltfAccessors[gltfString]);
	}

	if (jsonObject.hasKey("TEXCOORD_0"))
	{
		auto texCoord = jsonObject.getValue("TEXCOORD_0");

		texCoord->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfAccessors.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.texCoord = &(allGltfAccessors[gltfString]);
	}

	if (jsonObject.hasKey("JOINT"))
	{
		auto joint = jsonObject.getValue("JOINT");

		joint->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfAccessors.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.joint = &(allGltfAccessors[gltfString]);
	}

	if (jsonObject.hasKey("WEIGHT"))
	{
		auto weight = jsonObject.getValue("WEIGHT");

		weight->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (!allGltfAccessors.contains(gltfString))
		{
			state.push(GltfState_Error);
			return;
		}

		gltfPrimitive.weight = &(allGltfAccessors[gltfString]);
	}
}

void GltfVisitor::visitAnimation_Sampler(JSONobject& jsonObject)
{
	const auto& allKeys = jsonObject.getAllKeys();

	for (uint32_t i = 0; i < allKeys.size(); i++)
	{
		gltfAnimation_Sampler.input = nullptr;
		gltfAnimation_Sampler.interpolation = "LINEAR";
		gltfAnimation_Sampler.output = nullptr;
		gltfAnimation_Sampler.name = allKeys[i];

		//

		auto currentBuffer = jsonObject.getValue(allKeys[i]);

		state.push(GltfState_Animation_Sampler_Properties);
		currentBuffer->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		//

		gltfAnimation.samplers[allKeys[i]] = gltfAnimation_Sampler;
	}
}

void GltfVisitor::visitAnimation_Sampler_Properties(JSONobject& jsonObject)
{
	// Not processing extensions, extra.

	if (!jsonObject.hasKey("input") || !jsonObject.hasKey("output"))
	{
		state.push(GltfState_Error);
		return;
	}

	auto input = jsonObject.getValue("input");

	input->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	if (!allGltfAccessors.contains(gltfString))
	{
		state.push(GltfState_Error);
		return;
	}

	gltfAnimation_Sampler.input = &(allGltfAccessors[gltfString]);

	//

	auto output = jsonObject.getValue("output");

	output->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	if (!allGltfAccessors.contains(gltfString))
	{
		state.push(GltfState_Error);
		return;
	}

	gltfAnimation_Sampler.output = &(allGltfAccessors[gltfString]);

	//

	// Optional

	if (jsonObject.hasKey("interpolation"))
	{
		auto interpolation = jsonObject.getValue("interpolation");

		interpolation->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		if (gltfString != "STEP" && gltfString != "LINEAR")
		{
			state.push(GltfState_Error);
			return;
		}

		gltfAnimation_Sampler.interpolation = gltfString;
	}

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfAnimation_Sampler.name = gltfString;
	}
}

void GltfVisitor::visitAnimation_Channel(JSONobject& jsonObject)
{
	// Not processing extensions, extra.

	if (!jsonObject.hasKey("sampler") || !jsonObject.hasKey("target"))
	{
		state.push(GltfState_Error);
		return;
	}

	auto sampler = jsonObject.getValue("sampler");

	sampler->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	if (!gltfAnimation.samplers.contains(gltfString))
	{
		state.push(GltfState_Error);
		return;
	}

	gltfChannel.sampler = &gltfAnimation.samplers[gltfString];

	//

	auto target = jsonObject.getValue("target");

	state.push(GltfState_Animation_Channel_Target);
	target->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	//

	if (jsonObject.hasKey("name"))
	{
		auto name = jsonObject.getValue("name");

		name->visit(*this);

		if (state.top() == GltfState_Error)
		{
			return;
		}

		gltfChannel.name = gltfString;
	}
}

void GltfVisitor::visitAnimation_Channel_Target(JSONobject& jsonObject)
{
	if (!jsonObject.hasKey("id") || !jsonObject.hasKey("path"))
	{
		state.push(GltfState_Error);
		return;
	}

	auto id = jsonObject.getValue("id");

	id->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	if (!allGltfNodes.contains(gltfString))
	{
		state.push(GltfState_Error);
		return;
	}

	gltfChannel.targetNode = &allGltfNodes[gltfString];

	//

	auto path = jsonObject.getValue("path");

	path->visit(*this);

	if (state.top() == GltfState_Error)
	{
		return;
	}

	if (gltfString != "translation" && gltfString != "rotation" && gltfString != "scale")
	{
		state.push(GltfState_Error);
		return;
	}

	gltfChannel.targetPath = gltfString;
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

	gltfBool = VK_FALSE;
}

void GltfVisitor::visit(JSONtrue& jsonTrue)
{
	auto gltfState = state.top();

	if (gltfState == GltfState_Error)
	{
		return;
	}

	gltfBool = VK_TRUE;
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
		if (gltfState == GltfState_Mesh_Primitive)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				gltfPrimitive.position = nullptr;
				gltfPrimitive.normal = nullptr;
				gltfPrimitive.binormal = nullptr;
				gltfPrimitive.tangent = nullptr;
				gltfPrimitive.texCoord = nullptr;
				gltfPrimitive.joint = nullptr;
				gltfPrimitive.weight = nullptr;
				gltfPrimitive.indices = nullptr;
				gltfPrimitive.mode = 4;
				gltfPrimitive.material = nullptr;
				gltfPrimitive.name = "Primitive_" + std::to_string(i);

				//

				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				gltfMesh.primitives.append(gltfPrimitive);
			}
		}
		else if (gltfState == GltfState_Skin_InverseBindMatrices)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				if (!allGltfAccessors.contains(gltfString))
				{
					state.push(GltfState_Error);
					return;
				}

				gltfSkin.inverseBindMatrices.append(&(allGltfAccessors[gltfString]));
			}
		}
		else if (gltfState == GltfState_Skin_JointNames)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				gltfSkin.jointNames.append(gltfString);
			}
		}
		else if (gltfState == GltfState_Node_Children)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				gltfNode.children.append(gltfString);
			}
		}
		else if (gltfState == GltfState_Node_Skeletons)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				gltfNode.skeletons.append(gltfString);
			}
		}
		else if (gltfState == GltfState_Node_Mesh)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				if (!allGltfMeshes.contains(gltfString))
				{
					state.push(GltfState_Error);
					return;
				}

				gltfNode.meshes.append(&allGltfMeshes[gltfString]);
			}
		}
		else if (gltfState == GltfState_Animation_Channel)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				gltfChannel.sampler = nullptr;
				gltfChannel.targetNode = nullptr;
				gltfChannel.targetPath = "";
				gltfChannel.name = "Channel_" + std::to_string(i);

				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				gltfAnimation.channels.append(gltfChannel);
			}
		}
		else if (gltfState == GltfState_Scene_Node)
		{
			for (int32_t i = 0; i < (int32_t)jsonArray.size(); i++)
			{
				jsonArray.getValueAt(i)->visit(*this);

				if (state.top() == GltfState_Error)
				{
					return;
				}

				if (!allGltfNodes.contains(gltfString))
				{
					state.push(GltfState_Error);
					return;
				}

				gltfScene.nodes.append(&allGltfNodes[gltfString]);
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
		// Not processing extensionsUsed, extensionsRequired, cameras, programs, shaders, techniques, glExtensionsUsed, extensions, extras

		//

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

		// Optional.

		if (jsonObject.hasKey("images"))
		{
			auto images = jsonObject.getValue("images");

			state.push(GltfState_Images);
			images->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}
		}

		if (jsonObject.hasKey("samplers"))
		{
			auto samplers = jsonObject.getValue("samplers");

			state.push(GltfState_Samplers);
			samplers->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}
		}

		if (jsonObject.hasKey("textures"))
		{
			auto textures = jsonObject.getValue("textures");

			state.push(GltfState_Textures);
			textures->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}
		}

		if (jsonObject.hasKey("materials"))
		{
			auto materials = jsonObject.getValue("materials");

			state.push(GltfState_Materials);
			materials->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}
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

		// Optional.

		if (jsonObject.hasKey("skins"))
		{
			auto skins = jsonObject.getValue("skins");

			state.push(GltfState_Skins);
			skins->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}
		}

		//

		if (jsonObject.hasKey("nodes"))
		{
			auto nodes = jsonObject.getValue("nodes");

			state.push(GltfState_Nodes);
			nodes->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			// Resolve children node and skeletons pointers.
			for (uint32_t i = 0; i < allGltfNodes.values().size(); i++)
			{
				for (uint32_t k = 0; k < allGltfNodes.valueAt(i).skeletons.size(); k++)
				{
					std::string currentSkeletonName = allGltfNodes.valueAt(i).skeletons[k];

					if (!allGltfNodes.contains(currentSkeletonName))
					{
						state.push(GltfState_Error);
						return;
					}

					allGltfNodes.valueAt(i).skeletonsPointer.append(&allGltfNodes[currentSkeletonName]);
				}
			}

			// Resolve joint name pointers.
			for (uint32_t i = 0; i < allGltfSkins.values().size(); i++)
			{
				for (uint32_t k = 0; k < allGltfSkins.valueAt(i).jointNames.size(); k++)
				{
					std::string currentJointName = allGltfSkins.valueAt(i).jointNames[k];
					GltfNode* currentJointNode = nullptr;

					for (uint32_t m = 0; m < allGltfNodes.values().size(); m++)
					{
						if (allGltfNodes.valueAt(m).jointName == currentJointName)
						{
							currentJointNode = &allGltfNodes.valueAt(m);

							break;
						}
					}

					if (!currentJointNode)
					{
						state.push(GltfState_Error);
						return;
					}

					allGltfSkins.valueAt(i).jointNodes.append(currentJointNode);
				}
			}
		}

		//

		if (jsonObject.hasKey("animations"))
		{
			auto animations = jsonObject.getValue("animations");

			state.push(GltfState_Animations);
			animations->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}
		}


		//

		if (jsonObject.hasKey("scenes"))
		{
			auto scenes = jsonObject.getValue("scenes");

			state.push(GltfState_Scenes);
			scenes->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}
		}

		//

		if (jsonObject.hasKey("scene"))
		{
			if (!jsonObject.hasKey("scenes"))
			{
				state.push(GltfState_Error);
				return;
			}

			//

			auto scene = jsonObject.getValue("scene");

			scene->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			if (!allGltfScenes.contains(gltfString))
			{
				state.push(GltfState_Error);
				return;
			}

			defaultScene = &(allGltfScenes[gltfString]);
		}

		//
		// If this point is reached, the glTF file could be parsed successfully.
		//

		state.push(GltfState_End);

		return;
	}
	else if (gltfState == GltfState_Asset)
	{
		// Not processing copyright, generator, premultipliedAlpha, profile, name, extensions, extras

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

		std::string majorName = "";
		std::string minorName = "";

		auto dotIndex = gltfString.find(".");

		if (dotIndex != gltfString.npos)
		{
			majorName = gltfString.substr(0, dotIndex);
			minorName = gltfString.substr(dotIndex + 1);
		}
		else
		{
			majorName = gltfString;
			minorName = "0";
		}

		int32_t majorVersion = (int32_t)atoi(majorName.c_str());
		int32_t minorVersion = (int32_t)atoi(minorName.c_str());

		if (majorVersion != 1 || minorVersion != 1)
		{
			// FIXME: Revert, as soon as glTF 1.1 is final.
			//state.push(GltfState_Error);
			return;
		}
	}
	else if (gltfState == GltfState_Buffers)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfBuffer.binaryBuffer = IBinaryBufferSP();
			gltfBuffer.byteLength = 0;
			gltfBuffer.name = allKeys[i];

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

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfBufferView.buffer = nullptr;
			gltfBufferView.byteOffset = 0;
		    gltfBufferView.byteLength = 0;
			gltfBufferView.name = allKeys[i];

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

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfAccessor.bufferView = nullptr;
			gltfAccessor.byteOffset = 0;
			gltfAccessor.byteStride = 0;
			gltfAccessor.componentType = 0;
			gltfAccessor.normalized = VK_FALSE;
			gltfAccessor.count = 0;
			gltfAccessor.type = "";

			gltfAccessor.minByte.clear();
			gltfAccessor.minUnsignedByte.clear();
			gltfAccessor.minUnsignedShort.clear();
			gltfAccessor.minUnsignedInteger.clear();
			gltfAccessor.minFloat.clear();

			gltfAccessor.maxByte.clear();
			gltfAccessor.maxUnsignedByte.clear();
			gltfAccessor.maxShort.clear();
			gltfAccessor.maxUnsignedShort.clear();
			gltfAccessor.maxUnsignedInteger.clear();
			gltfAccessor.maxFloat.clear();

			gltfAccessor.name = allKeys[i];

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
	else if (gltfState == GltfState_Images)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfImage.imageData.reset();
			gltfImage.name = allKeys[i];

			//

			auto currentImage = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Image);
			currentImage->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfImages[allKeys[i]] = gltfImage;
		}
	}
	else if (gltfState == GltfState_Samplers)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfSampler.magFilter = 9729;
			gltfSampler.minFilter = 9986;
			gltfSampler.wrapS = 10497;
			gltfSampler.wrapT = 10497;
			gltfSampler.name = allKeys[i];

			//

			auto currentSampler = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Sampler);
			currentSampler->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfSamplers[allKeys[i]] = gltfSampler;
		}
	}
	else if (gltfState == GltfState_Textures)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfTexture.format = 6408;
			gltfTexture.internalFormat = 6408;
			gltfTexture.sampler = nullptr;
			gltfTexture.source = nullptr;
			gltfTexture.target = 3553;
			gltfTexture.type = 5121;
			gltfTexture.name = allKeys[i];

			//

			auto currentTexture = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Texture);
			currentTexture->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfTextures[allKeys[i]] = gltfTexture;
		}
	}
	else if (gltfState == GltfState_Materials)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfMaterial.materialModel = "";

			gltfMaterial.baseColorFactor[0] = 1.0f;
			gltfMaterial.baseColorFactor[1] = 1.0f;
			gltfMaterial.baseColorFactor[2] = 1.0f;
			gltfMaterial.baseColorFactor[3] = 1.0f;
			gltfMaterial.baseColorTexture = nullptr;

			gltfMaterial.metallicFactor = 1.0f;
			gltfMaterial.metallicTexture = nullptr;

			gltfMaterial.roughnessFactor = 1.0f;
			gltfMaterial.roughnessTexture = nullptr;

			gltfMaterial.normalFactor = 1.0f;
			gltfMaterial.normalTexture = nullptr;

			gltfMaterial.aoFactor = 1.0f;
			gltfMaterial.aoTexture = nullptr;

			gltfMaterial.emissiveFactor[0] = 1.0f;
			gltfMaterial.emissiveFactor[1] = 1.0f;
			gltfMaterial.emissiveFactor[2] = 1.0f;
			gltfMaterial.emissiveFactor[3] = 1.0f;
			gltfMaterial.emissiveTexture = nullptr;

			gltfMaterial.name = allKeys[i];

			//

			auto currentMaterial = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Material);
			currentMaterial->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfMaterials[allKeys[i]] = gltfMaterial;
		}
	}
	else if (gltfState == GltfState_Meshes)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfMesh.primitives.clear();
			gltfMesh.name = allKeys[i];

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
	else if (gltfState == GltfState_Skins)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			for (int32_t k = 0; k < 16; k++)
			{
				if ((k % 4) - (k / 4) == 0)
				{
					gltfSkin.bindShapeMatrix[k] = 1.0f;
				}
				else
				{
					gltfSkin.bindShapeMatrix[k] = 0.0f;
				}
			}

			gltfSkin.inverseBindMatrices.clear();
			gltfSkin.jointNames.clear();
			gltfSkin.jointNodes.clear();
			gltfSkin.name = allKeys[i];


			//

			auto currentSKin = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Skin);
			currentSKin->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfSkins[allKeys[i]] = gltfSkin;
		}
	}
	else if (gltfState == GltfState_Nodes)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfNode.children.clear();
			gltfNode.skeletons.clear();
			gltfNode.skin = nullptr;
			gltfNode.jointName = "";

			gltfNode.childrenPointer.clear();
			gltfNode.skeletonsPointer.clear();
			gltfNode.useMatrix = VK_FALSE;
			gltfNode.useRotation = VK_FALSE;
			gltfNode.useScale = VK_FALSE;
			gltfNode.useTranslation = VK_FALSE;
			gltfNode.name = allKeys[i];

			for (int32_t k = 0; k < 16; k++)
			{
				if ((k % 4) - (k / 4) == 0)
				{
					gltfNode.matrix[k] = 1.0f;
				}
				else
				{
					gltfNode.matrix[k] = 0.0f;
				}
			}

			gltfNode.meshes.clear();

			gltfNode.rotation[0] = 0.0f;
			gltfNode.rotation[1] = 0.0f;
			gltfNode.rotation[2] = 0.0f;
			gltfNode.rotation[3] = 1.0f;

			gltfNode.scale[0] = 1.0f;
			gltfNode.scale[1] = 1.0f;
			gltfNode.scale[2] = 1.0f;

			gltfNode.translation[0] = 0.0f;
			gltfNode.translation[1] = 0.0f;
			gltfNode.translation[2] = 0.0f;

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
	else if (gltfState == GltfState_Animations)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfAnimation.samplers.clear();
			gltfAnimation.channels.clear();
			gltfAnimation.name = allKeys[i];

			//

			auto currentAnimation = jsonObject.getValue(allKeys[i]);

			state.push(GltfState_Animation);
			currentAnimation->visit(*this);

			if (state.top() == GltfState_Error)
			{
				return;
			}

			//

			allGltfAnimations[allKeys[i]] = gltfAnimation;
		}
	}
	else if (gltfState == GltfState_Scenes)
	{
		const auto& allKeys = jsonObject.getAllKeys();

		for (uint32_t i = 0; i < allKeys.size(); i++)
		{
			gltfScene.nodes.clear();
			gltfScene.name = allKeys[i];

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
		visitBuffer(jsonObject);
	}
	else if (gltfState == GltfState_BufferView)
	{
		visitBufferView(jsonObject);
	}
	else if (gltfState == GltfState_Accessor)
	{
		visitAccessor(jsonObject);
	}
	else if (gltfState == GltfState_Image)
	{
		visitImage(jsonObject);
	}
	else if (gltfState == GltfState_Sampler)
	{
		visitSampler(jsonObject);
	}
	else if (gltfState == GltfState_Texture)
	{
		visitTexture(jsonObject);
	}
	else if (gltfState == GltfState_Material)
	{
		visitMaterial(jsonObject);
	}
	else if (gltfState == GltfState_Mesh)
	{
		visitMesh(jsonObject);
	}
	else if (gltfState == GltfState_Skin)
	{
		visitSkin(jsonObject);
	}
	else if (gltfState == GltfState_Node)
	{
		visitNode(jsonObject);
	}
	else if (gltfState == GltfState_Animation)
	{
		visitAnimation(jsonObject);
	}
	else if (gltfState == GltfState_Scene)
	{
		visitScene(jsonObject);
	}
	else if (gltfState == GltfState_Material_Extensions)
	{
		visitMaterial_Extensions(jsonObject);
	}
	else if (gltfState == GltfState_Material_Extensions_Pbr)
	{
		visitMaterial_Extensions_Pbr(jsonObject);
	}
	else if (gltfState == GltfState_Material_Extensions_Pbr_Values)
	{
		visitMaterial_Extensions_Pbr_Values(jsonObject);
	}
	else if (gltfState == GltfState_Mesh_Primitive)
	{
		visitMesh_Primitive(jsonObject);
	}
	else if (gltfState == GltfState_Mesh_Primitive_Attributes)
	{
		visitMesh_Primitive_Attributes(jsonObject);
	}
	else if (gltfState == GltfState_Animation_Sampler)
	{
		visitAnimation_Sampler(jsonObject);
	}
	else if (gltfState == GltfState_Animation_Sampler_Properties)
	{
		visitAnimation_Sampler_Properties(jsonObject);
	}
	else if (gltfState == GltfState_Animation_Channel)
	{
		visitAnimation_Channel(jsonObject);
	}
	else if (gltfState == GltfState_Animation_Channel_Target)
	{
		visitAnimation_Channel_Target(jsonObject);
	}
	else
	{
		state.push(GltfState_Error);
		return;
	}

	state.pop();
}

const std::string& GltfVisitor::getDirectory() const
{
	return directory;
}

enum GltfState GltfVisitor::getState() const
{
	if (state.size() > 0)
	{
		return state.top();
	}

	return GltfState_Error;
}


const Map<std::string, GltfBuffer>& GltfVisitor::getAllGltfBuffers() const
{
	return allGltfBuffers;
}

const Map<std::string, GltfBufferView>& GltfVisitor::getAllGltfBufferViews() const
{
	return allGltfBufferViews;
}

const Map<std::string, GltfAccessor>& GltfVisitor::getAllGltfAccessors() const
{
	return allGltfAccessors;
}

const Map<std::string, GltfImage>& GltfVisitor::getAllGltfImages() const
{
	return allGltfImages;
}

const Map<std::string, GltfSampler>& GltfVisitor::getAllGltfSamplers() const
{
	return allGltfSamplers;
}

const Map<std::string, GltfTexture>& GltfVisitor::getAllGltfTextures() const
{
	return allGltfTextures;
}

const Map<std::string, GltfMaterial>& GltfVisitor::getAllGltfMaterials() const
{
	return allGltfMaterials;
}

const Map<std::string, GltfMesh>& GltfVisitor::getAllGltfMeshes() const
{
	return allGltfMeshes;
}

const Map<std::string, GltfSkin>& GltfVisitor::getAllGltfSkins() const
{
	return allGltfSkins;
}

const Map<std::string, GltfNode>& GltfVisitor::getAllGltfNodes() const
{
	return allGltfNodes;
}

const Map<std::string, GltfAnimation>& GltfVisitor::getAllGltfAnimations() const
{
	return allGltfAnimations;
}

const Map<std::string, GltfScene>& GltfVisitor::getAllGltfScenes() const
{
	return allGltfScenes;
}

const GltfScene* GltfVisitor::getDefaultScene() const
{
	return defaultScene;
}

VkBool32 GltfVisitor::isByte(const int32_t componentType) const
{
	return (VkBool32)(componentType == 5120);
}

VkBool32 GltfVisitor::isUnsignedByte(const int32_t componentType) const
{
	return (VkBool32)(componentType == 5121);
}

VkBool32 GltfVisitor::isShort(const int32_t componentType) const
{
	return (VkBool32)(componentType == 5122);
}

VkBool32 GltfVisitor::isUnsignedShort(const int32_t componentType) const
{
	return (VkBool32)(componentType == 5123);
}

VkBool32 GltfVisitor::isUnsignedInt(const int32_t componentType) const
{
	return (VkBool32)(componentType == 5125);
}

VkBool32 GltfVisitor::isFloat(const int32_t componentType) const
{
	return (VkBool32)(componentType == 5126);
}

uint32_t GltfVisitor::getBytesPerComponent(const int32_t componentType) const
{
	switch (componentType)
	{
		case 5120:
		case 5121:
			return 1;
		case 5122:
		case 5123:
			return 2;
		case 5125:
		case 5126:
			return 4;
	}

	return 0;
}

uint32_t GltfVisitor::getComponentsPerType(const std::string& type) const
{
	if (type == "SCALAR")
	{
		return 1;
	}
	else if (type == "VEC2")
	{
		return 2;
	}
	else if (type == "VEC3")
	{
		return 3;
	}
	else if (type == "VEC4" || type == "MAT2")
	{
		return 4;
	}
	else if (type == "MAT3")
	{
		return 9;
	}
	else if (type == "MAT4")
	{
		return 16;
	}

	return 0;
}

const void* GltfVisitor::getBufferPointer(const GltfAccessor& accessor, const uint32_t element) const
{
	if (accessor.bufferView == nullptr)
	{
		return nullptr;
	}
	if (accessor.bufferView->buffer == nullptr)
	{
		return nullptr;
	}
	if (!accessor.bufferView->buffer->binaryBuffer.get())
	{
		return nullptr;
	}
	if (accessor.bufferView->buffer->byteLength != accessor.bufferView->buffer->binaryBuffer->getSize())
	{
		return nullptr;
	}
	if (element >= accessor.count)
	{
		return nullptr;
	}

	//

	uint32_t bytesPerComponent = getBytesPerComponent(accessor.componentType);

	if (bytesPerComponent == 0)
	{
		return nullptr;
	}

	//

	uint32_t componentsPerType = getComponentsPerType(accessor.type);

	if (componentsPerType == 0)
	{
		return nullptr;
	}

	//

	uint32_t elementSize = bytesPerComponent * componentsPerType + accessor.byteStride;

	uint32_t localOffset = elementSize * element + accessor.byteOffset;

	if (localOffset >= accessor.bufferView->byteLength || (localOffset + elementSize) > accessor.bufferView->byteLength)
	{
		return nullptr;
	}

	uint32_t totalOffset = localOffset + accessor.bufferView->byteOffset;

	if (totalOffset >= accessor.bufferView->buffer->byteLength || (totalOffset + elementSize) > accessor.bufferView->buffer->byteLength)
	{
		return nullptr;
	}

	//

	return (const void*)&(accessor.bufferView->buffer->binaryBuffer->getByteData()[totalOffset]);
}

const int8_t* GltfVisitor::getBytePointer(const GltfAccessor& accessor, const uint32_t element) const
{
	if (!isByte(accessor.componentType))
	{
		return nullptr;
	}

	return (const int8_t* )getBufferPointer(accessor, element);
}

const uint8_t* GltfVisitor::getUnsignedBytePointer(const GltfAccessor& accessor, const uint32_t element) const
{
	if (!isUnsignedByte(accessor.componentType))
	{
		return nullptr;
	}

	return (const uint8_t* )getBufferPointer(accessor, element);
}

const int16_t* GltfVisitor::getShortPointer(const GltfAccessor& accessor, const uint32_t element) const
{
	if (!isShort(accessor.componentType))
	{
		return nullptr;
	}

	return (const int16_t* )getBufferPointer(accessor, element);
}

const uint16_t* GltfVisitor::getUnsignedShortPointer(const GltfAccessor& accessor, const uint32_t element) const
{
	if (!isUnsignedShort(accessor.componentType))
	{
		return nullptr;
	}

	return (const uint16_t* )getBufferPointer(accessor, element);
}

const uint32_t* GltfVisitor::getUnsignedIntPointer(const GltfAccessor& accessor, const uint32_t element) const
{
	if (!isUnsignedInt(accessor.componentType))
	{
		return nullptr;
	}

	return (const uint32_t* )getBufferPointer(accessor, element);
}

const float* GltfVisitor::getFloatPointer(const GltfAccessor& accessor, const uint32_t element) const
{
	if (!isFloat(accessor.componentType))
	{
		return nullptr;
	}

	return (const float* )getBufferPointer(accessor, element);
}

}
