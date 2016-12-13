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
#ifndef ___C3D_GUI_EVENT_H___
#define ___C3D_GUI_EVENT_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
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
		 *\param[in]	p_type	The user input event type.
		 *\~french
		 *\~brief		Constructeur.
		 *\param[in]	p_type	Le type d'évènement entràe utilisateur.
		 */
		explicit UserInputEvent( UserInputEventType p_type )
			: m_eventType( p_type )
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
		inline UserInputEventType GetEventType()const
		{
			return m_eventType;
		}

	private:
		//!\~english The user input event type.	\~french Le type d'évènement entràe utilisateur.
		UserInputEventType m_eventType;
	};
}

#endif
