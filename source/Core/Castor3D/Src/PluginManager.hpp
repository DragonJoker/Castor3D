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

#include "ResourceManager.hpp"
#include "Plugin.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct ManagedObjectNamer< PluginBase >
	{
		C3D_API static const Castor::String Name;
	};
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
		: public ResourceManager< Castor::Path, PluginBase >
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
		C3D_API ~PluginManager();
		/**
		 *\copydoc		Castor::Manager::Cleanup
		 */
		C3D_API void Cleanup();
		/**
		 *\copydoc		Castor::Manager::Clear
		 */
		C3D_API void Clear();
		/**
		 *\~english
		 *\brief		Loads a plug-in, given the plug-in name (ex: libGlRenderSystem.dll => GlRenderSystem)
		 *\param[in]	p_pluginName	The plug-in name
		 *\param[in]	p_pathFolder	Optional path where the plug-in may be looked for
		 *\return		The loaded plug-in, \p nullptr if name was incorrect or path was not valid
		 *\~french
		 *\brief		Charge un plug-in, selon son nom (ex : libGlRenderSystem.dll => GlRenderSystem)
		 *\param[in]	p_pluginName	Le nom du plug-in
		 *\param[in]	p_pathFolder	Un chemin optionnel, pour y trouver le plug-in
		 *\return		Le plug-in chargé, \p nullptr si non trouvé (nom incorrect ou non trouvé dans le chemin donné ou le chemin principal)
		 */
		C3D_API PluginBaseSPtr LoadPlugin( Castor::String const & p_pluginName, Castor::Path const & p_pathFolder )throw();
		/**
		 *\~english
		 *\brief		Loads a plug-in, given the plug-in file's full path
		 *\param[in]	p_fileFullPath	The plug-in file path
		 *\return		The loaded plug-in, \p nullptr if path was incorrect or if it wasn't a valid plug-in
		 *\~french
		 *\brief		Charge un plug-in dont le chemin est donné
		 *\param[in]	p_fileFullPath	Le chemin du plug-in
		 *\return		Le plug-in chargé, \p nullptr si le chemin était incorrect ou s'il ne représentait pas un plug-in valide
		 */
		C3D_API PluginBaseSPtr LoadPlugin( Castor::Path const & p_fileFullPath )throw();
		/**
		 *\~english
		 *\brief		Retrieves the plug-ins of given type
		 *\param[in]	p_type	The plug-ins type
		 *\return		\p nullptr if not found
		 *\~french
		 *\brief		Récupère les plug-ins du type donné
		 *\param[in]	p_type	Le type de plu-ins
		 *\return		\p nullptr si non trouvé
		 */
		C3D_API PluginStrMap GetPlugins( ePLUGIN_TYPE p_type );
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
		C3D_API RenderSystem * LoadRenderer( eRENDERER_TYPE p_type );
		/**
		 *\~english
		 *\brief		Loads all the plug-ins located in working folder
		 *\param[in]	p_folder	The plug-ins' folder
		 *\~french
		 *\brief		Charge tous les plug-ins d'un dossier donné
		 *\param[in]	p_folder	Le dossier
		 */
		C3D_API void LoadAllPlugins( Castor::Path const & p_folder );
		/**
		 *\~english
		 *\brief		Retrieves an iterator on the renderer plug-ins array
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le tableau des plug-ins de rendu
		 *\return		L'itérateur
		 */
		inline RendererPtrArray const & GetRenderersList()const
		{
			return m_renderers;
		}

	private:
		PluginBaseSPtr LoadRendererPlugin( Castor::DynamicLibrarySPtr p_library );
		PluginBaseSPtr LoadTechniquePlugin( Castor::DynamicLibrarySPtr p_library );
		PluginBaseSPtr InternalLoadPlugin( Castor::Path const & p_pathFile );

	private:
		using ResourceManager< Castor::Path, PluginBase >::Create;

	private:
		//!\~english The loaded shared libraries map	\~french La map des shared libraries chargées
		DynamicLibraryPtrPathMapArray m_libraries;
		//!\~english The mutex protecting the loaded shared libraries map	\~french Le mutex protégeant la map des shared libraries chargées
		std::recursive_mutex m_mutexLibraries;
		//!\~english The loaded plug-ins map	\~french La map des plug-ins chargés
		PluginStrMapArray m_loadedPlugins;
		//!\~english The mutex protecting the loaded plug-ins map	\~french Le mutex protégeant la map des plug-ins chargés
		std::recursive_mutex m_mutexLoadedPlugins;
		//!\~english The loaded plug-ins map, sorted by plug-in type	\~french La map des plug-ins chargés, triés par type de plug-in
		PluginTypePathMap m_loadedPluginTypes;
		//!\~english The mutex protecting the loaded plug-ins map sorted by type	\~french Le mutex protégeant la map de plug-ins chargés triés par type
		std::recursive_mutex m_mutexLoadedPluginTypes;
		//!\~english The renderer plug-ins array	\~french Le tableau des plug-ins de rendu
		RendererPtrArray m_renderers;
		//!\~english The mutex protecting the renderer plug-ins array	\~french Le mutex protégeant le tableau des plug-ins de rendu
		std::recursive_mutex m_mutexRenderers;
	};
}

#endif
