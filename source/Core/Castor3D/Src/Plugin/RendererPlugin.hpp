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
#ifndef ___C3D_RENDERER_PLUGIN_H___
#define ___C3D_RENDERER_PLUGIN_H___

#include "Plugin.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Renderer Plugin class
	\~french
	\brief		Classe de plug-in de rendu
	*/
	class RendererPlugin
		: public Plugin
	{
	private:
		friend class Plugin;
		friend class Engine;

		typedef Castor::String GetRendererTypeFunction();
		typedef GetRendererTypeFunction * PGetRendererTypeFunction;

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
		C3D_API RendererPlugin( Castor::DynamicLibrarySPtr p_library, Engine * p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RendererPlugin();
		/**
		 *\~english
		 *\brief		Retrieves the renderer type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type de renderer
		 *\return		Le type
		 */
		C3D_API Castor::String GetRendererType();

	private:
		PGetRendererTypeFunction m_pfnGetRendererType;
	};
}

#endif

