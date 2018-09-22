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
		C3D_API void initialise( ashes::TextureView const & source
			, ashes::Format targetColour
			, ashes::Format targetDepth );
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
			REQUIRE( m_commandBuffer );
			return *m_commandBuffer;
		}

		inline ashes::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_finished );
			return *m_finished;
		}
		/**@}*/

	private:
		ashes::ShaderStageStateArray doInitialiseShader();
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( ashes::ShaderStageStateArray & program
			, ashes::TextureView const & texture
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
		ashes::PipelinePtr m_pipeline;
		SamplerSPtr m_sampler;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		castor::Matrix4x4r m_mtxModel;
	};
}

#endif
