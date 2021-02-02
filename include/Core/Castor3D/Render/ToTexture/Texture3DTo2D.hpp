/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelRenderer_H___
#define ___C3D_VoxelRenderer_H___

#include "RenderToTextureModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

namespace castor3d
{
	struct Texture3DTo2DData
	{
		castor::Point4f gridCenterCellSize;
		uint32_t gridSize;
	};

	class Texture3DTo2D
	{
	public:
		C3D_API Texture3DTo2D( RenderDevice const & device
			, VkExtent2D const & size
			, MatrixUbo const & matrixUbo );
		C3D_API ~Texture3DTo2D();

		C3D_API void createPasses( IntermediateViewArray intermediates );
		C3D_API void initialise();
		C3D_API void update( CpuUpdater & updater );
		C3D_API ashes::Semaphore const & render( ashes::Semaphore const & toWait );

		ashes::ImageView const & getTarget()
		{
			return m_targetView;
		}

	private:
		RenderSystem & m_renderSystem;
		RenderDevice const & m_device;
		MatrixUbo const & m_matrixUbo;
		ashes::ImagePtr m_target;
		ashes::ImageView m_targetView;
		TextureUnit m_depthBuffer;
		UniformBufferOffsetT< Texture3DTo2DData > m_uniformBuffer;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::RenderPassPtr m_renderPass;
		ShaderModule m_vertexShader;
		ShaderModule m_geometryShader;
		ShaderModule m_pixelShader;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::FrameBufferPtr m_frameBuffer;

		struct Texture3DToScreen
		{
			Texture3DToScreen();
			Texture3DToScreen( RenderDevice const & device
				, UniformBufferOffsetT< Texture3DTo2DData > const & uniformBuffer
				, MatrixUbo const & matrixUbo
				, IntermediateView const & texture3D
				, ashes::RenderPass const & renderPass
				, ashes::DescriptorSetPool const & descriptorSetPool
				, ashes::FrameBuffer const & frameBuffer
				, ashes::PipelineLayout const & pipelineLayout
				, ashes::GraphicsPipeline const & pipeline );

			ashes::DescriptorSetPtr descriptorSet;
			CommandsSemaphore commands;
		};

		IntermediateViewArray m_textures;
		std::vector< Texture3DToScreen > m_texture3DToScreen;
		uint32_t m_index{ 0u };
	};
}

#endif
