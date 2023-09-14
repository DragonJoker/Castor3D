/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechnique_H___
#define ___C3D_RenderTechnique_H___

#include "RenderModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Clustered/ClusteredModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizeModule.hpp"
#include "Castor3D/Render/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Prepass/PrepassModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMap.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Render/Transparent/TransparentModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"

#include "Castor3D/Render/Texture.hpp"
#include "Castor3D/Render/Opaque/OpaqueRendering.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/Prepass/PrepassRendering.hpp"
#include "Castor3D/Render/Transparent/TransparentRendering.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/FramePass.hpp>

namespace castor3d
{
	class RenderTechnique
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
		friend class RenderTechniquePass;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name			The technique name.
		 *\param[in]	renderTarget	The render target for this technique.
		 *\param[in]	device			The GPU device.
		 *\param[in]	queueData		The queue receiving the GPU commands.
		 *\param[in]	parameters		The technique parameters.
		 *\param[in]	colour			The result colour image.
		 *\param[in]	intermediate	The intermediate colour image.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	progress		The optional progress bar.
		 *\param[in]	visbuffer		\p true to enable visibility buffer.
		 *\param[in]	weightedBlended	\p true to enable weighted blended rendering.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	renderTarget	La render target pour cette technique.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	queueData		La queue recevant les commandes GPU.
		 *\param[in]	parameters		Les paramètres de la technique.
		 *\param[in]	colour			L'image couleur résultat.
		 *\param[in]	intermediate	L'image couleur intermédiaire.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	progress		La barre de progression optionnelle.
		 *\param[in]	visbuffer		\p true pour activer le visibility buffer.
		 *\param[in]	weightedBlended	\p true pour activer le weighed blended rendering.
		 */
		C3D_API RenderTechnique( castor::String const & name
			, RenderTarget & renderTarget
			, RenderDevice const & device
			, QueueData const & queueData
			, Parameters const & parameters
			, Texture const & colour
			, Texture const & intermediate
			, SsaoConfig const & ssaoConfig
			, ProgressBar * progress
			, bool visbuffer
			, bool weightedBlended );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RenderTechnique();
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
		 *\brief		Renders maps needed for the actual rendering.
		 *\param[in]	toWait	The semaphores to wait.
		 *\param[in]	queue	The queue receiving the render commands.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine les textures nécessaires au rendu.
		 *\param[in]	toWait	Les sémaphores à attendre.
		 *\param[in]	queue	The queue recevant les commandes de dessin.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API crg::SemaphoreWaitArray preRender( crg::SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		/**
		 *\~english
		 *\brief		Writes the technique into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit la technique dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API bool writeInto( castor::TextFile & file );
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
		C3D_API crg::FramePass const & getLastPass()const;
		C3D_API SsaoConfig const & getSsaoConfig()const;
		C3D_API ClustersConfig & getClustersConfig()const;
		C3D_API SsaoConfig & getSsaoConfig();
		C3D_API Texture const & getSsaoResult()const;
		C3D_API TechniquePassVector getCustomRenderPasses()const;
		C3D_API Texture const & getDiffuseLightingResult()const;
		C3D_API Texture const & getScatteringLightingResult()const;
		C3D_API Texture const & getBaseColourResult()const;
		C3D_API crg::ResourcesCache & getResources()const;
		C3D_API bool isOpaqueEnabled()const;
		C3D_API DebugConfig & getDebugConfig()const;

		castor::Size const & getSize()const
		{
			return m_rawSize;
		}

		crg::ImageViewIdArray getSampledResult()const
		{
			return { m_colour->sampledViewId };
		}

		crg::ImageViewIdArray getTargetResult()const
		{
			return { m_colour->targetViewId };
		}

		crg::ImageViewIdArray getSampledIntermediate()const
		{
			return { m_intermediate->sampledViewId };
		}

		crg::ImageViewIdArray getTargetIntermediate()const
		{
			return { m_intermediate->targetViewId };
		}

		crg::ImageViewIdArray getTargetDepth()const
		{
			return { m_depth.targetViewId };
		}

		Texture const & getResult()const
		{
			return *m_colour;
		}

		Texture const & getIntermediate()const
		{
			return *m_intermediate;
		}

		VkExtent3D const & getTargetExtent()const
		{
			return m_colour->getExtent();
		}

		VkFormat getTargetFormat()const
		{
			return m_colour->getFormat();
		}

		Texture const & getNormal()const
		{
			return m_normal;
		}

		Texture const & getDepthObj()const
		{
			return m_prepass.getDepthObj();
		}

		ashes::Buffer< int32_t > const & getDepthRange()const
		{
			return m_prepass.getDepthRange();
		}

		void setNeedsDepthRange( bool v )
		{
			m_prepass.setNeedsDepthRange( v );
		}

		ShadowMapResult const & getDirectionalShadowPassResult()const
		{
			return m_directionalShadowMap->getShadowPassResult( false );
		}

		ShadowMapResult const & getPointShadowPassResult()const
		{
			return m_pointShadowMap->getShadowPassResult( false );
		}

		ShadowMapResult const & getSpotShadowPassResult()const
		{
			return m_spotShadowMap->getShadowPassResult( false );
		}

		CameraUbo const & getCameraUbo()const
		{
			return m_cameraUbo;
		}

		CameraUbo & getCameraUbo()
		{
			return m_cameraUbo;
		}

		SceneUbo const & getSceneUbo()const
		{
			return m_sceneUbo;
		}

		SceneUbo & getSceneUbo()
		{
			return m_sceneUbo;
		}

		ShadowMapLightTypeArray const & getShadowMaps()const
		{
			return m_allShadowMaps;
		}

		RenderTarget const & getRenderTarget()const
		{
			return m_renderTarget;
		}

		RenderTarget & getRenderTarget()
		{
			return m_renderTarget;
		}

		bool isMultisampling()const
		{
			return false;
		}

		ashes::Buffer< uint32_t > const & getMaterialsCounts()const
		{
			return m_opaque.getMaterialsCounts();
		}

		ashes::Buffer< uint32_t > const & getMaterialsStarts()const
		{
			return m_opaque.getMaterialsStarts();
		}

		ashes::Buffer< castor::Point2ui > const & getPixelXY()const
		{
			return m_opaque.getPixelXY();
		}

		bool hasVisibility()const
		{
			return m_prepass.hasVisibility();
		}

		Texture const & getVisibilityResult()const
		{
			return m_prepass.getVisibility();
		}

		crg::FramePass const & getGetLastDepthPass()const
		{
			return *m_lastDepthPass;
		}

		crg::FramePass const & getDepthRangePass()const
		{
			return *m_depthRangePass;
		}

		crg::FramePass const & getGetLastOpaquePass()const
		{
			return *m_lastOpaquePass;
		}

		crg::FramePass const & getGetLastTransparentPass()const
		{
			return *m_lastTransparentPass;
		}

		crg::FramePassGroup & getGraph()
		{
			return m_graph;
		}

		IndirectLightingData const & getIndirectLighting()const noexcept
		{
			return m_indirectLighting;
		}

		bool hasShadowBuffer()const noexcept
		{
			return m_shadowBuffer != nullptr;
		}

		ShadowBuffer & getShadowBuffer()const noexcept
		{
			CU_Require( hasShadowBuffer() );
			return *m_shadowBuffer;
		}
		/**@}*/

	public:
		using ShadowMapArray = std::vector< ShadowMapUPtr >;

	private:
		crg::FramePassArray doCreateRenderPasses( ProgressBar * progress
			, TechniquePassEvent event
			, crg::FramePass const * previousPass );
		crg::FramePassArray doCreateRenderPasses( ProgressBar * progress
			, TechniquePassEvent event
			, crg::FramePassArray previousPasses );
		BackgroundRendererUPtr doCreateBackgroundPass( ProgressBar * progress );
		void doInitialiseLpv();
		void doUpdateShadowMaps( CpuUpdater & updater );
		void doUpdateShadowMaps( GpuUpdater & updater );
		void doUpdateLpv( CpuUpdater & updater );
		void doUpdateLpv( GpuUpdater & updater );

		crg::SemaphoreWaitArray doRenderShadowMaps( crg::SemaphoreWaitArray const & semaphore
			, ashes::Queue const & queue )const;
		crg::SemaphoreWaitArray doRenderLPV( crg::SemaphoreWaitArray const & semaphore
			, ashes::Queue const & queue );
		crg::SemaphoreWaitArray doRenderEnvironmentMaps( crg::SemaphoreWaitArray const & semaphore
			, ashes::Queue const & queue )const;
		crg::SemaphoreWaitArray doRenderVCT( crg::SemaphoreWaitArray const & semaphore
			, ashes::Queue const & queue )const;

	private:
		RenderTarget & m_renderTarget;
		RenderDevice const & m_device;
		castor::Size m_targetSize;
		castor::Size m_rawSize;
		Texture const * m_colour;
		Texture const * m_intermediate;
		Texture m_depth;
		Texture m_normal;
		CameraUbo m_cameraUbo;
		SceneUbo m_sceneUbo;
		LpvGridConfigUbo m_lpvConfigUbo;
		LayeredLpvGridConfigUbo m_llpvConfigUbo;
		VoxelizerUbo m_vctConfigUbo;
		crg::FramePassGroup & m_graph;
		ShadowBufferUPtr m_shadowBuffer;
		ShadowMapUPtr m_directionalShadowMap;
		ShadowMapUPtr m_pointShadowMap;
		ShadowMapUPtr m_spotShadowMap;
		VoxelizerUPtr m_voxelizer;
		LightVolumePassResultUPtr m_lpvResult;
		LightVolumePassResultArray m_llpvResult;
		IndirectLightingData m_indirectLighting;
		TechniquePasses m_renderPasses;
		PrepassRendering m_prepass;
		crg::FramePass const * m_lastDepthPass{};
		crg::FramePass const * m_depthRangePass{};
		RenderNodesPass * m_clustersFlagsPass{};
		crg::FramePass const * m_clustersLastPass{};
		BackgroundRendererUPtr m_background{};
		OpaqueRendering m_opaque;
		crg::FramePass const * m_lastOpaquePass{};
		TransparentRendering m_transparent;
		crg::FramePass const * m_lastTransparentPass{};
		crg::FrameGraph m_clearLpvGraph;
		crg::RunnableGraphPtr m_clearLpvRunnable;
		ShadowMapLightTypeArray m_allShadowMaps;
		ShadowMapLightArray m_activeShadowMaps;
		LightPropagationVolumesLightType m_lightPropagationVolumes;
		LayeredLightPropagationVolumesLightType m_layeredLightPropagationVolumes;
		LightPropagationVolumesGLightType m_lightPropagationVolumesG;
		LayeredLightPropagationVolumesGLightType m_layeredLightPropagationVolumesG;
	};
}

#endif
