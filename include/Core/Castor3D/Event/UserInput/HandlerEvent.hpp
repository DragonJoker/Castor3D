/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HandlerEvent_H___
#define ___C3D_HandlerEvent_H___

#include "Castor3D/Event/UserInput/UserInputEvent.hpp"

namespace castor3d
{
	class HandlerEvent
		: public UserInputEvent
	{
	public:
		/**
		 *\~english
		 *\~brief		Constructor.
		 *\param[in]	type	The handler event type.
		 *\param[in]	handler	A handler.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	type	Le type d'évènement de gestionnaire.
		 *\param[in]	handler	Un gestionnaire.
		 */
		HandlerEvent( HandlerEventType type
			, EventHandlerRPtr handler )
			: UserInputEvent{ UserInputEventType::eHandler }
			, m_handlerEventType{ type }
			, m_handler{ handler }
		{
		}
		/**
		 *\~english
		 *\return		The handler event type.
		 *\~french
		 *\return		Le type d'évènement de gestionnaire.
		 */
		HandlerEventType getHandlerEventType()const
		{
			return m_handlerEventType;
		}
		/**
		 *\~english
		 *\return		The handler.
		 *\~french
		 *\return		Le gestionnaire.
		 */
		EventHandlerRPtr getHandler()const
		{
			return m_handler;
		}

	private:
		//!\~english	The handler event type.
		//!\~french		Le type d'évènement de gestionnaire.
		HandlerEventType const m_handlerEventType;
		//!\~english	The handler.
		//!\~french		Le gestionnaire.
		EventHandlerRPtr const m_handler;
	};
}

#endif
