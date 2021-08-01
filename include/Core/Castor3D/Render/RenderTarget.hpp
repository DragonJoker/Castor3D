/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_TARGET_H___
#define ___C3D_RENDER_TARGET_H___

#include "RenderModule.hpp"
#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"
#include "Castor3D/Render/ToTexture/RenderToTextureModule.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace castor3d
{
	class RenderTarget
		: public std::enable_shared_from_this< RenderTarget >
		, public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	type	The render target type.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	type	Le type de render target.
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
		C3D_API ~RenderTarget();
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
		 *\brief		Renders one frame.
		 *\param[in]	device	The GPU device.
		 *\param[out]	info	Receives the render informations.
		 *\~french
		 *\brief		Dessine une frame.
		 *\param[in]	device	Le device GPU.
		 *\param[out]	info	Reçoit les informations de rendu.
		 */
		C3D_API crg::SemaphoreWait render( RenderDevice const & device
			, RenderInfo & info
			, ashes::Queue const & queue
			, crg::SemaphoreWaitArray const & signalsToWait = {} );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API void initialise( RenderDevice const & device
			, QueueData const & queueData );
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
		 *\brief		adds RenderTechnique parameters.
		 *\param[in]	parameters	The RenderTechnique parameters.
		 *\~french
		 *\brief		Ajoute des paramètres de RenderTechnique.
		 *\param[in]	parameters	Les paramètres de la RenderTechnique.
		 */
		C3D_API void addTechniqueParameters( Parameters const & parameters );
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
		C3D_API void setCamera( CameraSPtr camera );
		/**
		 *\~english
		 *\brief		Sets the scene.
		 *\param[in]	scene	The new scene.
		 *\~french
		 *\brief		Définit la scène.
		 *\param[in]	scene	La nouvelle scène.
		 */
		C3D_API void setScene( SceneSPtr scene );
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
		C3D_API void setToneMappingType( castor::String const & name
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		adds a post effect to the list.
		 *\param[in]	effect	The effect.
		 *\~french
		 *\brief		Ajoute un effet post rendu à la liste.
		 *\param[in]	effect	L'effet.
		 */
		C3D_API void addPostEffect( PostEffectSPtr effect );
		/**
		 *\~english
		 *\brief			Lists the intermediate views used by this render target.
		 *\param[in,out]	result	Receives the views.
		 *\~french
		 *\brief			Liste les vues intermédiaires utilisées par cette render target.
		 *\param[in,out]	result	Reçoit les vues.
		 */
		C3D_API void listIntermediateViews( IntermediateViewArray & result )const;
		C3D_API void resetSemaphore();
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

		SsaoConfig const & getSsaoConfig()const
		{
			return m_ssaoConfig;
		}

		bool isInitialised()const
		{
			return m_initialised;
		}

		castor::Size const & getSize()const
		{
			return m_size;
		}

		RenderTechniqueSPtr getTechnique()const
		{
			return m_renderTechnique;
		}

		SceneSPtr getScene()const
		{
			return m_scene.lock();
		}

		CameraSPtr getCamera()const
		{
			return m_camera.lock();
		}

		Texture const & getTexture()const
		{
			return m_combined;
		}

		Texture const & getVelocity()const
		{
			return m_velocity;
		}

		VkFormat getPixelFormat()const
		{
			return m_pixelFormat;
		}

		TargetType getTargetType()const
		{
			return m_type;
		}

		uint32_t getIndex()const
		{
			return m_index;
		}

		PostEffectPtrArray const & getHDRPostEffects()const
		{
			return m_hdrPostEffects;
		}

		PostEffectPtrArray const & getSRGBPostEffects()const
		{
			return m_srgbPostEffects;
		}

		ToneMappingSPtr getToneMapping()const
		{
			CU_Require( m_toneMapping );
			return m_toneMapping;
		}

		crg::SemaphoreWait const & getSemaphore()const
		{
			return m_signalFinished;
		}

		SceneCuller const & getCuller()const
		{
			CU_Require( m_culler );
			return *m_culler;
		}

		SceneCuller & getCuller()
		{
			CU_Require( m_culler );
			return *m_culler;
		}

		castor::Point2f const & getJitter()const
		{
			return m_jitter;
		}

		castor::String const & getName()const
		{
			return m_name;
		}

		HdrConfigUbo const & getHdrConfigUbo()const
		{
			CU_Require( m_hdrConfigUbo );
			return *m_hdrConfigUbo;
		}

		crg::FrameGraph & getGraph()
		{
			return m_graph;
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
		C3D_API void setExposure( float value );
		C3D_API void setGamma( float value );

		void setTechnique( RenderTechniqueSPtr technique )
		{
			m_renderTechnique = technique;
		}

		void setSsaoConfig( SsaoConfig const & config )
		{
			m_ssaoConfig = config;
		}

		void setJitter( castor::Point2f const & value )
		{
			m_jitter = value;
		}
		/**@}*/

	private:
		crg::FramePass & doCreateCombinePass();
		bool doInitialiseTechnique( RenderDevice const & device
			, QueueData const & queueData );
		crg::FramePass const & doInitialiseCopyCommands( RenderDevice const & device
			, castor::String const & name
			, crg::ImageViewId const & source
			, crg::ImageViewId const & target
			, crg::FramePass const & previousPass );
		void doInitCombineProgram();
		crg::SemaphoreWait doRender( RenderDevice const & device
			, RenderInfo & info
			, ashes::Queue const & queue
			, CameraSPtr camera
			, crg::SemaphoreWaitArray signalsToWait );
		crg::SemaphoreWait doRenderOverlays( RenderDevice const & device
			, ashes::Queue const & queue
			, crg::SemaphoreWaitArray const & toWait );

	public:
		//!\~english The render target default sampler name	\~french Le nom du sampler par défaut pour la cible de rendu
		C3D_API static const castor::String DefaultSamplerName;

	private:
		static uint32_t sm_uiCount;
		TargetType m_type;
		castor::Size m_size;
		castor::Size m_safeBandedSize;
		VkFormat m_pixelFormat;
		bool m_initialised;
		std::unique_ptr< HdrConfigUbo > m_hdrConfigUbo;
		RenderTechniqueSPtr m_renderTechnique;
		SceneWPtr m_scene;
		CameraWPtr m_camera;
		uint32_t m_index;
		castor::String m_name;
		Parameters m_techniqueParameters;
		PostEffectPtrArray m_hdrPostEffects;
		castor::String m_toneMappingName{ cuT( "linear" ) };
		ToneMappingSPtr m_toneMapping;
		PostEffectPtrArray m_srgbPostEffects;
		FramePassTimerUPtr m_overlaysTimer;
		ShaderModule m_combineVtx{ VK_SHADER_STAGE_VERTEX_BIT, "Target - Combine" };
		ShaderModule m_combinePxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Target - Combine" };
		ashes::PipelineShaderStageCreateInfoArray m_combineStages;
		SsaoConfig m_ssaoConfig;
		castor::Point2f m_jitter;
		OverlayRendererSPtr m_overlayRenderer;
		ashes::SemaphorePtr m_signalReady;
		crg::SemaphoreWait m_signalFinished{};
		SceneCullerUPtr m_culler;
		crg::FrameGraph m_graph;
		Texture m_velocity;
		Texture m_objects;
		Texture m_overlays;
		Texture m_combined;
		crg::FramePass & m_combinePass;
		crg::FramePass const * m_hdrLastPass{};
		crg::RunnableGraphPtr m_runnable;
		ashes::SemaphorePtr m_combineSemaphore;
	};
}

#endif
