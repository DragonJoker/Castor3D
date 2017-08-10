/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

#include "Miscellaneous/Version.hpp"

#include "Mesh/ImporterFactory.hpp"
#include "Mesh/MeshFactory.hpp"
#include "Mesh/SubdividerFactory.hpp"
#include "Render/RenderSystemFactory.hpp"

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
		C3D_API Engine();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Engine();
		/**
		 *\~english
		 *\brief		Initialisation function, sets the wanted frame rate
		 *\param[in]	p_wantedFPS		The wanted FPS count
		 *\param[in]	p_threaded		If \p false, the render can't be threaded
		 *\~french
		 *\brief		Fonction d'initialisation, définit le frame rate voulu
		 *\param[in]	p_wantedFPS		Le nombre voulu de FPS
		 *\param[in]	p_threaded		Si \p false, le rendu ne peut pas être threadé
		 */
		C3D_API void initialise( uint32_t p_wantedFPS = 100, bool p_threaded = false );
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
		 *\param[in]	p_type	The renderer type
		 *\return		\p true if ok
		 *\~french
		 *\brief		Charge un plug-in de rendu, selon le type de rendu
		 *\param[in]	p_type	Le type de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool loadRenderer( castor::String const & p_type );
		/**
		 *\~english
		 *\brief		Posts a frame event to the default frame listener
		 *\param[in]	p_pEvent	The event to add
		 *\~french
		 *\brief		Ajoute un évènement de frame au frame listener par défaut
		 *\param[in]	p_pEvent	L'évènement
		 */
		C3D_API void postEvent( FrameEventUPtr && p_pEvent );
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
		 *\brief		Checks the current support for given shader model
		 *\param[in]	p_eShaderModel	The shader model
		 *\return		\p true if the shader model is supported in actual configuration
		 *\~french
		 *\brief		Vérifie le support du shader model donné
		 *\param[in]	p_eShaderModel	le shader model
		 *\return		\p true si le shader model est supporté dans la configuration actuelle
		 */
		C3D_API bool supportsShaderModel( ShaderModel p_eShaderModel );
		/**
		 *\~english
		 *\brief		Registers additional parsers for SceneFileParser.
		 *\param[in]	p_name		The registering name.
		 *\param[in]	p_parsers	The parsers.
		 *\~french
		 *\brief		Enregistre des analyseurs supplémentaires pour SceneFileParser.
		 *\param[in]	p_name		Le nom d'enregistrement.
		 *\param[in]	p_parsers	Les analyseurs.
		 */
		C3D_API void registerParsers( castor::String const & p_name, castor::FileParser::AttributeParsersBySection && p_parsers );
		/**
		 *\~english
		 *\brief		Registers additional sections for SceneFileParser.
		 *\param[in]	p_name		The registering name.
		 *\param[in]	p_sections	The sections.
		 *\~french
		 *\brief		Enregistre des sections supplémentaires pour SceneFileParser.
		 *\param[in]	p_name		Le nom d'enregistrement.
		 *\param[in]	p_sections	Les sections.
		 */
		C3D_API void registerSections( castor::String const & p_name, castor::StrUIntMap const & p_sections );
		/**
		 *\~english
		 *\brief		Unregisters parsers for SceneFileParser.
		 *\param[in]	p_name		The registering name.
		 *\~french
		 *\brief		Désenregistre des analyseurs pour SceneFileParser.
		 *\param[in]	p_name		Le nom d'enregistrement.
		 */
		C3D_API void unregisterParsers( castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Unregisters sections for SceneFileParser.
		 *\param[in]	p_name		The registering name.
		 *\~french
		 *\brief		Désenregistre des sections pour SceneFileParser.
		 *\param[in]	p_name		Le nom d'enregistrement.
		 */
		C3D_API void unregisterSections( castor::String const & p_name );
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
		 *\brief		Retrieves the images collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'images
		 *\return		La collection
		 */
		inline castor::ImageCache const & getImageCache()const
		{
			return m_imageCache;
		}
		/**
		 *\~english
		 *\brief		Retrieves the images collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'images
		 *\return		La collection
		 */
		inline castor::ImageCache & getImageCache()
		{
			return m_imageCache;
		}
		/**
		 *\~english
		 *\brief		Retrieves the fonts collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de polices
		 *\return		La collection
		 */
		inline castor::FontCache const & getFontCache()const
		{
			return m_fontCache;
		}
		/**
		 *\~english
		 *\brief		Retrieves the fonts collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de polices
		 *\return		La collection
		 */
		inline castor::FontCache & getFontCache()
		{
			return m_fontCache;
		}
		/**
		 *\~english
		 *\return		The user input listener.
		 *\~french
		 *\return		Le listener d'entrées utilisateur.
		 */
		inline UserInputListenerSPtr getUserInputListener()
		{
			return m_userInputListener;
		}
		/**
		 *\~english
		 *\brief		sets the user input listener.
		 *\param[in]	p_listener	The new value.
		 *\~french
		 *\brief		Définit le listener d'entrées utilisateur.
		 *\param[in]	p_listener	La nouvelle valeur.
		 */
		inline void setUserInputListener( UserInputListenerSPtr p_listener )
		{
			m_userInputListener = p_listener;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RenderSystem
		 *\return		The RenderSystem
		 *\~french
		 *\brief		Récupère le RenderSystem
		 *\return		Le RenderSystem
		 */
		inline RenderSystem * getRenderSystem()const
		{
			return m_renderSystem.get();
		}
		/**
		 *\~english
		 *\brief		Retrieves the default Sampler
		 *\return		The Sampler
		 *\~french
		 *\brief		Récupère le Sampler par défault
		 *\return		Le Sampler
		 */
		inline SamplerSPtr getDefaultSampler()const
		{
			return m_defaultSampler;
		}
		/**
		 *\~english
		 *\brief		Retrieves the lighting Sampler.
		 *\return		The Sampler.
		 *\~french
		 *\brief		Récupère le Sampler pour les éclairages.
		 *\return		Le Sampler.
		 */
		inline SamplerSPtr getLightsSampler()const
		{
			return m_lightsSampler;
		}
		/**
		 *\~english
		 *\brief		Retrieves the SceneFileParser additional parsers.
		 *\return		The parsers.
		 *\~french
		 *\brief		Récupère les analyseurs supplémentaires pour SceneFileParser.
		 *\return		Les analyseurs.
		 */
		inline std::map< castor::String, castor::FileParser::AttributeParsersBySection > const & getAdditionalParsers()const
		{
			return m_additionalParsers;
		}
		/**
		 *\~english
		 *\brief		Retrieves the SceneFileParser additional sections.
		 *\return		The sections.
		 *\~french
		 *\brief		Récupère les sections supplémentaires pour SceneFileParser.
		 *\return		Les sections.
		 */
		inline std::map< castor::String, castor::StrUIntMap > const & getAdditionalSections()const
		{
			return m_additionalSections;
		}
		/**
		 *\~english
		 *\return		The engine version.
		 *\~french
		 *\return		La version du moteur.
		 */
		inline Version const & getVersion()const
		{
			return m_version;
		}
		/**
		 *\~english
		 *\return		The render loop.
		 *\~french
		 *\return		La boucle de rendu.
		 */
		inline bool hasRenderLoop()const
		{
			return m_renderLoop != nullptr;
		}
		/**
		 *\~english
		 *\return		The render loop.
		 *\~french
		 *\return		La boucle de rendu.
		 */
		inline RenderLoop const & getRenderLoop()const
		{
			return *m_renderLoop;
		}
		/**
		 *\~english
		 *\return		The render loop.
		 *\~french
		 *\return		La boucle de rendu.
		 */
		inline RenderLoop & getRenderLoop()
		{
			return *m_renderLoop;
		}
		/**
		 *\~english
		 *\brief		sets the need for per object lighting.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le besoin d'un éclairage par objet.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void setPerObjectLighting( bool p_value )
		{
			m_perObjectLighting = p_value;
		}
		/**
		 *\~english
		 *\return		The need for per object lighting.
		 *\~french
		 *\return		Le besoin d'un éclairage par objet.
		 */
		inline bool getPerObjectLighting()
		{
			return m_perObjectLighting;
		}
		/**
		 *\~english
		 *\return		Tells if the engine uses an asynchronous render loop.
		 *\~french
		 *\return		Dit si le moteur utilise un boucle de rendu asynchrone.
		 */
		inline bool isThreaded()
		{
			return m_threaded;
		}
		/**
		 *\~english
		 *\return		The RenderSystem factory.
		 *\~french
		 *\return		La fabrique de RenderSystem.
		 */
		inline RenderSystemFactory const & getRenderSystemFactory()const
		{
			return m_renderSystemFactory;
		}
		/**
		 *\~english
		 *\return		The RenderSystem factory.
		 *\~french
		 *\return		La fabrique de RenderSystem.
		 */
		inline RenderSystemFactory & getRenderSystemFactory()
		{
			return m_renderSystemFactory;
		}
		/**
		 *\~english
		 *\return		The MeshGenerator factory.
		 *\~french
		 *\return		La fabrique de MeshGenerator.
		 */
		inline MeshFactory const & getMeshFactory()const
		{
			return m_meshFactory;
		}
		/**
		 *\~english
		 *\return		The MeshGenerator factory.
		 *\~french
		 *\return		La fabrique de MeshGenerator.
		 */
		inline MeshFactory & getMeshFactory()
		{
			return m_meshFactory;
		}
		/**
		 *\~english
		 *\return		The Subdivider factory.
		 *\~french
		 *\return		La fabrique de Subdivider.
		 */
		inline SubdividerFactory const & getSubdividerFactory()const
		{
			return m_subdividerFactory;
		}
		/**
		 *\~english
		 *\return		The Subdivider factory.
		 *\~french
		 *\return		La fabrique de Subdivider.
		 */
		inline SubdividerFactory & getSubdividerFactory()
		{
			return m_subdividerFactory;
		}
		/**
		 *\~english
		 *\return		The Importer factory.
		 *\~french
		 *\return		La fabrique de Importer.
		 */
		inline ImporterFactory const & getImporterFactory()const
		{
			return m_importerFactory;
		}
		/**
		 *\~english
		 *\return		The Subdivider factory.
		 *\~french
		 *\return		La fabrique de Subdivider.
		 */
		inline ImporterFactory & getImporterFactory()
		{
			return m_importerFactory;
		}
		/**
		 *\~english
		 *\return		The CpuParticleSystem factory.
		 *\~french
		 *\return		La fabrique de CpuParticleSystem.
		 */
		inline ParticleFactory & getParticleFactory()
		{
			return m_particleFactory;
		}
		/**
		 *\~english
		 *\return		The CPU informations.
		 *\~french
		 *\return		Les informations CPU.
		 */
		inline castor::CpuInformations const & getCpuInformations()const
		{
			return m_cpuInformations;
		}
		/**
		 *\~english
		 *\return		The materials type.
		 *\~french
		 *\return		Le type des matériaux.
		 */
		inline MaterialType getMaterialsType()const
		{
			return m_materialType;
		}
		/**
		 *\~english
		 *\brief		sets the materials type.
		 *\param[in]	p_type	The new value.
		 *\~french
		 *\brief		Définit le type des matériaux.
		 *\param[in]	p_type	La nouvelle valeur.
		 */
		inline void setMaterialsType( MaterialType p_type )
		{
			m_materialType = p_type;
		}

	private:
		void doLoadCoreData();

	private:
		//!\~english	The mutex, to make the engine resources access thread-safe.
		//!\~french		Le mutex utilisé pour que l'accès aux ressources du moteur soit thread-safe.
		std::recursive_mutex m_mutexResources;
		//!\~english	The render loop.
		//!\~french		La boucle de rendu.
		RenderLoopUPtr m_renderLoop;
		//!\~english	The engine version.
		//!\~french		La version du moteur.
		Version m_version;
		//!\~english	The current RenderSystem.
		//!\~french		Le RenderSystem courant.
		RenderSystemUPtr m_renderSystem;
		//!\~english	Tells if engine is cleaned up.
		//!\~french		Dit si le moteur est nettoyé.
		bool m_cleaned;
		//!\~english	Tells if engine uses an asynchronous render loop.
		//!\~french		Dit si le moteur utilise un boucle de rendu asynchrone.
		bool m_threaded;
		//!\~english	The need for per object lighting.
		//!\~french		Le besoin d'un éclairage par objet.
		bool m_perObjectLighting;
		//!\~english	Default sampler.
		//!\~french		Le sampler par défaut.
		SamplerSPtr m_defaultSampler;
		//!\~english	Lights textures sampler.
		//!\~french		L'échantillonneur utilisé pour les textures de lumières.
		SamplerSPtr m_lightsSampler;
		//!\~english	The shaders collection.
		//!\~french		La collection de shaders.
		DECLARE_NAMED_CACHE_MEMBER( shader, ShaderProgram );
		//!\~english	The sampler states collection.
		//!\~french		La collection de sampler states.
		DECLARE_CACHE_MEMBER( sampler, Sampler );
		//!\~english	The materials cache.
		//!\~french		Le cache de matériaux.
		DECLARE_CACHE_MEMBER( material, Material );
		//!\~english	The plug-ins cache.
		//!\~french		Le cache de plug-ins.
		DECLARE_CACHE_MEMBER( plugin, Plugin );
		//!\~english	The overlays cache.
		//!\~french		La cache d'overlays.
		DECLARE_CACHE_MEMBER( overlay, Overlay );
		//!\~english	The scenes cache.
		//!\~french		La cache de scènes.
		DECLARE_CACHE_MEMBER( scene, Scene );
		//!\~english	The frame listeners cache.
		//!\~french		Le cache de frame listeners.
		DECLARE_CACHE_MEMBER( listener, FrameListener );
		//!\~english	The render targets cache.
		//!\~french		Le cache de cibles de rendu.
		DECLARE_NAMED_CACHE_MEMBER( target, RenderTarget );
		//!\~english	The render technique cache.
		//!\~french		Le cache de techniques de rendu.
		DECLARE_CACHE_MEMBER( technique, RenderTechnique );
		//!\~english	The render windows cache.
		//!\~french		Le cache de fenêtres de rendu.
		DECLARE_CACHE_MEMBER( window, RenderWindow );
		//!\~english	The fonts cache.
		//!\~french		La cache de polices.
		castor::FontCache m_fontCache;
		//!\~english	The images cache.
		//!\~french		La cache d'images.
		castor::ImageCache m_imageCache;
		//!\~english	The user input listener.
		//!\~french		Le listener d'entrées utilisateur.
		UserInputListenerSPtr m_userInputListener;
		//!\~english	The map holding the parsers, sorted by section, and plug-in name.
		//!\~french		La map de parseurs, triés par section, et nom de plug-in.
		std::map< castor::String, castor::FileParser::AttributeParsersBySection > m_additionalParsers;
		//!\~english	The map holding the sections, sorted plug-in name.
		//!\~french		La map de sections, triées par nom de plug-in.
		std::map< castor::String, castor::StrUIntMap > m_additionalSections;
		//!\~english	The default frame listener.
		//!\~french		Le frame listener par défaut.
		FrameListenerWPtr m_defaultListener;
		//!\~english	The RenderSystem factory.
		//!\~french		La fabrique de RenderSystem.
		RenderSystemFactory m_renderSystemFactory;
		//!\~english	The MeshGenerator factory.
		//!\~french		La fabrique de MeshGenerator.
		MeshFactory m_meshFactory;
		//!\~english	The subdivider factory.
		//!\~french		La fabrique de subdiviseurs.
		SubdividerFactory m_subdividerFactory;
		//!\~english	The importer factory.
		//!\~french		La fabrique d'importeurs.
		ImporterFactory m_importerFactory;
		//!\~english	The CpuParticleSystem factory.
		//!\~french		La fabrique de CpuParticleSystem.
		ParticleFactory m_particleFactory;
		//!\~english	The CPU informations.
		//!\~french		Les informations sur le CPU.
		castor::CpuInformations m_cpuInformations;
		//!\~english	The materials type.
		//!\~french		Le type des matériaux.
		MaterialType m_materialType;
	};
}

#endif
