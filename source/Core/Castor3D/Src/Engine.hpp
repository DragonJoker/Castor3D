/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ENGINE_H___
#define ___C3D_ENGINE_H___

#include "Cache/Cache.hpp"
#include "Cache/ListenerCache.hpp"
#include "Cache/MaterialCache.hpp"
#include "Cache/OverlayCache.hpp"
#include "Cache/PluginCache.hpp"
#include "Cache/SamplerCache.hpp"
#include "Cache/SceneCache.hpp"
#include "Cache/ShaderCache.hpp"
#include "Cache/TargetCache.hpp"
#include "Cache/TechniqueCache.hpp"
#include "Cache/WindowCache.hpp"
#include "Mesh/ImporterFactory.hpp"
#include "Mesh/MeshFactory.hpp"
#include "Mesh/SubdividerFactory.hpp"
#include "Miscellaneous/Version.hpp"
#include "Render/RenderSystemFactory.hpp"
#include "RenderToTexture/RenderDepthQuad.hpp"

#include <FileParser/FileParser.hpp>
#include <Graphics/FontCache.hpp>
#include <Graphics/ImageCache.hpp>
#include <Design/Unique.hpp>
#include <Miscellaneous/CpuInformations.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Main System
	\remark		Holds the render windows, the plug-ins, the render drivers...
	\~french
	\brief		Moteur principal
	\remark		Contient les fenêtres de rendu, les plug-ins, drivers de rendu...
	*/
	class Engine
		: public castor::Unique< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Engine( castor::String const & appName
			, bool enableValidation );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Engine();
		/**
		 *\~english
		 *\brief		Initialisation function, Sets the wanted frame rate
		 *\param[in]	wantedFPS		The wanted FPS count
		 *\param[in]	threaded		If \p false, the render can't be threaded
		 *\~french
		 *\brief		Fonction d'initialisation, définit le frame rate voulu
		 *\param[in]	wantedFPS		Le nombre voulu de FPS
		 *\param[in]	threaded		Si \p false, le rendu ne peut pas être threadé
		 */
		C3D_API void initialise( uint32_t wantedFPS = 100, bool threaded = false );
		/**
		 *\~english
		 *\brief		Cleanup function, destroys everything created from the beginning
		 *\remarks		Destroys also RenderWindows, the only things left after that is RenderSystem and loaded plug-ins
		 *\~french
		 *\brief		Fonction de nettoyage, détruit tout ce qui a été créé depuis le début
		 *\remarks		Détruit aussi les RenderWindows, les seules choses restantes après ça sont le RenderSystem et les plug-ins chargés
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Loads a renderer plug-in, given the renderer type
		 *\param[in]	type	The renderer type
		 *\return		\p true if ok
		 *\~french
		 *\brief		Charge un plug-in de rendu, selon le type de rendu
		 *\param[in]	type	Le type de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool loadRenderer( castor::String const & type );
		/**
		 *\~english
		 *\brief		If a device is enabled, executes the given event, if not posts it to the default frame listener.
		 *\param[in]	event	The event.
		 *\~french
		 *\brief		Si un device est actif, exécute l'évènement donné, sinon il est ajouté au frame listener par défaut.
		 *\param[in]	event	L'évènement.
		 */
		C3D_API void sendEvent( FrameEventUPtr && event );
		/**
		 *\~english
		 *\brief		Posts a frame event to the default frame listener
		 *\param[in]	event	The event to add
		 *\~french
		 *\brief		Ajoute un évènement de frame au frame listener par défaut
		 *\param[in]	event	L'évènement
		 */
		C3D_API void postEvent( FrameEventUPtr && event );
		/**
		 *\~english
		 *\brief		Retrieves the cleanup status
		 *\remarks		Thread-safe
		 *\return		\p true if cleaned up
		 *\~french
		 *\brief		Récupère le statut de nettoyage
		 *\remarks		Thread-safe
		 *\return		\p true si nettoyé
		 */
		C3D_API bool isCleaned();
		/**
		 *\~english
		 *\brief		Tells the engine is cleaned up
		 *\remarks		Thread-safe
		 *\~french
		 *\brief		Dit que le moteur est nettoyé
		 *\remarks		Thread-safe
		 */
		C3D_API void setCleaned();
		/**
		 *\~english
		 *\brief		Registers additional parsers for SceneFileParser.
		 *\param[in]	name	The registering name.
		 *\param[in]	parsers	The parsers.
		 *\~french
		 *\brief		Enregistre des analyseurs supplémentaires pour SceneFileParser.
		 *\param[in]	name	Le nom d'enregistrement.
		 *\param[in]	parsers	Les analyseurs.
		 */
		C3D_API void registerParsers( castor::String const & name, castor::FileParser::AttributeParsersBySection const & parsers );
		/**
		 *\~english
		 *\brief		Registers additional sections for SceneFileParser.
		 *\param[in]	name		The registering name.
		 *\param[in]	sections	The sections.
		 *\~french
		 *\brief		Enregistre des sections supplémentaires pour SceneFileParser.
		 *\param[in]	name		Le nom d'enregistrement.
		 *\param[in]	sections	Les sections.
		 */
		C3D_API void registerSections( castor::String const & name, castor::StrUIntMap const & sections );
		/**
		 *\~english
		 *\brief		Unregisters parsers for SceneFileParser.
		 *\param[in]	name		The registering name.
		 *\~french
		 *\brief		Désenregistre des analyseurs pour SceneFileParser.
		 *\param[in]	name		Le nom d'enregistrement.
		 */
		C3D_API void unregisterParsers( castor::String const & name );
		/**
		 *\~english
		 *\brief		Unregisters sections for SceneFileParser.
		 *\param[in]	name		The registering name.
		 *\~french
		 *\brief		Désenregistre des sections pour SceneFileParser.
		 *\param[in]	name		Le nom d'enregistrement.
		 */
		C3D_API void unregisterSections( castor::String const & name );
		/**
		 *\~english
		 *\return		\p true if the selected rendering API is top down.
		 *\~french
		 *\return		\p true si l'API de rendu sélectionnée est top down.
		 */
		C3D_API bool isTopDown()const;
		/**
		 *\~english
		 *\brief		Renders the given depth texture to the given frame buffer.
		 *\param[in]	renderPass	The render pass.
		 *\param[in]	frameBuffer	The frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture profondeur donnée dans le tampon d'image donné.
		 *\param[in]	renderPass	La passe de rendu.
		 *\param[in]	frameBuffer	Le frame buffer.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void renderDepth( renderer::RenderPass const & renderPass
			, renderer::FrameBuffer const & frameBuffer
			, castor::Position const & position
			, castor::Size const & size
			, TextureLayout const & texture );
		/**
		 *\~english
		 *\brief		Retrieves plug-ins path
		 *\return		The plug-ins path
		 *\~french
		 *\brief		Récupère le chemin des plug-ins
		 *\return		Le chemin des plug-ins
		 */
		C3D_API static castor::Path getPluginsDirectory();
		/**
		 *\~english
		 *\brief		Gives the Castor directory
		 *\return		The directory
		 *\~french
		 *\brief		donne le répertoire du Castor
		 *\return		Le répertoire
		 */
		C3D_API static castor::Path getEngineDirectory();
		/**
		 *\~english
		 *\brief		Retrieves data path
		 *\return		The data path
		 *\~french
		 *\brief		Récupère le chemin des données
		 *\return		Le chemin des données
		 */
		C3D_API static castor::Path getDataDirectory();
		/**
		 *\~english
		 *\brief		Retrieves data path
		 *\return		The data path
		 *\~french
		 *\brief		Récupère le chemin des données
		 *\return		Le chemin des données
		 */
		C3D_API static std::locale const & getLocale();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline castor::ImageCache const & getImageCache()const
		{
			return m_imageCache;
		}

		inline castor::ImageCache & getImageCache()
		{
			return m_imageCache;
		}

		inline castor::FontCache const & getFontCache()const
		{
			return m_fontCache;
		}

		inline castor::FontCache & getFontCache()
		{
			return m_fontCache;
		}

		inline UserInputListenerSPtr getUserInputListener()
		{
			return m_userInputListener;
		}

		inline RenderSystem * getRenderSystem()const
		{
			return m_renderSystem.get();
		}

		inline SamplerSPtr getDefaultSampler()const
		{
			return m_defaultSampler;
		}

		inline SamplerSPtr getLightsSampler()const
		{
			return m_lightsSampler;
		}

		inline std::map< castor::String, castor::FileParser::AttributeParsersBySection > const & getAdditionalParsers()const
		{
			return m_additionalParsers;
		}

		inline std::map< castor::String, castor::StrUIntMap > const & getAdditionalSections()const
		{
			return m_additionalSections;
		}

		inline Version const & getVersion()const
		{
			return m_version;
		}

		inline bool hasRenderLoop()const
		{
			return m_renderLoop != nullptr;
		}

		inline RenderLoop const & getRenderLoop()const
		{
			return *m_renderLoop;
		}

		inline RenderLoop & getRenderLoop()
		{
			return *m_renderLoop;
		}

		inline bool getPerObjectLighting()
		{
			return m_perObjectLighting;
		}

		inline bool isThreaded()
		{
			return m_threaded;
		}

		inline RenderSystemFactory const & getRenderSystemFactory()const
		{
			return m_renderSystemFactory;
		}

		inline RenderSystemFactory & getRenderSystemFactory()
		{
			return m_renderSystemFactory;
		}

		inline MeshFactory const & getMeshFactory()const
		{
			return m_meshFactory;
		}

		inline MeshFactory & getMeshFactory()
		{
			return m_meshFactory;
		}

		inline SubdividerFactory const & getSubdividerFactory()const
		{
			return m_subdividerFactory;
		}

		inline SubdividerFactory & getSubdividerFactory()
		{
			return m_subdividerFactory;
		}

		inline ImporterFactory const & getImporterFactory()const
		{
			return m_importerFactory;
		}

		inline ImporterFactory & getImporterFactory()
		{
			return m_importerFactory;
		}

		inline ParticleFactory & getParticleFactory()
		{
			return m_particleFactory;
		}

		inline castor::CpuInformations const & getCpuInformations()const
		{
			return m_cpuInformations;
		}

		inline MaterialType getMaterialsType()const
		{
			return m_materialType;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		inline void setUserInputListener( UserInputListenerSPtr listener )
		{
			m_userInputListener = listener;
		}

		inline void setPerObjectLighting( bool value )
		{
			m_perObjectLighting = value;
		}

		inline void setMaterialsType( MaterialType type )
		{
			m_materialType = type;
		}
		/**@}*/

	private:
		void doLoadCoreData();

	private:
		castor::String const m_appName;
		std::recursive_mutex m_mutexResources;
		RenderLoopUPtr m_renderLoop;
		Version m_version;
		RenderSystemUPtr m_renderSystem;
		bool m_cleaned;
		bool m_threaded;
		bool m_perObjectLighting;
		SamplerSPtr m_defaultSampler;
		SamplerSPtr m_lightsSampler;
		DECLARE_NAMED_CACHE_MEMBER( shader, ShaderProgram );
		DECLARE_CACHE_MEMBER( sampler, Sampler );
		DECLARE_CACHE_MEMBER( material, Material );
		DECLARE_CACHE_MEMBER( plugin, Plugin );
		DECLARE_CACHE_MEMBER( overlay, Overlay );
		DECLARE_CACHE_MEMBER( scene, Scene );
		DECLARE_CACHE_MEMBER( listener, FrameListener );
		DECLARE_NAMED_CACHE_MEMBER( target, RenderTarget );
		DECLARE_CACHE_MEMBER( technique, RenderTechnique );
		DECLARE_CACHE_MEMBER( window, RenderWindow );
		castor::FontCache m_fontCache;
		castor::ImageCache m_imageCache;
		UserInputListenerSPtr m_userInputListener;
		std::map< castor::String, castor::FileParser::AttributeParsersBySection > m_additionalParsers;
		std::map< castor::String, castor::StrUIntMap > m_additionalSections;
		FrameListenerWPtr m_defaultListener;
		RenderSystemFactory m_renderSystemFactory;
		MeshFactory m_meshFactory;
		SubdividerFactory m_subdividerFactory;
		ImporterFactory m_importerFactory;
		ParticleFactory m_particleFactory;
		castor::CpuInformations m_cpuInformations;
		MaterialType m_materialType;
		bool m_enableValidation{ false };
		std::unique_ptr< RenderDepthQuad > m_renderDepth;
	};
}

#endif
