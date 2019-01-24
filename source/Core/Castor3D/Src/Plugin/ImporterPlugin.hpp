/*
See LICENSE file in root folder
*/
#ifndef ___C3D_IMPORTER_PLUGIN_H___
#define ___C3D_IMPORTER_PLUGIN_H___

#include "Plugin.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Importer Plugin class
	\~french
	\brief		Classe de plug-in d'import
	*/
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
		 *\param[in]	p_library	The shared library holding the plug-in
		 *\param[in]	engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	engine	Le moteur
		 */
		C3D_API ImporterPlugin( castor::DynamicLibrarySPtr p_library, Engine * engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ImporterPlugin();
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
		 *\param[in]	p_extension	The extension.
		 *\~french
		 *\brief		Ajoute une extension supportée.
		 *\param[in]	p_extension	L'extension.
		 */
		C3D_API void addExtension( Extension const & p_extension );

	private:
		ExtensionArray m_extensions;
	};
}

#endif
