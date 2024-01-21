/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TransparentRendering_H___
#define ___C3D_TransparentRendering_H___

#include "TransparentModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Prepass/PrepassModule.hpp"
#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"

#include <RenderGraph/FramePassGroup.hpp>

namespace castor3d
{
	class TransparentRendering
		: public castor::OwnedBy< RenderTechnique >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent			The parent technique.
		 *\param[in]	device			The GPU device.
		 *\param[in]	previous		The prepass renderer.
		 *\param[in]	previousPasses	The passes this pass depends on.
		 *\param[in]	progress		The optional progress bar.
		 *\param[in]	weightedBlended	\p true to enable weighted blended rendering.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent			La technique parente.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	previous		Le renderer de prépasse.
		 *\param[in]	previousPasses	Les passes dont celle-ci dépend.
		 *\param[in]	progress		La barre de progression optionnelle.
		 *\param[in]	weightedBlended	\p true pour activer le weighted blended rendering.
		 */
		C3D_API TransparentRendering( RenderTechnique & parent
			, RenderDevice const & device
			, OpaqueRendering const & previous
			, crg::FramePassArray const & previousPasses
			, ProgressBar * progress
			, bool weightedBlended );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TransparentRendering()noexcept;
		/**
		 *\~english
		 *\return		The number of steps needed for initialisation, to show progression.
		 *\~french
		 *\return		Le nombre d'étapes nécessaires à l'initialisation, pour en montrer la progression.
		 */
		static uint32_t countInitialisationSteps()noexcept;
		/**
		 *\~english
		 *\brief		Lists the intermediate view used by the whole technique.
		 *\param[out]	intermediates	Receives the intermediate views.
		 *\~french
		 *\brief		Liste les vues intermédiaires utilisées par toute la technique.
		 *\param[out]	intermediates	Reçoit les vues intermédiaires.
		 */
		C3D_API void listIntermediates( std::vector< IntermediateView > & intermediates );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param visitor
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le ... visiteur.
		*/
		C3D_API void accept( RenderTechniqueVisitor & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API Engine * getEngine()const noexcept;
		C3D_API crg::FramePass const & getLastPass()const noexcept;
		/**@}*/

	private:
		crg::FramePass & doCreateMipGenPass( ProgressBar * progress
			, crg::FramePass const & lastPass
			, crg::FramePassArray previousPasses );
		crg::FramePass & doCreateForwardTransparentPass( ProgressBar * progress
			, crg::FramePass const & lastPass );
		crg::FramePass & doCreateWBTransparentPass( ProgressBar * progress
			, crg::FramePass const & lastPass );

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		Texture m_mippedColour;
		TransparentPassResultUPtr m_transparentPassResult;
		bool m_enabled{};
		crg::FramePass * m_mipgenPassDesc{};
		crg::FramePass * m_transparentPassDesc{};
		RenderTechniqueNodesPass * m_transparentPass{};
		WeightedBlendRenderingUPtr m_weightedBlendRendering;
	};
}

#endif
