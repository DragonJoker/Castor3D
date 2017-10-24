/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HANDLER_EVENT_H___
#define ___C3D_HANDLER_EVENT_H___

#include "UserInputEvent.hpp"

namespace castor3d
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
		HandlerEvent( HandlerEventType p_type, EventHandlerSPtr p_handler )
			: UserInputEvent{ UserInputEventType::eHandler }
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
		inline HandlerEventType getHandlerEventType()const
		{
			return m_handlerEventType;
		}
		/**
		 *\~english
		 *\return		The handler.
		 *\~french
		 *\return		Le gestionnaire.
		 */
		inline EventHandlerSPtr getHandler()const
		{
			REQUIRE( !m_handler.expired() );
			return m_handler.lock();
		}

	private:
		//!\~english The handler event type. \~french Le type d'évènement de gestionnaire.
		HandlerEventType const m_handlerEventType;
		//!\~english The handler.	\~french Le gestionnaire.
		EventHandlerWPtr const m_handler;
	};
}

#endif
