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
#ifndef ___C3D_MOUSE_EVENT_H___
#define ___C3D_MOUSE_EVENT_H___

#include "UserInputEvent.hpp"

#include <Graphics/Position.hpp>

namespace Castor3D
{
	/*!
	*\author	Sylvain DOREMUS
	*\date		17/04/2016
	*\version	0.9.0
	*\~english
	*\brief		Structure holding mouse state.
	*\~french
	*\brief		Structure contenant l'àtat de la souris.
	*/
	struct MouseState
	{
		//!\~english The current position.	\~french La position actuelle.
		Castor::Position m_position;
		//!\~english The wheel position.	\~french La position de la molette.
		Castor::Position m_wheel;
		//!\~english The buttons state (true = down).	\~french L'àtat des boutons (true = enfoncà).
		std::array< bool, size_t( MouseButton::eCount ) > m_buttons;
		//!\~english The button which had the last change.	\~french Le bouton ayant reàu le dernier changement.
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
		 *\param[in]	p_type		The mouse event type.
		 *\param[in]	p_position	The position, of wheel or mouse, depending on the event type.
		 *\param[in]	p_button	The mouse button.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	p_type		Le type d'évènement de souris.
		 *\param[in]	p_position	La position, de la souris ou de la molette, selon le type d'évènement.
		 *\param[in]	p_button	Le bouton de la souris.
		 */
		MouseEvent( MouseEventType p_type, Castor::Position const & p_position, MouseButton p_button = MouseButton::eCount )
			: UserInputEvent{ UserInputEventType::eMouse }
			, m_mouseEventType{ p_type }
			, m_position{ p_position }
			, m_button{ p_button }
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
		inline MouseEventType GetMouseEventType()const
		{
			return m_mouseEventType;
		}
		/**
		 *\~english
		 *\return		The mouse button.
		 *\~french
		 *\return		Le bouton de la souris.
		 */
		inline MouseButton GetButton()const
		{
			return m_button;
		}
		/**
		 *\~english
		 *\return		The position (wheel or mouse, depending on mouse event type).
		 *\~french
		 *\return		La position, de la souris ou de la molette, selon le type d'évènement.
		 */
		inline Castor::Position const & GetPosition()const
		{
			return m_position;
		}

	private:
		//!\~english The mouse event type.	\~french Le type d'évènement souris.
		MouseEventType const m_mouseEventType;
		//!\~english The mouse button.	\~french Le bouton de la souris
		MouseButton const m_button;
		//!\~english The position, (wheel or mouse, depending on mouse event type).	\~french La position (de la souris ou de la molette, selon le type d'évènement).
		Castor::Position const m_position;
	};
}

#endif
