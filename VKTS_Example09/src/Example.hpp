/**
 * VKTS Examples - Examples for Vulkan using VulKan ToolS.
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

#ifndef EXAMPLE_HPP_
#define EXAMPLE_HPP_

#include <vkts/vkts.hpp>

#include "LoadTask.hpp"

#define VKTS_EXAMPLE_NAME "Example09"

#define VKTS_NUMBER_DYNAMIC_STATES 2

#define VKTS_NUMBER_BUFFERS 2
#define VKTS_SHADER_STAGE_COUNT 2
#define VKTS_PIPELINE_CACHE_SIZE 0
#define VKTS_SAMPLE_COUNT_BIT VK_SAMPLE_COUNT_4_BIT
#define VKTS_SHADOW_MAP_SIZE 2048
#define VKTS_SHADOW_CAMERA_SCALE 0.11f
#define VKTS_SHADOW_CAMERA_DISTANCE 100.0f
#define VKTS_SHADOW_CAMERA_ORTHO_FAR 200.0f


#define VKTS_BINDING_VERTEX_BUFFER 0

#define VKTS_SKINNING_VERTEX_SHADER_NAME 			"shader/SPIR/V/phong_skinning_shadow.vert.spv"
#define VKTS_SKINNING_FRAGMENT_SHADER_NAME 			"shader/SPIR/V/phong_skinning_shadow.frag.spv"
#define VKTS_SKINNING_SHADOW_FRAGMENT_SHADER_NAME 	"shader/SPIR/V/phong_skinning_write_shadow.frag.spv"

#define VKTS_STANDARD_VERTEX_SHADER_NAME 			"shader/SPIR/V/phong_shadow.vert.spv"
#define VKTS_STANDARD_FRAGMENT_SHADER_NAME 			"shader/SPIR/V/phong_shadow.frag.spv"
#define VKTS_STANDARD_SHADOW_FRAGMENT_SHADER_NAME	"shader/SPIR/V/phong_write_shadow.frag.spv"

#define VKTS_DESCRIPTOR_SET_COUNT (VKTS_BINDING_UNIFORM_PHONG_BINDING_COUNT + VKTS_BINDING_UNIFORM_LIGHTING_BINDING_COUNT + VKTS_BINDING_UNIFORM_TRANSFORM_BINDING_COUNT + VKTS_BINDING_UNIFORM_BONES_BINDING_COUNT + VKTS_BINDING_UNIFORM_SHADOW_BINDING_COUNT)

class Example: public vkts::IUpdateThread
{

private:

	const vkts::IContextObjectSP contextObject;

	const int32_t windowIndex;

	const vkts::IVisualContextSP visualContext;

	const vkts::ISurfaceSP surface;

	VkFormat depthFormat;

	vkts::IUserCameraSP camera;
	vkts::IInputControllerSP inputController;

	vkts::SmartPointerVector<vkts::IUpdateableSP> allUpdateables;

	vkts::ICommandPoolSP commandPool;

    vkts::ISemaphoreSP imageAcquiredSemaphore;
    vkts::ISemaphoreSP renderingCompleteSemaphore;

	vkts::IDescriptorSetLayoutSP descriptorSetLayout;
	VkDescriptorImageInfo descriptorImageInfos[1];
    VkDescriptorBufferInfo descriptorBufferInfos[1 + 1 + 1];

    VkWriteDescriptorSet writeDescriptorSets[VKTS_DESCRIPTOR_SET_COUNT];

    std::map<uint32_t, VkTsDynamicOffset> dynamicOffsetsShadowPass;
    std::map<uint32_t, VkTsDynamicOffset> dynamicOffsetsColorPass;

	vkts::IBufferObjectSP vertexViewProjectionUniformBuffer;
	vkts::IBufferObjectSP fragmentUniformBuffer;
	vkts::IBufferObjectSP shadowUniformBuffer;

	vkts::IShaderModuleSP skinningVertexShaderModule;
	vkts::IShaderModuleSP skinningFragmentShaderModule;
	vkts::IShaderModuleSP skinningShadowFragmentShaderModule;

	vkts::IShaderModuleSP standardVertexShaderModule;
	vkts::IShaderModuleSP standardFragmentShaderModule;
	vkts::IShaderModuleSP standardShadowFragmentShaderModule;

	vkts::IPipelineLayoutSP pipelineLayout;

	ILoadTaskSP loadTask;

	VkBool32 sceneLoaded;
	vkts::ISceneRenderFactorySP renderFactory;
	vkts::ISceneManagerSP sceneManager;
	vkts::ISceneFactorySP sceneFactory;
	vkts::ISceneSP scene;

    vkts::ISwapchainSP swapchain;

	vkts::IRenderPassSP renderPass;
	vkts::IRenderPassSP shadowRenderPass;

	vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> allOpaqueGraphicsPipelines;
	vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> allBlendGraphicsPipelines;
	vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> allBlendCwGraphicsPipelines;
	vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> allShadowGraphicsPipelines;

	vkts::IImageObjectSP shadowTexture;
	vkts::IImageObjectSP msaaColorTexture;
	vkts::IImageObjectSP msaaDepthTexture;
	vkts::IImageObjectSP depthTexture;

	vkts::IImageViewSP shadowImageView;
	vkts::IImageViewSP msaaColorImageView;
	vkts::IImageViewSP msaaDepthStencilImageView;
	vkts::IImageViewSP depthStencilImageView;

	vkts::ISamplerSP shadowSampler;

    uint32_t swapchainImagesCount;

    vkts::SmartPointerVector<vkts::IImageViewSP> swapchainImageView;

    vkts::SmartPointerVector<vkts::IFramebufferSP> framebuffer;
    vkts::SmartPointerVector<vkts::IFramebufferSP> shadowFramebuffer;

    vkts::SmartPointerVector<vkts::ICommandBuffersSP> cmdBuffer;
    vkts::SmartPointerVector<vkts::ICommandBuffersSP> shadowCmdBuffer;

    vkts::SmartPointerVector<vkts::IFenceSP> cmdBufferFence;

	VkBool32 buildCmdBuffer(const int32_t usedBuffer);

	VkBool32 buildFramebuffer(const int32_t usedBuffer);

	VkBool32 buildSwapchainImageView(const int32_t usedBuffer);

	VkBool32 updateDescriptorSets();

	VkBool32 buildShadowSampler();

	VkBool32 buildDepthStencilImageView();

	VkBool32 buildMSAADepthStencilImageView();

	VkBool32 buildMSAAColorImageView();

	VkBool32 buildShadowImageView();

	VkBool32 buildDepthTexture(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildMSAADepthTexture(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildMSAAColorTexture(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildShadowTexture(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildPipeline();

	VkBool32 buildRenderPass();

	VkBool32 buildPipelineLayout();

	VkBool32 buildDescriptorSetLayout();

	VkBool32 buildShader();

	VkBool32 buildUniformBuffers();

	VkBool32 buildResources(const vkts::IUpdateThreadContext& updateContext);

	void terminateResources(const vkts::IUpdateThreadContext& updateContext);

public:

	Example(const vkts::IContextObjectSP& contextObject, const int32_t windowIndex, const vkts::IVisualContextSP& visualContext, const vkts::ISurfaceSP& surface);

	virtual ~Example();

	//
	// Vulkan initialization.
	//
	virtual VkBool32 init(const vkts::IUpdateThreadContext& updateContext);

	//
	// Vulkan update.
	//
	virtual VkBool32 update(const vkts::IUpdateThreadContext& updateContext);

	//
	// Vulkan termination.
	//
	virtual void terminate(const vkts::IUpdateThreadContext& updateContext);

};

#endif /* EXAMPLE_HPP_ */
