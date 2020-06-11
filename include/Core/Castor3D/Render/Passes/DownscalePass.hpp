/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DownscalePass_H___
#define ___C3D_DownscalePass_H___

#include "PassesModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

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
		 *\param[in]	engine			The engine.
		 *\param[in]	depthView		The depth buffer view.
		 *\param[in]	matrixUbo		The matrix UBO.
		 *\param[in]	modelMatrixUbo	The model matrix UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	depthView		L'attache du tampon de profondeur.
		 *\param[in]	matrixUbo		L'UBO des matrices.
		 *\param[in]	modelMatrixUbo	L'UBO des matrices modèle.
		 */
		C3D_API DownscalePass( Engine & engine
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
		TextureUnitArray m_result;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
	};
}

#endif
