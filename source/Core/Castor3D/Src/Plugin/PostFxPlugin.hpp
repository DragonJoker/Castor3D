/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
	\brief		Post effect plug-in class
	\~french
	\brief		Classe de plug-in d'effets post rendu
	*/
	class PostFxPlugin
		: public PluginBase
	{
	private:
		friend class PluginBase;
		typedef PostEffectSPtr CreateEffectFunction( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_params );
		typedef Castor::String GetPostEffectTypeFunction();

		typedef CreateEffectFunction * PCreateEffectFunction;
		typedef GetPostEffectTypeFunction * PGetPostEffectTypeFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_library	The shared library holding the plug-in
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_library	La bibliothèque partagée contenant le plug-in
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API PostFxPlugin( Castor::DynamicLibrarySPtr p_library, Engine * p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~PostFxPlugin();
		/**
		 *\~english
		 *\brief		Creates a PostEffect object.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_renderTarget	The render target to which is attached the effect.
		 *\param[in]	p_params		The optional parameters.
		 *\return		The created PostEffect instance.
		 *\~french
		 *\brief		Crée un objet PostEffect.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_renderTarget	La cible de rendu sur laquelle l'effet s'applique.
		 *\param[in]	p_params		Les paramètres optionnels.
		 *\return		L'instance de PostEffect créée.
		 */
		C3D_API PostEffectSPtr CreateEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_params );
		/**
		 *\~english
		 *\brief		Tells the effect short name
		 *\~french
		 *\brief		Donne le nom court de l'effet
		 */
		C3D_API Castor::String GetPostEffectType();

	private:
		PCreateEffectFunction m_pfnCreateEffect;
		PGetPostEffectTypeFunction m_pfnGetPostEffectType;
	};
}

#endif

