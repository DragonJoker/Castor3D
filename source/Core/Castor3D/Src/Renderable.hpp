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
#ifndef ___C3D_RENDERABLE_H___
#define ___C3D_RENDERABLE_H___

#include "Castor3DPrerequisites.hpp"
#include "Renderer.hpp"
#include "RenderSystem.hpp"
#include "Engine.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		29/10/2010
	\~english
	\brief		The renderable base class.
	\remark		A renderable is an object which can be rendered through a renderer
				<br />This is a helper class to delegate the creation of the renderer
				<br />\p _Renderable must be the child class
				<br />\p _Renderer is the class that will render the child class
	\~french
	\brief		La classe de base des objets "rendables"
	\remark		Un renderable est un objet pouvant être rendu au travers d'un Renderer
				<br />Il s'agit là d'un classe d'aide pour déléguer la création du Renderer
				<br />\p _Renderable doit être la classe fille
				<br />\p _Renderer est la classe du renderer de la classe fille
	*/
	template< class TRenderable, class TRenderer >
	class C3D_API Renderable
	{
	private:
		typedef typename std::weak_ptr<		TRenderer > renderer_wptr;
		typedef typename std::shared_ptr<	TRenderer > renderer_sptr;

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Constructor, cannot be used 'as if'. You must derivate a class from this one to be able to create a Renderable
		 *\param[in]	p_pEngine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\remark		Protégé afin de ne pouvoir instancier cette classe qu'au travers d'une classe fille
		 *\param[in]	p_pEngine	Le moteur
		 */
		Renderable( Engine * p_pEngine )
			:	m_pEngine( p_pEngine )
		{
		}
		/**
		 *\~english
		 *\brief		Creates the given renderable's renderer
		 *\param[in]	p_this	The renderable
		 *\~french
		 *\brief		Crée le renderer du renderable donné
		 *\param[in]	p_this	Le renderable
		 */
		void DoCreateRenderer( TRenderable * p_this )
		{
			renderer_sptr l_pRenderer = m_pEngine->GetRenderSystem()->CreateRenderer< TRenderer >();

			if ( l_pRenderer )
			{
				l_pRenderer->SetTarget( p_this );
				m_pRenderer = l_pRenderer;
			}
		}

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Renderable()
		{
		}
		/**
		 *\~english
		 *\brief		Main render function, to be implemented by every derivated class
		 *\~french
		 *\brief		Fonction de rendu, à implémenter par les classes filles
		 */
		virtual void Render()
		{
		}
		/**
		 *\~english
		 *\brief		Remove from render function.
		 *\remark		Base implementation is dummy, so derivated classes may or not implement a new behaviour for it
		 *\~french
		 *\brief		Fonction de fin de rendu.
		 *\remark		L'implémentation de base ne fait rien, ainsi les classes filles peuvent ne pas l'implémenter
		 */
		virtual void EndRender()
		{
		}
		/**
		 *\~english
		 *\brief		2D render function
		 *\remark		Base implementation is dummy, so derivated classes may or not implement a new behaviour for it
		 *\~english
		 *\brief		Fonction de rendu 2D
		 *\remark		L'implémentation de base ne fait rien, ainsi les classes filles peuvent ne pas l'implémenter
		 */
		virtual void Render2D()
		{
		}
		/**
		 *\~english
		 *\brief		Retrieves the Renderer
		 *\~french
		 *\brief		Récupère le Renderer
		 */
		inline renderer_sptr GetRenderer()const
		{
			return m_pRenderer.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the Engine
		 *\~french
		 *\brief		Récupère l'Engine
		 */
		virtual Engine * GetEngine()const
		{
			return m_pEngine;
		}

	protected:
		//!\~english The renderer	\~french Le renderer
		renderer_wptr m_pRenderer;
		//!\~english The core engine	\~french Le moteur
		Engine * m_pEngine;
	};
}

#pragma warning( pop )

#endif
