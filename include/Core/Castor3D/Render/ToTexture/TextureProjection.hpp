/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureProjection_H___
#define ___C3D_TextureProjection_H___

#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"

#include <ashespp/Buffer/PushConstantsBuffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	class TextureProjection
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		*name
		*	Construction / Destruction.
		*/
		/**@{*/
		C3D_API explicit TextureProjection( Engine & engine );
		C3D_API ~TextureProjection();
		/**@}*/
		/**
		*name
		*	Initialisation / Cleanup.
		*/
		/**@{*/
		C3D_API void initialise( ashes::ImageView const & source
			, VkFormat targetColour
			, VkFormat targetDepth );
		C3D_API void cleanup();
		/**@}*/
		/**
		*name
		*	Update.
		*/
		/**@{*/
		C3D_API void update( Camera const & camera );
		/**@}*/
		/**
		*name
		*	Getters.
		*/
		/**@{*/
		inline ashes::CommandBuffer const & getCommandBuffer()const
		{
			CU_Require( m_commandBuffer );
			return *m_commandBuffer;
		}

		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_finished );
			return *m_finished;
		}
		/**@}*/

	private:
		ashes::PipelineShaderStageCreateInfoArray doInitialiseShader();
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( ashes::PipelineShaderStageCreateInfoArray & program
			, ashes::ImageView const & texture
			, ashes::RenderPass const & renderPass );
		void doPrepareFrame();

	private:
		MatrixUbo m_matrixUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		castor::Size m_size;
		ashes::RenderPassPtr m_renderPass;
		ashes::PushConstantsBuffer< castor::Point2f > m_sizePushConstant;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		ashes::VertexBufferPtr< NonTexturedCube > m_vertexBuffer;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		SamplerSPtr m_sampler;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		castor::Matrix4x4f m_mtxModel;
	};
}

#endif
