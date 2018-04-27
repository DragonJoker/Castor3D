/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RadianceComputer_H___
#define ___C3D_RadianceComputer_H___

#include "RenderToTexture/RenderCube.hpp"
#include "Texture/Sampler.hpp"

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
	\brief		Computes the radiance map from an environment map.
	\~french
	\brief		Calcule la texture de radiance pour une texture d'environnement donnée.
	*/
	class RadianceComputer
		: private RenderCube
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
		C3D_API explicit RadianceComputer( Engine & engine
			, castor::Size const & size
			, renderer::Texture const & srcTexture );
		/**
		 *\~english
		 *\brief		Computes the radiance map.
		 *\~french
		 *\brief		Calcule la texture de radiance.
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
		struct RenderPass
		{
			renderer::TextureViewPtr dstView;
			renderer::FrameBufferPtr frameBuffer;
		};
		using RenderPasses = std::array< RenderPass, 6 >;

		RenderSystem & m_renderSystem;
		renderer::TexturePtr m_result;
		renderer::TextureViewPtr m_resultView;
		SamplerSPtr m_sampler;
		renderer::TextureViewPtr m_srcView;
		renderer::RenderPassPtr m_renderPass;
		RenderPasses m_renderPasses;
		renderer::CommandBufferPtr m_commandBuffer;
	};
}

#endif
