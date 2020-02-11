/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentPrefilter_H___
#define ___C3D_EnvironmentPrefilter_H___

#include "Castor3D/Render/ToTexture/RenderCube.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"

#include <ashespp/Buffer/PushConstantsBuffer.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <array>

namespace castor3d
{
	class EnvironmentPrefilter
	{
	private:
		class MipRenderCube
			: public RenderCube
		{
		public:
			MipRenderCube( RenderDevice const & device
				, ashes::RenderPass const & renderPass
				, uint32_t mipLevel
				, VkExtent2D const & originalSize
				, VkExtent2D const & size
				, ashes::ImageView const & srcView
				, ashes::Image const & dstTexture
				, SamplerSPtr sampler );
			void registerFrames();
			void render();

		private:
			struct FrameBuffer
			{
				ashes::ImageView dstView;
				ashes::FrameBufferPtr frameBuffer;
			};
			ashes::RenderPass const & m_renderPass;
			ashes::CommandBufferPtr m_commandBuffer;
			ashes::FencePtr m_fence;
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
		 *\param[in]	sampler		The sampler used for the source texture.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		La taille du rendu.
		 *\param[in]	srcTexture	La texture cube source.
		 *\param[in]	sampler		Le sampler utilisé pour la texture source.
		 */
		C3D_API explicit EnvironmentPrefilter( Engine & engine
			, castor::Size const & size
			, ashes::Image const & srcTexture
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
		RenderDevice const & m_device;
		ashes::ImageView m_srcView;
		ashes::ImagePtr m_result;
		ashes::ImageView m_resultView;
		SamplerSPtr m_sampler;
		ashes::RenderPassPtr m_renderPass;
		std::vector< std::unique_ptr< MipRenderCube > > m_renderPasses;
	};
}

#endif
