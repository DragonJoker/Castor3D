/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MouseEvent_H___
#define ___C3D_MouseEvent_H___

#include "Castor3D/Event/UserInput/UserInputEvent.hpp"

#include <CastorUtils/Graphics/Position.hpp>

namespace castor3d
{
	class MouseEvent
		: public UserInputEvent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type		The mouse event type.
		 *\param[in]	position	The position, of wheel or mouse, depending on the event type.
		 *\param[in]	button		The mouse button.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type		Le type d'évènement de souris.
		 *\param[in]	position	La position, de la souris ou de la molette, selon le type d'évènement.
		 *\param[in]	button		Le bouton de la souris.
		 */
		MouseEvent( MouseEventType type
			, castor::Position const & position
			, MouseButton button = MouseButton::eCount )
			: UserInputEvent{ UserInputEventType::eMouse }
			, m_mouseEventType{ type }
			, m_button{ button }
			, m_position{ position }
		{
		}
		/**
		 *\~english
		 *\return		The mouse event type.
		 *\~french
		 *\return		Le type d'évènement de souris.
		 */
		MouseEventType getMouseEventType()const
		{
			return m_mouseEventType;
		}
		/**
		 *\~english
		 *\return		The mouse button.
		 *\~french
		 *\return		Le bouton de la souris.
		 */
		MouseButton getButton()const
		{
			return m_button;
		}
		/**
		 *\~english
		 *\return		The position (wheel or mouse, depending on mouse event type).
		 *\~french
		 *\return		La position, de la souris ou de la molette, selon le type d'évènement.
		 */
		castor::Position const & getPosition()const
		{
			return m_position;
		}

	private:
		MouseEventType const m_mouseEventType;
		MouseButton const m_button;
		castor::Position const m_position;
	};
}

#endif
