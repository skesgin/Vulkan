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

#define VKTS_EXAMPLE_NAME "Example10"

#define VKTS_NUMBER_BUFFERS 2

#define VKTS_BINDING_VERTEX_BUFFER 0

#define VKTS_FONT_NAME "font/Arial_128.fnt"
#define VKTS_FONT_SCALE 32.0f


#define VKTS_BSDF0_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf.vert.spv"
#define VKTS_BSDF1_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_no_texcoord.vert.spv"
#define VKTS_BSDF2_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_tangents.vert.spv"
#define VKTS_BSDF3_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_skinning.vert.spv"


#define VKTS_ENV_VERTEX_SHADER_NAME "shader/SPIR/V/environment.vert.spv"
#define VKTS_ENV_FRAGMENT_SHADER_NAME "shader/SPIR/V/environment.frag.spv"

#define VKTS_RESOLVE_VERTEX_SHADER_NAME "shader/SPIR/V/texture_ndc.vert.spv"
#define VKTS_RESOLVE_FRAGMENT_SHADER_NAME "shader/SPIR/V/resolve_bsdf.frag.spv"


#define VKTS_SCENE_NAME "material_probes/material_probes.vkts"
#define VKTS_ENVIRONMENT_SCENE_NAME "primitives/sphere.vkts"

#define VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT (VKTS_BINDING_UNIFORM_TRANSFORM_BINDING_COUNT + VKTS_BINDING_UNIFORM_ENVIRONMENT_COUNT)

#define VKTS_MAX_CORES 32u

#define VKTS_OUTPUT_BUFFER_SIZE 1024

class Example: public vkts::IUpdateThread
{

private:

	const vkts::IInitialResourcesSP initialResources;

	const int32_t windowIndex;

	const vkts::ISurfaceSP surface;

	vkts::ICameraSP camera;
	vkts::IInputControllerSP inputController;

	vkts::SmartPointerVector<vkts::IUpdateableSP> allUpdateables;

	vkts::ICommandPoolSP commandPool;

    vkts::ISemaphoreSP imageAcquiredSemaphore;
    vkts::ISemaphoreSP renderingCompleteSemaphore;

	vkts::IDescriptorSetLayoutSP environmentDescriptorSetLayout;
	vkts::IDescriptorSetLayoutSP resolveDescriptorSetLayout;

	vkts::IDescriptorPoolSP resolveDescriptorPool;
	vkts::IDescriptorSetsSP resolveDescriptorSet;

    VkDescriptorBufferInfo environmentDescriptorBufferInfos[1];
    VkDescriptorBufferInfo descriptorBufferInfos[1];
    VkDescriptorImageInfo environmentDescriptorImageInfos[1];
    VkDescriptorBufferInfo resolveDescriptorBufferInfos[1];
    VkDescriptorImageInfo resolveDescriptorImageInfos[8];

    VkWriteDescriptorSet writeDescriptorSets[VKTS_BINDING_UNIFORM_BSDF_TOTAL_BINDING_COUNT];
    VkWriteDescriptorSet environmentWriteDescriptorSets[VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT];
    VkWriteDescriptorSet resolveWriteDescriptorSets[9];

    vkts::IBufferObjectSP vertexViewProjectionUniformBuffer;
	vkts::IBufferObjectSP environmentVertexViewProjectionUniformBuffer;
	vkts::IBufferObjectSP resolveFragmentMatricesUniformBuffer;

	vkts::SmartPointerVector<vkts::IShaderModuleSP> allBSDFVertexShaderModules;

	vkts::IShaderModuleSP envVertexShaderModule;
	vkts::IShaderModuleSP envFragmentShaderModule;

	vkts::IShaderModuleSP resolveVertexShaderModule;
	vkts::IShaderModuleSP resolveFragmentShaderModule;

	vkts::IPipelineLayoutSP environmentPipelineLayout;
	vkts::IPipelineLayoutSP resolvePipelineLayout;

    vkts::IFontSP font;

	vkts::IContextSP sceneContext;
	vkts::ISceneSP scene;

	vkts::IContextSP environmentSceneContext;
	vkts::ISceneSP environmentScene;

	vkts::IBufferObjectSP screenPlaneVertexBuffer;

	vkts::ISwapchainSP swapchain;

	vkts::IRenderPassSP renderPass;
	vkts::IRenderPassSP gbufferRenderPass;

	vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> allGraphicsPipelines;
	vkts::IGraphicsPipelineSP resolveGraphicsPipeline;

	vkts::SmartPointerVector<vkts::IMemoryImageSP> allGBufferTextures;
	vkts::SmartPointerVector<vkts::IImageViewSP> allGBufferImageViews;
	vkts::ISamplerSP gbufferSampler;

    uint32_t swapchainImagesCount;

    vkts::SmartPointerVector<vkts::IImageViewSP> swapchainImageView;

    vkts::SmartPointerVector<vkts::IFramebufferSP> gbufferFramebuffer;
    vkts::SmartPointerVector<vkts::IFramebufferSP> framebuffer;

    vkts::SmartPointerVector<vkts::ICommandBuffersSP> cmdBuffer;
    uint32_t rebuildCmdBufferCounter;

    uint32_t fps;
    uint64_t ram;
    float cpuUsageApp;
    uint32_t processors;
    float cpuUsage[VKTS_MAX_CORES];

	VkBool32 buildCmdBuffer(const int32_t usedBuffer);

	VkBool32 buildFramebuffer(const int32_t usedBuffer);

	VkBool32 updateDescriptorSets();

	VkBool32 buildScene(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildSwapchainImageView(const int32_t usedBuffer);

	VkBool32 buildGBufferSampler();

	VkBool32 buildGBufferImageView();

	VkBool32 buildGBufferTexture(const vkts::ICommandBuffersSP& cmdBuffer);

	VkBool32 buildPipeline();

	VkBool32 buildRenderPass();

	VkBool32 buildPipelineLayout();

	VkBool32 buildDescriptorSets();

	VkBool32 buildDescriptorSetPool();

	VkBool32 buildDescriptorSetLayout();

	VkBool32 buildShader();

	VkBool32 buildUniformBuffers();

	VkBool32 buildResources(const vkts::IUpdateThreadContext& updateContext);

	void terminateResources(const vkts::IUpdateThreadContext& updateContext);

public:

	Example(const vkts::IInitialResourcesSP& initialResources, const int32_t windowIndex, const vkts::ISurfaceSP& surface);

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
