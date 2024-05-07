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
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapsModule.hpp"
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
		 *\param[in]	colour			The result colour image.
		 *\param[in]	intermediate	The intermediate colour image.
		 *\param[in]	previousPasses	The passes that the technique has to wait for.
		 *\param[in]	progress		The optional progress bar.
		 *\param[in]	visbuffer		\p true to enable visibility buffer.
		 *\param[in]	weightedBlended	\p true to enable weighted blended rendering.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	renderTarget	La render target pour cette technique.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	colour			L'image couleur résultat.
		 *\param[in]	intermediate	L'image couleur intermédiaire.
		 *\param[in]	previousPasses	Les passes que la technique doit attendre.
		 *\param[in]	progress		La barre de progression optionnelle.
		 *\param[in]	visbuffer		\p true pour activer le visibility buffer.
		 *\param[in]	weightedBlended	\p true pour activer le weighed blended rendering.
		 */
		C3D_API RenderTechnique( castor::String const & name
			, RenderTarget & renderTarget
			, RenderDevice const & device
			, Texture const & colour
			, Texture const & intermediate
			, crg::FramePassArray previousPasses
			, ProgressBar * progress
			, bool visbuffer
			, bool weightedBlended );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~RenderTechnique()noexcept;
		/**
		 *\~english
		 *\return		The number of steps needed for initialisation, to show progression.
		 *\~french
		 *\return		Le nombre d'étapes nécessaires à l'initialisation, pour en montrer la progression.
		 */
		static uint32_t countInitialisationSteps()noexcept;
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
		C3D_API ClustersConfig const * getClustersConfig()const;
		C3D_API SsaoConfig & getSsaoConfig();
		C3D_API Texture const & getSsaoResult()const;
		C3D_API Texture const & getSssDiffuse()const;
		C3D_API TechniquePassVector getCustomRenderPasses()const;
		C3D_API crg::ResourcesCache & getResources()const;
		C3D_API bool isOpaqueEnabled()const;
		C3D_API DebugConfig & getDebugConfig()const;
		C3D_API bool areDebugTargetsEnabled()const noexcept;
		C3D_API CameraUbo const & getCameraUbo()const noexcept;
		C3D_API SceneUbo const & getSceneUbo()const noexcept;

		castor::Size const & getSize()const noexcept
		{
			return m_rawSize;
		}

		crg::ImageViewIdArray getSampledResult()const noexcept
		{
			return { m_colour->sampledViewId };
		}

		crg::ImageViewIdArray getTargetResult()const noexcept
		{
			return { m_colour->targetViewId };
		}

		crg::ImageViewIdArray getSampledIntermediate()const noexcept
		{
			return { m_intermediate->sampledViewId };
		}

		crg::ImageViewIdArray getTargetIntermediate()const noexcept
		{
			return { m_intermediate->targetViewId };
		}

		crg::ImageViewIdArray getTargetDepth()const noexcept
		{
			return { m_depth.targetViewId };
		}

		Texture const & getDepth()const noexcept
		{
			return m_depth;
		}

		Texture const & getResult()const noexcept
		{
			return *m_colour;
		}

		Texture const & getIntermediate()const noexcept
		{
			return *m_intermediate;
		}

		VkExtent3D const & getTargetExtent()const noexcept
		{
			return m_colour->getExtent();
		}

		VkFormat getTargetFormat()const noexcept
		{
			return m_colour->getFormat();
		}

		Texture const & getNormal()const noexcept
		{
			return m_normal;
		}

		Texture const & getScattering()const noexcept
		{
			return m_scattering;
		}

		Texture const & getDiffuse()const noexcept
		{
			return m_diffuse;
		}

		Texture const & getDepthObj()const noexcept
		{
			return m_prepass.getDepthObj();
		}

		ashes::Buffer< int32_t > const & getDepthRange()const noexcept
		{
			return m_prepass.getDepthRange();
		}

		void setNeedsDepthRange( bool v )noexcept
		{
			m_prepass.setNeedsDepthRange( v );
		}

		ShadowMapResult const & getDirectionalShadowPassResult()const noexcept
		{
			return m_directionalShadowMap->getShadowPassResult( false );
		}

		ShadowMapResult const & getPointShadowPassResult()const noexcept
		{
			return m_pointShadowMap->getShadowPassResult( false );
		}

		ShadowMapResult const & getSpotShadowPassResult()const noexcept
		{
			return m_spotShadowMap->getShadowPassResult( false );
		}

		ShadowMapLightTypeArray const & getShadowMaps()const noexcept
		{
			return m_allShadowMaps;
		}

		RenderTarget const & getRenderTarget()const noexcept
		{
			return m_renderTarget;
		}

		RenderTarget & getRenderTarget()noexcept
		{
			return m_renderTarget;
		}

		bool isMultisampling()const noexcept
		{
			return false;
		}

		ashes::Buffer< uint32_t > const & getMaterialsCounts()const noexcept
		{
			return m_opaque.getMaterialsCounts();
		}

		ashes::Buffer< castor::Point3ui > const & getMaterialsIndirectCounts()const noexcept
		{
			return m_opaque.getMaterialsIndirectCounts();
		}

		ashes::Buffer< uint32_t > const & getMaterialsStarts()const noexcept
		{
			return m_opaque.getMaterialsStarts();
		}

		ashes::Buffer< castor::Point2ui > const & getPixelXY()const noexcept
		{
			return m_opaque.getPixelXY();
		}

		bool hasVisibility()const noexcept
		{
			return m_prepass.hasVisibility();
		}

		Texture const & getVisibilityResult()const noexcept
		{
			return m_prepass.getVisibility();
		}

		crg::FramePass const & getGetLastDepthPass()const noexcept
		{
			return *m_lastDepthPass;
		}

		crg::FramePass const & getDepthRangePass()const noexcept
		{
			return *m_depthRangePass;
		}

		crg::FramePass const & getGetLastOpaquePass()const noexcept
		{
			return *m_lastOpaquePass;
		}

		crg::FramePass const & getGetLastTransparentPass()const noexcept
		{
			return *m_lastTransparentPass;
		}

		crg::FramePassGroup & getGraph()noexcept
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
		using ShadowMapArray = castor::Vector< ShadowMapUPtr >;

	private:
		crg::FramePassArray doCreateRenderPasses( TechniquePassEvent event
			, crg::FramePass const * previousPass
			, crg::FramePassArray previousPasses = {} );
		BackgroundRendererUPtr doCreateBackgroundPass( ProgressBar * progress );
		void doInitialiseRsm();
		void doInitialiseLpv();
		void doUpdateShadowMaps( CpuUpdater & updater );
		void doUpdateShadowMaps( GpuUpdater & updater )const;
		void doUpdateRsm( CpuUpdater & updater );
		void doUpdateLpv( CpuUpdater & updater );

		crg::SemaphoreWaitArray doRenderShadowMaps( crg::SemaphoreWaitArray const & semaphore
			, ashes::Queue const & queue )const;
		crg::SemaphoreWaitArray doRenderRSM( crg::SemaphoreWaitArray const & semaphore
			, ashes::Queue const & queue );
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
		Texture m_scattering;
		Texture m_diffuse;
		LpvGridConfigUbo m_lpvConfigUbo;
		LayeredLpvGridConfigUbo m_llpvConfigUbo;
		VoxelizerUbo m_vctConfigUbo;
		crg::FramePassGroup & m_graph;
		ShadowBufferUPtr m_shadowBuffer;
		ShadowMapUPtr m_directionalShadowMap;
		ShadowMapUPtr m_pointShadowMap;
		ShadowMapUPtr m_spotShadowMap;
		VoxelizerUPtr m_voxelizer;
		TextureUPtr m_rsmResult;
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
		ReflectiveShadowMapsUPtr m_reflectiveShadowMaps;
	};
}

#endif
