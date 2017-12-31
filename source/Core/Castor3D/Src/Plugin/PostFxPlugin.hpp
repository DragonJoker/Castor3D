/*
See LICENSE file in root folder
*/
#ifndef ___C3D_POST_FX_PLUGIN_H___
#define ___C3D_POST_FX_PLUGIN_H___

#include "Plugin.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		19/08/2012
	\~english
	\brief		Post effect plug-in class
	\~french
	\brief		Classe de plug-in d'effets post rendu
	*/
	class PostFxPlugin
		: public Plugin
	{
	private:
		friend class Plugin;

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
		C3D_API PostFxPlugin( castor::DynamicLibrarySPtr p_library, Engine * engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~PostFxPlugin();
	};
}

#endif

