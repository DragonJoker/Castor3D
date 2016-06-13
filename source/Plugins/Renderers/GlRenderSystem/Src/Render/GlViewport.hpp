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
#ifndef ___GL_VIEWPORT_H___
#define ___GL_VIEWPORT_H___

#include "Common/GlHolder.hpp"

#include <Render/Viewport.hpp>

namespace GlRender
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		16/04/2016
	\~english
	\brief		Render API specific viewport implementation.
	\~french
	\brief		Implémentation de viewport spécifique à l'API de rendu.
	*/
	class GlViewport
		: public Castor3D::IViewportImpl
		, public Holder
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_viewport		The parent viewport.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_viewport		Le viewport parent.
		 */
		GlViewport( GlRenderSystem & p_renderSystem, Castor3D::Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~GlViewport();
		/**
		 *\copydoc		Castor3D::IViewportImpl::Apply
		 */
		virtual bool Apply();
	};
}

#endif
