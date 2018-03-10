/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureProjection_H___
#define ___C3D_TextureProjection_H___

#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <Miscellaneous/PushConstantRange.hpp>
#include <Pipeline/VertexLayout.hpp>

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
		C3D_API void initialise( TextureLayout const & texture
			, renderer::RenderPass const & renderPass );
		C3D_API void cleanup();
		/**@}*/
		/**
		*name
		*	Update.
		*/
		/**@{*/
		C3D_API void update( Camera const & camera );
		/**@}*/

		inline renderer::CommandBuffer const & getCommandBuffer()const
		{
			REQUIRE( m_commandBuffer );
			return *m_commandBuffer;
		}

	private:
		renderer::ShaderStageStateArray doInitialiseShader();
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( renderer::ShaderStageStateArray & program
			, TextureLayout const & texture
			, renderer::RenderPass const & renderPass );
		void doPrepareFrame();

	private:
		MatrixUbo m_matrixUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		castor::Size m_size;
		renderer::PushConstantsBuffer< castor::Point2f > m_sizePushConstant;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::PushConstantRange m_pushConstantRange;
		renderer::VertexBufferPtr< NonTexturedCube > m_vertexBuffer;
		RenderPipelineUPtr m_pipeline;
		SamplerSPtr m_sampler;
		renderer::CommandBufferPtr m_commandBuffer;
		castor::Matrix4x4r m_mtxModel;
	};
}

#endif
