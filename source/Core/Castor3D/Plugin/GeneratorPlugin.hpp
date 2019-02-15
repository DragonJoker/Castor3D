/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GENERATOR_PLUGIN_H___
#define ___C3D_GENERATOR_PLUGIN_H___

#include "Plugin.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Generator Plugin class
	\~french
	\brief		Classe de plug-in de générateur procédural
	*/
	class GeneratorPlugin
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
		C3D_API GeneratorPlugin( castor::DynamicLibrarySPtr library
			, Engine * engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~GeneratorPlugin();
	};
}

#endif

