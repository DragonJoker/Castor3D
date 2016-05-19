/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_HANDLER_EVENT_H___
#define ___C3D_HANDLER_EVENT_H___

#include "UserInputEvent.hpp"

namespace Castor3D
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Description of a handler event.
	*\~french
	*\brief		Description d'un évènement de gestionnaire.
	*/
	class HandlerEvent
		: public UserInputEvent
	{
	public:
		/**
		 *\~english
		 *\~brief		Constructor.
		 *\param[in]	p_type		The handler event type.
		 *\param[in]	p_handler	A handler.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	p_type		Le type d'évènement de gestionnaire.
		 *\param[in]	p_handler	Un gestionnaire.
		 */
		HandlerEvent( eHANDLER_EVENT p_type, EventHandlerSPtr p_handler )
			: UserInputEvent{ eEVENT_TYPE_HANDLER }
			, m_handlerEventType{ p_type }
			, m_handler{ p_handler }
		{
		}
		/**
		 *\~english
		 *\~brief		Destructor.
		 *\~french
		 *\~brief		Destructeur.
		 */
		virtual ~HandlerEvent()
		{
		}
		/**
		 *\~english
		 *\return		The handler event type.
		 *\~french
		 *\return		Le type d'évènement de gestionnaire.
		 */
		inline eHANDLER_EVENT GetHandlerEventType()const
		{
			return m_handlerEventType;
		}
		/**
		 *\~english
		 *\return		The handler.
		 *\~french
		 *\return		Le gestionnaire.
		 */
		inline EventHandlerSPtr GetHandler()const
		{
			REQUIRE( !m_handler.expired() );
			return m_handler.lock();
		}

	private:
		//!\~english The handler event type. \~french Le type d'évènement de gestionnaire.
		eHANDLER_EVENT const m_handlerEventType;
		//!\~english The handler.	\~french Le gestionnaire.
		EventHandlerWPtr const m_handler;
	};
}

#endif
