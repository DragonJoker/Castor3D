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
#ifndef ___C3D_PLUGIN_MANAGER_H___
#define ___C3D_PLUGIN_MANAGER_H___

#include "Manager.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Plug-ins manager.
	\~french
	\brief		Gestionnaire de plug-ins.
	*/
	class PluginManager
		: public Manager< Castor::Path, PluginBase >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API PluginManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~PluginManager();
		/**
		 *\copydoc		Castor::Manager::Cleanup
		 */
		C3D_API virtual void Cleanup();
		/**
		 *\copydoc		Castor::Manager::Clear
		 */
		C3D_API virtual void Clear();
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
		C3D_API PluginBaseSPtr LoadPlugin( Castor::String const & p_strPluginName, Castor::Path const & p_pathFolder )throw();
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
		C3D_API PluginBaseSPtr LoadPlugin( Castor::Path const & p_fileFullPath )throw();
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
		C3D_API PluginStrMap GetPlugins( ePLUGIN_TYPE p_type );
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
		C3D_API RenderSystem * LoadRenderer( eRENDERER_TYPE p_type );
		/**
		 *\~english
		 *\brief		Loads all the plugins located in working folder
		 *\param[in]	p_strFolder	The plugins' folder
		 *\~french
		 *\brief		Charge tous les plugins d'un dossier donné
		 *\param[in]	p_strFolder	Le dossier
		 */
		C3D_API void LoadAllPlugins( Castor::Path const & p_strFolder );
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the plugins map
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la map des plugins
		 *\return		L'itérateur
		 */
		inline PluginStrMap const & GetPluginsList( ePLUGIN_TYPE p_type )const
		{
			return m_loadedPlugins[p_type];
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the renderer plugins array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le tableau des plugins de rendu
		 *\return		L'itérateur
		 */
		inline RendererPtrArray const & GetRenderersList()const
		{
			return m_renderers;
		}

	private:
		PluginBaseSPtr LoadRendererPlugin( Castor::DynamicLibrarySPtr p_pLibrary );
		PluginBaseSPtr LoadTechniquePlugin( Castor::DynamicLibrarySPtr p_pLibrary );
		PluginBaseSPtr InternalLoadPlugin( Castor::Path const & p_pathFile );

	private:
		//!\~english The loaded shared libraries map	\~french La map des shared libraries chargées
		DynamicLibraryPtrPathMapArray m_libraries;
		//!\~english The mutex protecting the loaded shared libraries map	\~french Le mutex protégeant la map des shared libraries chargées
		std::recursive_mutex m_mutexLibraries;
		//!\~english The loaded plugins map	\~french La map des plugins chargés
		PluginStrMapArray m_loadedPlugins;
		//!\~english The mutex protecting the loaded plugins map	\~french Le mutex protégeant la map des plugins chargés
		std::recursive_mutex m_mutexLoadedPlugins;
		//!\~english The loaded plugins map, sorted by plugin type	\~french La map des plugins chargés, triés par type de plugin
		PluginTypePathMap m_loadedPluginTypes;
		//!\~english The mutex protecting the loaded plugins map sorted by type	\~french Le mutex protégeant la map de plugins chargés triés par type
		std::recursive_mutex m_mutexLoadedPluginTypes;
		//!\~english The renderer plugins array	\~french Le tableau des plugins de rendu
		RendererPtrArray m_renderers;
		//!\~english The mutex protecting the renderer plugins array	\~french Le mutex protégeant le tableau des plugins de rendu
		std::recursive_mutex m_mutexRenderers;
	};
}

#endif
