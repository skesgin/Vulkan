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

#define VKTS_NUMBER_DYNAMIC_UNIFORM_BUFFERS 3
#define VKTS_MAX_NUMBER_BUFFERS 3

#define VKTS_NUMBER_BUFFERS 2

#define VKTS_BINDING_VERTEX_BUFFER 0

#define VKTS_MAX_LIGHTS 16

#define VKTS_FONT_DISTANCE_FIELD VK_TRUE
#define VKTS_FONT_NAME "font/Arial_32.fnt"
#define VKTS_FONT_DF_NAME "font/Arial_32_DF.fnt"
#define VKTS_FONT_SIZE 16.0f
#define VKTS_FONT_COLOR glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)


#define VKTS_BSDF0_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf.vert.spv"
#define VKTS_BSDF1_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_no_texcoord.vert.spv"
#define VKTS_BSDF2_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_tangents.vert.spv"
#define VKTS_BSDF3_VERTEX_SHADER_NAME "shader/SPIR/V/bsdf_skinning.vert.spv"


#define VKTS_ENV_VERTEX_SHADER_NAME "shader/SPIR/V/environment.vert.spv"
#define VKTS_ENV_FRAGMENT_SHADER_NAME "shader/SPIR/V/environment.frag.spv"

#define VKTS_RESOLVE_VERTEX_SHADER_NAME "shader/SPIR/V/texture_ndc.vert.spv"
#define VKTS_RESOLVE_FRAGMENT_SHADER_NAME "shader/SPIR/V/resolve_bsdf.frag.spv"


//#define VKTS_SCENE_NAME "buster_drone/buster_drone.vkts"
#define VKTS_SCENE_NAME "material_probes/material_probes.vkts"
//#define VKTS_SCENE_NAME "panorama/panorama.vkts"

#define VKTS_ENVIRONMENT_SCENE_NAME "primitives/sphere.vkts"

#define VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT (VKTS_BINDING_UNIFORM_TRANSFORM_BINDING_COUNT + VKTS_BINDING_UNIFORM_ENVIRONMENT_COUNT + VKTS_BINDING_UNIFORM_LIGHTING_BINDING_COUNT)

#define VKTS_MAX_CORES 32u

#define VKTS_OUTPUT_BUFFER_SIZE 1024

#define VKTS_BSDF_DESCRIPTOR_SET_COUNT (7 + 1 + 1)

class Example: public vkts::IUpdateThread
{

private:

	const vkts::IContextObjectSP contextObject;

	const int32_t windowIndex;

	const vkts::IVisualContextSP visualContext;

	const vkts::ISurfaceSP surface;

	VkBool32 showStats;

	vkts::IUserCameraSP camera;
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
    VkDescriptorBufferInfo resolveDescriptorBufferInfos[1 + 1];
    VkDescriptorImageInfo resolveDescriptorImageInfos[VKTS_BSDF_DESCRIPTOR_SET_COUNT];

    VkWriteDescriptorSet writeDescriptorSets[VKTS_BINDING_UNIFORM_BSDF_DEFERRED_TOTAL_BINDING_COUNT];
    VkWriteDescriptorSet environmentWriteDescriptorSets[VKTS_ENVIRONMENT_DESCRIPTOR_SET_COUNT];
    VkWriteDescriptorSet resolveWriteDescriptorSets[VKTS_BSDF_DESCRIPTOR_SET_COUNT];

    std::map<uint32_t, VkTsDynamicOffset> dynamicOffsets;

    vkts::IBufferObjectSP vertexViewProjectionUniformBuffer;
	vkts::IBufferObjectSP environmentVertexViewProjectionUniformBuffer;
	vkts::IBufferObjectSP resolveFragmentLightsUniformBuffer;
	vkts::IBufferObjectSP resolveFragmentMatricesUniformBuffer;

	vkts::SmartPointerVector<vkts::IShaderModuleSP> allBSDFVertexShaderModules;

	vkts::IShaderModuleSP envVertexShaderModule;
	vkts::IShaderModuleSP envFragmentShaderModule;

	vkts::IShaderModuleSP resolveVertexShaderModule;
	vkts::IShaderModuleSP resolveFragmentShaderModule;

	vkts::IPipelineLayoutSP environmentPipelineLayout;
	vkts::IPipelineLayoutSP resolvePipelineLayout;

    vkts::IGuiRenderFactorySP guiRenderFactory;
	vkts::IGuiManagerSP guiManager;
	vkts::IGuiFactorySP guiFactory;
    vkts::IFontSP font;

    vkts::ISceneRenderFactorySP renderFactory;
	vkts::ISceneManagerSP sceneManager;
	vkts::ISceneFactorySP sceneFactory;
	vkts::ISceneSP scene;

    vkts::ISceneRenderFactorySP environmentRenderFactory;
	vkts::ISceneManagerSP environmentSceneManager;
	vkts::ISceneFactorySP environmentSceneFactory;
	vkts::ISceneSP environmentScene;

	vkts::IBufferObjectSP screenPlaneVertexBuffer;

	vkts::ISwapchainSP swapchain;

	vkts::IRenderPassSP renderPass;
	vkts::IRenderPassSP gbufferRenderPass;

	vkts::SmartPointerVector<vkts::IGraphicsPipelineSP> allGraphicsPipelines;
	vkts::IGraphicsPipelineSP resolveGraphicsPipeline;

	vkts::SmartPointerVector<vkts::IImageObjectSP> allGBufferTextures;
	vkts::SmartPointerVector<vkts::IImageViewSP> allGBufferImageViews;
	vkts::ISamplerSP gbufferSampler;

    uint32_t swapchainImagesCount;

    vkts::SmartPointerVector<vkts::IImageViewSP> swapchainImageView;

    vkts::SmartPointerVector<vkts::IFramebufferSP> gbufferFramebuffer;
    vkts::SmartPointerVector<vkts::IFramebufferSP> framebuffer;

    vkts::SmartPointerVector<vkts::ICommandBuffersSP> cmdBuffer;

    vkts::SmartPointerVector<vkts::IFenceSP> cmdBufferFence;

    uint32_t rebuildCmdBufferCounter;

    uint32_t fps;
    uint64_t ram;
    float cpuUsageApp;
    uint32_t processors;
    float cpuUsage[VKTS_MAX_CORES];

	VkBool32 buildCmdBuffer(const int32_t usedBuffer);

	VkBool32 buildFramebuffer(const int32_t usedBuffer);

	VkBool32 updateDescriptorSets(const int32_t usedBuffer);

	VkBool32 buildScene(const vkts::ICommandObjectSP& commandObject);

	VkBool32 buildSwapchainImageView(const int32_t usedBuffer);

	VkBool32 buildGBufferSampler();

	VkBool32 buildGBufferImageView(const int32_t usedBuffer);

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
