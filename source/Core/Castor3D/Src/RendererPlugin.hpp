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
	\brief		Classe de plugin de rendu
	*/
	class C3D_API RendererPlugin
		: public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;

		typedef RenderSystem *	CreateRenderSystemFunction( Engine * p_pEngine, Castor::Logger * p_pLogger );
		typedef void			DestroyRenderSystemFunction( RenderSystem * p_pRenderSystem );
		typedef eRENDERER_TYPE	GetRendererTypeFunction();

		typedef CreateRenderSystemFunction	*	PCreateRenderSystemFunction;
		typedef DestroyRenderSystemFunction	*	PDestroyRenderSystemFunction;
		typedef GetRendererTypeFunction		*	PGetRendererTypeFunction;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		RendererPlugin( Castor::DynamicLibrarySPtr p_pLibrary );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RendererPlugin();
		/**
		 *\~english
		 *\brief		Creates the RenderSystem
		 *\param[in]	p_pEngine	The core engine
		 *\param[in]	p_pLogger	The logger instance
		 *\return		The created RenderSystem instance
		 *\~french
		 *\brief		Crée le RenderSystem
		 *\param[in]	p_pEngine	Le moteur
		 *\param[in]	p_pLogger	L'instance de logger
		 *\return		L'instance de RenderSystem créée
		 */
		RenderSystem * CreateRenderSystem( Engine * p_pEngine, Castor::Logger * p_pLogger );
		/**
		 *\~english
		 *\brief		Destroys the given RenderSystem
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\~french
		 *\brief		Détruit le RenderSystem donné
		 *\param[in]	p_pRenderSystem	Le RenderSystem
		 */
		void DestroyRenderSystem( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Retrieves the renderer type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type de renderer
		 *\return		Le type
		 */
		eRENDERER_TYPE GetRendererType();

	private:
		PCreateRenderSystemFunction		m_pfnCreateRenderSystem;
		PDestroyRenderSystemFunction	m_pfnDestroyRenderSystem;
		PGetRendererTypeFunction		m_pfnGetRendererType;
	};
}

#endif

