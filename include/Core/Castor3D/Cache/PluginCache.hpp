/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PluginCache_H___
#define ___C3D_PluginCache_H___

#include <CastorUtils/Design/ResourceCache.hpp>

#include "Castor3D/Cache/CacheModule.hpp"
#include "Castor3D/Plugin/PluginModule.hpp"

#include "Castor3D/Plugin/Plugin.hpp"

namespace castor
{
	/**
	\~english
	\brief		Plug-ins cache.
	\~french
	\brief		Cache de plug-ins.
	*/
	template<>
	class ResourceCacheT< castor3d::Plugin, String, castor3d::PluginCacheTraits > final
		: public ResourceCacheBaseT< castor3d::Plugin, String, castor3d::PluginCacheTraits >
	{
	public:
		using ElementT = castor3d::Plugin;
		using ElementKeyT = String;
		using ElementCacheTraitsT = castor3d::PluginCacheTraits;
		using ElementCacheT = ResourceCacheBaseT< ElementT, ElementKeyT, ElementCacheTraitsT >;
		using ElementPtrT = typename ElementCacheT::ElementPtrT;
		using ElementContT = typename ElementCacheT::ElementContT;
		using ElementInitialiserT = typename ElementCacheT::ElementInitialiserT;
		using ElementCleanerT = typename ElementCacheT::ElementCleanerT;
		using ElementMergerT = typename ElementCacheT::ElementMergerT;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API ResourceCacheT( castor3d::Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ResourceCacheT() = default;
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
		 *\param[in]	pluginName	The plug-in name
		 *\param[in]	pathFolder	Optional path where the plug-in may be looked for
		 *\return		The loaded plug-in, \p nullptr if name was incorrect or path was not valid
		 *\~french
		 *\brief		Charge un plug-in, selon son nom (ex : libGlRenderSystem.dll => GlRenderSystem)
		 *\param[in]	pluginName	Le nom du plug-in
		 *\param[in]	pathFolder	Un chemin optionnel, pour y trouver le plug-in
		 *\return		Le plug-in chargé, \p nullptr si non trouvé (nom incorrect ou non trouvé dans le chemin donné ou le chemin principal)
		 */
		C3D_API ElementPtrT loadPlugin( String const & pluginName
			, Path const & pathFolder )noexcept;
		/**
		 *\~english
		 *\brief		Loads a plug-in, given the plug-in file's full path
		 *\param[in]	fileFullPath	The plug-in file path
		 *\return		The loaded plug-in, \p nullptr if path was incorrect or if it wasn't a valid plug-in
		 *\~french
		 *\brief		Charge un plug-in dont le chemin est donné
		 *\param[in]	fileFullPath	Le chemin du plug-in
		 *\return		Le plug-in chargé, \p nullptr si le chemin était incorrect ou s'il ne représentait pas un plug-in valide
		 */
		C3D_API ElementPtrT loadPlugin( Path const & fileFullPath )noexcept;
		/**
		 *\~english
		 *\brief		Retrieves the plug-ins of given type
		 *\param[in]	type	The plug-ins type
		 *\return		\p nullptr if not found
		 *\~french
		 *\brief		Récupère les plug-ins du type donné
		 *\param[in]	type	Le type de plu-ins
		 *\return		\p nullptr si non trouvé
		 */
		C3D_API castor3d::PluginStrMap getPlugins( castor3d::PluginType type );
		/**
		 *\~english
		 *\brief		Loads all the plug-ins located in working folder
		 *\param[in]	folder	The plug-ins' folder
		 *\~french
		 *\brief		Charge tous les plug-ins d'un dossier donné
		 *\param[in]	folder	Le dossier
		 */
		C3D_API void loadAllPlugins( Path const & folder );

	private:
		ElementPtrT doloadPlugin( Path const & pathFile );

	private:
		castor3d::Engine & m_engine;
		//!\~english	The loaded shared libraries map.
		//!\~french		La map des shared libraries chargées.
		castor3d::DynamicLibraryPtrPathMapArray m_libraries;
		//!\~english	The mutex protecting the loaded shared libraries map.
		//!\~french		Le mutex protégeant la map des shared libraries chargées.
		std::recursive_mutex m_mutexLibraries;
		//!\~english	The loaded plug-ins map.
		//!\~french		La map des plug-ins chargés.
		castor3d::PluginStrMapArray m_loadedPlugins;
		//!\~english	The mutex protecting the loaded plug-ins map.
		//!\~french		Le mutex protégeant la map des plug-ins chargés.
		std::recursive_mutex m_mutexLoadedPlugins;
		//!\~english	The loaded plug-ins map, sorted by plug-in type.
		//!\~french		La map des plug-ins chargés, triés par type de plug-in.
		castor3d::PluginTypePathMap m_loadedPluginTypes;
		//!\~english	The mutex protecting the loaded plug-ins map sorted by type.
		//!\~french		Le mutex protégeant la map de plug-ins chargés triés par type.
		std::recursive_mutex m_mutexLoadedPluginTypes;
	};
}

#endif
