/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DividerPlugin_H___
#define ___C3D_DividerPlugin_H___

#include "Castor3D/Plugin/Plugin.hpp"

namespace castor3d
{
	class DividerPlugin
		: public Plugin
	{
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
		C3D_API DividerPlugin( castor::DynamicLibrarySPtr library
			, Engine * engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~DividerPlugin();
	};
}

#endif

