/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Engine_H___
#define ___C3D_Engine_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Event/UserInput/UserInputEventModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Plugin/PluginModule.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"
#include "Castor3D/Render/ToneMapping/ToneMappingModule.hpp"
#include "Castor3D/Render/ToTexture/RenderToTextureModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/Version.hpp"
#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

#include <CastorUtils/Design/Unique.hpp>
#include <CastorUtils/FileParser/AttributeParsersBySection.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>
#include <CastorUtils/Graphics/ImageCache.hpp>
#include <CastorUtils/Graphics/ImageLoader.hpp>
#include <CastorUtils/Graphics/ImageWriter.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Log/LoggerInstance.hpp>
#include <CastorUtils/Miscellaneous/CpuInformations.hpp>
#include <CastorUtils/Multithreading/AsyncJobQueue.hpp>

#include <ashespp/Core/RendererList.hpp>

#include <RenderGraph/ResourceHandler.hpp>

#include <unordered_set>

namespace castor3d
{
	class Engine
		: public castor::Unique< Engine >
	{
	private:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Engine( castor::String const & appName
			, Version const & appVersion
			, bool enableValidation
			, castor::LoggerInstancePtr ownedLogger
			, castor::LoggerInstance * logger );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Engine( castor::String const & appName
			, Version const & appVersion
			, bool enableValidation );
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Engine( castor::String const & appName
			, Version const & appVersion
			, bool enableValidation
			, castor::LoggerInstance & logger );
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
		 *\brief		Loads a renderer plug-in, given the renderer type
		 *\param[in]	type	The renderer type
		 *\return		\p true if ok
		 *\~french
		 *\brief		Charge un plug-in de rendu, selon le type de rendu
		 *\param[in]	type	Le type de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API void loadRenderer( Renderer renderer );
		/**
		 *\~english
		 *\brief		Posts a frame event to the default frame listener
		 *\param[in]	event	The event to add
		 *\~french
		 *\brief		Ajoute un évènement de frame au frame listener par défaut
		 *\param[in]	event	L'évènement
		 */
		C3D_API CpuFrameEvent * postEvent( CpuFrameEventUPtr event );
		/**
		 *\~english
		 *\brief		If a device is enabled, executes the given event, if not posts it to the default frame listener.
		 *\param[in]	event	The event.
		 *\~french
		 *\brief		Si un device est actif, exécute l'évènement donné, sinon il est ajouté au frame listener par défaut.
		 *\param[in]	event	L'évènement.
		 */
		C3D_API void sendEvent( GpuFrameEventUPtr event );
		/**
		 *\~english
		 *\brief		Posts a frame event to the default frame listener
		 *\param[in]	event	The event to add
		 *\~french
		 *\brief		Ajoute un évènement de frame au frame listener par défaut
		 *\param[in]	event	L'évènement
		 */
		C3D_API GpuFrameEvent * postEvent( GpuFrameEventUPtr event );
		/**
		 *\~english
		 *\~brief		Fires a mouse move event.
		 *\param[in]	position	The mouse position.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\~brief		Lance un évènement de déplacement de souris.
		 *\param[in]	position	La position de la souris.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseMove( castor::Position const & position );
		/**
		 *\~english
		 *\brief			Updates the engine, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour le moteur, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the engine, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour le moteur, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates the buffers.
		 *\~french
		 *\brief		Met à jour les buffers.
		 */
		C3D_API void upload( ashes::CommandBuffer const & commandBuffer );
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
		 *\brief		Registers a RenderWindow.
		 *\~french
		 *\brief		Enregistre une RenderWindow.
		 */
		C3D_API void registerWindow( RenderWindow & window );
		/**
		 *\~english
		 *\brief		Unregisters a RenderWindow.
		 *\~french
		 *\brief		Désenregistre une RenderWindow.
		 */
		C3D_API void unregisterWindow( RenderWindow const & window );
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
		C3D_API void registerParsers( castor::String const & name, castor::AttributeParsers const & parsers );
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
		C3D_API void registerSections( castor::String const & name, castor::StrUInt32Map const & sections );
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
		 *\brief		Enqueues the given CPU job.
		 *\~french
		 *\brief		Met dans la file la tâche CPU donnée.
		 */
		C3D_API void pushCpuJob( castor::AsyncJobQueue::Job job );
		/**
		 *\~english
		 *\brief		Enqueues the given GPU job.
		 *\~french
		 *\brief		Met dans la file la tâche GPU donnée.
		 */
		C3D_API void pushGpuJob( std::function< void( RenderDevice const &, QueueData const & ) > job );
		/**
		 *\~english
		 *\brief		Retrieves a colour issued from a rainbow colours iterator.
		 *\~french
		 *\brief		Récupère une couleur issue d'un itérateur de couleurs d'arc-en-ciel.
		 */
		C3D_API castor::RgbaColour getNextRainbowColour()const;
		/**
		 *\~english
		 *\brief		Registers a render pass timer.
		 *\param[in]	timer	The timer to register.
		 *\return		The query ID.
		 *\~french
		 *\brief		Enregistre un timer de passe de rendu.
		 *\param[in]	timer	Le timer à enregistrer.
		 *\return		L'ID de la requête.
		 */
		C3D_API uint32_t registerTimer( castor::String const & category
			, FramePassTimer & timer );
		/**
		 *\~english
		 *\brief		Unregisters a render pass timer.
		 *\param[in]	timer	The timer to unregister.
		 *\~french
		 *\brief		Désenregistre un timer de passe de rendu.
		 *\param[in]	timer	Le timer à désenregistrer.
		 */
		C3D_API void unregisterTimer( castor::String const & category
			, FramePassTimer & timer );
		/**
		 *\~english
		 *\brief		Registers a RenderWindow.
		 *\~french
		 *\brief		Enregistre une RenderWindow.
		 */
		C3D_API void registerLightingModel( castor::String const & name
			, shader::LightingModelCreator creator );
		/**
		 *\~english
		 *\brief		Unregisters a RenderWindow.
		 *\~french
		 *\brief		Désenregistre une RenderWindow.
		 */
		C3D_API void unregisterLightingModel( castor::String const & name );
		/**
		 *\~english
		 *\brief		Registers a RenderWindow.
		 *\~french
		 *\brief		Enregistre une RenderWindow.
		 */
		C3D_API void registerBuffer( ShaderBuffer const & buffer );
		/**
		 *\~english
		 *\brief		Unregisters a RenderWindow.
		 *\~french
		 *\brief		Désenregistre une RenderWindow.
		 */
		C3D_API void unregisterBuffer( ShaderBuffer const & buffer );
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
		C3D_API castor::String getPassesName()const;
		C3D_API ToneMappingFactory const & getToneMappingFactory()const;
		C3D_API ToneMappingFactory & getToneMappingFactory();
		C3D_API PostEffectFactory const & getPostEffectFactory()const;
		C3D_API PostEffectFactory & getPostEffectFactory();

		castor::String const & getAppName()const
		{
			return m_appName;
		}
		
		Version const & getAppVersion()const
		{
			return m_appVersion;
		}
		
		castor::ImageCache const & getImageCache()const
		{
			return m_imageCache;
		}

		castor::ImageCache & getImageCache()
		{
			return m_imageCache;
		}

		castor::FontCache const & getFontCache()const
		{
			return m_fontCache;
		}

		castor::FontCache & getFontCache()
		{
			return m_fontCache;
		}

		UserInputListenerSPtr getUserInputListener()
		{
			return m_userInputListener;
		}

		RenderSystem * getRenderSystem()const
		{
			return m_renderSystem.get();
		}

		auto getDefaultSampler()const
		{
			return m_defaultSampler;
		}

		auto getLightsSampler()const
		{
			return m_lightsSampler;
		}

		std::map< castor::String, castor::AttributeParsers > const & getAdditionalParsers()const
		{
			return m_additionalParsers;
		}

		std::map< castor::String, castor::StrUInt32Map > const & getAdditionalSections()const
		{
			return m_additionalSections;
		}

		Version const & getVersion()const
		{
			return m_version;
		}

		bool isValidationEnabled()const
		{
			return m_enableValidation;
		}

		bool isApiTraceEnabled()const
		{
			return m_enableApiTrace;
		}

		bool hasRenderLoop()const
		{
			return m_renderLoop != nullptr;
		}

		RenderLoop const & getRenderLoop()const
		{
			return *m_renderLoop;
		}

		RenderLoop & getRenderLoop()
		{
			return *m_renderLoop;
		}

		bool isThreaded()
		{
			return m_threaded;
		}

		MeshFactory const & getMeshFactory()const
		{
			return *m_meshFactory;
		}

		MeshFactory & getMeshFactory()
		{
			return *m_meshFactory;
		}

		PassFactory const & getPassFactory()const
		{
			return *m_passFactory;
		}

		PassFactory & getPassFactory()
		{
			return *m_passFactory;
		}

		MeshSubdividerFactory const & getSubdividerFactory()const
		{
			return *m_subdividerFactory;
		}

		MeshSubdividerFactory & getSubdividerFactory()
		{
			return *m_subdividerFactory;
		}

		MeshImporterFactory const & getImporterFactory()const
		{
			return *m_importerFactory;
		}

		MeshImporterFactory & getImporterFactory()
		{
			return *m_importerFactory;
		}

		ParticleFactory & getParticleFactory()
		{
			return *m_particleFactory;
		}

		castor::CpuInformations const & getCpuInformations()const
		{
			return m_cpuInformations;
		}

		PassTypeID getPassesType()const
		{
			return m_passesType;
		}

		castor::ImageLoader const & getImageLoader()const
		{
			return m_imageLoader;
		}

		castor::ImageLoader & getImageLoader()
		{
			return m_imageLoader;
		}

		castor::ImageWriter const & getImageWriter()const
		{
			return m_imageWriter;
		}

		ashes::RendererList const & getRenderersList()const
		{
			return m_rendererList;
		}

		castor::LoggerInstance & getLogger()
		{
			return *m_logger;
		}

		uint32_t getLpvGridSize()
		{
			return m_lpvGridSize;
		}

		std::map< castor::String, RenderWindow * > const & getRenderWindows()const
		{
			return m_renderWindows;
		}

		crg::ResourceHandler & getGraphResourceHandler()
		{
			return m_resourceHandler;
		}

		shader::LightingModelFactory const & getLightingModelFactory()const
		{
			return m_lightingModelFactory;
		}

		SceneSPtr getLoadingScene()const
		{
			return m_loadingScene;
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
		C3D_API void setLoadingScene( SceneSPtr scene );

		void setUserInputListener( UserInputListenerSPtr listener )
		{
			m_userInputListener = listener;
		}

		void setPassesType( PassTypeID type )
		{
			m_passesType = type;
		}

		void setLpvGridSize( uint32_t size )
		{
			m_lpvGridSize = size;
		}
		/**@}*/

	private:
		void doLoadCoreData();

	private:
		castor::LoggerInstancePtr m_ownedLogger;
		castor::LoggerInstance * m_logger;
		castor::String const m_appName;
		Version const m_appVersion;
		RenderLoopUPtr m_renderLoop;
		Version m_version;
		ashes::RendererList m_rendererList;
		RenderSystemUPtr m_renderSystem;
		bool m_cleaned{ true };
		bool m_threaded{ false };
		SamplerResPtr m_defaultSampler;
		SamplerResPtr m_lightsSampler;
		castor::ImageLoader m_imageLoader;
		castor::ImageWriter m_imageWriter;
		DECLARE_CACHE_MEMBER( shader, ShaderProgram );
		DECLARE_CACHE_MEMBER( sampler, Sampler );
		DECLARE_CACHE_MEMBER( material, Material );
		DECLARE_CACHE_MEMBER( plugin, Plugin );
		DECLARE_CACHE_MEMBER( overlay, Overlay );
		DECLARE_CACHE_MEMBER( scene, Scene );
		DECLARE_CACHE_MEMBER( listener, FrameListener );
		FrameListenerWPtr m_defaultListener;
		std::map< castor::String, RenderWindow * > m_renderWindows;
		std::map< RenderWindow const *, UserInputListenerSPtr > m_windowInputListeners;
		UserInputListenerSPtr m_userInputListener;
		DECLARE_CACHE_MEMBER( target, RenderTarget );
		DECLARE_CACHE_MEMBER( texture, TextureUnit );
		castor::FontCache m_fontCache;
		castor::ImageCache m_imageCache;
		std::map< castor::String, castor::AttributeParsers > m_additionalParsers;
		std::map< castor::String, castor::StrUInt32Map > m_additionalSections;
		MeshFactoryUPtr m_meshFactory;
		MeshSubdividerFactoryUPtr m_subdividerFactory;
		MeshImporterFactoryUPtr m_importerFactory;
		ParticleFactoryUPtr m_particleFactory;
		PassFactoryUPtr m_passFactory;
		castor::CpuInformations m_cpuInformations;
		PassTypeID m_passesType{ 0u };
		bool m_enableValidation{ false };
		bool m_enableApiTrace{ false };
		uint32_t m_lpvGridSize{ 32u };
		castor::AsyncJobQueue m_cpuJobs;
		castor::AsyncJobQueue m_gpuJobs;
		crg::ResourceHandler m_resourceHandler;
		shader::LightingModelFactory m_lightingModelFactory;
		SceneSPtr m_loadingScene;
	};
}

#endif
