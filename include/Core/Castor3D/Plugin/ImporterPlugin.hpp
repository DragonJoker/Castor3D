/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ImporterPlugin_H___
#define ___C3D_ImporterPlugin_H___

#include "Castor3D/Plugin/Plugin.hpp"

namespace castor3d
{
	class ImporterPlugin
		: public Plugin
	{
	public:
		typedef std::pair< castor::String, castor::String > Extension;
		CU_DeclareVector( Extension, Extension );

	private:
		friend class Plugin;
		friend class Engine;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	library	The shared library holding the plug-in
		 *\param[in]	engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	engine	Le moteur
		 */
		C3D_API ImporterPlugin( castor::DynamicLibrarySPtr library
			, Engine * engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ImporterPlugin()override;
		/**
		 *\~english
		 *\brief		Retrieves the supported file extensions array
		 *\return		The supported extensions array
		 *\~french
		 *\brief		Récupère le tableau des extensions supportées
		 *\return		Le tableau d'extensions supportées
		 */
		C3D_API ExtensionArray const & getExtensions();
		/**
		 *\~english
		 *\brief		adds a supported extension.
		 *\param[in]	extension	The extension.
		 *\~french
		 *\brief		Ajoute une extension supportée.
		 *\param[in]	extension	L'extension.
		 */
		C3D_API void addExtension( Extension const & extension );

	private:
		ExtensionArray m_extensions;
	};
}

#endif
