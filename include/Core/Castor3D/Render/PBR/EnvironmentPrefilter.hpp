/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentPrefilter_H___
#define ___C3D_EnvironmentPrefilter_H___

#include "PbrModule.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
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
				, QueueData const & queueData
				, crg::ResourceHandler & handler
				, ashes::RenderPass const & renderPass
				, uint32_t mipLevel
				, VkExtent2D const & originalSize
				, VkExtent2D const & size
				, ashes::ImageView const & srcView
				, Texture const & dstTexture
				, SamplerResPtr sampler );
			void registerFrames();
			void render( QueueData const & queueData );
			ashes::Semaphore const & render( QueueData const & queueData
				, ashes::Semaphore const & toWait );

		private:
			struct FrameBuffer
			{
				VkImageView dstView;
				ashes::FrameBufferPtr frameBuffer;
			};
			ashes::RenderPass const & m_renderPass;
			SamplerResPtr m_sampler;
			std::array< FrameBuffer, 6u > m_frameBuffers;
			CommandsSemaphore m_commands;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	device		The GPU device.
		 *\param[in]	size		The render size.
		 *\param[in]	srcTexture	The cube texture source.
		 *\param[in]	sampler		The sampler used for the source texture.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	size		La taille du rendu.
		 *\param[in]	srcTexture	La texture cube source.
		 *\param[in]	sampler		Le sampler utilisé pour la texture source.
		 */
		C3D_API explicit EnvironmentPrefilter( Engine & engine
			, RenderDevice const & device
			, castor::Size const & size
			, Texture const & srcTexture
			, SamplerResPtr sampler );
		C3D_API ~EnvironmentPrefilter();
		/**
		 *\~english
		 *\brief		Generates the mipmap levels.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Génère les niveaux de mipmap.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 */
		C3D_API void render( QueueData const & queueData );
		/**
		 *\~english
		 *\brief		Generates the mipmap levels.
		 *\param[in]	queueData	The queue receiving the render commands.
		 *\param[in]	toWait		The semaphore from the previous render pass.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Génère les niveaux de mipmap.
		 *\param[in]	queueData	La queue recevant les commandes d'initialisation.
		 *\param[in]	toWait		Le sémaphore de la passe de rendu précédente.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API ashes::Semaphore const & render( QueueData const & queueData
			, ashes::Semaphore const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Texture const & getResult()const
		{
			return m_result;
		}

		ashes::Sampler const & getSampler()const
		{
			return m_sampler.lock()->getSampler();
		}
		/**@}*/

	public:
		C3D_API static uint32_t const MaxIblReflectionLod;

	private:
		RenderDevice const & m_device;
		Texture const & m_srcView;
		ashes::ImagePtr m_srcImage;
		ashes::ImageView m_srcImageView;
		Texture m_result;
		SamplerResPtr m_sampler;
		ashes::RenderPassPtr m_renderPass;
		std::vector< std::unique_ptr< MipRenderCube > > m_renderPasses;
	};
}

#endif
