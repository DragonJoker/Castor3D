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
				, crg::ResourcesCache & resources
				, ashes::RenderPass const & renderPass
				, uint32_t mipLevel
				, VkExtent2D const & originalSize
				, VkExtent2D const & size
				, ashes::ImageView const & srcView
				, Texture const & dstTexture
				, SamplerObs sampler
				, bool isCharlie );
			void registerFrames();
			void render( QueueData const & queueData )const;
			crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray const & signalsToWait
				, ashes::Queue const & queue )const;

		private:
			struct FrameBuffer
			{
				VkImageView dstView;
				ashes::FrameBufferPtr frameBuffer;
			};
			ashes::RenderPass const & m_renderPass;
			castor::String m_prefix;
			castor::Array< FrameBuffer, 6u > m_frameBuffers;
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
		 *\param[in]	isCharlie	\p true for Charlie distribution (for sheen), \p false for GGX.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	size		La taille du rendu.
		 *\param[in]	srcTexture	La texture cube source.
		 *\param[in]	sampler		Le sampler utilisé pour la texture source.
		 *\param[in]	isCharlie	\p true pour la distribution Charlie (pour le sheen), \p false pour la distribution GGX.
		 */
		C3D_API explicit EnvironmentPrefilter( Engine & engine
			, RenderDevice const & device
			, castor::Size const & size
			, Texture const & srcTexture
			, SamplerObs sampler
			, bool isCharlie );
		C3D_API ~EnvironmentPrefilter()noexcept;
		/**
		 *\~english
		 *\brief		Generates the mipmap levels.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Génère les niveaux de mipmap.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 */
		C3D_API void render( QueueData const & queueData )const;
		/**
		 *\~english
		 *\brief		Generates the mipmap levels.
		 *\param[in]	queue			The queue receiving the render commands.
		 *\param[in]	signalsToWait	The semaphores from the previous render pass.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Génère les niveaux de mipmap.
		 *\param[in]	queue			La queue recevant les commandes d'initialisation.
		 *\param[in]	signalsToWait	Les sémaphores de la passe de rendu précédente.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray signalsToWait
			, ashes::Queue const & queue )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Texture const & getResult()const noexcept
		{
			return m_result;
		}

		ashes::Sampler const & getSampler()const noexcept
		{
			CU_Require( m_sampler != nullptr );
			return m_sampler->getSampler();
		}
		/**@}*/

	private:
		RenderDevice const & m_device;
		Texture const & m_srcView;
		castor::String m_prefix;
		ashes::Image * m_srcImage;
		ashes::ImageView m_srcImageView;
		Texture m_result;
		SamplerObs m_sampler{};
		ashes::RenderPassPtr m_renderPass;
		castor::Vector< castor::RawUniquePtr< MipRenderCube > > m_renderPasses;
	};
}

#endif
