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
#ifndef ___C3D_SUBDIVISION_FRAME_EVENT_H___
#define ___C3D_SUBDIVISION_FRAME_EVENT_H___

#include "FrameEvent.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		11/01/2011
	\~english
	\brief		Threaded Subdivision Frame event
	\remark		At the end of the subdivision, buffers of a submesh may be generated, it happens in this event
	\~french
	\brief		Threaded Subdivision Frame event
	\remark		A la fin d'une subdivision de submesh, les tampons peuvent être regénérés, cette action est effectuée dans cet évènement
	*/
	class SubdivisionFrameEvent
		: public FrameEvent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pSubmesh	The subdivision target
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pSubmesh	La cible de la subdivision
		 */
		C3D_API SubdivisionFrameEvent( SubmeshSPtr p_pSubmesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~SubdivisionFrameEvent();
		/**
		 *\~english
		 *\brief		Applies the event : computes normals, generates buffers, initialises the renderer
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement : génère les normales et les tampons, initialise le renderer
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		C3D_API virtual bool Apply();

	private:
		//!\~english The subdivision target	\~french La cible de la subdivision
		SubmeshSPtr m_pSubmesh;
	};
}

#endif
