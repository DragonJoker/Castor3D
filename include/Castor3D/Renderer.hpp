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
#ifndef ___C3D_Renderer___
#define ___C3D_Renderer___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		29/10/2010
	\~english
	\brief		A renderer template class, holds the object it renders
	\remark		_Renderable must be the rendered class (which must derivate from Renderable)
				<br />_Renderer is the child class
	\~french
	\brief		Classe de modèle pour les renderers, contient le renderable
	\remark		_Renderable Doit être la classe rendue (doit étendre Renderable)
				<br />_Renderer est la classe fille
	*/
	template< class TRenderable, class TRenderer >
	class C3D_API Renderer
	{
	protected:
		typedef TRenderable * target_ptr;
		target_ptr		m_target;
		RenderSystem *	m_pRenderSystem;

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Constructor, cannot be used 'as if'. You must derivate a class from this one to be able to create a Renderer
		 *\~french
		 *\brief		Constructeur
		 *\remark		Protégé afin de ne pouvoir instancier cette classe qu'au travers d'une classe fille
		 */
		Renderer( RenderSystem * p_pRenderSystem )
			:	m_target		( nullptr			)
			,	m_pRenderSystem	( p_pRenderSystem	)
		{
		}
	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Renderer()
		{
		}
		/**
		 *\~english
		 *\brief		Retrieves the RenderSystem
		 *\return		The RenderSystem
		 *\~french
		 *\brief		Récupère le RenderSystem
		 *\return		Le RenderSystem
		 */
		virtual RenderSystem * GetRenderSystem()const { return m_pRenderSystem; }
		/**
		 *\~english
		 *\brief		Retrieves the Renderable
		 *\return		The Renderable
		 *\~french
		 *\brief		Récupère le Renderable
		 *\return		Le Renderable
		 */
		virtual target_ptr GetTarget()const { return m_target; }
		/**
		 *\~english
		 *\brief		Sets the Renderable
		 *\param[in]	p_target	The new Renderable
		 *\~french
		 *\brief		Définit le Renderable
		 *\param[in]	p_target	Le nouveau Renderable
		 */
		virtual void SetTarget( target_ptr p_target ) { m_target = p_target; }
	};
}

#pragma warning( pop )

#endif
