/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_ENGINE_H___
#define ___C3D_ENGINE_H___

#include "Castor3DPrerequisites.hpp"

#include "FrameEvent.hpp"
#include "TechniqueFactory.hpp"
#include "Version.hpp"

#include <FileParser.hpp>
#include <FontManager.hpp>
#include <Unique.hpp>
#include <SquareMatrix.hpp>

#define DECLARE_MANAGED_MEMBER( memberName, className )\
	public:\
		inline className##Manager & Get##className##Manager()\
		{\
			return *m_##memberName##Manager;\
		}\
		inline className##Manager const & Get##className##Manager()const\
		{\
			return *m_##memberName##Manager;\
		}\
	private:\
		className##ManagerUPtr m_##memberName##Manager

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Main System
	\remark		Holds the render windows, the plugins, the render drivers...
	\~french
	\brief		Moteur principal
	\remark		Contient les fenêtres de rendu, les plugins, drivers de rendu...
	*/
	class Engine
		: Castor::Unique< Engine >
		, public Castor::Aligned< CASTOR_ALIGN_OF( Castor::Point3r ) >
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
		 *\param[in]	p_bThreaded		If \p false, the render can't be threaded
		 *\~french
		 *\brief		Fonction d'initialisation, définit le frame rate voulu
		 *\param[in]	p_wantedTBEF	Le nombre voulu du FPS
		 *\param[in]	p_bThreaded		Si \p false, le rendu ne peut pas être threadé
		 */
		C3D_API void Initialise( uint32_t p_wantedFPS = 100, bool p_bThreaded = false );
		/**
		 *\~english
		 *\brief		Cleanup function, destroys everything created from the beginning
		 *\remark		Destroys also RenderWindows, the only things left after that is RenderSystem and loaded plugins
		 *\~french
		 *\brief		Fonction de nettoyage, détruit tout ce qui a été créé depuis le début
		 *\remark		Détruit aussi les RenderWindows, les seules choses restantes après ça sont le RenderSystem et les plugins chargés
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Asks for render context creation
		 *\param[in]	p_pRenderWindow	The render window used to initialise the render context
		 *\return		The created context, or the existing one
		 *\~french
		 *\brief		Demande la création du contexte de rendu
		 *\param[in]	p_pRenderWindow	La render window utilisée pour initialiser le contexte de rendu
		 *\return		Le contexte créé, ou l'existant
		 */
		C3D_API ContextSPtr CreateContext( RenderWindow * p_pRenderWindow );
		/**
		 *\~english
		 *\brief		Starts threaded render loop
		 *\~french
		 *\brief		Commence le rendu threadé
		 */
		C3D_API void StartRendering();
		/**
		 *\~english
		 *\brief		Ends the render, cleans up engine
		 *\remark		Ends the threaded render loop, if any
		 *\~french
		 *\brief		Termine le rendu, nettoie le moteur
		 *\remark		Arrête la boucle de rendu threadé, si elle existe
		 */
		C3D_API void EndRendering();
		/**
		 *\~english
		 *\brief		Renders one frame, only if not in render loop
		 *\~french
		 *\brief		Rend une image, uniquement hors de la boucle de rendu
		 */
		C3D_API void RenderOneFrame();
		/**
		 *\~english
		 *\brief		Loads a renderer plugin, given the renderer type
		 *\param[in]	p_type	The renderer type
		 *\return		\p true if ok
		 *\~french
		 *\brief		Charge un plugin de rendu, selon le type de rendu
		 *\param[in]	p_type	Le type de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool LoadRenderer( eRENDERER_TYPE p_type );
		/**
		 *\~english
		 *\brief		Posts a frame event to the default frame listener
		 *\param[in]	p_pEvent	The event to add
		 *\~french
		 *\brief		Ajoute un évènement de frame au frame listener par défaut
		 *\param[in]	p_pEvent	L'évènement
		 */
		C3D_API void PostEvent( FrameEventSPtr p_pEvent );
		/**
		 *\~english
		 *\brief		Retrieves the end status
		 *\remark		Thread-safe
		 *\return		\p true if ended
		 *\~french
		 *\brief		Récupère le statut de fin
		 *\remark		Thread-safe
		 *\return		\p true si arrêté
		 */
		C3D_API bool IsEnded();
		/**
		 *\~english
		 *\brief		Tells the engine the render is ended
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le rendu est stoppé
		 *\remark		Thread-safe
		 */
		C3D_API void SetEnded();
		/**
		 *\~english
		 *\brief		Retrieves the render start status
		 *\remark		Thread-safe
		 *\return		\p true if started
		 *\~french
		 *\brief		Récupère le statut de début de rendu
		 *\remark		Thread-safe
		 *\return		\p true si démarré
		 */
		C3D_API bool IsStarted();
		/**
		 *\~english
		 *\brief		Tells the engine the render is started
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le rendu est démarré
		 *\remark		Thread-safe
		 */
		C3D_API void SetStarted();
		/**
		 *\~english
		 *\brief		Retrieves the context creation status
		 *\remark		Thread-safe
		 *\return		\p true if created
		 *\~french
		 *\brief		Récupère le statut de création du contexte de rendu
		 *\remark		Thread-safe
		 *\return		\p true si créé
		 */
		C3D_API bool IsCreated();
		/**
		 *\~english
		 *\brief		Tells the engine the render context is created
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le contexte de rendu est créé
		 *\remark		Thread-safe
		 */
		C3D_API void SetCreated();
		/**
		 *\~english
		 *\brief		Retrieves the render to-create status
		 *\remark		Thread-safe
		 *\return		\p true if the render context is to create
		 *\~french
		 *\brief		Récupère le statut de demande de création du contexte
		 *\remark		Thread-safe
		 *\return		\p true si à créer
		 */
		C3D_API bool IsToCreate();
		/**
		 *\~english
		 *\brief		Tells the engine the render context is to create
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le contexte de rendu est à créer
		 *\remark		Thread-safe
		 */
		C3D_API void SetToCreate();
		/**
		 *\~english
		 *\brief		Retrieves the cleanup status
		 *\remark		Thread-safe
		 *\return		\p true if cleaned up
		 *\~french
		 *\brief		Récupère le statut de nettoyage
		 *\remark		Thread-safe
		 *\return		\p true si nettoyé
		 */
		C3D_API bool IsCleaned();
		/**
		 *\~english
		 *\brief		Tells the engine is cleaned up
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le moteur est nettoyé
		 *\remark		Thread-safe
		 */
		C3D_API void SetCleaned();
		/**
		 *\~english
		 *\brief		Retrieves the wanted frame time
		 *\remark		Thread-safe
		 *\return		The time, in seconds
		 *\~french
		 *\brief		Récupère le temps voulu pour une frame
		 *\remark		Thread-safe
		 *\return		Le temps, en secondes
		 */
		C3D_API double GetFrameTime();
		/**
		 *\~english
		 *\brief		Creates an RenderTechnique from a technique name
		 *\param[in]	p_name		The technique name
		 *\param[in]	p_renderTarget	The technique render target
		 *\param[in]	p_params		The technique parameters
		 *\return		The created RenderTechnique
		 *\~french
		 *\brief		Crée une RenderTechnique à partir d'un nom de technique
		 *\param[in]	p_key			Le type d'objet
		 *\param[in]	p_renderTarget	La cible de rendu de la technique
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		La RenderTechnique créée
		 */
		C3D_API RenderTechniqueBaseSPtr CreateTechnique( Castor::String const & p_name, RenderTarget & p_renderTarget, Parameters const & p_params );
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
		C3D_API bool SupportsShaderModel( eSHADER_MODEL p_eShaderModel );
		/**
		 *\~english
		 *\brief		Tells if the renderer API supports depth buffer for main FBO
		 *\return		The support status
		 *\~french
		 *\brief		Dit si l'API de rendu supporte les tampons de profondeur pour le FBO principal
		 *\return		Le statut du support
		 */
		C3D_API bool SupportsDepthBuffer()const;
		/**
		 *\~english
		 *\brief		Creates a FrameListener.
		 *\param[in]	p_name	The FrameListener's name.
		 *\return		The created FrameListener.
		 *\~french
		 *\brief		Crée un FrameListener.
		 *\param[in]	p_name	Le nom du FrameListener.
		 *\return		Le FrameListener créé.
		 */
		C3D_API FrameListenerWPtr CreateFrameListener( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Creates a FrameListener.
		 *\param[in]	p_name		The listener's name.
		 *\param[in]	p_listener	The listener.
		 *\~french
		 *\brief		Crée un FrameListener.
		 *\param[in]	p_name		Le nom du listener.
		 *\param[in]	p_listener	Le listener.
		 */
		C3D_API void AddFrameListener( Castor::String const & p_name, FrameListenerSPtr && p_listener );
		/**
		 *\~english
		 *\brief		Retrieves a FrameListener.
		 *\remarks		If the listener is not found, a CastorException is thrown.
						Never keep this reference more than your needs, since the pointer referenced by it can be destroyed.
		 *\param[in]	p_name	The FrameListener's name.
		 *\return		The reference to the wanted listener.
		 *\~french
		 *\brief		Récupère un FrameListener.
		 *\remarks		Si le listener n'a pas été trouvé, une CastorException est levée.
						Ne gardez pas la référence plus que de besoin, le pointeur référencé peut être détruit.
		 *\param[in]	p_name	Le nom du FrameListener.
		 *\return		Une référence sur le listener.
		 */
		C3D_API FrameListener & GetFrameListener( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destroys a FrameListener.
		 *\param[in]	p_name	The FrameListener's name.
		 *\~french
		 *\brief		Détruit un FrameListener.
		 *\param[in]	p_name	Le nom du FrameListener.
		 */
		C3D_API void DestroyFrameListener( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Show or hide debug overlays
		 *\param[in]	p_show	The status
		 *\~french
		 *\brief		Affiche ou cache les incrustations de débogage
		 *\param[in]	p_show	Le statut
		 */
		C3D_API void ShowDebugOverlays( bool p_show );
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
		C3D_API void RegisterParsers( Castor::String const & p_name, Castor::FileParser::AttributeParsersBySection && p_parsers );
		/**
		 *\~english
		 *\brief		Unregisters parsers for SceneFileParser.
		 *\param[in]	p_name		The registering name.
		 *\~french
		 *\brief		Désenregistre des analyseurs pour SceneFileParser.
		 *\param[in]	p_name		Le nom d'enregistrement.
		 */
		C3D_API void UnregisterParsers( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves plugins path
		 *\return		The plugins path
		 *\~french
		 *\brief		Récupère le chemin des plugins
		 *\return		Le chemin des plugins
		 */
		C3D_API static Castor::Path GetPluginsDirectory();
		/**
		 *\~english
		 *\brief		Gives the Castor directory
		 *\return		The directory
		 *\~french
		 *\brief		Donne le répertoire du Castor
		 *\return		Le répertoire
		 */
		C3D_API static Castor::Path GetEngineDirectory();
		/**
		 *\~english
		 *\brief		Retrieves data path
		 *\return		The data path
		 *\~french
		 *\brief		Récupère le chemin des données
		 *\return		Le chemin des données
		 */
		C3D_API static Castor::Path GetDataDirectory();
		/**
		 *\~english
		 *\brief		Retrieves the RenderTechnique factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique de RenderTechnique
		 *\return		La fabrique
		 */
		TechniqueFactory const & GetTechniqueFactory()const
		{
			return m_techniqueFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RenderTechnique factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique de RenderTechnique
		 *\return		La fabrique
		 */
		TechniqueFactory & GetTechniqueFactory()
		{
			return m_techniqueFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the images collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'images
		 *\return		La collection
		 */
		inline ImageCollection const & GetImageManager()const
		{
			return m_imageManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the images collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'images
		 *\return		La collection
		 */
		inline ImageCollection & GetImageManager()
		{
			return m_imageManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the fonts collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de polices
		 *\return		La collection
		 */
		inline Castor::FontManager const & GetFontManager()const
		{
			return m_fontManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the fonts collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de polices
		 *\return		La collection
		 */
		inline Castor::FontManager & GetFontManager()
		{
			return m_fontManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RenderSystem
		 *\return		The RenderSystem
		 *\~french
		 *\brief		Récupère le RenderSystem
		 *\return		Le RenderSystem
		 */
		inline RenderSystem * GetRenderSystem()const
		{
			return m_renderSystem;
		}
		/**
		 *\~english
		 *\brief		Tells the engine the render may be threaded or not
		 *\~french
		 *\brief		Dit si le rendu peut être threadé
		 */
		inline bool IsThreaded()const
		{
			return m_bThreaded;
		}
		/**
		 *\~english
		 *\brief		Retrieves the default BlendState (no blend)
		 *\return		The value
		 *\~french
		 *\brief		Récupère le BlendState par défault (pas de blend)
		 *\return		La valeur
		 */
		inline BlendStateSPtr GetDefaultBlendState()const
		{
			return m_pDefaultBlendState;
		}
		/**
		 *\~english
		 *\brief		Retrieves the default Sampler
		 *\return		The Sampler
		 *\~french
		 *\brief		Récupère le Sampler par défault
		 *\return		Le Sampler
		 */
		inline SamplerSPtr GetDefaultSampler()const
		{
			return m_pDefaultSampler;
		}
		/**
		 *\~english
		 *\brief		Retrieves the lights Sampler
		 *\return		The Sampler
		 *\~french
		 *\brief		Récupère le Sampler pour les textures de lumières
		 *\return		Le Sampler
		 */
		inline SamplerSPtr GetLightsSampler()const
		{
			return m_pLightsSampler;
		}
		/**
		 *\~english
		 *\brief		Retrieves the SceneFileParser additional parsers.
		 *\return		The parsers.
		 *\~french
		 *\brief		Récupère les analyseurs supplémentaires pour SceneFileParser.
		 *\return		Les analyseurs
		 */
		inline std::map< Castor::String, Castor::FileParser::AttributeParsersBySection > GetAdditionalParsers()const
		{
			return m_additionalParsers;
		}
		/**
		 *\~english
		 *\return		The engine version.
		 *\~french
		 *\return		La version du moteur.
		 */
		inline Version const & GetVersion()const
		{
			return m_version;
		}

	private:
		uint32_t DoMainLoop();
		void DoPreRender();
		void DoRender( bool p_bForce, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount );
		void DoPostRender();
		void DoUpdate( bool p_bForce );
		void DoRenderOneFrame();
		void DoRenderFlushFrame();
		void DoLoadCoreData();

	private:
		//!\~english The mutex, to make the engine resources access thread-safe	\~french Le mutex utilisé pour que l'accès aux ressources du moteur soit thread-safe
		std::recursive_mutex m_mutexResources;
		//!\~english The mutex, to make the main loop thread-safe	\~french Le mutex utilisé pour que la boucle principale soit thread-safe
		std::mutex m_mutexMainLoop;
		//!\~english The main loop, in case of threaded rendering	\~french La boucle principale, au cas où on fait du rendu threadé
		std::unique_ptr< std::thread > m_mainLoopThread;
		//!\~english Tells if render has ended, by any reason	\~french Dit si le rendu est terminé
		bool m_bEnded;
		//!\~english The wanted FPS, used in threaded render mode	\~french Le nombre de FPS souhaité, utilisé en rendu threadé
		uint32_t m_uiWantedFPS;
		//!\~english The wanted time for a frame	\~french Le temps voulu pour une frame
		double m_dFrameTime;
		//!\~english The engine version	\~french La version du moteur
		Version m_version;
		//!\~english The default frame listener.	\~french Le frame listener par défaut.
		FrameListenerWPtr m_defaultListener;
		//!\~english The RenderTechnique factory	\~french La fabrique de RenderTechnique
		TechniqueFactory m_techniqueFactory;
		//!\~english  The current RenderSystem	\~french Le RenderSystem courant
		RenderSystem * m_renderSystem;
		//!\~english  If \p false, the render can't be threaded	\~french Si \p false, le rendu ne peut pas être threadé
		bool m_bThreaded;
		//!\~english Tells if render is running	\~french Dit si le rendu est en cours
		bool m_bStarted;
		//!\~english Tells if render context is to be create	\~french Dit si le contexte de rendu est à créer
		bool m_bCreateContext;
		//!\~english Tells if render context is created	\~french Dit si le contexte de rendu est créé
		bool m_bCreated;
		//!\~english Tells if engine is cleaned up	\~french Dit si le moteur est nettoyé
		bool m_bCleaned;
		//!\~english The render window used to initalise the main rendering context	\~french La render window utilisée pour initialiser le contexte de rendu principal
		RenderWindow * m_pMainWindow;
		//!\~english Default blend states (no blend)	\~french Etats de blend par défaut (pas de blend)
		BlendStateSPtr m_pDefaultBlendState;
		//!\~english Default sampler	\~french Le sampler par défaut
		SamplerSPtr m_pDefaultSampler;
		//!\~english Lights textures sampler	\~french Le sampler utilisé pour les textures de lumières
		SamplerSPtr m_pLightsSampler;
		//!\~english Tells default sampler and default blend state are initialised	\~french Dit si le sampler et le blend state par défaut sont initialisés
		bool m_bDefaultInitialised;
		//!\~english The debug overlays are shown.	\~french Les incrustations de débogage.
		std::unique_ptr< DebugOverlays > m_debugOverlays;
		//!\~english The animations collection.	\~french La collection d'animations.
		DECLARE_MANAGED_MEMBER( animation, Animation );
		//!\~english The shaders collection.	\~french La collection de shaders.
		DECLARE_MANAGED_MEMBER( shader, Shader );
		//!\~english The sampler states collection.	\~french La collection de sampler states.
		DECLARE_MANAGED_MEMBER( sampler, Sampler );
		//!\~english The DepthStencilState collection.	\~french La collection de DepthStencilState.
		DECLARE_MANAGED_MEMBER( depthStencilState, DepthStencilState );
		//!\~english The RasteriserState collection.	\~french La collection de RasteriserState.
		DECLARE_MANAGED_MEMBER( rasteriserState, RasteriserState );
		//!\~english The BlendState collection.	\~french La collection de BlendState.
		DECLARE_MANAGED_MEMBER( blendState, BlendState );
		//!\~english The materials manager.	\~french Le gestionnaire de matériaux.
		DECLARE_MANAGED_MEMBER( material, Material );
		//!\~english The materials manager.	\~french Le gestionnaire de fenêtres.
		DECLARE_MANAGED_MEMBER( window, Window );
		//!\~english The meshes manager.	\~french Le gestionnaire de maillages.
		DECLARE_MANAGED_MEMBER( mesh, Mesh );
		//!\~english The plug-ins manager.	\~french Le gestionnaire de plug-ins.
		DECLARE_MANAGED_MEMBER( plugin, Plugin );
		//!\~english The overlays collection.	\~french La collection d'overlays.
		DECLARE_MANAGED_MEMBER( overlay, Overlay );
		//!\~english The scenes collection.	\~french La collection de scènes.
		DECLARE_MANAGED_MEMBER( scene, Scene );
		//!\~english The render targets map.	\~french La map des cibles de rendu.
		DECLARE_MANAGED_MEMBER( target, Target );
		//!\~english The frame listeners array	\~french Le tableau de frame listeners
		FrameListenerPtrStrMap m_listeners;
		//!\~english The fonts collection	\~french La collection de polices
		Castor::FontManager m_fontManager;
		//!\~english The images collection	\~french La collection d'images
		ImageCollection m_imageManager;
		//!\~english The map holding the parsers, sorted by section, and plugin name	\~french La map de parseurs, triés par section, et nom de plugin
		std::map< Castor::String, Castor::FileParser::AttributeParsersBySection > m_additionalParsers;
	};
}

#undef DECLARE_MANAGED_MEMBER

#endif
