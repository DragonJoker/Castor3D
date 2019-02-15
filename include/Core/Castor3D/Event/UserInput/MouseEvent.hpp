/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MOUSE_EVENT_H___
#define ___C3D_MOUSE_EVENT_H___

#include "UserInputEvent.hpp"

#include <Graphics/Position.hpp>

namespace castor3d
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Structure holding mouse state.
	*\~french
	*\brief		Structure contenant l'état de la souris.
	*/
	struct MouseState
	{
		//!\~english	The current position.
		//!\~french		La position actuelle.
		castor::Position m_position;
		//!\~english	The wheel position.
		//!\~french		La position de la molette.
		castor::Position m_wheel;
		//!\~english	The buttons state (true = down).
		//!\~french		L'état des boutons (true = enfoncé).
		std::array< bool, size_t( MouseButton::eCount ) > m_buttons;
		//!\~english	The button which had the last change.
		//!\~french		Le bouton ayant reçu le dernier changement.
		MouseButton m_changed;
	};
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Description of a mouse event.
	*\~french
	*\brief		Description d'un évènement de souris.
	*/
	class MouseEvent
		: public UserInputEvent
	{
	public:
		/**
		 *\~english
		 *\~brief		Constructor.
		 *\param[in]	type		The mouse event type.
		 *\param[in]	position	The position, of wheel or mouse, depending on the event type.
		 *\param[in]	button		The mouse button.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	type		Le type d'évènement de souris.
		 *\param[in]	position	La position, de la souris ou de la molette, selon le type d'évènement.
		 *\param[in]	button		Le bouton de la souris.
		 */
		MouseEvent( MouseEventType type
			, castor::Position const & position
			, MouseButton button = MouseButton::eCount )
			: UserInputEvent{ UserInputEventType::eMouse }
			, m_mouseEventType{ type }
			, m_position{ position }
			, m_button{ button }
		{
		}
		/**
		 *\~english
		 *\~brief		Destructor.
		 *\~french
		 *\~brief		Destructeur.
		 */
		virtual ~MouseEvent()
		{
		}
		/**
		 *\~english
		 *\return		The mouse event type.
		 *\~french
		 *\return		Le type d'évènement de souris.
		 */
		inline MouseEventType getMouseEventType()const
		{
			return m_mouseEventType;
		}
		/**
		 *\~english
		 *\return		The mouse button.
		 *\~french
		 *\return		Le bouton de la souris.
		 */
		inline MouseButton getButton()const
		{
			return m_button;
		}
		/**
		 *\~english
		 *\return		The position (wheel or mouse, depending on mouse event type).
		 *\~french
		 *\return		La position, de la souris ou de la molette, selon le type d'évènement.
		 */
		inline castor::Position const & getPosition()const
		{
			return m_position;
		}

	private:
		//!\~english	The mouse event type.
		//!\~french		Le type d'évènement souris.
		MouseEventType const m_mouseEventType;
		//!\~english	The mouse button.
		//!\~french		Le bouton de la souris
		MouseButton const m_button;
		//!\~english	The position, (wheel or mouse, depending on mouse event type).
		//!\~french		La position (de la souris ou de la molette, selon le type d'évènement).
		castor::Position const m_position;
	};
}

#endif
