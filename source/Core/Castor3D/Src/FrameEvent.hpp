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
#ifndef ___C3D_FRAME_EVENT_H___
#define ___C3D_FRAME_EVENT_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		03/03/2010
	\version	0.1
	\~english
	\brief		The interface which represents a frame event.
	\remark		Basically a frame event has a eEVENT_TYPE to know when it must be applied.
				<br />It can be applied, so the function must be implemented by children classes
	\~french
	\brief		Interface représentant un évènement de frame
	\remark		Un évènement a un eEVENT_TYPE pour savoir quand il doit être traité.
				<br />La fonction de traitement doit être implémentée par les classes filles.
	*/
	class C3D_API FrameEvent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType	The event type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType	Le type d'évènement
		 */
		FrameEvent( eEVENT_TYPE p_eType );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		FrameEvent( FrameEvent const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		FrameEvent( FrameEvent && p_object );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_object	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_object	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		FrameEvent & operator =( FrameEvent const & p_object );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_object	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		FrameEvent & operator =( FrameEvent && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~FrameEvent();
		/**
		 *\~english
		 *\brief		Applies the event
		 *\remark		Must be implemented by children classes
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement
		 *\remark		Doit être implémentée dans les classes filles
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		virtual bool Apply() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the event type
		 *\return		The event type
		 *\~french
		 *\brief		Récupère le type de l'évènement
		 *\return		Le type de l'évènement
		 */
		inline eEVENT_TYPE GetType()
		{
			return m_eType;
		}

	protected:
		//!\~english The event type	\~french Le type d'évènement
		eEVENT_TYPE m_eType;
	};
}

#endif
