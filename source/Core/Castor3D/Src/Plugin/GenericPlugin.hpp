/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GENERIC_PLUGIN_H___
#define ___C3D_GENERIC_PLUGIN_H___

#include "Plugin.hpp"
#include "PluginException.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		21/09/2015
	\~english
	\brief		Generic Plugin class.
	\remarks	Allows general specialisations for Castor3D.
	\~french
	\brief		Classe de plug-in générique.
	\remarks	Permet des spécialisation générales pour Castor3D.
	*/
	class GenericPlugin
		: public Plugin
	{
	private:
		friend class Plugin;
		friend class Engine;
		typedef void addOptionalParsersFunction( SceneFileParser * parser );

		typedef addOptionalParsersFunction * PAddOptionalParsersFunction;

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
		C3D_API GenericPlugin( castor::DynamicLibrarySPtr library
			, Engine * engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~GenericPlugin();

	private:
		PAddOptionalParsersFunction m_pfnAddOptionalParsers;
	};
}

#endif

