/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_POST_FX_PLUGIN_H___
#define ___C3D_POST_FX_PLUGIN_H___

#include "Plugin.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		19/08/2012
	\~english
	\brief		Post effect plugin class
	\~french
	\brief		Classe de plugin d'effets post rendu
	*/
	class C3D_API PostFxPlugin
		: public PluginBase
	{
	private:
		friend class PluginBase;
		typedef PostEffectSPtr	CreateEffectFunction( RenderSystem * p_renderSystem );

		typedef CreateEffectFunction * PCreateEffectFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 *\param[in]	p_engine	Le moteur
		 */
		PostFxPlugin( Castor::DynamicLibrarySPtr p_pLibrary, Engine * p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PostFxPlugin();
		/**
		 *\~english
		 *\brief		Creates a PostEffect object
		 *\param[in]	p_renderSystem	The render system
		 *\return		The created PostEffect instance
		 *\~french
		 *\brief		Crée un objet PostEffect
		 *\param[in]	p_renderSystem	Le render system
		 *\return		L'instance de PostEffect créée
		 */
		PostEffectSPtr CreateEffect( RenderSystem * p_renderSystem );

	private:
		PCreateEffectFunction m_pfnCreateEffect;
	};
}

#endif

