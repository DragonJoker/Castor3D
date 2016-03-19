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

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		20/11/2012
	\~english
	\brief		Post render effect base class.
	\remark		A post render effect is an effect applied after 3D rendering and before 2D rendering.
				<br />Post render effects are applied in a cumulative way.
	\~french
	\brief		Classe de base d'effet post rendu.
	\remark		Une effet post rendu est un effet appliqué après le rendu 3D et avant le rendu 2D.
				<br />Les effets post rendu sont appliqués de manière cumulative.
	*/
	class PostEffect
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_renderTarget	The render target to which is attached this effect.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_renderTarget	La cible de rendu à laquelle est affectée cet effet.
		 */
		C3D_API PostEffect( RenderSystem * p_renderSystem, RenderTarget & p_renderTarget, Parameters const & p_param );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~PostEffect();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Render function, applies the effect to the given framebuffer.
		 *\param[in,out]p_framebuffer	The framebuffer.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction de rendu, applique l'effet au tampon d'image donné.
		 *\param[in,out]p_framebuffer	Le tampon d'image.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool Apply( FrameBuffer & p_framebuffer ) = 0;

	protected:
		//!\~english The render system.	\~french Le render system.
		RenderSystem * m_renderSystem;
		//!\~english The render target to which this effect is attached.	\~french La cible de rendu à laquelle est attachée cet effet.
		RenderTarget & m_renderTarget;
	};
}

#endif
