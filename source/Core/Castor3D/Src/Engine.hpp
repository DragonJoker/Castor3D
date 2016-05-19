/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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

#include "Technique/TechniqueFactory.hpp"
#include "Miscellaneous/Version.hpp"

#include <FileParser.hpp>
#include <FontManager.hpp>
#include <ImageManager.hpp>
#include <Unique.hpp>

#define DECLARE_MANAGER_MEMBER( memberName, className )\
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
	\remark		Holds the render windows, the plug-ins, the render drivers...
	\~french
	\brief		Moteur principal
	\remark		Contient les fenêtres de rendu, les plug-ins, drivers de rendu...
	*/
	class Engine
		: Castor::Unique< Engine >
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
		C3D_API void Initialise( uint32_t p_wantedFPS = 100, bool p_threaded = false );
		/**
		 *\~english
		 *\brief		Cleanup function, destroys everything created from the beginning
		 *\remarks		Destroys also RenderWindows, the only things left after that is RenderSystem and loaded plug-ins
		 *\~french
		 *\brief		Fonction de nettoyage, détruit tout ce qui a été créé depuis le début
		 *\remarks		Détruit aussi les RenderWindows, les seules choses restantes après ça sont le RenderSystem et les plug-ins chargés
		 */
		C3D_API void Cleanup();
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
		 *\brief		Retrieves the cleanup status
		 *\remarks		Thread-safe
		 *\return		\p true if cleaned up
		 *\~french
		 *\brief		Récupère le statut de nettoyage
		 *\remarks		Thread-safe
		 *\return		\p true si nettoyé
		 */
		C3D_API bool IsCleaned();
		/**
		 *\~english
		 *\brief		Tells the engine is cleaned up
		 *\remarks		Thread-safe
		 *\~french
		 *\brief		Dit que le moteur est nettoyé
		 *\remarks		Thread-safe
		 */
		C3D_API void SetCleaned();
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
		 *\brief		Registers additional sections for SceneFileParser.
		 *\param[in]	p_name		The registering name.
		 *\param[in]	p_sections	The sections.
		 *\~french
		 *\brief		Enregistre des sections supplémentaires pour SceneFileParser.
		 *\param[in]	p_name		Le nom d'enregistrement.
		 *\param[in]	p_sections	Les sections.
		 */
		C3D_API void RegisterSections( Castor::String const & p_name, Castor::StrUIntMap const & p_sections );
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
		 *\brief		Unregisters sections for SceneFileParser.
		 *\param[in]	p_name		The registering name.
		 *\~french
		 *\brief		Désenregistre des sections pour SceneFileParser.
		 *\param[in]	p_name		Le nom d'enregistrement.
		 */
		C3D_API void UnregisterSections( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Retrieves plug-ins path
		 *\return		The plug-ins path
		 *\~french
		 *\brief		Récupère le chemin des plug-ins
		 *\return		Le chemin des plug-ins
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
		 *\brief		Retrieves the images collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'images
		 *\return		La collection
		 */
		inline Castor::ImageManager const & GetImageManager()const
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
		inline Castor::ImageManager & GetImageManager()
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
		 *\return		The user input listener.
		 *\~french
		 *\return		Le listener d'entrées utilisateur.
		 */
		inline UserInputListenerSPtr GetUserInputListener()
		{
			return m_userInputListener;
		}
		/**
		 *\~english
		 *\brief		Sets the user input listener.
		 *\param[in]	p_listener	The new value.
		 *\~french
		 *\brief		Définit le listener d'entrées utilisateur.
		 *\param[in]	p_listener	La nouvelle valeur.
		 */
		inline void SetUserInputListener( UserInputListenerSPtr p_listener )
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
		inline RenderSystem * GetRenderSystem()const
		{
			return m_renderSystem;
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
			return m_defaultBlendState;
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
		inline SamplerSPtr GetLightsSampler()const
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
		inline std::map< Castor::String, Castor::FileParser::AttributeParsersBySection > const & GetAdditionalParsers()const
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
		inline std::map< Castor::String, Castor::StrUIntMap > const & GetAdditionalSections()const
		{
			return m_additionalSections;
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
		/**
		 *\~english
		 *\return		The render loop.
		 *\~french
		 *\return		La boucle de rendu.
		 */
		inline bool HasRenderLoop()const
		{
			return m_renderLoop != nullptr;
		}
		/**
		 *\~english
		 *\return		The render loop.
		 *\~french
		 *\return		La boucle de rendu.
		 */
		inline RenderLoop const & GetRenderLoop()const
		{
			return *m_renderLoop;
		}
		/**
		 *\~english
		 *\return		The render loop.
		 *\~french
		 *\return		La boucle de rendu.
		 */
		inline RenderLoop & GetRenderLoop()
		{
			return *m_renderLoop;
		}
		/**
		 *\~english
		 *\brief		Sets the need for per object lighting.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le besoin d'un éclairage par objet.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetPerObjectLighting( bool p_value )
		{
			m_perObjectLighting = p_value;
		}
		/**
		 *\~english
		 *\return		The need for per object lighting.
		 *\~french
		 *\return		Le besoin d'un éclairage par objet.
		 */
		inline bool GetPerObjectLighting()
		{
			return m_perObjectLighting;
		}
		/**
		 *\~english
		 *\return		Tells if the engine uses an asynchronous render loop.
		 *\~french
		 *\return		Dit si le moteur utilise un boucle de rendu asynchrone.
		 */
		inline bool IsThreaded()
		{
			return m_threaded;
		}

	private:
		void DoLoadCoreData();

	private:
		//!\~english The mutex, to make the engine resources access thread-safe	\~french Le mutex utilisé pour que l'accès aux ressources du moteur soit thread-safe
		std::recursive_mutex m_mutexResources;
		//!\~english The render loop.	\~french La boucle de rendu.
		RenderLoopUPtr m_renderLoop;
		//!\~english The engine version	\~french La version du moteur
		Version m_version;
		//!\~english  The current RenderSystem	\~french Le RenderSystem courant
		RenderSystem * m_renderSystem;
		//!\~english Tells if engine is cleaned up	\~french Dit si le moteur est nettoyé
		bool m_cleaned;
		//!\~english Tells if engine uses an asynchronous render loop.	\~french Dit si le moteur utilise un boucle de rendu asynchrone.
		bool m_threaded;
		//!\~english Default blend states (no blend).	\~french Etats de blend par défaut (pas de blend).
		BlendStateSPtr m_defaultBlendState;
		//!\~english Default sampler.	\~french Le sampler par défaut.
		SamplerSPtr m_defaultSampler;
		//!\~english Lights textures sampler.	\~french L'échantillonneur utilisé pour les textures de lumières.
		SamplerSPtr m_lightsSampler;
		//!\~english The shaders collection.	\~french La collection de shaders.
		DECLARE_MANAGER_MEMBER( shader, Shader );
		//!\~english The sampler states collection.	\~french La collection de sampler states.
		DECLARE_MANAGER_MEMBER( sampler, Sampler );
		//!\~english The DepthStencilState collection.	\~french La collection de DepthStencilState.
		DECLARE_MANAGER_MEMBER( depthStencilState, DepthStencilState );
		//!\~english The RasteriserState collection.	\~french La collection de RasteriserState.
		DECLARE_MANAGER_MEMBER( rasteriserState, RasteriserState );
		//!\~english The BlendState collection.	\~french La collection de BlendState.
		DECLARE_MANAGER_MEMBER( blendState, BlendState );
		//!\~english The materials manager.	\~french Le gestionnaire de matériaux.
		DECLARE_MANAGER_MEMBER( material, Material );
		//!\~english The materials manager.	\~french Le gestionnaire de fenêtres.
		DECLARE_MANAGER_MEMBER( window, Window );
		//!\~english The meshes manager.	\~french Le gestionnaire de maillages.
		DECLARE_MANAGER_MEMBER( mesh, Mesh );
		//!\~english The plug-ins manager.	\~french Le gestionnaire de plug-ins.
		DECLARE_MANAGER_MEMBER( plugin, Plugin );
		//!\~english The overlays collection.	\~french La collection d'overlays.
		DECLARE_MANAGER_MEMBER( overlay, Overlay );
		//!\~english The scenes collection.	\~french La collection de scènes.
		DECLARE_MANAGER_MEMBER( scene, Scene );
		//!\~english The render targets map.	\~french La map des cibles de rendu.
		DECLARE_MANAGER_MEMBER( target, Target );
		//!\~english The frame listeners array	\~french Le tableau de frame listeners
		DECLARE_MANAGER_MEMBER( listener, Listener );
		//!\~english The render technique manager	\~french Le gestionnaire de techniques de rendu.
		DECLARE_MANAGER_MEMBER( technique, RenderTechnique );
		//!\~english The fonts collection	\~french La collection de polices
		Castor::FontManager m_fontManager;
		//!\~english The images collection	\~french La collection d'images
		Castor::ImageManager m_imageManager;
		//!\~english The user input listener.	\~french La listener d'entrées utilisateur.
		UserInputListenerSPtr m_userInputListener;
		//!\~english The map holding the parsers, sorted by section, and plug-in name	\~french La map de parseurs, triés par section, et nom de plug-in
		std::map< Castor::String, Castor::FileParser::AttributeParsersBySection > m_additionalParsers;
		//!\~english The map holding the sections, sorted plug-in name.	\~french La map de sections, triées par nom de plug-in.
		std::map< Castor::String, Castor::StrUIntMap > m_additionalSections;
		//!\~english The need for per object lighting.	\~french Le besoin d'un éclairage par objet.
		bool m_perObjectLighting;
	};
}

#undef DECLARE_MANAGER_MEMBER

#endif
