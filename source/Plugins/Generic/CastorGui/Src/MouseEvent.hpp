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
#ifndef ___CI_MOUSE_EVENT_H___
#define ___CI_MOUSE_EVENT_H___

#include "GuiEvent.hpp"

#include <Position.hpp>

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMU
	 *\date		02/03/201
	 *\version		0.1.
	 *\brief		Structure holding mouse stat
	*/
	struct MouseState
	{
		//! The position
		Castor::Position m_position;
		//! The wheel position
		Castor::Position m_wheel;
		//! The buttons state (true = down)
		bool m_buttons[eMOUSE_BUTTON_COUNT];
		//! The button which had the last change
		eMOUSE_BUTTON m_changed;
	};
	/*!
	 *\author		Sylvain DOREMU
	 *\version		0.1.
	 *\brief		Description of a mouse even
	*/
	class MouseEvent
		: public GuiEvent
	{
	public:
		/** Constructor
		*\param[in]	p_type		The mouse event typ
		*\param[in]	p_position		The position, of wheel or mouse, depending on the event typ
		*\param[in]	p_button		The mouse butto
		*/
		MouseEvent( eMOUSE_EVENT p_type, Castor::Position const & p_position, eMOUSE_BUTTON p_button = eMOUSE_BUTTON_COUNT )
			: GuiEvent( eEVENT_TYPE_MOUSE )
			, m_mouseEventType( p_type )
			, m_position( p_position )
			, m_button( p_button )
		{
		}

		/** Destructor
		*/
		virtual ~MouseEvent()
		{
		}

		/** Retrieves the mouse event type
		 *\return		The typ
		*/
		inline eMOUSE_EVENT GetMouseEventType()const
		{
			return m_mouseEventType;
		}

		/** Retrieves the mouse button
		 *\return		The butto
		*/
		inline eMOUSE_BUTTON GetButton()const
		{
			return m_button;
		}

		/** Retrieves the position (wheel or mouse, depending on mouse event type)
		 *\return		The positio
		*/
		inline Castor::Position const & GetPosition()const
		{
			return m_position;
		}

	private:
		//! The mouse event type
		eMOUSE_EVENT m_mouseEventType;
		//! The mouse button
		eMOUSE_BUTTON m_button;
		//! The position, (wheel or mouse, depending on mouse event type)
		Castor::Position m_position;
	};
}

#endif
