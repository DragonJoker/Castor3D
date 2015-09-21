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
#ifndef ___C3D_POST_EFFECT_H___
#define ___C3D_POST_EFFECT_H___

#include "Castor3DPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		20/11/2012
	\~english
	\brief		Post render effect base class
	\remark		A post render effect is an effect applied after 3D rendering and before 2D rendering
				<br />Post render effects are applied in a cumulative way
	\~french
	\brief		Classe de base d'effet post rendu
	\remark		Une effet post rendu est un effet appliqué après le rendu 3D et avant le rendu 2D
				<br />Les effets post rendu sont appliqués de manière cumulative
	*/
	class C3D_API PostEffect
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		PostEffect( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PostEffect();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\return		\p true if ok
		 */
		virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Render function, applies the effect to a render target
		 *\param[in]	p_renderTarget	The render target for this technique
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de rendu, applique l'effet sur une render target
		 *\param[in]	p_renderTarget	La render target pour cette technique
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Apply( RenderTarget * p_pRenderTarget ) = 0;

	protected:
		//!\~english The	render system	\~french Le render system
		RenderSystem *	m_renderSystem;
	};
}

#pragma warning( pop )

#endif
