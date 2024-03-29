/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaqueRendering_H___
#define ___C3D_OpaqueRendering_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizeModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Prepass/PrepassModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Ssao/SsaoPass.hpp"
#include "Castor3D/Render/Transparent/TransparentModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/Attachment.hpp>
#include <RenderGraph/FramePass.hpp>

namespace castor3d
{
	class OpaqueRendering
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
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	parent			La technique parente.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	previous		Le renderer de prépasse.
		 *\param[in]	previousPasses	Les passes dont celle-ci dépend.
		 *\param[in]	progress		La barre de progression optionnelle.
		 */
		C3D_API OpaqueRendering( RenderTechnique & parent
			, RenderDevice const & device
			, PrepassRendering const & previous
			, crg::FramePassArray const & previousPasses
			, ProgressBar * progress );
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
		C3D_API void listIntermediates( castor::Vector< IntermediateView > & intermediates );
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
		C3D_API void update( GpuUpdater & updater )const;
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
		C3D_API Texture const & getSsaoResult()const noexcept;
		C3D_API Texture const & getSssDiffuse()const noexcept;
		C3D_API bool isEnabled()const noexcept;

		ashes::Buffer< uint32_t > const & getMaterialsCounts()const noexcept
		{
			return *m_materialsCounts;
		}

		ashes::Buffer< castor::Point3ui > const & getMaterialsIndirectCounts()const noexcept
		{
			return *m_materialsIndirectCounts;
		}

		ashes::Buffer< uint32_t > const & getMaterialsStarts()const noexcept
		{
			return *m_materialsStarts;
		}

		ashes::Buffer< castor::Point2ui > const & getPixelXY()const noexcept
		{
			return *m_pixelsXY;
		}
		/**@}*/

	public:
		using ShadowMapArray = castor::Vector< ShadowMapUPtr >;

	private:
		SsaoPassUPtr doCreateSsaoPass( ProgressBar * progress
			, crg::FramePass const & lastPass
			, crg::FramePassArray previousPasses )const;
		crg::FramePass & doCreateVisibilityResolve( ProgressBar * progress
			, PrepassRendering const & previous
			, crg::FramePassArray const & previousPasses
			, bool isDeferredLighting );
		crg::FramePass & doCreateVisibilityOpaquePass( ProgressBar * progress
			, crg::FramePass const & lastPass
			, crg::FramePassArray const & previousPasses );
		crg::FramePass & doCreateOpaquePass( ProgressBar * progress
			, crg::FramePass const & lastPass
			, crg::FramePassArray const & previousPasses
			, bool isDeferredLighting );
		bool doIsOpaquePassEnabled()const;
		bool doIsDeferredOpaquePassEnabled()const;
		bool doIsVisibilityOpaquePassEnabled()const;

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		ashes::BufferPtr< uint32_t > m_materialsCounts;
		ashes::BufferPtr< castor::Point3ui > m_materialsIndirectCounts;
		ashes::BufferPtr< uint32_t > m_materialsStarts;
		ashes::BufferPtr< castor::Point2ui > m_pixelsXY;
		crg::RunnablePass::IsEnabledCallback m_opaquePassEnabled;
		crg::RunnablePass::IsEnabledCallback m_deferredOpaquePassEnabled;
		crg::RunnablePass::IsEnabledCallback m_visibilityOpaquePassEnabled;
		VisibilityReorderPassUPtr m_visibilityReorder;
		SsaoPassUPtr m_ssao;
		crg::FramePass * m_visibilityResolveDesc{};
		crg::FramePass * m_opaquePassDesc{};
		RenderTechniquePass * m_opaquePass{};
		SubsurfaceScatteringPassUPtr m_subsurfaceScattering{};
		crg::FramePass * m_deferredVisibilityResolveDesc{};
		crg::FramePass * m_deferredOpaquePassDesc{};
		RenderTechniquePass * m_deferredOpaquePass{};
		crg::FramePass * m_visibilityOpaquePassDesc{};
		RenderTechniquePass * m_visibilityOpaquePass{};
	};
}

#endif
