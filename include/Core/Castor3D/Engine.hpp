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
#include "Castor3D/Gui/GuiModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"
#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Model/Skeleton/SkeletonModule.hpp"
#include "Castor3D/Overlay/OverlayModule.hpp"
#include "Castor3D/Plugin/PluginModule.hpp"
#include "Castor3D/Render/PostEffect/PostEffectModule.hpp"
#include "Castor3D/Render/ToneMapping/ToneMappingModule.hpp"
#include "Castor3D/Render/ToTexture/RenderToTextureModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
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
#include <CastorUtils/Math/Length.hpp>
#include <CastorUtils/Miscellaneous/CpuInformations.hpp>
#include <CastorUtils/Multithreading/AsyncJobQueue.hpp>

#include <ashespp/Core/RendererList.hpp>

#include <RenderGraph/ResourceHandler.hpp>

#include <ShaderAST/ShaderAllocator.hpp>

#include <unordered_map>
#include <unordered_set>

namespace castor3d
{
	struct EngineConfig
	{
		/**
		*\~english
		*	The user application name.
		*\~french
		*	Le nom de l'application.
		*/
		castor::String appName;
		/**
		*\~english
		*	The user application version.
		*\~french
		*	La version de l'application.
		*/
		Version appVersion;
		/**
		*\~english
		*	\p true to enable rendering API validation.
		*\~french
		*	\p true pour activer la validation via l'API de rendu.
		*/
		bool enableValidation{ false };
		/**
		*\~english
		*	\p true to generate random seeds at each run.
		*\~french
		*	\p true pour générer des random seeds à chaque lancement.
		*/
		bool enableRandom{ true };
		/**
		*\~english
		*	\p true to enable update optimisations (to prevent running passes for which data haven't changed).
		*\~french
		*	\p true pour activer les optimisations de mise à jour (pour éviter de lancer les passes pour lesquelles les données n'ont pas changé).
		*/
		bool enableUpdateOptimisations{ true };
		/**
		*\~english
		*	The debug shader level.
		*\~french
		*	Le niveau de débogage lors de la compilation des shaders.
		*/
		uint32_t shaderDebugLevel{ 0u };
		/**
		*\~english
		*	\p true to enable shaders validation through glslang.
		*\~french
		*	\p true pour activer la validation des shaders via glslang.
		*/
		bool enableShaderValidation{ false };
		/**
		*\~english
		*	\p true to enable rendering API trace.
		*\~french
		*	\p true pour activer les traces via l'API de rendu.
		*/
		bool enableApiTrace{ false };
	};

	class Engine
		: public castor::Unique< Engine >
	{
	private:
		Engine( EngineConfig config
			, castor::LoggerInstancePtr ownedLogger
			, castor::LoggerInstance * logger );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	appName				The user application name.
		 *\param[in]	appVersion			The user application version.
		 *\param[in]	enableValidation	\p true to enable rendering API validation.
		 *\param[in]	enableRandom		\p true to generate random seeds at each run.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	appName				Le nom de l'application.
		 *\param[in]	appVersion			La version de l'application.
		 *\param[in]	enableValidation	\p true pour activer la validation via l'API de rendu.
		 *\param[in]	enableRandom		\p true pour générer des random seeds à chaque lancement.
		 */
		C3D_API Engine( EngineConfig config );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	appName				The user application name.
		 *\param[in]	appVersion			The user application version.
		 *\param[in]	enableValidation	\p true to enable rendering API validation.
		 *\param[in]	enableRandom		\p true to generate random seeds at each run.
		 *\param[in]	logger				The logger instance.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	appName				Le nom de l'application.
		 *\param[in]	appVersion			La version de l'application.
		 *\param[in]	enableValidation	\p true pour activer la validation via l'API de rendu.
		 *\param[in]	enableRandom		\p true pour générer des random seeds à chaque lancement.
		 *\param[in]	logger				L'instance de logger.
		 */
		C3D_API Engine( EngineConfig config
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
		 *\brief		Fires a mouse move event.
		 *\param[in]	position	The mouse position.
		 *\param[in]	ctrl		Tells if the Ctrl key is down.
		 *\param[in]	alt			Tells if the Alt key is down.
		 *\param[in]	shift		Tells if the Shift key is down.
		 *\return		\p true if the event is processed by a handler.
		 *\~french
		 *\brief		Lance un évènement de déplacement de souris.
		 *\param[in]	position	La position de la souris.
		 *\param[in]	ctrl		Dit si la touche Ctrl est enfoncée.
		 *\param[in]	alt			Dit si la touche Alt est enfoncée.
		 *\param[in]	shift		Dit si la touche Shift est enfoncée.
		 *\return		\p true si l'évènement est traité par un gestionnaire.
		 */
		C3D_API bool fireMouseMove( castor::Position const & position
			, bool ctrl
			, bool alt
			, bool shift );
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
		C3D_API void upload( UploadData & uploader );
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
		 *\~french
		 *\brief		Enregistre un timer de passe de rendu.
		 *\param[in]	category	Le nom de la catégorie du timer.
		 *\param[in]	timer		Le timer à enregistrer.
		 */
		C3D_API void registerTimer( castor::String const & category
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
		 *\param[in]	name				The model name.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\param[in]	creator				The model creation function.
		 *\return		The model ID.
		 *\~french
		 *\brief		Enregistre un Lighting Model.
		 *\param[in]	name				Le nom du model.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 *\param[in]	creator				La fonction de création du modèle.
		 *\return		L'ID du modèle.
		 */
		C3D_API LightingModelID registerLightingModel( castor::String const & name
			, shader::LightingModelCreator creator
			, BackgroundModelID backgroundModelId );
		/**
		 *\~english
		 *\brief		Unregisters a combination of Lighting Model and Background Model.
		 *\param[in]	lightingModelId		The lighting model ID.
		 *\param[in]	backgroundModelId	The bavkground model ID.
		 *\~french
		 *\brief		Désenregistre une combinaison de Lighting Model et de Background Model.
		 *\param[in]	lightingModelId		L'ID du modèle d'éclairage.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 */
		C3D_API void unregisterLightingModel( LightingModelID lightingModelId
			, BackgroundModelID backgroundModelId );
		/**
		 *\~english
		 *\brief		Unregisters a Lighting Model.
		 *\param[in]	name				The model name.
		 *\param[in]	backgroundModelId	The background model ID.
		 *\~french
		 *\brief		Désenregistre un Lighting Model.
		 *\param[in]	name				Le nom du modèle.
		 *\param[in]	backgroundModelId	L'ID du modèle de fond.
		 */
		C3D_API void unregisterLightingModel( castor::String const & name
			, BackgroundModelID backgroundModelId );
		/**
		 *\~english
		 *\brief		Registers a Background Model.
		 *\param[in]	name	The model name.
		 *\param[in]	creator	The model creation function.
		 *\return		The model ID.
		 *\~french
		 *\brief		Enregistre un Background Model.
		 *\param[in]	name	Le nom du model.
		 *\param[in]	creator	La fonction dde création du modèle.
		 *\return		L'ID du modèle.
		 */
		C3D_API BackgroundModelID registerBackgroundModel( castor::String const & name
			, shader::BackgroundModelCreator creator );
		/**
		 *\~english
		 *\brief		Unregisters a Background Model.
		 *\param[in]	name	The model name.
		 *\~french
		 *\brief		Désenregistre un Background Model.
		 *\param[in]	name	Le nom du modèle.
		 */
		C3D_API BackgroundModelID unregisterBackgroundModel( castor::String const & name );
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
		 *\brief		Registers the pass info with given background model.
		 *\param[in]	backgroundModelId	The background model.
		 *\param[in]	info				The pass registering info.
		 *\~french
		 *\brief		Enregistre les informations de passe avec le modèle de fond donnés.
		 *\param[in]	backgroundModelId	Le modèle de fond.
		 *\param[in]	info				Les informations d'enregistrement de la passe.
		 */
		C3D_API void registerPassModel( BackgroundModelID backgroundModelId
			, PassRegisterInfo info );
		/**
		 *\~english
		 *\brief		Registers the given pass info with all background models.
		 *\param[in]	info	The pass registering info.
		 *\~french
		 *\brief		Enregistre les informations de passe avec tous les modèles de fond.
		 *\param[in]	info	Les informations d'enregistrement de la passe.
		 */
		C3D_API void registerPassModels( PassRegisterInfo info );
		/**
		 *\~english
		 *\brief		Unregisters a combination of lighting model and background model.
		 *\param[in]	backgroundModelId	The background model.
		 *\param[in]	lightingModelId		The lighting model.
		 *\~french
		 *\brief		Désenregistre une combinaison de modèle d'éclairage et de modèle de fond.
		 *\param[in]	backgroundModelId	Le modèle de fond.
		 *\param[in]	lightingModelId		Le modèle d'éclairage.
		 */
		C3D_API void unregisterPassModel( BackgroundModelID backgroundModelId
			, LightingModelID lightingModelId );
		/**
		 *\~english
		 *\brief		Unregisters all combinations of given lighting model and background models.
		 *\param[in]	type	The lighting model name.
		 *\~french
		 *\brief		Désenregistre toutes les combinaisons du modèle d'éclairage et des modèles de fond.
		 *\param[in]	type	Le nom du modèle d'éclairage.
		 */
		C3D_API void unregisterPassModels( castor::String const & type );
		/**
		 *\~english
		 *\brief			Registers a specific data shader buffer.
		 *\param[in,out]	name	The shader buffer name.
		 *\param[out]		buffer	The shader buffer.
		 *\~french
		 *\brief			Enregistre un shader buffer de données spécifiques.
		 *\param[in,out]	name	Le nom du shader buffer.
		 *\param[out]		buffer	Le shader buffer.
		 */
		C3D_API void registerSpecificsBuffer( std::string const & name
			, castor3d::SpecificsBuffer buffer );
		/**
		 *\~english
		 *\brief			Unregisters a pass' specific data buffer.
		 *\param[in,out]	name	The shader buffer name.
		 *\~french
		 *\brief			Désenregistre un shader buffer de données spécifiques.
		 *\param[in,out]	name	Le nom du shader buffer.
		 */
		C3D_API void unregisterSpecificsBuffer( std::string const & name );
		/**
		 *\~english
		 *\brief			Addw the pass' specific data buffer into the given descriptor layout bindings array.
		 *\param[in,out]	bindings		Receives the buffers descriptor layout bindings.
		 *\param[in]		shaderStages	The shader stage flags.
		 *\param[in,out]	index			The binding index.
		 *\~french
		 *\brief			Ecrit les shader buffers de données spécifiques dans le tableau de descriptor layout bindings donné.
		 *\param[in,out]	bindings		Reçoit les descriptor layout bindings des buffers.
		 *\param[in]		shaderStages	Les indicateurs de shader stage.
		 *\param[in,out]	index			L'indice de binding.
		 */
		C3D_API void addSpecificsBuffersBindings( ashes::VkDescriptorSetLayoutBindingArray & bindings
			, VkShaderStageFlags shaderStages
			, uint32_t & index )const;
		/**
		 *\~english
		 *\brief			Writes the pass' specific data buffer into the given descriptor writes array.
		 *\param[in,out]	descriptorWrites	Receives the buffers descriptor writes.
		 *\param[in,out]	index				The binding index.
		 *\~french
		 *\brief			Ecrit les shader buffers de données spécifiques dans le tableau de descriptor writes donné.
		 *\param[in,out]	descriptorWrites	Reçoit les descriptor writes des buffers.
		 *\param[in,out]	index				L'indice de binding.
		 */
		C3D_API void addSpecificsBuffersDescriptors( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & index )const;
		/**
		 *\~english
		 *\brief			Writes the pass' specific data buffer bindings into given frame pass.
		 *\param[in,out]	pass	The target frame pass.
		 *\param[in,out]	index	The binding index.
		 *\~french
		 *\brief			Ecrit les bindings des shader buffers de données spécifiques dans la frame pass donnée.
		 *\param[in,out]	pass	La frame pass ciblée.
		 *\param[in,out]	index	L'indice de binding.
		 */
		C3D_API void createSpecificsBuffersPassBindings( crg::FramePass & pass
			, uint32_t & index )const;
		/**
		 *\~english
		 *\brief			Declares pass' specific data shader buffers.
		 *\param[in,out]	writer	The shader writer.
		 *\param[out]		buffers	Receives the registered shader buffers.
		 *\param[in,out]	binding	The descriptor binding index.
		 *\param[in]		set		The descriptor set index.
		 *\~french
		 *\brief			Déclare dans les shaders les buffers spécifiques enregistrés.
		 *\param[in,out]	writer	Le writer de shader.
		 *\param[out]		buffers	Reçoit les shader buffers enregistrés.
		 *\param[in,out]	binding	L'indice de descripteur.
		 *\param[in]		set		L'indice de descriptor set.
		 */
		C3D_API void declareSpecificsShaderBuffers( sdw::ShaderWriter & writer
			, std::map< std::string, castor3d::shader::BufferBaseUPtr > & buffers
			, uint32_t & binding
			, uint32_t set )const;
		/**
		 *\~english
		 *\brief		Registers a pass component.
		 *\param[in]	type			The component type name.
		 *\param[in]	componentPlugin	The component's specific functions.
		 *\~french
		 *\brief		Enregistre un composant de passe.
		 *\param[in]	type			Le nom du type de composant.
		 *\param[in]	componentPlugin	Les fonctions spécifiques du composant.
		 */
		C3D_API PassComponentID registerPassComponent( castor::String const & type
			, PassComponentPluginUPtr componentPlugin );
		/**
		 *\~english
		 *\brief		Unregisters a pass component.
		 *\param[in]	type	The component type name.
		 *\~french
		 *\brief		Désenregistre un composant de passe.
		 *\param[in]	type	Le nom du type de composant.
		 */
		C3D_API void unregisterPassComponent( castor::String const & type );
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
		*\name
		*	Getters.
		*\~french
		*\name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API castor::String getDefaultLightingModelName()const;
		C3D_API ToneMappingFactory const & getToneMappingFactory()const;
		C3D_API ToneMappingFactory & getToneMappingFactory();
		C3D_API PostEffectFactory const & getPostEffectFactory()const;
		C3D_API PostEffectFactory & getPostEffectFactory();
		C3D_API uint32_t getWantedFps()const;
		C3D_API castor3d::MaterialObs getDefaultMaterial()const;
		C3D_API bool hasMeshShaders()const;
		C3D_API uint32_t getMaxPassTypeCount()const;
		C3D_API RenderDevice * getRenderDevice()const;
		C3D_API ControlsManager * getControlsManager()const;
		C3D_API UploadData & getUploadData()const noexcept;
		C3D_API ast::ShaderAllocator & getShaderAllocator();

		castor::String const & getAppName()const noexcept
		{
			return m_config.appName;
		}
		
		Version const & getAppVersion()const noexcept
		{
			return m_config.appVersion;
		}

		bool isShaderValidationEnabled()const noexcept
		{
			return m_config.enableShaderValidation;
		}

		bool isValidationEnabled()const noexcept
		{
			return m_config.enableValidation;
		}

		uint32_t getShaderDebugLevel()const noexcept
		{
			return m_config.shaderDebugLevel;
		}

		bool areUpdateOptimisationsEnabled()const noexcept
		{
			return m_config.enableUpdateOptimisations;
		}

		bool isRandomisationEnabled()const noexcept
		{
			return m_config.enableRandom;
		}

		bool isApiTraceEnabled()const noexcept
		{
			return m_config.enableApiTrace;
		}
		
		castor::ImageCache const & getImageCache()const noexcept
		{
			return m_imageCache;
		}

		castor::ImageCache & getImageCache()noexcept
		{
			return m_imageCache;
		}

		castor::FontCache const & getFontCache()const noexcept
		{
			return m_fontCache;
		}

		castor::FontCache & getFontCache()noexcept
		{
			return m_fontCache;
		}

		UserInputListenerRPtr getUserInputListener()noexcept
		{
			return m_userInputListener.get();
		}

		RenderSystem * getRenderSystem()const noexcept
		{
			return m_renderSystem.get();
		}

		auto getDefaultSampler()const noexcept
		{
			return m_defaultSampler;
		}

		auto getLightsSampler()const noexcept
		{
			return m_lightsSampler;
		}

		std::map< castor::String, castor::AdditionalParsers > const & getAdditionalParsers()const noexcept
		{
			return m_additionalParsers;
		}

		Version const & getVersion()const noexcept
		{
			return m_version;
		}

		bool hasRenderLoop()const noexcept
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

		MeshFactory & getMeshFactory()const noexcept
		{
			return *m_meshFactory;
		}

		PassFactory & getPassFactory()const noexcept
		{
			return *m_passFactory;
		}

		PassComponentRegister & getPassComponentsRegister()const noexcept
		{
			return *m_passComponents;
		}

		ImporterFileFactory & getImporterFileFactory()const noexcept
		{
			return *m_importerFileFactory;
		}

		ParticleFactory & getParticleFactory()const noexcept
		{
			return *m_particleFactory;
		}

		castor::CpuInformations const & getCpuInformations()const noexcept
		{
			return m_cpuInformations;
		}

		LightingModelID getDefaultLightingModel()const noexcept
		{
			return m_lightingModelId;
		}

		castor::ImageLoader const & getImageLoader()const noexcept
		{
			return m_imageLoader;
		}

		castor::ImageLoader & getImageLoader()noexcept
		{
			return m_imageLoader;
		}

		castor::ImageWriter const & getImageWriter()const noexcept
		{
			return m_imageWriter;
		}

		ashes::RendererList const & getRenderersList()const noexcept
		{
			return m_rendererList;
		}

		castor::LoggerInstance & getLogger()const noexcept
		{
			return *m_logger;
		}

		uint32_t getLpvGridSize()const noexcept
		{
			return m_lpvGridSize;
		}

		uint32_t getMaxImageSize()const noexcept
		{
			return m_maxImageSize;
		}

		std::map< castor::String, RenderWindow * > const & getRenderWindows()const noexcept
		{
			return m_renderWindows;
		}

		crg::ResourceHandler & getGraphResourceHandler()noexcept
		{
			return m_resourceHandler;
		}

		LightingModelFactory & getLightingModelFactory()const noexcept
		{
			return *m_lightingModelFactory;
		}

		shader::BackgroundModelFactory const & getBackgroundModelFactory()const noexcept
		{
			return m_backgroundModelFactory;
		}

		shader::BackgroundModelFactory & getBackgroundModelFactory()noexcept
		{
			return m_backgroundModelFactory;
		}

		SceneRPtr getLoadingScene()const noexcept
		{
			return m_loadingScene.get();
		}

		Texture const & getPrefilteredBrdfTexture()const noexcept
		{
			return m_brdf;
		}

		castor::LengthUnit getLengthUnit()const noexcept
		{
			return m_unit;
		}
		/**@}*/
		/**
		*\~english
		*\name
		*	Mutators.
		*\~french
		*\name
		*	Mutateurs.
		*/
		/**@{*/
		C3D_API void setLoadingScene( SceneUPtr scene );

		void setUserInputListener( UserInputListenerUPtr listener )noexcept
		{
			m_userInputListener = std::move( listener );
		}

		template< typename ListenerT >
		void setUserInputListenerT( castor::UniquePtr< ListenerT > listener )noexcept
		{
			m_userInputListener = castor::ptrRefCast< UserInputListener >( listener );
		}

		void setDefaultLightingModel( LightingModelID value )noexcept
		{
			m_lightingModelId = value;
		}

		void setMaxImageSize( uint32_t size )noexcept
		{
			m_maxImageSize = size;
		}

		void setLpvGridSize( uint32_t size )noexcept
		{
			m_lpvGridSize = size;
		}

		template< typename ComponentT >
		PassComponentID registerPassComponent( CreatePassComponentPlugin createPlugin = &ComponentT::createPlugin )
		{
			return registerPassComponent( ComponentT::TypeName
				, createPlugin( *m_passComponents ) );
		}

		void setLengthUnit( castor::LengthUnit value )noexcept
		{
			m_unit = value;
		}
		/**@}*/
		/**
		*\name
		*	Fonts.
		*/
		/**@{*/
		template< typename ... ParametersT >
		castor::FontCache::ElementPtrT createFont( castor::FontCache::ElementKeyT const & key
			, ParametersT && ... parameters )const
		{
			return getFontCache().create( key
				, std::forward< ParametersT >( parameters )... );
		}

		template< typename ... ParametersT >
		castor::FontCache::ElementObsT addNewFont( castor::FontCache::ElementKeyT const & key
			, ParametersT && ... parameters )
		{
			return getFontCache().add( key
				, std::forward< ParametersT >( parameters )... );
		}

		castor::FontCache::ElementObsT addFont( castor::FontCache::ElementKeyT const & key
			, castor::FontCache::ElementPtrT & element
			, bool initialise = false )
		{
			return getFontCache().add( key, element, initialise );
		}

		void removeFont( castor::FontCache::ElementKeyT const & key
			, bool cleanup = false )
		{
			getFontCache().remove( key, cleanup );
		}

		castor::FontCache::ElementObsT findFont( castor::FontCache::ElementKeyT const & key )const
		{
			return getFontCache().find( key );
		}

		bool hasFont( castor::FontCache::ElementKeyT const & key )const
		{
			return getFontCache().has( key );
		}

		castor::FontCache::ElementObsT tryFindFont( castor::FontCache::ElementKeyT const & key )const
		{
			return getFontCache().tryFind( key );
		}
		/**@}*/
		/**
		*\name
		*	Images.
		*/
		/**@{*/
		template< typename ... ParametersT >
		castor::ImageCache::ElementPtrT createImage( castor::ImageCache::ElementKeyT const & key
			, ParametersT && ... parameters )const
		{
			return getImageCache().create( key
				, std::forward< ParametersT >( parameters )... );
		}

		template< typename ... ParametersT >
		castor::ImageCache::ElementObsT addNewImage( castor::ImageCache::ElementKeyT const & key
			, ParametersT && ... parameters )
		{
			return getImageCache().add( key
				, std::forward< ParametersT >( parameters )... );
		}

		castor::ImageCache::ElementObsT addImage( castor::ImageCache::ElementKeyT const & key
			, castor::ImageCache::ElementPtrT & element
			, bool initialise = false )
		{
			return getImageCache().add( key, element, initialise );
		}

		void removeImage( castor::ImageCache::ElementKeyT const & key
			, bool cleanup = false )
		{
			getImageCache().remove( key, cleanup );
		}

		castor::ImageCache::ElementObsT findImage( castor::ImageCache::ElementKeyT const & key )const
		{
			return getImageCache().find( key );
		}

		bool hasImage( castor::ImageCache::ElementKeyT const & key )const
		{
			return getImageCache().has( key );
		}

		castor::ImageCache::ElementObsT tryFindImage( castor::ImageCache::ElementKeyT const & key )const
		{
			return getImageCache().tryFind( key );
		}

		template< typename ... ParametersT >
		castor::ImageCache::ElementObsT tryAddImage( castor::ImageCache::ElementKeyT const & name
			, bool initialise
			, castor::ImageCache::ElementObsT & created
			, ParametersT && ... parameters )
		{
			return getImageCache().tryAdd( name
				, initialise
				, created
				, std::forward< ParametersT >( parameters )... );
		}
		/**@}*/

	private:
		void doLoadCoreData();

	private:
		castor::LoggerInstancePtr m_ownedLogger;
		castor::LoggerInstance * m_logger;
		EngineConfig const m_config;
		RenderLoopUPtr m_renderLoop;
		Version m_version;
		ashes::RendererList m_rendererList;
		RenderSystemUPtr m_renderSystem;
		Texture m_brdf;
		bool m_cleaned{ true };
		bool m_threaded{ false };
		SamplerObs m_defaultSampler{};
		SamplerObs m_lightsSampler{};
		castor::ImageLoader m_imageLoader;
		castor::ImageWriter m_imageWriter;
		DECLARE_CACHE_MEMBER_MIN( shader, ShaderProgram );
		DECLARE_CACHE_MEMBER( sampler, Sampler );
		DECLARE_CACHE_MEMBER( material, Material );
		DECLARE_CACHE_MEMBER( plugin, Plugin );
		DECLARE_CACHE_MEMBER( overlay, Overlay );
		DECLARE_CACHE_MEMBER( scene, Scene );
		DECLARE_CACHE_MEMBER( listener, FrameListener );
		FrameListenerRPtr m_defaultListener{};
		std::map< castor::String, RenderWindow * > m_renderWindows;
		std::map< RenderWindow const *, UserInputListenerUPtr > m_windowInputListeners;
		UserInputListenerUPtr m_userInputListener;
		DECLARE_CACHE_MEMBER_MIN( target, RenderTarget );
		DECLARE_CACHE_MEMBER_MIN( texture, TextureUnit );
		castor::FontCache m_fontCache;
		castor::ImageCache m_imageCache;
		std::map< castor::String, castor::AdditionalParsers > m_additionalParsers;
		MeshFactoryUPtr m_meshFactory;
		ImporterFileFactoryUPtr m_importerFileFactory;
		ParticleFactoryUPtr m_particleFactory;
		PassFactoryUPtr m_passFactory;
		PassComponentRegisterUPtr m_passComponents;
		castor::CpuInformations m_cpuInformations;
		LightingModelID m_lightingModelId{};
		uint32_t m_lpvGridSize{ 32u };
		uint32_t m_maxImageSize{ 0xFFFFFFFF };
		castor::AsyncJobQueue m_cpuJobs;
		crg::ResourceHandler m_resourceHandler;
		crg::ResourcesCache m_resources;
		LightingModelFactoryUPtr m_lightingModelFactory;
		shader::BackgroundModelFactory m_backgroundModelFactory;
		SceneUPtr m_loadingScene;
		std::unordered_map< castor::String, castor::UniquePtr< RenderPassRegisterInfo > > m_passRenderPassTypes;
		std::unordered_map< castor::String, std::pair< RenderPassTypeID, Parameters > > m_renderPassTypes;
		castor::LengthUnit m_unit{ castor::LengthUnit::eMetre };
		mutable std::mutex m_allocMutex;
		std::unordered_map< std::thread::id, std::unique_ptr< ast::ShaderAllocator > > m_shaderAllocators;
	};
}

#endif
