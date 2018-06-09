/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureProjection_H___
#define ___C3D_TextureProjection_H___

#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Miscellaneous/PushConstantRange.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <RenderPass/RenderPass.hpp>
#include <Sync/Semaphore.hpp>

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Class used to render colour equirectangular textures to cube maps.
	\~french
	\brief		Classe utilisée pour rendre les textures couleur équirectangulaires dans des cube maps.
	*/
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
		C3D_API void initialise( renderer::TextureView const & source
			, renderer::Format targetColour
			, renderer::Format targetDepth );
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
		inline renderer::CommandBuffer const & getCommandBuffer()const
		{
			REQUIRE( m_commandBuffer );
			return *m_commandBuffer;
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_finished );
			return *m_finished;
		}
		/**@}*/

	private:
		renderer::ShaderStageStateArray doInitialiseShader();
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( renderer::ShaderStageStateArray & program
			, renderer::TextureView const & texture
			, renderer::RenderPass const & renderPass );
		void doPrepareFrame();

	private:
		MatrixUbo m_matrixUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		castor::Size m_size;
		renderer::RenderPassPtr m_renderPass;
		renderer::PushConstantsBuffer< castor::Point2f > m_sizePushConstant;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::VertexBufferPtr< NonTexturedCube > m_vertexBuffer;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::PipelinePtr m_pipeline;
		SamplerSPtr m_sampler;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_finished;
		castor::Matrix4x4r m_mtxModel;
	};
}

#endif
