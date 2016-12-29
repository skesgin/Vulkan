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

#ifndef VKTS_GLTFVISITOR_HPP_
#define VKTS_GLTFVISITOR_HPP_

#include <vkts/scenegraph/vkts_scenegraph.hpp>

namespace vkts
{

enum GltfState {
	GltfState_Start,
	GltfState_End,

	GltfState_Error,

	GltfState_Asset,
	GltfState_Buffers,
	GltfState_BufferViews,
	GltfState_Accessors,
	GltfState_Animations,
	GltfState_Meshes,
	GltfState_Nodes,
	GltfState_Scenes,

	GltfState_Buffer,
	GltfState_BufferView,
	GltfState_Accessor,
	GltfState_Animation,
	GltfState_Mesh,
	GltfState_Node,
	GltfState_Scene,

	GltfState_Animation_Sampler,
	GltfState_Animation_Sampler_Parameters,
	GltfState_Animation_Channel,
	GltfState_Animation_Channel_Target,
	GltfState_Mesh_Primitive,
	GltfState_Mesh_Primitive_Attribute,
	GltfState_Node_Mesh,
	GltfState_Scene_Node
};

typedef struct _GltfBuffer {
	IBinaryBufferSP binaryBuffer;
	size_t byteLength;
} GltfBuffer;

typedef struct _GltfBufferView {
	GltfBuffer* buffer;
    size_t byteOffset;
    size_t byteLength;
} GltfBufferView;

typedef struct _GltfAccessor {
	GltfBufferView* bufferView;
    size_t byteOffset;
    int32_t componentType;
    int32_t count;
    std::string type;

    Vector<int8_t> minByte;
    Vector<uint8_t> minUnsignedByte;
    Vector<int16_t> minShort;
    Vector<uint16_t> minUnsignedShort;
    Vector<uint32_t> minUnsignedInteger;
    Vector<float> minFloat;

    Vector<int8_t> maxByte;
    Vector<uint8_t> maxUnsignedByte;
    Vector<int16_t> maxShort;
    Vector<uint16_t> maxUnsignedShort;
    Vector<uint32_t> maxUnsignedInteger;
    Vector<float> maxFloat;

} GltfAccessor;

typedef struct _GltfPrimitive {
	// Attributes
	GltfAccessor* position;
	GltfAccessor* normal;
	GltfAccessor* binormal;
	GltfAccessor* tangent;
	GltfAccessor* texCoord;
	GltfAccessor* joint;
	GltfAccessor* weight;
	//
	GltfAccessor* indices;
} GltfPrimitive;

typedef struct _GltfMesh {
	Vector<GltfPrimitive> primitives;
} GltfMesh;

typedef struct _GltfNode {
	Vector<GltfMesh*> meshes;
} GltfNode;

typedef struct _GltfSampler {
	GltfAccessor* input;
    std::string interpolation;
    GltfAccessor* output;
} GltfSampler;

typedef struct _GltfChannel {
	GltfSampler* sampler;
	GltfNode* targetNode;
	std::string targetPath;
} GltfChannel;

typedef struct _GltfAnimation {
	Map<std::string, GltfSampler> samplers;
	Vector<GltfChannel> channels;
} GltfAnimation;

typedef struct _GltfScene {
	Vector<GltfNode*> nodes;
} GltfScene;

class GltfVisitor : public JsonVisitor
{

private:

	const std::string directory;

	std::stack<enum GltfState> state;

	std::string gltfString;
	std::int32_t gltfInteger;
	float gltfFloat;
	std::int32_t gltfIntegerArray[16];
	float gltfFloatArray[16];

	size_t arrayIndex;
	size_t arraySize;
	VkBool32 numberArray;

	VkBool32 objectArray;

	GltfBuffer gltfBuffer;
	GltfBufferView gltfBufferView;
	GltfAccessor gltfAccessor;
	GltfAnimation gltfAnimation;
	GltfSampler gltfSampler;
	GltfChannel gltfChannel;
	GltfMesh gltfMesh;
	GltfNode gltfNode;
	GltfScene gltfScene;

	GltfPrimitive gltfPrimitive;

	Map<std::string, GltfBuffer> allGltfBuffers;
	Map<std::string, GltfBufferView> allGltfBufferViews;
	Map<std::string, GltfAccessor> allGltfAccessors;
	Map<std::string, GltfAnimation> allGltfAnimations;
	Map<std::string, GltfMesh> allGltfMeshes;
	Map<std::string, GltfNode> allGltfNodes;
	Map<std::string, GltfScene> allGltfScenes;

public:

	GltfVisitor() = delete;

	GltfVisitor(const std::string& directory);

	virtual ~GltfVisitor();

	//

	virtual void visit(JSONnull& jsonNull) override;

	virtual void visit(JSONfalse& jsonFalse) override;

	virtual void visit(JSONtrue& jsonTrue) override;

	virtual void visit(JSONfloat& jsonFloat) override;

	virtual void visit(JSONinteger& jsonInteger) override;

	virtual void visit(JSONstring& jsonString) override;

	virtual void visit(JSONarray& jsonArray) override;

	virtual void visit(JSONobject& jsonObject) override;

	//

	enum GltfState getState() const;

};

}

#endif /* VKTS_GLTFVISITOR_HPP_ */
