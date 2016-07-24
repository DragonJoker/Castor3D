/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
	*\brief		Description d'un �v�nement de gestionnaire.
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
		 *\param[in]	p_type		Le type d'�v�nement de gestionnaire.
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
		 *\return		Le type d'�v�nement de gestionnaire.
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
		//!\~english The handler event type. \~french Le type d'�v�nement de gestionnaire.
		eHANDLER_EVENT const m_handlerEventType;
		//!\~english The handler.	\~french Le gestionnaire.
		EventHandlerWPtr const m_handler;
	};
}

#endif
