/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelRenderer_H___
#define ___C3D_VoxelRenderer_H___

#include "RenderToTextureModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <RenderGraph/FrameGraphPrerequisites.hpp>

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

		C3D_API void createPasses( QueueData const & queueData
			, IntermediateViewArray intermediates );
		C3D_API void initialise( QueueData const & queueData );
		C3D_API void update( CpuUpdater & updater );
		C3D_API crg::SemaphoreWait render( ashes::Queue const & queue
			, crg::SemaphoreWait const & toWait );
		C3D_API void render( ashes::Queue const & queue
			, ashes::VkSemaphoreArray & semaphores
			, ashes::VkPipelineStageFlagsArray & stages );

		Texture const & getTarget()
		{
			return m_target;
		}

	private:
		RenderDevice const & m_device;
		MatrixUbo const & m_matrixUbo;
		Texture m_target;
		Texture m_depthBuffer;
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
				, QueueData const & queueData
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
