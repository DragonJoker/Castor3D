/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GenericPlugin_H___
#define ___C3D_GenericPlugin_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Plugin/Plugin.hpp"
#include "Castor3D/Plugin/PluginException.hpp"

namespace castor3d
{
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

