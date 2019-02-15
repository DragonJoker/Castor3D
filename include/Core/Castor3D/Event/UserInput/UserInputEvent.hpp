/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GUI_EVENT_H___
#define ___C3D_GUI_EVENT_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Description of a user input event.
	*\remarks	You may use this one, but prefer using MouseEvent or KeyboardEvent.
	*\~french
	*\brief		Description d'un évènement de tpye entràe utilisateur.
	*\remarks	Vous pouvez utiliser cette classe, mais il est conseillà d'utiliser les  classes MouseEvent ou KeyboardEvent.
	*/
	class UserInputEvent
	{
	public:
		/**
		 *\~english
		 *\~brief		Constructor.
		 *\param[in]	type	The user input event type.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	type	Le type d'évènement entràe utilisateur.
		 */
		explicit UserInputEvent( UserInputEventType type )
			: m_eventType( type )
		{
		}
		/**
		 *\~english
		 *\~brief		Destructor.
		 *\~french
		 *\~brief		Destructeur.
		 */
		virtual ~UserInputEvent()
		{
		}
		/**
		 *\~english
		 *\return		The user input event type.
		 *\~french
		 *\return		Le type d'évènement entràe utilisateur.
		 */
		inline UserInputEventType getEventType()const
		{
			return m_eventType;
		}

	private:
		//!\~english The user input event type.	\~french Le type d'évènement entràe utilisateur.
		UserInputEventType m_eventType;
	};
}

#endif
