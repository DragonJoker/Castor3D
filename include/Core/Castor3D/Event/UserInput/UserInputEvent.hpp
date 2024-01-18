/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UserInputEvent_H___
#define ___C3D_UserInputEvent_H___

#include "UserInputEventModule.hpp"

namespace castor3d
{
	/**
	*\~english
	*\brief		Description of a user input event.
	*\remarks	You may use this one, but prefer using MouseEvent or KeyboardEvent.
	*\~french
	*\brief		Description d'un évènement de tpye entrée utilisateur.
	*\remarks	Vous pouvez utiliser cette classe, mais il est conseillé d'utiliser les  classes MouseEvent ou KeyboardEvent.
	*/
	class UserInputEvent
	{
	public:
		C3D_API UserInputEvent( UserInputEvent && ) = default;
		C3D_API UserInputEvent( UserInputEvent const & ) = default;
		C3D_API UserInputEvent & operator=( UserInputEvent && ) = default;
		C3D_API UserInputEvent & operator=( UserInputEvent const & ) = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type	The user input event type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type	Le type d'évènement entrée utilisateur.
		 */
		explicit UserInputEvent( UserInputEventType type )
			: m_eventType( type )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~UserInputEvent()noexcept = default;
		/**
		 *\~english
		 *\return		The user input event type.
		 *\~french
		 *\return		Le type d'évènement entrée utilisateur.
		 */
		UserInputEventType getEventType()const
		{
			return m_eventType;
		}

	private:
		UserInputEventType m_eventType;
	};
}

#endif
