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

#include <RenderGraph/FrameGraphPrerequisites.hpp>

namespace castor3d
{
	class DownscalePass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	device		The GPU device.
		 *\param[in]	category	The debug category name.
		 *\param[in]	srcViews	The source views (must be of same dimensions).
		 *\param[in]	dstSize		The wanted size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	category	Le nom de la catégorie de debug.
		 *\param[in]	srcViews	Les vues source (devant avoir le mêmes dimensions).
		 *\param[in]	dstSize		La taille voulue.
		 */
		C3D_API DownscalePass( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, castor::String const & category
			, TextureArray const & srcViews
			, VkExtent2D const & dstSize );
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
		inline TextureArray const & getResult()const
		{
			return m_result;
		}
		/**@}*/

	private:
		RenderDevice const & m_device;
		TextureArray m_result;
	};
}

#endif
