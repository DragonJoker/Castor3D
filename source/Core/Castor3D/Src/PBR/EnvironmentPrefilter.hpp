/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentPrefilter_H___
#define ___C3D_EnvironmentPrefilter_H___

#include "Texture/TextureUnit.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>

#include <array>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Prefilters an environment map, by generating its mipmap levels.
	\~french
	\brief		Préfiltre une texture d'environnement, en générant ses différents niveaux de mipmap.
	*/
	class EnvironmentPrefilter
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render size.
		 *\param[in]	srcTexture	The cube texture source.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		La taille du rendu.
		 *\param[in]	srcTexture	La texture cube source.
		 */
		C3D_API explicit EnvironmentPrefilter( Engine & engine
			, castor::Size const & size
			, renderer::Texture const & srcTexture );
		/**
		 *\~english
		 *\brief		Generates the mipmap levels.
		 *\~french
		 *\brief		Génère les niveaux de mipmap.
		 */
		C3D_API void render();
		/**
		 *\~english
		 *\return		The prefiltered environment texture.
		 *\~french
		 *\return		La texture d'environnement préfiltrée.
		 */
		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		/**
		 *\~english
		 *\brief		Creates the filtering shader program.
		 *\~french
		 *\brief		Crée le programme shader de filtrage.
		 */
		renderer::ShaderStageStateArray doCreateProgram( castor::Size const & size );

	private:
		struct RenderPass
		{
			renderer::TextureViewPtr srcView;
			renderer::TextureViewPtr dstView;
			renderer::DescriptorSetPtr descriptorSet;
			renderer::RenderPassPtr renderPass;
			renderer::FrameBufferPtr frameBuffer;
			renderer::PipelinePtr pipeline;
			renderer::PushConstantsBufferPtr< float > pushConstants;
		};

		using CubePasses = std::array< RenderPass, 6 >;
		using RenderPasses = std::vector< CubePasses >;
		RenderSystem & m_renderSystem;
		TextureUnit m_result;
		NonTexturedCube m_vertexData;
		SamplerSPtr m_sampler;
		renderer::VertexBufferPtr< NonTexturedCube > m_vertexBuffer;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::UniformBufferPtr< renderer::Mat4 > m_configUbo;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		RenderPasses m_renderPasses;
		renderer::CommandBufferPtr m_commandBuffer;
	};
}

#endif
