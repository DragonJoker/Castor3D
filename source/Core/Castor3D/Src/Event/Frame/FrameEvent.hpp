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
#ifndef ___C3D_FRAME_EVENT_H___
#define ___C3D_FRAME_EVENT_H___

#include "Castor3DPrerequisites.hpp"

#include <Miscellaneous/Debug.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		03/03/2010
	\version	0.1
	\~english
	\brief		The interface which represents a frame event.
	\remark		Basically a frame event has a eEVENT_TYPE to know when it must be applied.
				<br />It can be applied, so the function must be implemented by children classes
	\~french
	\brief		Interface représentant un évènement de frame
	\remark		Un évènement a un eEVENT_TYPE pour savoir quand il doit être traité.
				<br />La fonction de traitement doit être implémentée par les classes filles.
	*/
	class FrameEvent
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type	The event type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type	Le type d'évènement
		 */
		C3D_API explicit FrameEvent( eEVENT_TYPE p_type );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		C3D_API FrameEvent( FrameEvent const & p_object );
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		C3D_API FrameEvent( FrameEvent && p_object );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_object	The object to copy
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_object	L'objet à copier
		 *\return		Une référence sur cet objet
		 */
		C3D_API FrameEvent & operator =( FrameEvent const & p_object );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_object	The object to move
		 *\return		A reference to this object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 *\return		Une référence sur cet objet
		 */
		C3D_API FrameEvent & operator =( FrameEvent && p_object );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~FrameEvent();
		/**
		 *\~english
		 *\brief		Applies the event
		 *\remarks		Must be implemented by children classes
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement
		 *\remarks		Doit être implémentée dans les classes filles
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		C3D_API virtual bool Apply() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the event type
		 *\return		The event type
		 *\~french
		 *\brief		Récupère le type de l'évènement
		 *\return		Le type de l'évènement
		 */
		inline eEVENT_TYPE GetType()
		{
			return m_type;
		}

	protected:
		//!\~english The event type	\~french Le type d'évènement
		eEVENT_TYPE m_type;
	};
}

#endif
