/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TONE_MAPPING_PLUGIN_H___
#define ___C3D_TONE_MAPPING_PLUGIN_H___

#include "Plugin.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		01/07/2016
	\~english
	\brief		Tone mapping plug-in class.
	\~french
	\brief		Classe de plug-in de mappage de tons.
	*/
	class ToneMappingPlugin
		: public Plugin
	{
	private:
		friend class Plugin;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	library	The shared library holding the plug-in.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	library	La bibliothèque partagée contenant le plug-in.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API ToneMappingPlugin( castor::DynamicLibrarySPtr library
			, Engine * engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ToneMappingPlugin();
	};
}

#endif

