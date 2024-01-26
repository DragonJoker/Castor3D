/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RadianceComputer_H___
#define ___C3D_RadianceComputer_H___

#include "PbrModule.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ToTexture/RenderCube.hpp"

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
		 *\param[in]	device		The GPU device.
		 *\param[in]	size		The render size.
		 *\param[in]	srcTexture	The cube texture source.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	size		La taille du rendu.
		 *\param[in]	srcTexture	La texture cube source.
		 */
		C3D_API explicit RadianceComputer( Engine & engine
			, RenderDevice const & device
			, castor::Size const & size
			, Texture const & srcTexture );
		C3D_API ~RadianceComputer()override;
		/**
		 *\~english
		 *\brief		Computes the radiance map.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Calcule la texture de radiance.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 */
		C3D_API void render( QueueData const & queueData );
		/**
		 *\~english
		 *\brief		Computes the radiance map.
		 *\param[in]	queue			The queue receiving the GPU commands.
		 *\param[in]	signalsToWait	The semaphores from the previous render pass.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Calcule la texture de radiance.
		 *\param[in]	queue			La queue recevant les commandes GPU.
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
		C3D_API ashes::Sampler const & getSampler()const noexcept;

		Texture const & getResult()const noexcept
		{
			return m_result;
		}
		/**@}*/

	private:
		struct RenderPass
		{
			VkImageView dstView;
			ashes::FrameBufferPtr frameBuffer;
		};
		using RenderPasses = std::array< RenderPass, 6 >;

		Texture m_result;
		SamplerObs m_sampler{};
		Texture const & m_srcView;
		ashes::Image * m_srcImage;
		ashes::ImageView m_srcImageView;
		ashes::RenderPassPtr m_renderPass;
		RenderPasses m_renderPasses;
		CommandsSemaphore m_commands;
	};
}

#endif
