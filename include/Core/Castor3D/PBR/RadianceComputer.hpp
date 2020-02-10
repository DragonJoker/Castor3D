/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RadianceComputer_H___
#define ___C3D_RadianceComputer_H___

#include "Castor3D/RenderToTexture/RenderCube.hpp"
#include "Castor3D/Texture/Sampler.hpp"

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <array>

namespace castor3d
{
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
			, ashes::Image const & srcTexture );
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
		inline ashes::ImageView const & getResult()const
		{
			return m_resultView;
		}

		inline ashes::Sampler const & getSampler()const
		{
			return m_sampler->getSampler();
		}
		/**@}*/

	private:
		struct RenderPass
		{
			ashes::ImageView dstView;
			ashes::FrameBufferPtr frameBuffer;
		};
		using RenderPasses = std::array< RenderPass, 6 >;

		ashes::ImagePtr m_result;
		ashes::ImageView m_resultView;
		SamplerSPtr m_sampler;
		ashes::ImageView m_srcView;
		ashes::RenderPassPtr m_renderPass;
		RenderPasses m_renderPasses;
		ashes::CommandBufferPtr m_commandBuffer;
	};
}

#endif
