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
	GltfState_Meshes,
	GltfState_Skins,
	GltfState_Nodes,
	GltfState_Animations,
	GltfState_Scenes,

	GltfState_Buffer,
	GltfState_BufferView,
	GltfState_Accessor,
	GltfState_Mesh,
	GltfState_Skin,
	GltfState_Node,
	GltfState_Animation,
	GltfState_Scene,

	GltfState_Mesh_Primitive,
	GltfState_Mesh_Primitive_Attributes,
	GltfState_Skin_InverseBindMatrices,
	GltfState_Skin_JointNames,
	GltfState_Node_Children,
	GltfState_Node_Skeletons,
	GltfState_Node_Mesh,
	GltfState_Animation_Sampler,
	GltfState_Animation_Sampler_Properties,
	GltfState_Animation_Channel,
	GltfState_Animation_Channel_Target,
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
    size_t byteStride;
    int32_t componentType;
    VkBool32 normalzed;
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
	int32_t mode;
} GltfPrimitive;

typedef struct _GltfMesh {
	Vector<GltfPrimitive> primitives;
} GltfMesh;

struct _GltfNode;

typedef struct _GltfSkin {
	float bindShapeMatrix[16];
	Vector<GltfAccessor*> inverseBindMatrices;
	Vector<std::string> jointNames;
	Vector<struct _GltfNode*> jointNodes;
} GltfSkin;

typedef struct _GltfNode {
	Vector<std::string> children;
	Vector<const struct _GltfNode*> childrenPointer;
	Vector<std::string> skeletons;
	Vector<const struct _GltfNode*> skeletonsPointer;
	GltfSkin* skin;
	std::string jointName;
	float matrix[16];
	Vector<GltfMesh*> meshes;
	float rotation[4];
	float scale[3];
	float translation[3];
} GltfNode;

typedef struct _GltfAnimation_Sampler {
	GltfAccessor* input;
    std::string interpolation;
    GltfAccessor* output;
} GltfAnimation_Sampler;

typedef struct _GltfChannel {
	GltfAnimation_Sampler* sampler;
	GltfNode* targetNode;
	std::string targetPath;
} GltfChannel;

typedef struct _GltfAnimation {
	Map<std::string, GltfAnimation_Sampler> samplers;
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

	VkBool32 gltfBool;
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
	GltfPrimitive gltfPrimitive;
	GltfMesh gltfMesh;
	GltfSkin gltfSkin;
	GltfNode gltfNode;
	GltfAnimation_Sampler gltfAnimation_Sampler;
	GltfChannel gltfChannel;
	GltfAnimation gltfAnimation;
	GltfScene gltfScene;

	Map<std::string, GltfBuffer> allGltfBuffers;
	Map<std::string, GltfBufferView> allGltfBufferViews;
	Map<std::string, GltfAccessor> allGltfAccessors;
	Map<std::string, GltfMesh> allGltfMeshes;
	Map<std::string, GltfSkin> allGltfSkins;
	Map<std::string, GltfNode> allGltfNodes;
	Map<std::string, GltfAnimation> allGltfAnimations;
	Map<std::string, GltfScene> allGltfScenes;

	void visitBuffer(JSONobject& jsonObject);
	void visitBufferView(JSONobject& jsonObject);
	void visitAccessor(JSONobject& jsonObject);
	void visitMesh(JSONobject& jsonObject);
	void visitSkin(JSONobject& jsonObject);
	void visitNode(JSONobject& jsonObject);
	void visitAnimation(JSONobject& jsonObject);
	void visitScene(JSONobject& jsonObject);

	void visitMesh_Primitive(JSONobject& jsonObject);
	void visitMesh_Primitive_Attributes(JSONobject& jsonObject);
	void visitAnimation_Sampler(JSONobject& jsonObject);
	void visitAnimation_Sampler_Properties(JSONobject& jsonObject);
	void visitAnimation_Channel(JSONobject& jsonObject);
	void visitAnimation_Channel_Target(JSONobject& jsonObject);

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

	//

	const Map<std::string, GltfBuffer>& getAllGltfBuffers() const;

	const Map<std::string, GltfBufferView>& getAllGltfBufferViews() const;

	const Map<std::string, GltfAccessor>& getAllGltfAccessors() const;

	const Map<std::string, GltfMesh>& getAllGltfMeshes() const;

	const Map<std::string, GltfSkin>& getAllGltfSkins() const;

	const Map<std::string, GltfNode>& getAllGltfNodes() const;

	const Map<std::string, GltfAnimation>& getAllGltfAnimations() const;

	const Map<std::string, GltfScene>& getAllGltfScenes() const;

};

}

#endif /* VKTS_GLTFVISITOR_HPP_ */
