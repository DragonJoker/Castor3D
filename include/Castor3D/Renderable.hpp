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
#ifndef ___C3D_Renderable___
#define ___C3D_Renderable___

#include "Prerequisites.hpp"
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
		//!\~english	The renderer	\~french	Le renderer
		renderer_wptr m_pRenderer;
		//!\~english	The core engine	\~french	Le moteur
		Engine * m_pEngine;

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Constructor, cannot be used 'as if'. You must derivate a class from this one to be able to create a Renderable
		 *\~french
		 *\brief		Constructeur
		 *\remark		Protégé afin de ne pouvoir instancier cette classe qu'au travers d'une classe fille
		 */
		Renderable( Engine * p_pEngine )
			:	m_pEngine	( p_pEngine	)
		{
			renderer_sptr l_pRenderer = m_pEngine->GetRenderSystem()->CreateRenderer< TRenderer >();

			if( l_pRenderer )
			{
				l_pRenderer->SetTarget( (TRenderable *)this );
				m_pRenderer = l_pRenderer;
			}
		}
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		Renderable( Renderable< TRenderable, TRenderer > const & p_copy )
			:	m_pEngine	( p_copy.m_pEngine		)
		{
			renderer_sptr l_pRenderer = m_pEngine->GetRenderSystem()->CreateRenderer< TRenderer >();

			if( l_pRenderer )
			{
				l_pRenderer->SetTarget( (TRenderable *)this );
				m_pRenderer = l_pRenderer;
			}
		}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_copy	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_copy	L'objet à déplacer
		 */
		Renderable( Renderable<TRenderable, TRenderer > && p_copy )
			:	m_pRenderer ( std::move( p_copy.m_pRenderer	) )
			,	m_pEngine	( std::move( p_copy.m_pEngine	) )
		{
			renderer_sptr l_pRenderer = m_pRenderer.lock();

			if( l_pRenderer )
			{
				l_pRenderer->SetTarget( (TRenderable *)this );
			}

			p_copy.m_pRenderer.reset();
			p_copy.m_pEngine = NULL;
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_copy	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_copy	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		Renderable & operator =( Renderable<TRenderable, TRenderer > const & p_copy )
		{
			m_pEngine	= p_copy.m_pEngine;

			renderer_sptr l_pRenderer = m_pEngine->GetRenderSystem()->CreateRenderer< TRenderer >();

			if( l_pRenderer )
			{
				l_pRenderer->SetTarget( (TRenderable *)this );
				m_pRenderer = l_pRenderer;
			}

			return * this;
		}
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_copy	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_copy	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		Renderable & operator =( Renderable< TRenderable, TRenderer > && p_copy )
		{
			if( this != &p_copy )
			{
				m_pRenderer.reset();
				m_pEngine = NULL;

				m_pRenderer = std::move( p_copy.m_pRenderer	);
				m_pEngine	= std::move( p_copy.m_pEngine	);

				renderer_sptr l_pRenderer = m_pRenderer.lock();

				if( l_pRenderer )
				{
					l_pRenderer->SetTarget( (TRenderable *)this );
				}

				p_copy.m_pRenderer.reset();
				p_copy.m_pEngine = NULL;
			}

			return * this;
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
		virtual void Render(){}
		/**
		 *\~english
		 *\brief		Remove from render function.
		 *\remark		Base implementation is dummy, so derivated classes may or not implement a new behaviour for it
		 *\~french
		 *\brief		Fonction de fin de rendu.
		 *\remark		L'implémentation de base ne fait rien, ainsi les classes filles peuvent ne pas l'implémenter
		 */
		virtual void EndRender(){}
		/**
		 *\~english
		 *\brief		2D render function
		 *\remark		Base implementation is dummy, so derivated classes may or not implement a new behaviour for it
		 *\~english
		 *\brief		Fonction de rendu 2D
		 *\remark		L'implémentation de base ne fait rien, ainsi les classes filles peuvent ne pas l'implémenter
		 */
		virtual void Render2D(){}
		/**
		 *\~english
		 *\brief		Retrieves the Renderer
		 *\~french
		 *\brief		Récupère le Renderer
		 */
		inline renderer_sptr GetRenderer()const { return m_pRenderer.lock(); }
		/**
		 *\~english
		 *\brief		Retrieves the Engine
		 *\~french
		 *\brief		Récupère l'Engine
		 */
		virtual Engine * GetEngine()const { return m_pEngine; }
	};
}

#pragma warning( pop )

#endif
