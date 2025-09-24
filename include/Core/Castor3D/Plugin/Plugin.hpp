/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PLUGIN_H___
#define ___C3D_PLUGIN_H___

#include "PluginModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

namespace c3d
{
	class Plugin
		: public OwnedBy< Engine >
	{
	public:
		//!< Signature for the plug-in's loading function
		using OnLoadFunction = void ( * )( Engine * );
		//!< Signature for the plug-in's unloading function
		using OnUnloadFunction = void ( * )( Engine * )noexcept;
		//!< Signature for the plug-in's debug mode retrieval function
		using IsDebugFunction = void ( * )( int * );
		//!< Signature for the plug-in's type retrieval function
		using GetTypeFunction = void ( * )( PluginType * );
		//!< Signature for the plug-in's version checking function
		using GetRequiredVersionFunction = void ( * )( Version * version );
		//!< Signature for the plug-in's name retrieval function
		using GetNameFunction = void ( * )( char const ** );

		using Extension = Pair< String, String >;
		CU_DeclareVector( Extension, Extension );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	type		The plug-in type
		 *\param[in]	library	The shared library holding the plug-in
		 *\param[in]	engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	type		Le type du plug-in
		 *\param[in]	library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	engine	Le moteur
		 */
		C3D_API Plugin( PluginType type
			, DynamicLibraryUPtr library
			, Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Plugin()noexcept;
		/**
		 *\~english
		 *\brief		Retrieves the required version for the plug-in to work correctly
		 *\param[out]	version	Receives the version.
		 *\~french
		 *\brief		Récupère la version nécessaire au bon fonctionnement du plug-in
		 *\param[out]	version	Reçoit la version.
		 */
		C3D_API void getRequiredVersion( Version & version )const;
		/**
		 *\~english
		 *\brief		Retrieves the plug-in name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom du plug-in
		 *\return		Le nom
		 */
		C3D_API String getName()const;
		/**
		 *\~english
		 *\brief		Retrieves the plug-in type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type du plug-in
		 *\return		Le type
		 */
		PluginType getType()const
		{
			return m_type;
		}

	protected:
		/**
		 *\~english
		 *\brief		Calls load function for the plug-in.
		 *\~french
		 *\brief		Exécute la fonction de chargement du plug-in.
		 */
		void load();
		/**
		 *\~english
		 *\brief		Calls unload function for the plug-in.
		 *\~french
		 *\brief		Exécute la fonction de déchargement du plug-in.
		 */
		void unload()const noexcept;

	private:
		DynamicLibraryUPtr m_library{};
		GetRequiredVersionFunction m_pfnGetRequiredVersion{};
		GetNameFunction m_pfnGetName{};
		OnLoadFunction m_pfnOnLoad{};
		OnUnloadFunction m_pfnOnUnload{};
		PluginType m_type{};
	};
}

#endif

