/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_TARGET_H___
#define ___C3D_RENDER_TARGET_H___

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Clustered/ClusteredModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Debug/DebugModule.hpp"
#include "Castor3D/Render/Overlays/OverlaysModule.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"
#include "Castor3D/Render/ToTexture/RenderToTextureModule.hpp"

#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/ToneMapping/ColourGradingConfig.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"
#include "Castor3D/Shader/Ubos/ColourGradingUbo.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/ResourceHandler.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace castor3d
{
	class RenderTarget
		: public castor::OwnedBy< Engine >
	{
		using OnInitialisedFunc = castor::Function< void( RenderTarget const &, QueueData const & ) >;
		using OnInitialised = castor::SignalT< OnInitialisedFunc >;
		using OnInitialisedConnection = castor::ConnectionT< OnInitialised >;
		using PostEffectArray = castor::Vector< PostEffectUPtr >;

	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	type		The render target type.
		 *\param[in]	size		The render target dimensions.
		 *\param[in]	pixelFormat	The target's pixels format.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	type		Le type de render target.
		 *\param[in]	size		Les dimensions de la render target.
		 *\param[in]	pixelFormat	Le format des pixels de la render target.
		 */
		C3D_API RenderTarget( Engine & engine
			, TargetType type
			, castor::Size const & size
			, castor::PixelFormat pixelFormat );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderTarget()noexcept;
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
		 *\brief		Uploads overlays GPU buffers to VRAM.
		 *\param[in]	cb	The command buffer on which transfer commands are recorded.
		 *\~french
		 *\brief		Met à jour tous les tampons GPU d'incrustations en VRAM.
		 *\param[in]	cb	Le command buffer sur lequel les commandes de transfert sont enregistrées.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief		Renders one frame.
		 *\param[in]	device	The GPU device.
		 *\param[out]	info	Receives the render informations.
		 *\param[in]	queue	The queue receiving the render commands.
		 *\param[in]	toWait	The semaphores to wait.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine une frame.
		 *\param[in]	device	Le device GPU.
		 *\param[out]	info	Reçoit les informations de rendu.
		 *\param[in]	queue	The queue recevant les commandes de dessin.
		 *\param[in]	toWait	Les sémaphores à attendre.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API crg::SemaphoreWaitArray render( ashes::Queue const & queue
			, crg::SemaphoreWaitArray const & toWait = {} );
		/**
		 *\~english
		 *\return		The number of steps needed for initialisation, to show progression.
		 *\~french
		 *\return		Le nombre d'étapes nécessaires à l'initialisation, pour en montrer la progression.
		 */
		uint32_t countInitialisationSteps()const noexcept;
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	device		The GPU device.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\param[in]	progress	The optional progress bar.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 *\param[in]	progress	La barre de progression optionnelle.
		 */
		C3D_API void initialise( RenderDevice const & device
			, QueueData const & queueData
			, ProgressBar * progress = nullptr );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	onInitialised	The callback called when initialisation ends.
		 *\param[in]	progress		The optional progress bar.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	onInitialised	Le callback appelé lorsque l'initialisation est terminée.
		 *\param[in]	progress		La barre de progression optionnelle.
		 */
		C3D_API void initialise( OnInitialisedFunc const & onInitialised
			, ProgressBar * progress = nullptr );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\return		The ViewportType.
		 *\~french
		 *\return		Le ViewportType.
		 */
		C3D_API ViewportType getViewportType()const;
		/**
		 *\~english
		 *\brief		Sets the ViewportType.
		 *\param[in]	value	The new ViewportType.
		 *\~french
		 *\brief		Définit le ViewportType.
		 *\param[in]	value	Le nouveau ViewportType.
		 */
		C3D_API void setViewportType( ViewportType value );
		/**
		 *\~english
		 *\brief		Sets the camera.
		 *\remarks		Defines also LEye and REye cameras.
		 *\param[in]	camera	The new camera.
		 *\~french
		 *\brief		Définit la caméra.
		 *\remarks		Définit aussi les caméras des yeux gauche et droit.
		 *\param[in]	camera	La nouvelle caméra.
		 */
		C3D_API void setCamera( Camera & camera );
		/**
		 *\~english
		 *\brief		Sets the scene.
		 *\param[in]	scene	The new scene.
		 *\~french
		 *\brief		Définit la scène.
		 *\param[in]	scene	La nouvelle scène.
		 */
		C3D_API void setScene( Scene & scene );
		/**
		 *\~english
		 *\brief		Sets the tone mapping implementation type.
		 *\param[in]	name		The type.
		 *\param[in]	parameters	The parameters.
		 *\~french
		 *\brief		Définit le type d'implémentation de mappage de tons.
		 *\param[in]	name		Le type.
		 *\param[in]	parameters	Les paramètres.
		 */
		C3D_API void setToneMappingType( castor::StringView name );
		/**
		 *\~english
		 *\brief		adds a post effect to the list.
		 *\param[in]	name	The effect name.
		 *\~french
		 *\brief		Ajoute un effet post rendu à la liste.
		 *\param[in]	name	Le nom de l'effet.
		 */
		C3D_API PostEffectRPtr getPostEffect( castor::String const & name )const;
		C3D_API void resetSemaphore();
		C3D_API crg::FramePass const & createVertexTransformPass( crg::FramePassGroup & graph );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		C3D_API HdrConfig const & getHdrConfig()const;
		C3D_API HdrConfig & getHdrConfig();
		C3D_API ColourGradingConfig const & getColourGradingConfig()const;
		C3D_API ColourGradingConfig & getColourGradingConfig();
		C3D_API ShadowMapLightTypeArray getShadowMaps()const;
		C3D_API ShadowBuffer * getShadowBuffer()const;
		C3D_API TechniquePassVector getCustomRenderPasses()const;
		C3D_API CameraUbo const & getCameraUbo()const;
		C3D_API SceneUbo const & getSceneUbo()const;
		C3D_API bool hasIndirect()const noexcept;
		C3D_API bool hasSss()const noexcept;
		C3D_API bool areDebugTargetsEnabled()const noexcept;

		SsaoConfig const & getSsaoConfig()const noexcept
		{
			return m_ssaoConfig;
		}
		
		SsaoConfig & getSsaoConfig()noexcept
		{
			return m_ssaoConfig;
		}

		bool isInitialised()const noexcept
		{
			return m_initialised;
		}

		castor::Size const & getSize()const noexcept
		{
			return m_size;
		}

		bool hasTechnique()const noexcept
		{
			return ( m_renderTechnique != nullptr );
		}

		RenderTechnique & getTechnique()const noexcept
		{
			CU_Require( hasTechnique() );
			return *m_renderTechnique;
		}

		SceneRPtr getScene()const noexcept
		{
			return m_scene;
		}

		CameraRPtr getCamera()const noexcept
		{
			return m_camera;
		}

		Texture const & getTexture()const noexcept
		{
			return m_combined;
		}

		Texture const & getVelocity()const noexcept
		{
			return m_velocity;
		}

		VkFormat getPixelFormat()const noexcept
		{
			return m_pixelFormat;
		}

		TargetType getTargetType()const noexcept
		{
			return m_type;
		}

		uint32_t getIndex()const noexcept
		{
			return m_index;
		}

		PostEffectArray const & getHDRPostEffects()const noexcept
		{
			return m_hdrPostEffects;
		}

		PostEffectArray const & getSRGBPostEffects()const noexcept
		{
			return m_srgbPostEffects;
		}

		ToneMappingRPtr getToneMapping()const noexcept
		{
			CU_Require( m_toneMapping );
			return m_toneMapping.get();
		}

		crg::SemaphoreWaitArray const & getSemaphore()const noexcept
		{
			return m_signalFinished;
		}

		SceneCuller const & getCuller()const noexcept
		{
			CU_Require( m_culler );
			return *m_culler;
		}

		SceneCuller & getCuller()noexcept
		{
			CU_Require( m_culler );
			return *m_culler;
		}

		castor::Point2f const & getJitter()const noexcept
		{
			return m_jitter;
		}

		castor::String const & getName()const noexcept
		{
			return m_name;
		}

		HdrConfigUbo const & getHdrConfigUbo()const noexcept
		{
			CU_Require( m_hdrConfigUbo );
			return *m_hdrConfigUbo;
		}

		ColourGradingUbo const & getColourGradingUbo()const noexcept
		{
			CU_Require( m_colourGradingUbo );
			return *m_colourGradingUbo;
		}

		crg::FrameGraph & getGraph()noexcept
		{
			return m_graph;
		}

		crg::ResourcesCache & getResources()noexcept
		{
			return m_resources;
		}

		bool isInitialising()const noexcept
		{
			return m_initialising;
		}

		bool isUsingStereo()const noexcept
		{
			return m_stereo.enabled;
		}

		float getIntraOcularDistance()const noexcept
		{
			return m_stereo.intraOcularDistance;
		}

		IntermediateViewArray const & getIntermediateViews()const noexcept
		{
			return m_intermediates;
		}

		bool isFullLoadingEnabled()const noexcept
		{
			return m_enableFullLoading;
		}

		ClustersConfig const & getClustersConfig()const noexcept
		{
			return m_clustersConfig;
		}

		FrustumClusters * getFrustumClusters()const noexcept
		{
			return m_frustumClusters.get();
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		**/
		/**@{*/
		void setSsaoConfig( SsaoConfig config )noexcept
		{
			m_ssaoConfig = castor::move( config );
		}

		void setJitter( castor::Point2f const & value )noexcept
		{
			m_jitter = value;
		}

		void setStereo( bool value )noexcept
		{
			m_stereo.enabled = value;
		}

		void setIntraOcularDistance( float value )noexcept
		{
			m_stereo.intraOcularDistance = value;
		}

		TargetDebugConfig & getDebugConfig()noexcept
		{
			return m_debugConfig;
		}

		ClustersConfig & getClustersConfig()noexcept
		{
			return m_clustersConfig;
		}

		void enableFullLoading( bool value )noexcept
		{
			m_enableFullLoading = value;
		}

		void setClustersConfig( ClustersConfig config )noexcept
		{
			m_clustersConfig = castor::move( config );
		}
		/**@}*/

	private:
		void doInitialise( RenderDevice const & device
			, QueueData const & queueData
			, ProgressBar * progress = nullptr );
		crg::FramePass & doCreateOverlayPass( ProgressBar * progress
			, RenderDevice const & device );
		crg::FramePass & doCreateCombinePass( ProgressBar * progress
			, crg::ImageViewIdArray source );
		bool doInitialiseTechnique( RenderDevice const & device
			, QueueData const & queueData
			, ProgressBar * progress
			, crg::FramePassArray previousPasses );
		void doCleanupTechnique();
		void doCleanupCopyCommands();
		void doInitCombineProgram();
		void doCleanupCombineProgram();
		Texture const & doUpdatePostEffects( CpuUpdater & updater
			, PostEffectArray const & effects
			, castor::Vector< Texture const * > const & images )const;
		crg::SemaphoreWaitArray doRender( ashes::Queue const & queue
			, crg::SemaphoreWaitArray signalsToWait );
		void doListIntermediateViews( IntermediateViewArray & result )const;

	public:
		//!\~english The render target default sampler name	\~french Le nom du sampler par défaut pour la cible de rendu
		C3D_API static const castor::String DefaultSamplerName;

	private:
		static uint32_t sm_uiCount;
		RenderDevice & m_device;
		TargetType m_type{};
		castor::Size m_size;
		castor::Size m_safeBandedSize;
		VkFormat m_pixelFormat{};
		std::atomic_bool m_initialised{};
		std::atomic_bool m_initialising{};
		crg::ResourcesCache m_resources;
		castor::RawUniquePtr< HdrConfigUbo > m_hdrConfigUbo;
		castor::RawUniquePtr< ColourGradingUbo > m_colourGradingUbo;
		RenderTechniqueUPtr m_renderTechnique{};
		SceneRPtr m_scene{};
		CameraRPtr m_camera{};
		uint32_t m_index{};
		castor::String m_name;
		PostEffectArray m_hdrPostEffects;
		castor::String m_toneMappingName{ cuT( "linear" ) };
		ToneMappingUPtr m_toneMapping;
		PostEffectArray m_srgbPostEffects;
		FramePassTimerUPtr m_overlaysTimer;
		FramePassTimerUPtr m_cpuUpdateTimer;
		FramePassTimerUPtr m_gpuUpdateTimer;
		ashes::PipelineShaderStageCreateInfoArray m_combineStages;
		SsaoConfig m_ssaoConfig;
		castor::Point2f m_jitter;
		ashes::SemaphorePtr m_signalReady;
		crg::SemaphoreWaitArray m_signalFinished;
		SceneCullerUPtr m_culler;
		crg::FrameGraph m_graph;
		Texture m_velocity;
		castor::Array< Texture, 2u > m_srgbObjects;
		castor::Array< Texture, 2u > m_hdrObjects;
		Texture m_overlays;
		Texture m_combined;
		crg::FramePass & m_overlayPassDesc;
		OverlayPass * m_overlayPass{};
		uint32_t m_hdrCopyPassIndex{ 0u };
		Texture const * m_hdrCopyPassSource{};
		uint32_t m_combinePassIndex{ 1u };
		Texture const * m_combinePassSource{};
		crg::FramePass * m_combinePass{};
		crg::FramePass const * m_hdrLastPass{};
		crg::RunnableGraphPtr m_runnable;
		ashes::SemaphorePtr m_combineSemaphore;
		OnInitialised m_onInitialised;
		castor::Vector< OnInitialisedConnection > m_onTargetInitialised;
		IntermediateViewArray m_intermediates;
		TargetDebugConfig m_debugConfig;
		ClustersConfig m_clustersConfig;
		FrustumClustersUPtr m_frustumClusters;
		bool m_enableFullLoading{ false };
		DebugDrawerUPtr m_debugDrawer{};

		struct StereoConfig
		{
			bool enabled{};
			float intraOcularDistance{};
		};
		StereoConfig m_stereo;
	};

	struct RootContext;
	struct TextureContext;
	struct WindowContext;

	struct TargetContext
	{
		TextureContext * texture{};
		WindowContext * window{};
		TargetType targetType{};
		SsaoConfig ssaoConfig{};
		castor::Size size{};
		castor::PixelFormat hdrPixelFormat{};
		castor::PixelFormat srgbPixelFormat{};
		RenderTargetRPtr renderTarget{};
	};

	C3D_API Engine * getEngine( TargetContext const & context );
	C3D_API RootContext * getRootContext( TargetContext const & context );
}

#endif
