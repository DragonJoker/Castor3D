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
#ifndef ___C3D_PLUGIN_CACHE_H___
#define ___C3D_PLUGIN_CACHE_H___

#include "Cache/Cache.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	\remarks	Specialisation for Plugin.
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	\remarks	Spécialisation pour Plugin.
	*/
	template< typename KeyType >
	struct CacheTraits< Plugin, KeyType >
	{
		C3D_API static const castor::String Name;
		using Producer = std::function< std::shared_ptr< Plugin >( KeyType const &, PluginType, castor::DynamicLibrarySPtr ) >;
		using Merger = std::function< void( CacheBase< Plugin, KeyType > const &
											, castor::Collection< Plugin, KeyType > &
											, std::shared_ptr< Plugin > ) >;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Plug-ins cache.
	\~french
	\brief		Cache de plug-ins.
	*/
	template<>
	class Cache< Plugin, castor::String >
		: public CacheBase< Plugin, castor::String >
	{
	public:
		using MyCacheType = CacheBase< Plugin, castor::String >;
		using Element = typename MyCacheType::Element;
		using Key = typename MyCacheType::Key;
		using Collection = typename MyCacheType::Collection;
		using ElementPtr = typename MyCacheType::ElementPtr;
		using Producer = typename MyCacheType::Producer;
		using Initialiser = typename MyCacheType::Initialiser;
		using Cleaner = typename MyCacheType::Cleaner;
		using Merger = typename MyCacheType::Merger;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	p_produce		The element producer.
		 *\param[in]	p_initialise	The element initialiser.
		 *\param[in]	p_clean			The element cleaner.
		 *\param[in]	p_merge			The element collection merger.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	p_produce		Le créateur d'objet.
		 *\param[in]	p_initialise	L'initialiseur d'objet.
		 *\param[in]	p_clean			Le nettoyeur d'objet.
		 *\param[in]	p_merge			Le fusionneur de collection d'objets.
		 */
		C3D_API Cache( Engine & engine
					   , Producer && p_produce
					   , Initialiser && p_initialise = Initialiser{}
					   , Cleaner && p_clean = Cleaner{}
					   , Merger && p_merge = Merger{} );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Cache();
		/**
		 *\~english
		 *\brief		Flushes the collection.
		 *\~french
		 *\brief		Vide la collection.
		 */
		C3D_API void clear();
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
		C3D_API PluginSPtr loadPlugin( castor::String const & p_pluginName, castor::Path const & p_pathFolder )throw();
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
		C3D_API PluginSPtr loadPlugin( castor::Path const & p_fileFullPath )throw();
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
		C3D_API PluginStrMap getPlugins( PluginType p_type );
		/**
		 *\~english
		 *\brief		Loads all the plug-ins located in working folder
		 *\param[in]	p_folder	The plug-ins' folder
		 *\~french
		 *\brief		Charge tous les plug-ins d'un dossier donné
		 *\param[in]	p_folder	Le dossier
		 */
		C3D_API void loadAllPlugins( castor::Path const & p_folder );

	private:
		PluginSPtr doloadPlugin( castor::Path const & p_pathFile );

	private:
		//!\~english	The loaded shared libraries map.
		//!\~french		La map des shared libraries chargées.
		DynamicLibraryPtrPathMapArray m_libraries;
		//!\~english	The mutex protecting the loaded shared libraries map.
		//!\~french		Le mutex protégeant la map des shared libraries chargées.
		std::recursive_mutex m_mutexLibraries;
		//!\~english	The loaded plug-ins map.
		//!\~french		La map des plug-ins chargés.
		PluginStrMapArray m_loadedPlugins;
		//!\~english	The mutex protecting the loaded plug-ins map.
		//!\~french		Le mutex protégeant la map des plug-ins chargés.
		std::recursive_mutex m_mutexLoadedPlugins;
		//!\~english	The loaded plug-ins map, sorted by plug-in type.
		//!\~french		La map des plug-ins chargés, triés par type de plug-in.
		PluginTypePathMap m_loadedPluginTypes;
		//!\~english	The mutex protecting the loaded plug-ins map sorted by type.
		//!\~french		Le mutex protégeant la map de plug-ins chargés triés par type.
		std::recursive_mutex m_mutexLoadedPluginTypes;
	};
	using PluginCache = Cache< Plugin, castor::String >;
	DECLARE_SMART_PTR( PluginCache );
}

#endif
