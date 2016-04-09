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
#ifndef ___CI_CONTROL_EVENT_H___
#define ___CI_CONTROL_EVENT_H___

#include "GuiEvent.hpp"

namespace CastorGui
{
	/*!
	 *\author		Sylvain DOREMUS
	 *\version		0.1.0
	 *\brief		Description of a control event
	 */
	class ControlEvent
		: public GuiEvent
	{
	public:
		/** Constructor
		 *\param[in]	p_type		The control event type
		 *\param[in]	p_ctrl		A control (depends on the event type)
		 */
		ControlEvent( eCONTROL_EVENT p_type, ControlSPtr p_ctrl )
			: GuiEvent( eEVENT_TYPE_CONTROL )
			, m_controlEventType( p_type )
			, m_ctrl( p_ctrl )
		{
		}

		/** Destructor
		 */
		virtual ~ControlEvent()
		{
		}

		/** Retrieves the event type
		 *\return		The type
		 */
		inline eCONTROL_EVENT GetControlEventType()const
		{
			return m_controlEventType;
		}

		/** Retrieves the key code
		 *\return		The key
		 */
		inline ControlSPtr GetControl()const
		{
			return m_ctrl.lock();
		}

	private:
		//! The event type
		eCONTROL_EVENT m_controlEventType;
		//! The control
		ControlWPtr m_ctrl;
	};
}

#endif
