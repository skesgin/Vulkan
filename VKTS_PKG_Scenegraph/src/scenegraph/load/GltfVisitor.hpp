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

enum GltfState {GltfState_Start,

				GltfState_Error,
				GltfState_Success,

				GltfState_JsonObject,
				GltfState_GltfBuffers,
				GltfState_GltfBufferViews,
				GltfState_GltfAccessors,
				GltfState_GltfMeshes,
				GltfState_GltfNodes,
				GltfState_GltfScenes,

				GltfState_GltfBuffer,
				GltfState_GltfBufferView};

typedef struct _GltfBufferView {
	IBinaryBufferSP buffer;
    size_t byteOffset;
    size_t byteLength;
} GltfBufferView;

class GltfVisitor : public JsonVisitor
{

private:

	const std::string directory;

	std::stack<enum GltfState> state;

	std::string currentString;
	std::int32_t currentInteger;
	float currentFloat;
	VkBool32 currentBool;

	GltfBufferView currentBufferView;

	SmartPointerMap<std::string, IBinaryBufferSP>	allBuffers;
	std::map<std::string, GltfBufferView>	allBufferViews;

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

};

}

#endif /* VKTS_GLTFVISITOR_HPP_ */
