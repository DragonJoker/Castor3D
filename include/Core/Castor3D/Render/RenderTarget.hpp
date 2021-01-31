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
#include "Castor3D/Render/Passes/CombinePass.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelSceneData.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

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
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		RenderTarget loader
		\~english
		\brief		Loader de RenderTarget
		*/
		class TextWriter
			: public castor::TextWriter< RenderTarget >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	tabs	The tabulations to put at the beginning of each line
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	tabs	Les tabulations à mettre à chaque début de ligne
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a render target into a text file
			 *\param[in]	target	the render target
			 *\param[in]	file	the file
			 *\~french
			 *\brief		Ecrit une cible de rendu dans un fichier texte
			 *\param[in]	target	La cible de rendu
			 *\param[in]	file	Le fichier
			 */
			C3D_API bool operator()( castor3d::RenderTarget const & target
				, castor::TextFile & file )override;
		};

	public:
		/**
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		19/12/2012
		\~english
		\brief		Internal struct holding a complete frame buffer
		\~french
		\brief		Structure interne contenant un tampon d'image complet
		*/
		struct TargetFbo
		{
		public:
			explicit TargetFbo( RenderTarget & renderTarget );
			bool initialise( RenderDevice const & device
				, ashes::RenderPass & renderPass
				, VkFormat format
				, castor::Size const & size );
			void cleanup( RenderDevice const & device );

			//!\~english	The texture receiving the color render.
			//!\~french		La texture recevant le rendu couleur.
			TextureUnit colourTexture;
			//!\~english	The frame buffer.
			//!\~french		Le tampon d'image.
			ashes::FrameBufferPtr frameBuffer;

		private:
			RenderTarget & renderTarget;
		};

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
			, TargetType type );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderTarget();
		/**
		 *\~english
		 *\brief		Updates culling.
		 *\~french
		 *\brief		Met à jour le culling.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates GPU data.
		 *\~french
		 *\brief		Met à jour les données GPU.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders one frame.
		 *\param[out]	info	Receives the render informations.
		 *\~french
		 *\brief		Dessine une frame.
		 *\param[out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void render( RenderDevice const & device
			, RenderInfo & info );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 */
		C3D_API void initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Sets the target dimensions.
		 *\remarks		This method must be called before initialisation, otherwise it will have no effect.
		 *\param[in]	size	The new dimensions.
		 *\~french
		 *\brief		Définit les dimensions la cible.
		 *\remarks		Cette méthode doit être appelée avant l'initialisation, sinon elle n'aura aucun effet.
		 *\param[in]	size	Les nouvelles dimensions.
		 */
		C3D_API void setSize( castor::Size const & size );
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
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		C3D_API HdrConfig const & getHdrConfig()const;
		C3D_API HdrConfig & getHdrConfig();

		VoxelSceneData const & getVoxelConeTracingConfig()const
		{
			return m_voxelConfig;
		}

		VoxelSceneData & getVoxelConeTracingConfig()
		{
			return m_voxelConfig;
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

		TextureUnit const & getTexture()const
		{
			return m_combinedFrameBuffer.colourTexture;
		}

		TextureUnit const & getVelocity()const
		{
			return m_velocityTexture;
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

		ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_signalFinished );
			return *m_signalFinished;
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

		std::vector< IntermediateView > const & getIntermediateViews()const
		{
			return m_intermediates;
		}

		HdrConfigUbo const & getHdrConfigUbo()const
		{
			return m_hdrConfigUbo;
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

		void setPixelFormat( VkFormat value )
		{
			m_pixelFormat = value;
		}

		void setJitter( castor::Point2f const & value )
		{
			m_jitter = value;
		}
		/**@}*/

	private:
		C3D_API void doInitialiseRenderPass( RenderDevice const & device );
		C3D_API bool doInitialiseFrameBuffer( RenderDevice const & device );
		C3D_API bool doInitialiseVelocityTexture( RenderDevice const & device );
		C3D_API bool doInitialiseTechnique( RenderDevice const & device );
		C3D_API bool doInitialiseToneMapping( RenderDevice const & device );
		C3D_API void doInitialiseCopyCommands( RenderDevice const & device
			, castor::String const & name
			, ashes::CommandBufferPtr & commandBuffer
			, ashes::ImageView const & source
			, ashes::ImageView const & target );
		C3D_API void doInitialiseCombine( RenderDevice const & device );
		C3D_API void doRender( RenderDevice const & device
			, RenderInfo & info
			, TargetFbo & fbo
			, CameraSPtr camera );
		C3D_API ashes::Semaphore const & doApplyPostEffects( RenderDevice const & device
			, ashes::Semaphore const & toWait
			, PostEffectPtrArray const & effects
			, ashes::CommandBufferPtr const & copyCommandBuffer
			, ashes::SemaphorePtr const & copyFinished
			, castor::Nanoseconds const & elapsedTime );
		C3D_API ashes::Semaphore const & doApplyToneMapping( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		C3D_API ashes::Semaphore const & doRenderOverlays( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		C3D_API ashes::Semaphore const & doCombine( ashes::Semaphore const & toWait );

		void addIntermediateView( castor::String name
			, ashes::ImageView view
			, VkImageLayout layout )
		{
			m_intermediates.push_back( { std::move( name ), std::move( view ), layout } );
		}

	public:
		//!\~english The render target default sampler name	\~french Le nom du sampler par défaut pour la cible de rendu
		C3D_API static const castor::String DefaultSamplerName;

	private:
		static uint32_t sm_uiCount;
		TargetType m_type;
		bool m_initialised;
		castor::Size m_size;
		HdrConfigUbo m_hdrConfigUbo;
		RenderTechniqueSPtr m_renderTechnique;
		SceneWPtr m_scene;
		CameraWPtr m_camera;
		ashes::RenderPassPtr m_renderPass;
		ashes::CommandBufferPtr m_toneMappingCommandBuffer;
		TargetFbo m_objectsFrameBuffer;
		TargetFbo m_overlaysFrameBuffer;
		TargetFbo m_combinedFrameBuffer;
		VkFormat m_pixelFormat;
		uint32_t m_index;
		castor::String m_name;
		Parameters m_techniqueParameters;
		PostEffectPtrArray m_hdrPostEffects;
		ashes::CommandBufferPtr m_hdrCopyCommands;
		ashes::SemaphorePtr m_hdrCopyFinished;
		ToneMappingSPtr m_toneMapping;
		PostEffectPtrArray m_srgbPostEffects;
		ashes::CommandBufferPtr m_srgbCopyCommands;
		ashes::SemaphorePtr m_srgbCopyFinished;
		RenderPassTimerSPtr m_toneMappingTimer;
		RenderPassTimerSPtr m_overlaysTimer;
		ShaderModule m_combineVtx{ VK_SHADER_STAGE_VERTEX_BIT, "Target - Combine" };
		ShaderModule m_combinePxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Target - Combine" };
		CombinePassUPtr m_combinePass;
		SsaoConfig m_ssaoConfig;
		castor::Point2f m_jitter;
		TextureUnit m_velocityTexture;
		OverlayRendererSPtr m_overlayRenderer;
		ashes::SemaphorePtr m_signalReady;
		ashes::Semaphore const * m_signalFinished{ nullptr };
		castor::PreciseTimer m_timer;
		SceneCullerUPtr m_culler;
		IntermediateViewArray m_intermediates;
		VoxelSceneData m_voxelConfig;
	};
}

#endif
