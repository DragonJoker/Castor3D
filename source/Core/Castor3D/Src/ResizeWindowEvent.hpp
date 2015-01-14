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
#ifndef ___C3D_RESIZE_WINDOW_EVENT_H___
#define ___C3D_RESIZE_WINDOW_EVENT_H___

#include "FrameEvent.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/04/2013
	\version	0.7.0
	\~english
	\brief		Event used to resize a RenderWindow before rendering it
	\~french
	\brief		Evènement utilisé pour redimensionner une RenderWindow avant son rendu
	*/
	class C3D_API ResizeWindowEvent
		:	public FrameEvent
	{
	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		ResizeWindowEvent( ResizeWindowEvent const & p_copy );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		ResizeWindowEvent & operator=( ResizeWindowEvent const & p_copy );

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_window	The RenderWindow to resize
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_window	La RenderWindow à redimensionner
		 */
		ResizeWindowEvent( RenderWindow & p_window );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~ResizeWindowEvent();
		/**
		 *\~english
		 *\brief		Applies the event : resizes the window
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement : redimensionne la fenêtre
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		virtual bool Apply();

	private:
		//!\~english The RenderWindow to resize	\~french La RenderWindow à redimensionner
		RenderWindow & m_window;
	};
}

#pragma warning( pop )

#endif
