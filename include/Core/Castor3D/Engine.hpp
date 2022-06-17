/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Engine_H___
#define ___C3D_Engine_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Animation/AnimationModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/PluginCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/TextureCache.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Event/UserInput/UserInputEventModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Plugin/PluginModule.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"
#include "Castor3D/Render/ToneMapping/ToneMappingModule.hpp"
#include "Castor3D/Render/ToTexture/RenderToTextureModule.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
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

#include <unordered_map>
#include <unordered_set>

namespace castor3d
{
	class Engine
		: public castor::Unique< Engine >
	{
	private:
		Engine( castor::String const & appName
			, Version const & appVersion
			, bool enableValidation
			, castor::LoggerInstancePtr ownedLogger
			, castor::LoggerInstance * logger );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	appName				The user application name.
		 *\param[in]	appVersion			The user application version.
		 *\param[in]	enableValidation	\p true to enable rendering API validation.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	appName				Le nom de l'application.
		 *\param[in]	appVersion			La version de l'application.
		 *\param[in]	enableValidation	\p true pour activer la validation via l'API de rendu.
		 */
		C3D_API Engine( castor::String const & appName
			, Version const & appVersion
			, bool enableValidation );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	appName				The user application name.
		 *\param[in]	appVersion			The user application version.
		 *\param[in]	enableValidation	\p true to enable rendering API validation.
		 *\param[in]	logger				The logger instance.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	appName				Le nom de l'application.
		 *\param[in]	appVersion			La version de l'application.
		 *\param[in]	enableValidation	\p true pour activer la validation via l'API de rendu.
		 *\param[in]	logger				L'instance de logger.
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
		 *\param[in]	type	The renderer type.
		 *\return		\p true if ok
		 *\~french
		 *\brief		Charge un plug-in de rendu, selon le type de rendu
		 *\param[in]	type	Le type de renderer.
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool loadRenderer( castor::String const & type );
		/**
		 *\~english
		 *\brief		Loads a renderer plug-in, given the renderer type
		 *\param[in]	renderer	The renderer.
		 *\~french
		 *\brief		Charge un plug-in de rendu, selon le type de rendu
		 *\param[in]	renderer	Le rendere.
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
		 *\param[in]	commandBuffer	Receives the upload commands.
		 *\~french
		 *\brief		Met à jour les buffers.
		 *\param[in]	commandBuffer	Reçoit les commandes d'upload.
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
		 *\brief		Enqueues the given CPU job.
		 *\param[in]	job	The job to execute.
		 *\~french
		 *\brief		Met dans la file la tâche CPU donnée.
		 *\param[in]	job	Le job à exécuter.
		 */
		C3D_API void pushCpuJob( castor::AsyncJobQueue::Job job );
		/**
		 *\~english
		 *\brief		Retrieves a colour issued from a rainbow colours iterator.
		 *\~french
		 *\brief		Récupère une couleur issue d'un itérateur de couleurs d'arc-en-ciel.
		 */
		C3D_API castor::RgbaColour getNextRainbowColour()const;
		/**
		 *\~english
		 *\brief		Registers a RenderWindow.
		 *\param[in]	window	The RenderWindow.
		 *\~french
		 *\brief		Enregistre une RenderWindow.
		 *\param[in]	window	La RenderWindow.
		 */
		C3D_API void registerWindow( RenderWindow & window );
		/**
		 *\~english
		 *\brief		Unregisters a RenderWindow.
		 *\param[in]	window	The RenderWindow.
		 *\~french
		 *\brief		Désenregistre une RenderWindow.
		 *\param[in]	window	La RenderWindow.
		 */
		C3D_API void unregisterWindow( RenderWindow const & window );
		/**
		 *\~english
		 *\brief		Registers additional parsers for SceneFileParser.
		 *\param[in]	name			The registering name.
		 *\param[in]	parsers			The parsers.
		 *\param[in]	sections		The sections.
		 *\param[in]	contextCreator	The user parser context creation function.
		 *\~french
		 *\brief		Enregistre des analyseurs supplémentaires pour SceneFileParser.
		 *\param[in]	name			Le nom d'enregistrement.
		 *\param[in]	parsers			Les analyseurs.
		 *\param[in]	sections		Les sections.
		 *\param[in]	contextCreator	La fonction de création de contexte de parser.
		 */
		C3D_API void registerParsers( castor::String const & name
			, castor::AttributeParsers const & parsers
			, castor::StrUInt32Map const & sections
			, castor::UserContextCreator contextCreator );
		/**
		 *\~english
		 *\brief		Unregisters parsers for SceneFileParser.
		 *\param[in]	name	The registering name.
		 *\~french
		 *\brief		Désenregistre des analyseurs pour SceneFileParser.
		 *\param[in]	name	Le nom d'enregistrement.
		 */
		C3D_API void unregisterParsers( castor::String const & name );
		/**
		 *\~english
		 *\brief		Registers a render pass timer.
		 *\param[in]	category	The timer category name.
		 *\param[in]	timer		The timer to register.
		 *\return		The query ID.
		 *\~french
		 *\brief		Enregistre un timer de passe de rendu.
		 *\param[in]	category	Le nom de la catégorie du timer.
		 *\param[in]	timer		Le timer à enregistrer.
		 *\return		L'ID de la requête.
		 */
		C3D_API uint32_t registerTimer( castor::String const & category
			, FramePassTimer & timer );
		/**
		 *\~english
		 *\brief		Unregisters a render pass timer.
		 *\param[in]	category	The timer category name.
		 *\param[in]	timer		The timer to unregister.
		 *\~french
		 *\brief		Désenregistre un timer de passe de rendu.
		 *\param[in]	category	Le nom de la catégorie du timer.
		 *\param[in]	timer		Le timer à désenregistrer.
		 */
		C3D_API void unregisterTimer( castor::String const & category
			, FramePassTimer & timer );
		/**
		 *\~english
		 *\brief		Registers a Lighting Model.
		 *\param[in]	name	The model name.
		 *\param[in]	creator	The model creation function.
		 *\~french
		 *\brief		Enregistre un Lighting Model.
		 *\param[in]	name	Le nom du model.
		 *\param[in]	creator	La fonction de création du modèle.
		 */
		C3D_API void registerLightingModel( castor::String const & name
			, shader::LightingModelCreator creator );
		/**
		 *\~english
		 *\brief		Unregisters a Lighting Model.
		 *\param[in]	name	The model name.
		 *\~french
		 *\brief		Désenregistre un Lighting Model.
		 *\param[in]	name	Le nom du modèle.
		 */
		C3D_API void unregisterLightingModel( castor::String const & name );
		/**
		 *\~english
		 *\brief		Registers a Background Model.
		 *\param[in]	name	The model name.
		 *\param[in]	creator	The model creation function.
		 *\~french
		 *\brief		Enregistre un Background Model.
		 *\param[in]	name	Le nom du model.
		 *\param[in]	creator	La fonction dde création du modèle.
		 */
		C3D_API void registerBackgroundModel( castor::String const & name
			, shader::BackgroundModelCreator creator );
		/**
		 *\~english
		 *\brief		Unregisters a Background Model.
		 *\param[in]	name	The model name.
		 *\~french
		 *\brief		Désenregistre un Background Model.
		 *\param[in]	name	Le nom du modèle.
		 */
		C3D_API void unregisterBackgroundModel( castor::String const & name );
		/**
		 *\~english
		 *\brief		Registers a ShaderBuffer.
		 *\param[in]	buffer	The ShaderBuffer.
		 *\~french
		 *\brief		Enregistre un ShaderBuffer.
		 *\param[in]	buffer	Le ShaderBuffer.
		 */
		C3D_API void registerBuffer( ShaderBuffer const & buffer );
		/**
		 *\~english
		 *\brief		Unregisters a ShaderBuffer.
		 *\param[in]	buffer	The ShaderBuffer.
		 *\~french
		 *\brief		Désenregistre un ShaderBuffer.
		 *\param[in]	buffer	Le ShaderBuffer.
		 */
		C3D_API void unregisterBuffer( ShaderBuffer const & buffer );
		/**
		 *\~english
		 *\brief		Registers a material pass type.
		 *\param[in]	type	The pass type name.
		 *\param[in]	info	The pass creation informations.
		 *\~french
		 *\brief		Enregistre un type de passe de matériau.
		 *\param[in]	type	Le nom du type de la passe.
		 *\param[in]	info	Les informations de création de la passe.
		 */
		C3D_API void registerPassType( castor::String const & type
			, PassRegisterInfo info );
		/**
		 *\~english
		 *\brief		Unregisters a material pass type.
		 *\param[in]	type	The pass type name.
		 *\~french
		 *\brief		Désenregistre un type de passe de matériau.
		 *\param[in]	type	Le nom du type de la passe.
		 */
		C3D_API void unregisterPassType( castor::String const & type );
		/**
		 *\~english
		 *\brief		Registers a scene render pass type, used to render given material pass type.
		 *\param[in]	renderPassType	The pass type name.
		 *\param[in]	info			The pass creation informations.
		 *\~french
		 *\brief		Enregistre un type de passe de rendu de scène, pour le type de passe de matériau donné.
		 *\param[in]	renderPassType	Le nom du type de la passe.
		 *\param[in]	info			Les informations de création de la passe.
		 */
		C3D_API void registerRenderPassType( castor::String const & renderPassType
			, castor::UniquePtr< RenderPassRegisterInfo > info );
		/**
		 *\~english
		 *\brief		Registers a scene render pass type's configuration parameters.
		 *\param[in]	renderPassType	The pass type name.
		 *\param[in]	parameters		The pass configuration parameters.
		 *\~french
		 *\brief		Enregistre les paramètres de configuration d'un type de passe de rendu de scène.
		 *\param[in]	renderPassType	Le nom du type de la passe.
		 *\param[in]	parameters		Les paramètres de configuration de la passe.
		 */
		C3D_API void setRenderPassTypeConfiguration( castor::String const & renderPassType
			, Parameters parameters );
		/**
		 *\~english
		 *\brief		Retrieve a scene render pass type's configuration parameters.
		 *\param[in]	renderPassType	The pass type name.
		 *\return		The pass configuration parameters.
		 *\~french
		 *\brief		Récupère les paramètres de configuration d'un type de passe de rendu de scène.
		 *\param[in]	renderPassType	Le nom du type de la passe.
		 *\return		Les paramètres de configuration de la passe.
		 */
		C3D_API Parameters getRenderPassTypeConfiguration( castor::String const & renderPassType )const;
		/**
		 *\~english
		 *\brief		Retrieves the ID for given scene render pass type name.
		 *\param[in]	renderPassType	The pass type name.
		 *\~french
		 *\brief		Récupère l'ID correspondant au nom de type de passe de rendu de scène donné.
		 *\param[in]	renderPassType	Le nom du type de la passe.
		 */
		C3D_API RenderPassTypeID getRenderPassTypeID( castor::String const & renderPassType )const;
		/**
		 *\~english
		 *\brief		Retrieves a scene render pass type's creation informations.
		 *\param[in]	renderPassType	The pass type name.
		 *\~french
		 *\brief		Récupère les informations de création d'un type de passe de rendu de scène.
		 *\param[in]	renderPassType	Le nom du type de la passe.
		 */
		C3D_API RenderPassRegisterInfo * getRenderPassInfo( castor::String const & renderPassType )const;
		/**
		 *\~english
		 *\brief		Retrieves the scene render passes type creation informations.
		 *\param[in]	event	The event type for wanted passes.
		 *\~french
		 *\brief		Récupère les informations de création des types de passes de rendu de scène.
		 *\param[in]	event	Le type d'évènement des passes considérées.
		 */
		C3D_API std::vector< RenderPassRegisterInfo * > getRenderPassInfos( TechniquePassEvent event )const;
		/**
		 *\~english
		 *\brief		Unregisters a scene render pass type.
		 *\param[in]	renderPassType	The pass type name.
		 *\~french
		 *\brief		Désenregistre un type de passe de rendu de scène.
		 *\param[in]	renderPassType	Le nom du type de la passe.
		 */
		C3D_API void unregisterRenderPassType( castor::String const & renderPassType );
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
		C3D_API uint32_t getWantedFps()const;
		C3D_API castor3d::MaterialRPtr getDefaultMaterial()const;
		C3D_API bool hasMeshShaders()const;

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

		std::map< castor::String, castor::AdditionalParsers > const & getAdditionalParsers()const
		{
			return m_additionalParsers;
		}

		Version const & getVersion()const
		{
			return m_version;
		}

		bool isValidationEnabled()const
		{
			return m_enableValidation;
		}

		void enableUpdateOptimisations( bool value )
		{
			m_enableUpdateOptimisations = value;
		}

		bool areUpdateOptimisationsEnabled()const
		{
			return m_enableUpdateOptimisations;
		}

		bool isApiTraceEnabled()const
		{
			return m_enableApiTrace;
		}

		bool hasRenderLoop()const
		{
			return m_renderLoop != nullptr;
		}

		RenderLoop & getRenderLoop()const
		{
			return *m_renderLoop;
		}

		bool isThreaded()
		{
			return m_threaded;
		}

		MeshFactory & getMeshFactory()const
		{
			return *m_meshFactory;
		}

		PassFactory & getPassFactory()const
		{
			return *m_passFactory;
		}

		ImporterFileFactory & getImporterFileFactory()const
		{
			return *m_importerFileFactory;
		}

		ParticleFactory & getParticleFactory()const
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

		castor::LoggerInstance & getLogger()const
		{
			return *m_logger;
		}

		uint32_t getLpvGridSize()const
		{
			return m_lpvGridSize;
		}

		uint32_t getMaxImageSize()const
		{
			return m_maxImageSize;
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

		shader::BackgroundModelFactory const & getBackgroundModelFactory()const
		{
			return m_backgroundModelFactory;
		}

		SceneRPtr getLoadingScene()const
		{
			return m_loadingScene.get();
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
		C3D_API void setLoadingScene( SceneUPtr scene );

		void setUserInputListener( UserInputListenerSPtr listener )
		{
			m_userInputListener = listener;
		}

		void setPassesType( PassTypeID type )
		{
			m_passesType = type;
		}

		void setMaxImageSize( uint32_t size )
		{
			m_maxImageSize = size;
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
		DECLARE_CACHE_MEMBER_MIN( shader, ShaderProgram );
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
		DECLARE_CACHE_MEMBER_MIN( target, RenderTarget );
		DECLARE_CACHE_MEMBER_MIN( texture, TextureUnit );
		castor::FontCache m_fontCache;
		castor::ImageCache m_imageCache;
		std::map< castor::String, castor::AdditionalParsers > m_additionalParsers;
		MeshFactoryUPtr m_meshFactory;
		ImporterFileFactoryUPtr m_importerFileFactory;
		ParticleFactoryUPtr m_particleFactory;
		PassFactoryUPtr m_passFactory;
		castor::CpuInformations m_cpuInformations;
		PassTypeID m_passesType{ 0u };
		bool m_enableValidation{ false };
		bool m_enableApiTrace{ false };
		bool m_enableUpdateOptimisations{ true };
		uint32_t m_lpvGridSize{ 32u };
		uint32_t m_maxImageSize{ 0xFFFFFFFF };
		castor::AsyncJobQueue m_cpuJobs;
		crg::ResourceHandler m_resourceHandler;
		shader::LightingModelFactory m_lightingModelFactory;
		shader::BackgroundModelFactory m_backgroundModelFactory;
		SceneUPtr m_loadingScene;
		std::unordered_map< castor::String, castor::UniquePtr< RenderPassRegisterInfo > > m_passRenderPassTypes;
		std::unordered_map< castor::String, std::pair< RenderPassTypeID, Parameters > > m_renderPassTypes;
	};
}

#endif
