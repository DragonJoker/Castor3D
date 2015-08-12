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
#include "Version.hpp"
#include "FrameEvent.hpp"

#include <Unique.hpp>
#include <SquareMatrix.hpp>

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4251 )
#endif

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
	class C3D_API Engine
		: Castor::Unique< Engine >
		, std::enable_shared_from_this< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Engine();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Engine();
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
		void Initialise( uint32_t p_wantedFPS = 100, bool p_bThreaded = false );
		/**
		 *\~english
		 *\brief		Cleanup function, destroys everything created from the beginning
		 *\remark		Destroys also RenderWindows, the only things left after that is RenderSystem and loaded plugins
		 *\~french
		 *\brief		Fonction de nettoyage, détruit tout ce qui a été créé depuis le début
		 *\remark		Détruit aussi les RenderWindows, les seules choses restantes après ça sont le RenderSystem et les plugins chargés
		 */
		void Cleanup();
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
		ContextSPtr CreateContext( RenderWindow * p_pRenderWindow );
		/**
		 *\~english
		 *\brief		Starts threaded render loop
		 *\~french
		 *\brief		Commence le rendu threadé
		 */
		void StartRendering();
		/**
		 *\~english
		 *\brief		Ends the render, cleans up engine
		 *\remark		Ends the threaded render loop, if any
		 *\~french
		 *\brief		Termine le rendu, nettoie le moteur
		 *\remark		Arrête la boucle de rendu threadé, si elle existe
		 */
		void EndRendering();
		/**
		 *\~english
		 *\brief		Renders one frame, only if not in render loop
		 *\~french
		 *\brief		Rend une image, uniquement hors de la boucle de rendu
		 */
		void RenderOneFrame();
		/**
		 *\~english
		 *\brief		Renders all visible overlays
		 *\param[in]	p_scene	The scene displayed, to display its overlays and the global ones
		 *\param[in]	p_size	The render target size
		 *\~french
		 *\brief		Fonction de rendu des overlays visibles
		 *\param[in]	p_scene	La scène rendue, pour afficher ses overlays en plus des globaux
		 *\param[in]	p_size	Les dimensions de la cible du rendu
		 */
		void RenderOverlays( Scene const & p_scene, Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Creates a scene with the given name.
		 *\remark		If a scene with the given name already exists, it is returned and no scene is created
		 *\param[in]	p_strName	The scene name
		 *\return		The created scene
		 *\~french
		 *\brief		Crée une scène avec le nom donné
		 *\remark		Si une scène avec le nom voulu existe déjà, elle est retournée et aucune scène n'est créée
		 *\param[in]	p_strName	Le nom de la scène
		 *\return		La scène
		 */
		SceneSPtr CreateScene( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Empties each scene
		 *\~french
		 *\brief		Vide chaque scène
		 */
		void ClearScenes();
		/**
		 *\~english
		 *\brief		Creates a mesh with the given informations
		 *\remark		If a mesh with the given name already exists, it is returned and no mesh is created
		 *\param[in]	p_eType			Mesh type
		 *\param[in]	p_strName		The wanted name
		 *\return		The created mesh
		 *\~french
		 *\brief		Crée un mesh avec les informations données
		 *\remark		Si un mesh avec le nom donné existe déjà, il est retourné et aucun mesh n'est créé
		 *\param[in]	p_eType			Le type de mesh
		 *\param[in]	p_strName		Le nom du mesh
		 *\return		Le mesh créé
		 */
		MeshSPtr CreateMesh( eMESH_TYPE p_eType, Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Creates a mesh with the given informations
		 *\remark		If a mesh with the given name already exists, it is returned and no mesh is created
		 *\param[in]	p_eType			Mesh type
		 *\param[in]	p_strName		The wanted name
		 *\param[in]	p_arrayFaces	The array of faces (for non custom meshes)
		 *\return		The created mesh
		 *\~french
		 *\brief		Crée un mesh avec les informations données
		 *\remark		Si un mesh avec le nom donné existe déjà, il est retourné et aucun mesh n'est créé
		 *\param[in]	p_eType			Le type de mesh
		 *\param[in]	p_strName		Le nom du mesh
		 *\param[in]	p_arrayFaces	Le tableau de faces (pour les mesh non custom)
		 *\return		Le mesh créé
		 */
		MeshSPtr CreateMesh( eMESH_TYPE p_eType, Castor::String const & p_strName, UIntArray const & p_arrayFaces );
		/**
		 *\~english
		 *\brief		Creates a mesh with the given informations
		 *\remark		If a mesh with the given name already exists, it is returned and no mesh is created
		 *\param[in]	p_eType			Mesh type
		 *\param[in]	p_strName		The wanted name
		 *\param[in]	p_arrayFaces	The array of faces (for non custom meshes)
		 *\param[in]	p_arraySizes	The array of dimensions (for non custom meshes)
		 *\return		The created mesh
		 *\~french
		 *\brief		Crée un mesh avec les informations données
		 *\remark		Si un mesh avec le nom donné existe déjà, il est retourné et aucun mesh n'est créé
		 *\param[in]	p_eType			Le type de mesh
		 *\param[in]	p_strName		Le nom du mesh
		 *\param[in]	p_arrayFaces	Le tableau de faces (pour les mesh non custom)
		 *\param[in]	p_arraySizes	Le tableau de dimensions (pour les mesh non custom)
		 *\return		Le mesh créé
		 */
		MeshSPtr CreateMesh( eMESH_TYPE p_eType, Castor::String const & p_strName, UIntArray const & p_arrayFaces, RealArray const & p_arraySizes );
		/**
		 *\~english
		 *\brief		Saves in a file all currently loaded meshes
		 *\param[out]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Enregistre tous les meshes chargés dans un fichier
		 *\param[out]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool SaveMeshes( Castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		Loads all meshes contained in the given file
		 *\param[in]	p_file	The file
		 *\return		\p true if ok
		 *\~french
		 *\brief		Charge tous les meshes contenus dans un fichier
		 *\param[out]	p_file	Le fichier
		 *\return		\p true si tout s'est bien passé
		 */
		bool LoadMeshes( Castor::BinaryFile & p_file );
		/**
		 *\~english
		 *\brief		Creates an overlay, given a type and the overlay definitions
		 *\remark		If an overlay with the given name already exists, no creation is done, the return is the existing overlay
		 *\param[in]	p_eType		The overlay type (panel, text ...)
		 *\param[in]	p_strName	The overlay name
		 *\param[in]	p_pParent	The parent overlay, NULL if none
		 *\param[in]	p_pScene	The scene that holds the overlay
		 *\return		The created overlay
		 *\~french
		 *\brief		Crée un overlay
		 *\remark		Si un overlay avec le même nom existe déjà, aucune création n'est faite, l'existant est retourné
		 *\param[in]	p_eType		Le type d'overlay
		 *\param[in]	p_strName	Le nom voulu pour l'overlay
		 *\param[in]	p_pParent	L'overlay parent, nullptr si aucun
		 *\param[in]	p_pScene	La scène contenant l'overlay
		 *\return		L'overlay
		 */
		OverlaySPtr CreateOverlay( eOVERLAY_TYPE p_eType, Castor::String const & p_strName, OverlaySPtr p_pParent, SceneSPtr p_pScene );
		/**
		 *\~english
		 *\brief		Creates a render window
		 *\return		The created render window
		 *\~french
		 *\brief		Crée une fenêtre de rendu
		 *\return		La fenêtre de rendu créée
		 */
		RenderWindowSPtr CreateRenderWindow();
		/**
		 *\~english
		 *\brief		Removes a render window, by index
		 *\param[in]	p_uiIndex	The window index
		 *\return		\p false if the render window was not found
		 *\~french
		 *\brief		Supprime une fenêtre de rendu, par index
		 *\param[in]	p_uiIndex	L'index de la fenêtre
		 *\return		\p false si la fenêtre n'a pas été trouvée
		 */
		bool RemoveRenderWindow( uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Removes a render window, by pointer
		 *\param[in]	p_window	The render window pointer
		 *\return		\p false if the render window was not found
		 *\~french
		 *\brief		Supprime une fenêtre de rendu, par pointeur
		 *\param[in]	p_window	Le pointeur sur la fenêtre de rendu
		 *\return		\p false si la fenêtre n'a pas été trouvée
		 */
		bool RemoveRenderWindow( RenderWindowSPtr p_window );
		/**
		 *\~english
		 *\brief		Removes all render windows
		 *\~french
		 *\brief		Supprime toutes les fenêtres de rendu
		 */
		void RemoveAllRenderWindows();
		/**
		 *\~english
		 *\brief		Loads a plugin, given the plugin name (ex: libGlRenderSystem.dll => GlRenderSystem)
		 *\param[in]	p_strPluginName	The plugin name
		 *\param[in]	p_pathFolder	Optional path where the plugin may be looked for
		 *\return		The loaded plugin, \p nullptr if name was incorrect or path was not valid
		 *\~french
		 *\brief		Charge un plugin, selon son nom (ex : libGlRenderSystem.dll => GlRenderSystem)
		 *\param[in]	p_strPluginName	Le nom du plugin
		 *\param[in]	p_pathFolder	Un chemin optionnel, pour y trouver le plugin
		 *\return		Le plugin chargé, \p nullptr si non trouvé (nom incorrect ou non trouvé dans le chemin donné ou le chemin principal)
		 */
		PluginBaseSPtr LoadPlugin( Castor::String const & p_strPluginName, Castor::Path const & p_pathFolder )throw();
		/**
		 *\~english
		 *\brief		Loads a plugin, given the plugin file's full path
		 *\param[in]	p_filePath	The plugin file path
		 *\return		The loaded plugin, \p nullptr if path was incorrect or if it wasn't a valid plugin
		 *\~french
		 *\brief		Charge un plugin dont le chemin est donné
		 *\param[in]	p_filePath	Le chemin du plugin
		 *\return		Le plugin chargé, \p nullptr si le chemin était incorrect ou s'il ne représentait pas un plugin valide
		 */
		PluginBaseSPtr LoadPlugin( Castor::Path const & p_fileFullPath )throw();
		/**
		 *\~english
		 *\brief		Retrieves a shader plugin for given shader language
		 *\param[in]	p_eLanguage	The shader language
		 *\return		\p NULL if not found
		 *\~french
		 *\brief		Récupère un ShaderPlugin pour le langage donné
		 *\param[in]	p_eLanguage	Le langage
		 *\return		\p NULL si non trouvé
		 */
		ShaderPluginSPtr GetShaderPlugin( eSHADER_LANGUAGE p_eLanguage );
		/**
		 *\~english
		 *\brief		Loads a renderer plugin, given the renderer type
		 *\param[in]	p_eType	The renderer type
		 *\return		\p true if ok
		 *\~french
		 *\brief		Charge un plugin de rendu, selon le type de rendu
		 *\param[in]	p_eType	Le type de rendu
		 *\return		\p true si tout s'est bien passé
		 */
		bool LoadRenderer( eRENDERER_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Loads all the plugins located in working folder
		 *\param[in]	p_strFolder	The plugins' folder
		 *\~french
		 *\brief		Charge tous les plugins d'un dossier donné
		 *\param[in]	p_strFolder	Le dossier
		 */
		void LoadAllPlugins( Castor::Path const & p_strFolder );
		/**
		 *\~english
		 *\brief		Retrieves plugins path
		 *\return		The plugins path
		 *\~french
		 *\brief		Récupère le chemin des plugins
		 *\return		Le chemin des plugins
		 */
		static Castor::Path GetPluginsDirectory();
		/**
		 *\~english
		 *\brief		Gives the Castor directory
		 *\return		The directory
		 *\~french
		 *\brief		Donne le répertoire du Castor
		 *\return		Le répertoire
		 */
		static Castor::Path GetEngineDirectory();
		/**
		 *\~english
		 *\brief		Retrieves data path
		 *\return		The data path
		 *\~french
		 *\brief		Récupère le chemin des données
		 *\return		Le chemin des données
		 */
		static Castor::Path GetDataDirectory();
		/**
		 *\~english
		 *\brief		Posts a frame event
		 *\param[in]	p_pEvent	The event to add
		 *\~french
		 *\brief		Ajoute un évènement de frame à la queue
		 *\param[in]	p_pEvent	L'évènement
		 */
		void PostEvent( FrameEventSPtr p_pEvent );
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
			return *m_pTechniqueFactory;
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
			return *m_pTechniqueFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the plugins map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la map des plugins
		 *\return		L'itérateur
		 */
		inline PluginStrMapIt PluginsBegin( ePLUGIN_TYPE p_eType )
		{
			return m_arrayLoadedPlugins[p_eType].begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the plugins map end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la find de la map des plugins
		 *\return		L'itérateur
		 */
		inline PluginStrMapIt PluginsEnd( ePLUGIN_TYPE p_eType )
		{
			return m_arrayLoadedPlugins[p_eType].end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the plugins map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la map des plugins
		 *\return		L'itérateur
		 */
		inline PluginStrMapConstIt PluginsBegin( ePLUGIN_TYPE p_eType )const
		{
			return m_arrayLoadedPlugins[p_eType].begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the plugins map end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la find de la map des plugins
		 *\return		L'itérateur
		 */
		inline PluginStrMapConstIt PluginsEnd( ePLUGIN_TYPE p_eType )const
		{
			return m_arrayLoadedPlugins[p_eType].end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the renderer plugins array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le tableau des plugins de rendu
		 *\return		L'itérateur
		 */
		inline RendererPtrArrayIt RenderersBegin()
		{
			return m_arrayRenderers.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the renderer plugins array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le tableau des plugins de rendu
		 *\return		L'itérateur
		 */
		inline RendererPtrArrayConstIt RenderersBegin()const
		{
			return m_arrayRenderers.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the renderer plugins array end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin du tableau des plugins de rendu
		 *\return		L'itérateur
		 */
		inline RendererPtrArrayIt RenderersEnd()
		{
			return m_arrayRenderers.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves a constant iterator on the renderer plugins array end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur constant sur la fin du tableau des plugins de rendu
		 *\return		L'itérateur
		 */
		inline RendererPtrArrayConstIt RenderersEnd()const
		{
			return m_arrayRenderers.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the render windows map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la map de fenêtres de rendu
		 *\return		L'itérateur
		 */
		inline RenderWindowMapIt RenderWindowsBegin()
		{
			return m_mapWindows.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the render windows map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la map de fenêtres de rendu
		 *\return		L'itérateur
		 */
		inline RenderWindowMapConstIt RenderWindowsBegin()const
		{
			return m_mapWindows.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the render windows map end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map de fenêtres de rendu
		 *\return		L'itérateur
		 */
		inline RenderWindowMapIt RenderWindowsEnd()
		{
			return m_mapWindows.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the render windows map end
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la map de fenêtres de rendu
		 *\return		L'itérateur
		 */
		inline RenderWindowMapConstIt RenderWindowsEnd()const
		{
			return m_mapWindows.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves the materials collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de matériaux
		 *\return		La collection
		 */
		inline MaterialManager const & GetMaterialManager()const
		{
			return *m_pMaterialManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the materials collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de matériaux
		 *\return		La collection
		 */
		inline MaterialManager & GetMaterialManager()
		{
			return *m_pMaterialManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlays collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'overlays
		 *\return		La collection
		 */
		inline OverlayManager const & GetOverlayManager()const
		{
			return *m_pOverlayManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the overlays collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'overlays
		 *\return		La collection
		 */
		inline OverlayManager & GetOverlayManager()
		{
			return *m_pOverlayManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the shaders collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de shaders
		 *\return		La collection
		 */
		inline ShaderManager const & GetShaderManager()const
		{
			return *m_pShaderManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the shaders collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de shaders
		 *\return		La collection
		 */
		inline ShaderManager & GetShaderManager()
		{
			return *m_pShaderManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de scènes
		 *\return		La collection
		 */
		inline SceneCollection const & GetSceneManager()const
		{
			return *m_pSceneManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the scene collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de scènes
		 *\return		La collection
		 */
		inline SceneCollection & GetSceneManager()
		{
			return *m_pSceneManager;
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
			return *m_pImageManager;
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
			return *m_pImageManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the fonts collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de polices
		 *\return		La collection
		 */
		inline FontCollection const & GetFontManager()const
		{
			return *m_pFontManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the fonts collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de polices
		 *\return		La collection
		 */
		inline FontCollection & GetFontManager()
		{
			return *m_pFontManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animations collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'animations
		 *\return		La collection
		 */
		inline AnimationCollection const & GetAnimationManager()const
		{
			return *m_pAnimationManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animations collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection d'animations
		 *\return		La collection
		 */
		inline AnimationCollection & GetAnimationManager()
		{
			return *m_pAnimationManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the meshes collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de maillages
		 *\return		La collection
		 */
		inline MeshCollection const & GetMeshManager()const
		{
			return *m_pMeshManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the meshes collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de maillages
		 *\return		La collection
		 */
		inline MeshCollection & GetMeshManager()
		{
			return *m_pMeshManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the DepthStencilState collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de DepthStencilState
		 *\return		La collection
		 */
		inline DepthStencilStateCollection const & GetDepthStencilStateManager()const
		{
			return *m_pDepthStencilStateManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the DepthStencilState collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de DepthStencilState
		 *\return		La collection
		 */
		inline DepthStencilStateCollection & GetDepthStencilStateManager()
		{
			return *m_pDepthStencilStateManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RasteriserState collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de RasteriserState
		 *\return		La collection
		 */
		inline RasteriserStateCollection const & GetRasteriserStateManager()const
		{
			return *m_pRasteriserStateManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RasteriserState collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de RasteriserState
		 *\return		La collection
		 */
		inline RasteriserStateCollection & GetRasteriserStateManager()
		{
			return *m_pRasteriserStateManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the BlendState collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de BlendState
		 *\return		La collection
		 */
		inline BlendStateCollection const & GetBlendStateManager()const
		{
			return *m_pBlendStateManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the BlendState collection
		 *\return		The collection
		 *\~french
		 *\brief		Récupère la collection de BlendState
		 *\return		La collection
		 */
		inline BlendStateCollection & GetBlendStateManager()
		{
			return *m_pBlendStateManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Mesh factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique de Mesh
		 *\return		La fabrique
		 */
		inline MeshFactory const & GetMeshFactory()const
		{
			return *m_pMeshFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Mesh factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique de Mesh
		 *\return		La fabrique
		 */
		inline MeshFactory  & GetMeshFactory()
		{
			return *m_pMeshFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Light factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique de Light
		 *\return		La fabrique
		 */
		inline LightFactory const & GetLightFactory()const
		{
			return *m_pLightFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Light factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique de Light
		 *\return		La fabrique
		 */
		inline LightFactory & GetLightFactory()
		{
			return *m_pLightFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Overlay factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique d'Overlay
		 *\return		La fabrique
		 */
		inline OverlayFactory const & GetOverlayFactory()const
		{
			return *m_pOverlayFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Overlay factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique d'Overlay
		 *\return		La fabrique
		 */
		inline OverlayFactory & GetOverlayFactory()
		{
			return *m_pOverlayFactory;
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
			return m_pRenderSystem;
		}
		/**
		 *\~english
		 *\brief		Retrieves the sampler collection
		 *\return		The sampler collection
		 *\~french
		 *\brief		Récupère la collection de samplers
		 *\return		La collection de samplers
		 */
		inline SamplerCollection const & GetSamplerManager()const
		{
			return *m_pSamplerManager;
		}
		/**
		 *\~english
		 *\brief		Retrieves the sampler collection
		 *\return		The sampler collection
		 *\~french
		 *\brief		Récupère la collection de samplers
		 *\return		La collection de samplers
		 */
		inline SamplerCollection & GetSamplerManager()
		{
			return *m_pSamplerManager;
		}
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
		bool IsEnded();
		/**
		 *\~english
		 *\brief		Tells the engine the render is ended
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le rendu est stoppé
		 *\remark		Thread-safe
		 */
		void SetEnded();
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
		bool IsStarted();
		/**
		 *\~english
		 *\brief		Tells the engine the render is started
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le rendu est démarré
		 *\remark		Thread-safe
		 */
		void SetStarted();
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
		bool IsCreated();
		/**
		 *\~english
		 *\brief		Tells the engine the render context is created
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le contexte de rendu est créé
		 *\remark		Thread-safe
		 */
		void SetCreated();
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
		bool IsToCreate();
		/**
		 *\~english
		 *\brief		Tells the engine the render context is to create
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le contexte de rendu est à créer
		 *\remark		Thread-safe
		 */
		void SetToCreate();
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
		bool IsCleaned();
		/**
		 *\~english
		 *\brief		Tells the engine is cleaned up
		 *\remark		Thread-safe
		 *\~french
		 *\brief		Dit que le moteur est nettoyé
		 *\remark		Thread-safe
		 */
		void SetCleaned();
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
		double GetFrameTime();
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
		 *\brief		Updates the overlays collection
		 *\~french
		 *\brief		Met à jour la collection d'overlays
		 */
		void UpdateOverlayManager();
		/**
		 *\~english
		 *\brief		Updates the shaders collection
		 *\~french
		 *\brief		Met à jour la collection de shaders
		 */
		void UpdateShaderManager();
		/**
		 *\~english
		 *\brief		Creates an RenderTechnique from a technique name
		 *\param[in]	p_strName		The technique name
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
		RenderTechniqueBaseSPtr CreateTechnique( Castor::String const & p_strName, RenderTarget & p_renderTarget, Parameters const & p_params );
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
		bool SupportsShaderModel( eSHADER_MODEL p_eShaderModel );
		/**
		 *\~english
		 *\brief		Tells if the renderer API supports depth buffer for main FBO
		 *\return		The support status
		 *\~french
		 *\brief		Dit si l'API de rendu supporte les tampons de profondeur pour le FBO principal
		 *\return		Le statut du support
		 */
		bool SupportsDepthBuffer()const;
		/**
		 *\~english
		 *\brief		Creates a render target of given type
		 *\param[in]	p_eType	The render target type
		 *\return		The render target
		 *\~french
		 *\brief		Crée une cible de rendu du type voulu
		 *\param[in]	p_eType	Le type
		 *\return		La cible de rendu
		 */
		RenderTargetSPtr CreateRenderTarget( eTARGET_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Removes a render target from the render loop
		 *\param[in]	p_pRenderTarget	The render target
		 *\~french
		 *\brief		Enlève une cible de rendu de la boucle de rendu
		 *\param[in]	p_pRenderTarget	La cible de rendu
		 */
		void RemoveRenderTarget( RenderTargetSPtr && p_pRenderTarget );
		/**
		 *\~english
		 *\brief		Creates a FrameListener
		 *\return		The created FrameListener
		 *\~french
		 *\brief		Crée un FrameListener
		 *\return		Le FrameListener créé
		 */
		FrameListenerSPtr CreateFrameListener();
		/**
		 *\~english
		 *\brief		Destroys a FrameListener
		 *\param[in]	p_pListener	The FrameListener
		 *\~french
		 *\brief		Détruit un FrameListener
		 *\param[in]	p_pListener	Le FrameListener
		 */
		void DestroyFrameListener( FrameListenerSPtr & p_pListener );
		/**
		 *\~english
		 *\brief		Creates and returns a Sampler, given a name
		 *\remark		If a Sampler with the same name exists, none is created
		 *\param[in]	p_strName	The Sampler name
		 *\return		The created or existing Sampler
		 *\~french
		 *\brief		Crée et renvoie un Sampler, avec le nom donné
		 *\remark		Si un Sampler avec le même nom existe, aucun n'est créé
		 *\param[in]	p_strName	Le nom du Sampler
		 *\return		Le Sampler créé ou existant
		 */
		SamplerSPtr CreateSampler( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Creates and returns a DepthStencilState, given a name
		 *\remark		If a DepthStencilState with the same name exists, none is created
		 *\param[in]	p_strName	The DepthStencilState name
		 *\return		The created or existing DepthStencilState
		 *\~french
		 *\brief		Crée et renvoie un DepthStencilState, avec le nom donné
		 *\remark		Si un DepthStencilState avec le même nom existe, aucun n'est créé
		 *\param[in]	p_strName	Le nom du DepthStencilState
		 *\return		Le DepthStencilState créé ou existant
		 */
		DepthStencilStateSPtr CreateDepthStencilState( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Creates and returns a RasteriserState, given a name
		 *\remark		If a RasteriserState with the same name exists, none is created
		 *\param[in]	p_strName	The RasteriserState name
		 *\return		The created or existing RasteriserState
		 *\~french
		 *\brief		Crée et renvoie un RasteriserState, avec le nom donné
		 *\remark		Si un RasteriserState avec le même nom existe, aucun n'est créé
		 *\param[in]	p_strName	Le nom du RasteriserState
		 *\return		Le RasteriserState créé ou existant
		 */
		RasteriserStateSPtr CreateRasteriserState( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Creates and returns a BlendState, given a name
		 *\remark		If a BlendState with the same name exists, none is created
		 *\param[in]	p_strName	The BlendState name
		 *\return		The created or existing BlendState
		 *\~french
		 *\brief		Crée et renvoie un BlendState, avec le nom donné
		 *\remark		Si un BlendState avec le même nom existe, aucun n'est créé
		 *\param[in]	p_strName	Le nom du BlendState
		 *\return		Le BlendState créé ou existant
		 */
		BlendStateSPtr CreateBlendState( Castor::String const & p_strName );
		/**
		 *\~english
		 *\brief		Show or hide debug overlays
		 *\param[in]	p_show	The status
		 *\~french
		 *\brief		Affiche ou cache les incrustations de débogage
		 *\param[in]	p_show	Le statut
		 */
		void ShowDebugOverlays( bool p_show );
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

	private:
		uint32_t DoMainLoop();
		void DoPreRender( double & p_cpuTime, double & p_gpuTime );
		void DoRender( bool p_bForce, double & p_cpuTime, double & p_gpuTime, uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount );
		void DoPostRender( double & p_cpuTime, double & p_gpuTime );
		void DoUpdate( bool p_bForce );
		void DoLock();
		void DoUnlock();
		PluginBaseSPtr LoadRendererPlugin( Castor::DynamicLibrarySPtr p_pLibrary );
		PluginBaseSPtr LoadProgramPlugin( Castor::DynamicLibrarySPtr p_pLibrary );
		PluginBaseSPtr LoadTechniquePlugin( Castor::DynamicLibrarySPtr p_pLibrary );
		PluginBaseSPtr InternalLoadPlugin( Castor::Path const & p_pathFile );
		void DoRenderOneFrame();
		void DoRenderFlushFrame();
		void DoLoadCoreData();
		void DoDisplayDebugOverlays( double p_cpuTime, double p_gpuTime, double p_totalTime, uint32_t p_vertices, uint32_t p_faces, uint32_t p_objects );

	private:
		DECLARE_MULTIMAP( eTARGET_TYPE, RenderTargetSPtr, RenderTarget );
		//!\~english The mutex, to make the engine resources access thread-safe	\~french Le mutex utilisé pour que l'accès aux ressources du moteur soit thread-safe
		std::recursive_mutex m_mutexResources;
		//!\~english The mutex, to make the main loop thread-safe	\~french Le mutex utilisé pour que la boucle principale soit thread-safe
		std::mutex m_mutexMainLoop;
		//!\~english The main loop, in case of threaded rendering	\~french La boucle principale, au cas où on fait du rendu threadé
		std::unique_ptr< std::thread > m_pThreadMainLoop;
		//!\~english The loaded shared libraries map	\~french La map des shared libraries chargées
		DynamicLibraryPtrPathMapArray m_librariesMap;
		//!\~english The mutex protecting the loaded shared libraries map	\~french Le mutex protégeant la map des shared libraries chargées
		std::recursive_mutex m_mutexLibraries;
		//!\~english The loaded plugins map	\~french La map des plugins chargés
		PluginStrMapArray m_arrayLoadedPlugins;
		//!\~english The mutex protecting the loaded plugins map	\~french Le mutex protégeant la map des plugins chargés
		std::recursive_mutex m_mutexLoadedPlugins;
		//!\~english The loaded shader plugins map	\~french La map des plugins de shader chargés
		ShaderPluginMap m_mapShaderPlugins;
		//!\~english The mutex protecting the loaded shader plugins map	\~french Le mutex protégeant la map des plugins de shader chargés
		std::recursive_mutex m_mutexShaderPlugins;
		//!\~english The loaded plugins map, sorted by plugin type	\~french La map des plugins chargés, triés par type de plugin
		PluginTypePathMap m_mapLoadedPluginTypes;
		//!\~english The mutex protecting the loaded plugins map sorted by type	\~french Le mutex protégeant la map de plugins chargés triés par type
		std::recursive_mutex m_mutexLoadedPluginTypes;
		//!\~english The renderer plugins array	\~french Le tableau des plugins de rendu
		RendererPtrArray m_arrayRenderers;
		//!\~english The mutex protecting the renderer plugins array	\~french Le mutex protégeant le tableau des plugins de rendu
		std::recursive_mutex m_mutexRenderers;
		//!\~english The render windows map, sorted by ID	\~french La map des fenêtre de rendu, triées par ID
		RenderWindowMap m_mapWindows;
		//!\~english Tells if render has ended, by any reason	\~french Dit si le rendu est terminé
		bool m_bEnded;
		//!\~english The wanted FPS, used in threaded render mode	\~french Le nombre de FPS souhaité, utilisé en rendu threadé
		uint32_t m_uiWantedFPS;
		//!\~english The wanted time for a frame	\~french Le temps voulu pour une frame
		double m_dFrameTime;
		//!\~english The engine version	\~french La version du moteur
		Version m_version;
		//!\~english The frame listeners array	\~french Le tableau de frame listeners
		FrameListenerPtrArray m_arrayListeners;
		//!\~english The animations collection	\~french La collection d'animations
		AnimationCollectionUPtr m_pAnimationManager;
		//!\~english The meshes collection	\~french La collection de maillages
		MeshCollectionUPtr m_pMeshManager;
		//!\~english The fonts collection	\~french La collection de polices
		FontCollectionUPtr m_pFontManager;
		//!\~english The images collection	\~french La collection d'images
		ImageCollectionUPtr m_pImageManager;
		//!\~english The scenes collection	\~french La collection de scènes
		SceneCollectionUPtr m_pSceneManager;
		//!\~english The overlays collection	\~french La collection d'overlays
		OverlayManagerUPtr m_pOverlayManager;
		//!\~english The shaders collection	\~french La collection de shaders
		ShaderManagerUPtr m_pShaderManager;
		//!\~english The materials collection	\~french La collection de matériaux
		MaterialManagerUPtr m_pMaterialManager;
		//!\~english The LightCategory factory	\~french La fabrique de LightCategory
		LightFactoryUPtr m_pLightFactory;
		//!\~english The MeshCategory factory	\~french La fabrique de MeshCategory
		MeshFactoryUPtr m_pMeshFactory;
		//!\~english The OverlayCategory factory	\~french La fabrique de OverlayCategory
		OverlayFactoryUPtr m_pOverlayFactory;
		//!\~english The RenderTechnique factory	\~french La fabrique de RenderTechnique
		TechniqueFactoryUPtr m_pTechniqueFactory;
		//!\~english The sampler states collection	\~french La collection de sampler states
		SamplerCollectionUPtr m_pSamplerManager;
		//!\~english The DepthStencilState collection	\~french La collection de DepthStencilState
		DepthStencilStateCollectionUPtr m_pDepthStencilStateManager;
		//!\~english The RasteriserState collection	\~french La collection de RasteriserState
		RasteriserStateCollectionUPtr m_pRasteriserStateManager;
		//!\~english The BlendState collection	\~french La collection de BlendState
		BlendStateCollectionUPtr m_pBlendStateManager;
		//!\~english  The current RenderSystem	\~french Le RenderSystem courant
		RenderSystem * m_pRenderSystem;
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
		//!\~english The render window used to initalise the main context	\~french La render window utilisée pour initialiser le contexte de rendu principal
		RenderWindow * m_pMainWindow;
		//!\~english The render targets map	\~french La map des cibles de rendu
		RenderTargetMMap m_mapRenderTargets;
		//!\~english Default blend states (no blend)	\~french Etats de blend par défaut (pas de blend)
		BlendStateSPtr m_pDefaultBlendState;
		//!\~english Default sampler	\~french Le sampler par défaut
		SamplerSPtr m_pDefaultSampler;
		//!\~english Lights textures sampler	\~french Le sampler utilisé pour les textures de lumières
		SamplerSPtr m_pLightsSampler;
		//!\~english Tells default sampler and default blend state are initialised	\~french Dit si le sampler et le blend state par défaut sont initialisés
		bool m_bDefaultInitialised;
		//!\~english The base debug panel overlay	\~french Le panneau d'incrustations de débogage
		OverlayWPtr m_debugPanel;
		//!\~english The CPU time value overlay	\~french L'incrustation contenant la valeur de temps CPU
		TextOverlayWPtr m_debugCpuTime;
		//!\~english The GPU time value overlay	\~french L'incrustation contenant la valeur de temps GPU
		TextOverlayWPtr m_debugGpuTime;
		//!\~english The total time value overlay	\~french L'incrustation contenant la valeur de temps total
		TextOverlayWPtr m_debugTotalTime;
		//!\~english The vertex count value overlay	\~french L'incrustation contenant la valeur de nombre de sommets
		TextOverlayWPtr m_debugVertexCount;
		//!\~english The face count value overlay	\~french L'incrustation contenant la valeur de nombre de faces
		TextOverlayWPtr m_debugFaceCount;
		//!\~english The object count value overlay	\~french L'incrustation contenant la valeur de combre d'objets
		TextOverlayWPtr m_debugObjectCount;
		//!\~english The vertex count value	\~french La valeur de nombre de sommets
		uint32_t m_vertexCount;
		//!\~english The face count value	\~french La valeur de nombre de faces
		uint32_t m_faceCount;
		//!\~english The object count value	\~french La valeur de combre d'objets
		uint32_t m_objectCount;
		//!\~english Defines if the debug overlays are shown	\~french Définit si les incrustations de débogage sont affichées ou cachées
		bool m_showDebug;
	};
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#endif
