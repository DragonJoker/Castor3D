/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DownscalePass_H___
#define ___C3D_DownscalePass_H___

#include "PassesModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class DownscalePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	srcViews	The source views (must be of same dimensions).
		 *\param[in]	dstSize		The wanted size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	srcViews	Les vues source (devant avoir le mêmes dimensions).
		 *\param[in]	dstSize		La taille voulue.
		 */
		C3D_API DownscalePass( Engine & engine
			, RenderDevice const & device
			, castor::String const & category
			, ashes::ImageViewArray const & srcViews
			, VkExtent2D const & dstSize );
		/**
		 *\~english
		 *\brief		Renders the stencil pass.
		 *\param[in]	toWait	The semaphore to wait.
		 *\~french
		 *\brief		Dessine la passe de stencil.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		C3D_API ashes::Semaphore const & compute( ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline TextureUnitArray const & getResult()const
		{
			return m_result;
		}
		/**@}*/

	private:
		Engine const & m_engine;
		RenderDevice const & m_device;
		TextureUnitArray m_result;
		RenderPassTimerSPtr m_timer;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
