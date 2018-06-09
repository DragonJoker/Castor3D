/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentPrefilter_H___
#define ___C3D_EnvironmentPrefilter_H___

#include "RenderToTexture/RenderCube.hpp"
#include "Texture/Sampler.hpp"

#include <Buffer/PushConstantsBuffer.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>

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
	private:
		class MipRenderCube
			: public RenderCube
		{
		public:
			MipRenderCube( RenderSystem & renderSystem
				, renderer::RenderPass const & renderPass
				, uint32_t mipLevel
				, renderer::Extent2D const & originalSize
				, renderer::Extent2D const & size
				, renderer::TextureView const & srcView
				, renderer::Texture const & dstTexture
				, SamplerSPtr sampler );
			void registerFrames();
			void render();

		private:
			struct FrameBuffer
			{
				renderer::TextureViewPtr dstView;
				renderer::FrameBufferPtr frameBuffer;
			};
			renderer::Device const & m_device;
			renderer::RenderPass const & m_renderPass;
			renderer::CommandBufferPtr m_commandBuffer;
			renderer::FencePtr m_fence;
			SamplerSPtr m_sampler;
			std::array< FrameBuffer, 6u > m_frameBuffers;
		};

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
			, renderer::Texture const & srcTexture
			, SamplerSPtr sampler );
		/**
		 *\~english
		 *\brief		Generates the mipmap levels.
		 *\~french
		 *\brief		Génère les niveaux de mipmap.
		 */
		C3D_API void render();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline renderer::TextureView const & getResult()const
		{
			return *m_resultView;
		}

		inline renderer::Sampler const & getSampler()const
		{
			return m_sampler->getSampler();
		}
		/**@}*/

	private:
		RenderSystem & m_renderSystem;
		renderer::TextureViewPtr m_srcView;
		renderer::TexturePtr m_result;
		renderer::TextureViewPtr m_resultView;
		SamplerSPtr m_sampler;
		renderer::RenderPassPtr m_renderPass;
		std::vector< std::unique_ptr< MipRenderCube > > m_renderPasses;
	};
}

#endif
