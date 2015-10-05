/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (At your option ) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CI_GUI_EVENT_H___
#define ___CI_GUI_EVENT_H___

#include "CastorGuiPrerequisites.hpp"

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMU
	 *\version		0.1.
	 *\brief		Description of an event
		You may use this one, but prefer using ControlEvent, MouseEvent or KeyboardEvent.
	*/
	class GuiEvent
	{
	public:
		/** Constructor
		*\param[in]	p_type		The event typ
		*/
		GuiEvent( eEVENT_TYPE p_type )
			: m_eventType( p_type )
		{
		}

		/** Destructor
		*/
		virtual ~GuiEvent()
		{
		}

		/** Retrieves the event type
		 *\return		The typ
		*/
		inline eEVENT_TYPE GetEventType()const
		{
			return m_eventType;
		}

	private:
		//! Event type
		eEVENT_TYPE m_eventType;
	};
}

#endif
