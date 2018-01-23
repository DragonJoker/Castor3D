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

#include <Design/OwnedBy.hpp>
#include <Graphics/Size.hpp>
#include <Design/OwnedBy.hpp>

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

	private:
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
			bool initialise( uint32_t index, castor::Size const & size );
			void cleanup();

			//!\~english	The texture receiving the color render.
			//!\~french		La texture recevant le rendu couleur.
			TextureUnit m_colourTexture;
			//!\~english	The frame buffer.
			//!\~french		Le tampon d'image.
			renderer::FrameBufferPtr m_frameBuffer;
			//!\~english	The attach between the colour texture and main frame buffer.
			//!\~french		L'attache entre la texture de couleurs et le tampon principal.
			// TextureAttachmentSPtr m_colourAttach;

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
		 *\remarks		Initialises the buffers.
		 *\param[in]	index	The base texture index.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\remarks		Initialise les buffers.
		 *\param[in]	index	L'index de texture de base.
		 */
		C3D_API void initialise( uint32_t index );
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
		 *\return		The intialisation status.
		 *\~french
		 *\return		Le statut de l'initialisation.
		 */
		inline bool isInitialised()const
		{
			return m_initialised;
		}
		/**
		 *\~english
		 *\return		The target's dimensions.
		 *\~english
		 *\return		Les dimensions de la cible.
		 */
		castor::Size const & getSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The RenderTechnique.
		 *\~french
		 *\return		La RenderTechnique.
		 */
		inline RenderTechniqueSPtr getTechnique()const
		{
			return m_renderTechnique;
		}
		/**
		 *\~english
		 *\brief		Defines the RenderTechnique.
		 *\param[in]	technique	The RenderTechnique.
		 *\~french
		 *\brief		Définit la RenderTechnique.
		 *\param[in]	technique	La RenderTechnique.
		 */
		inline void setTechnique( RenderTechniqueSPtr technique )
		{
			m_renderTechnique = technique;
		}
		/**
		 *\~english
		 *\return		The Scene.
		 *\~french
		 *\return		La Scene.
		 */
		inline SceneSPtr getScene()const
		{
			return m_scene.lock();
		}
		/**
		 *\~english
		 *\return		The Camera.
		 *\~french
		 *\return		La Camera.
		 */
		inline CameraSPtr getCamera()const
		{
			return m_camera.lock();
		}
		/**
		 *\~english
		 *\brief		Sets the Scene.
		 *\param[in]	scene	The new Scene.
		 *\~french
		 *\brief		Définit la Scene.
		 *\param[in]	scene	La nouvelle Scene.
		 */
		inline void setScene( SceneSPtr scene )
		{
			m_scene = scene;
		}
		/**
		 *\~english
		 *\brief		Sets the SSAO configuration.
		 *\param[in]	config	The new value.
		 *\~french
		 *\brief		Définit la configuration du SSAO.
		 *\param[in]	config	La nouvelle value.
		 */
		inline void setSsaoConfig( SsaoConfig const & config )
		{
			m_ssaoConfig = config;
		}
		/**
		 *\~english
		 *\return		The frame buffer.
		 *\~french
		 *\return		Le tampon d'image.
		 */
		inline renderer::FrameBuffer const & getFrameBuffer()const
		{
			return *m_frameBuffer.m_frameBuffer;
		}
		/**
		 *\~english
		 *\return		The RGB texture.
		 *\~french
		 *\return		La texture RGB.
		 */
		inline TextureUnit const & getTexture()const
		{
			return m_frameBuffer.m_colourTexture;
		}
		/**
		 *\~english
		 *\return		The velocity texture.
		 *\~french
		 *\return		La texture de vélocités.
		 */
		inline TextureUnit const & getVelocity()const
		{
			return m_velocityTexture;
		}
		/**
		 *\~english
		 *\return		The window pixel format.
		 *\~french
		 *\return		Le format des pixels de la fenêtre.
		 */
		inline castor::PixelFormat getPixelFormat()const
		{
			return m_pixelFormat;
		}
		/**
		 *\~english
		 *\brief		Sets the window pixel format
		 *\param[in]	value	The new window pixel format
		 *\~french
		 *\brief		Définit le format des pixels de la fenêtre
		 *\param[in]	value	Le nouveau format des pixels de la fenêtre
		 */
		inline void setPixelFormat( castor::PixelFormat value )
		{
			m_pixelFormat = value;
		}
		/**
		 *\~english
		 *\return		The target type.
		 *\~french
		 *\return		Le type de cible.
		 */
		inline TargetType getTargetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The target index.
		 *\~french
		 *\return		L'indice de la cible.
		 */
		inline uint32_t getIndex()const
		{
			return m_index;
		}
		/**
		 *\~english
		 *\return		The post effects array.
		 *\~french
		 *\return		Le tableau d'effets de post rendu.
		 */
		inline PostEffectPtrArray const & getPostEffects()const
		{
			return m_postEffects;
		}
		/**
		 *\~english
		 *\return		The tone mapping implementation.
		 *\~french
		 *\return		L'implémentation de mappage de tons.
		 */
		inline ToneMappingSPtr getToneMapping()const
		{
			return m_toneMapping;
		}
		/**
		 *\~english
		 *\brief		Sets the jittering value.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la valeur de jittering.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setJitter( castor::Point2r const & value )
		{
			m_jitter = value;
		}

	private:
		C3D_API void doRender( RenderInfo & info
			, TargetFbo & fbo
			, CameraSPtr camera );

	public:
		//!\~english The render target default sampler name	\~french Le nom du sampler par défaut pour la cible de rendu
		C3D_API static const castor::String DefaultSamplerName;

	private:
		//!\~english	The number of actually created render targets.
		//!\~french		Le compte de render target actuellement créées.
		static uint32_t sm_uiCount;
		//!\~english	The render target type.
		//!\~french		Type de RenderTarget.
		TargetType m_type;
		//!\~english	Tells if the target is initalised.
		//!\~french		Dit si la cible est initialisée.
		bool m_initialised;
		//!\~english	The target size.
		//!\~french		Les dimensions de la cible.
		castor::Size m_size;
		//!\~english	The technique used to render this target.
		//!\~french		La technique utilisée pour rendre cette cible.
		RenderTechniqueSPtr m_renderTechnique;
		//!\~english	The scene rendered in this render target.
		//!\~french		La scène rendue par cette RenderTarget.
		SceneWPtr m_scene;
		//!\~english	The camera used to render the scene.
		//!\~french		La caméra utilisée pour rendre la scène.
		CameraWPtr m_camera;
		//!\~english	Frame buffer.
		//!\~french		tampon d'image.
		TargetFbo m_frameBuffer;
		//!\~english	The target display format.
		//!\~french		Le format des pixels de la cible.
		castor::PixelFormat m_pixelFormat;
		//!\~english	This render target's index.
		//!\~french		L'index de cette render target.
		uint32_t m_index;
		//!\~english	The render technique parameters.
		//!\~french		Les paramètres de la technique de rendu.
		Parameters m_techniqueParameters;
		//!\~english	The post effects.
		//!\~french		Les effets post rendu.
		PostEffectPtrArray m_postEffects;
		//!\~english	The tone mapping implementation.
		//!\~french		L'implémentation de mappage de ton.
		ToneMappingSPtr m_toneMapping;
		//!\~english	The post effects applying after tone mapping.
		//!\~french		Les effets post rendu s'appliquant après le mappage de tons.
		PostEffectPtrArray m_postPostEffects;
		//!\~english	The post effect timer.
		//!\~french		Le timer d'effets post-rendu.
		RenderPassTimerSPtr m_postPostFxTimer;
		//!\~english	The overlay rendering timer.
		//!\~french		Le timer de rendu des incrustations.
		RenderPassTimerSPtr m_overlaysTimer;
		//!\~english	The SSAO configuration.
		//!\~french		La configuration du SSAO.
		SsaoConfig m_ssaoConfig;
		//!\~english	The jittering value.
		//!\~french		La valeur de jittering.
		castor::Point2r m_jitter;
		//!\~english	The texture receiving the velocity render.
		//!\~french		La texture recevant le rendu vélocité.
		TextureUnit m_velocityTexture;
	};
}

#endif
