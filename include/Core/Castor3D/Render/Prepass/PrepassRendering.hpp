/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PrepassRendering_H___
#define ___C3D_PrepassRendering_H___

#include "PrepassModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"

#include <RenderGraph/FramePassGroup.hpp>

namespace castor3d
{
	class PrepassRendering
		: public castor::OwnedBy< RenderTechnique >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	parent			The parent technique.
		 *\param[in]	device			The GPU device.
		 *\param[in]	queueData		The queue receiving the GPU commands.
		 *\param[in]	previousPasses	The passes this pass depends on.
		 *\param[in]	progress		The optional progress bar.
		 *\param[in]	deferred		\p true to enable deferred rendering.
		 *\param[in]	visbuffer		\p true to enable visibility buffer.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent			La technique parente.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	queueData		La queue recevant les commandes GPU.
		 *\param[in]	previousPasses	Les passes dont celle-ci dépend.
		 *\param[in]	progress		La barre de progression optionnelle.
		 *\param[in]	deferred		\p true pour activer le deferred rendering.
		 *\param[in]	visbuffer		\p true pour activer le buffer de visibilité.
		 */
		C3D_API PrepassRendering( RenderTechnique & parent
			, RenderDevice const & device
			, QueueData const & queueData
			, crg::FramePassArray const & previousPasses
			, ProgressBar * progress
			, bool deferred
			, bool visbuffer );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~PrepassRendering();
		/**
		 *\~english
		 *\return		The number of steps needed for initialisation, to show progression.
		 *\~french
		 *\return		Le nombre d'étapes nécessaires à l'initialisation, pour en montrer la progression.
		 */
		C3D_API static uint32_t countInitialisationSteps();
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
		C3D_API Engine * getEngine()const;
		C3D_API crg::FramePass const & getLastPass()const;

		Texture const & getDepthObj()const
		{
			return *m_depthObj;
		}

		bool hasVisibility()const
		{
			return m_visibility != nullptr;
		}

		Texture const & getVisibility()const
		{
			CU_Require( hasVisibility() );
			return *m_visibility;
		}

		ashes::Buffer< int32_t > const & getDepthRange()const
		{
			CU_Require( m_depthRange );
			return *m_depthRange;
		}

		bool needsDepthRange()const
		{
			return m_needsDepthRange;
		}

		void setNeedsDepthRange( bool value )
		{
			m_needsDepthRange = value;
		}

		VisibilityPass const & getVisibilityPass()const
		{
			CU_Require( m_visibilityPass );
			return *m_visibilityPass;
		}
		/**@}*/

	private:
		crg::FramePass & doCreateVisibilityPass( ProgressBar * progress
			, crg::FramePassArray const & previousPasses );
		crg::FramePass & doCreateForwardDepthPass( ProgressBar * progress
			, crg::FramePassArray const & previousPasses );
		crg::FramePass & doCreateDeferredDepthPass( ProgressBar * progress
			, crg::FramePassArray const & previousPasses );
		crg::FramePass & doCreateComputeDepthRange( ProgressBar * progress );

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		TexturePtr m_depthObj;
		TexturePtr m_visibility;
		crg::FramePass * m_visibilityPassDesc{};
		VisibilityPass * m_visibilityPass{};
		crg::FramePass * m_depthPassDesc{};
		DepthPass * m_depthPass{};
		ashes::BufferPtr< int32_t > m_depthRange;
		crg::FramePass * m_computeDepthRangeDesc{};
		bool m_needsDepthRange{};
	};
}

#endif
