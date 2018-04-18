/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_TARGET_H___
#define ___C3D_RENDER_TARGET_H___

#include "Castor3DPrerequisites.hpp"
#include "Miscellaneous/Parameter.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "Texture/TextureUnit.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <Sync/Semaphore.hpp>

#include <Design/OwnedBy.hpp>
#include <Graphics/Size.hpp>
#include <Miscellaneous/PreciseTimer.hpp>

namespace castor3d
{
	template< typename KeyType >
	struct ElementProducer< RenderTarget, KeyType >
	{
		using Type = std::function< std::shared_ptr< RenderTarget >( KeyType const &, TargetType ) >;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Render target class
	\remark		A render target draws a scene in a frame buffer that can then be used by a window to have a direct render, or a texture to have offscreen rendering
	\~french
	\brief		Classe de cible de rendu (render target)
	\remark		Une render target dessine une scène dans un tampon d'image qui peut ensuite être utilisé dans une fenêtre pour un rendu direct, ou une texture pour un rendu hors écran
	*/
	class RenderTarget
		: public std::enable_shared_from_this< RenderTarget >
		, public castor::OwnedBy< Engine >
	{
	public:
		/*!
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
			C3D_API bool operator()( castor3d::RenderTarget const & target, castor::TextFile & file )override;
		};

	public:
		/*!
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
			bool initialise( renderer::RenderPass & renderPass
				, castor::Size const & size );
			void cleanup();

			//!\~english	The texture receiving the color render.
			//!\~french		La texture recevant le rendu couleur.
			TextureUnit m_colourTexture;
			//!\~english	The frame buffer.
			//!\~french		Le tampon d'image.
			renderer::FrameBufferPtr m_frameBuffer;

		private:
			RenderTarget & m_renderTarget;
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
		C3D_API RenderTarget( Engine & engine, TargetType type = TargetType::eWindow );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderTarget();
		/**
		 *\~english
		 *\brief		Renders one frame.
		 *\param[out]	info	Receives the render informations.
		 *\~french
		 *\brief		Dessine une frame.
		 *\param[out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void render( RenderInfo & info );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Sets the target dimensions.
		 *\remarks		This method must be called before initialisation, otherwise it will have no effect.
		 *\param[in]	size	The new dimensions.
		 *\~english
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
		inline bool isInitialised()const
		{
			return m_initialised;
		}

		castor::Size const & getSize()const
		{
			return m_size;
		}

		inline RenderTechniqueSPtr getTechnique()const
		{
			return m_renderTechnique;
		}

		inline SceneSPtr getScene()const
		{
			return m_scene.lock();
		}

		inline CameraSPtr getCamera()const
		{
			return m_camera.lock();
		}

		inline renderer::FrameBuffer const & getFrameBuffer()const
		{
			return *m_frameBuffer.m_frameBuffer;
		}

		inline TextureUnit const & getTexture()const
		{
			return m_frameBuffer.m_colourTexture;
		}

		inline TextureUnit const & getVelocity()const
		{
			return m_velocityTexture;
		}

		inline renderer::Format getPixelFormat()const
		{
			return m_pixelFormat;
		}

		inline TargetType getTargetType()const
		{
			return m_type;
		}

		inline uint32_t getIndex()const
		{
			return m_index;
		}

		inline PostEffectPtrArray const & getHDRPostEffects()const
		{
			return m_hdrPostEffects;
		}

		inline ToneMappingSPtr getToneMapping()const
		{
			return m_toneMapping;
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_signalFinished );
			return *m_signalFinished;
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
		inline void setTechnique( RenderTechniqueSPtr technique )
		{
			m_renderTechnique = technique;
		}

		inline void setScene( SceneSPtr scene )
		{
			m_scene = scene;
		}

		inline void setSsaoConfig( SsaoConfig const & config )
		{
			m_ssaoConfig = config;
		}

		inline void setPixelFormat( renderer::Format value )
		{
			m_pixelFormat = value;
		}

		inline void setJitter( castor::Point2r const & value )
		{
			m_jitter = value;
		}
		/**@}*/

	private:
		C3D_API void doInitialiseRenderPass();
		C3D_API bool doInitialiseFrameBuffer();
		C3D_API bool doInitialiseVelocityTexture();
		C3D_API bool doInitialiseTechnique();
		C3D_API bool doInitialiseToneMapping();
		C3D_API void doRender( RenderInfo & info
			, TargetFbo & fbo
			, CameraSPtr camera );
		C3D_API renderer::Semaphore const * doApplyPostEffects( renderer::Semaphore const & toWait
			, PostEffectPtrArray const & effects
			, RenderPassTimer & timer
			, castor::Nanoseconds const & elapsedTime );
		C3D_API renderer::Semaphore const * doApplyToneMapping( renderer::Semaphore const & toWait );
		C3D_API renderer::Semaphore const * doRenderOverlays( renderer::Semaphore const & toWait
			, Camera const & camera );

	public:
		//!\~english The render target default sampler name	\~french Le nom du sampler par défaut pour la cible de rendu
		C3D_API static const castor::String DefaultSamplerName;

	private:
		static uint32_t sm_uiCount;
		TargetType m_type;
		bool m_initialised;
		castor::Size m_size;
		RenderTechniqueSPtr m_renderTechnique;
		SceneWPtr m_scene;
		CameraWPtr m_camera;
		renderer::RenderPassPtr m_renderPass;
		renderer::CommandBufferPtr m_toneMappingCommandBuffer;
		TargetFbo m_frameBuffer;
		renderer::Format m_pixelFormat;
		uint32_t m_index;
		Parameters m_techniqueParameters;
		PostEffectPtrArray m_hdrPostEffects;
		ToneMappingSPtr m_toneMapping;
		PostEffectPtrArray m_srgbPostEffects;
		RenderPassTimerSPtr m_hdrPostFxTimer;
		RenderPassTimerSPtr m_srgbPostFxTimer;
		RenderPassTimerSPtr m_toneMappingTimer;
		RenderPassTimerSPtr m_overlaysTimer;
		SsaoConfig m_ssaoConfig;
		castor::Point2r m_jitter;
		TextureUnit m_velocityTexture;
		OverlayRendererSPtr m_overlayRenderer;
		renderer::SemaphorePtr m_signalReady;
		renderer::Semaphore const * m_signalFinished;
		renderer::FencePtr m_fence;
		castor::PreciseTimer m_timer;
	};
}

#endif
