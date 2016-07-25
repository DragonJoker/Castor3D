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
#ifndef ___C3D_FRAME_LISTENER_H___
#define ___C3D_FRAME_LISTENER_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		03/03/2010
	\~english
	\brief		User event synchronisation class
	\remark		The handler of the frame events. It can add frame events and applies them at the wanted times
	\~french
	\brief		Classe de synchronisation des évènements
	\remark		Le gestionnaire des évènements de frame, on peut y ajouter des évènements à traiter, qui le seront au moment voulu (en fonction de leur eEVENT_TYPE)
	*/
	class FrameListener
		: public Castor::Named
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The listener's name.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom du listener
		 */
		C3D_API explicit FrameListener( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~FrameListener();
		/**
		 *\~english
		 *\brief		Empties the event lists.
		 *\~french
		 *\brief		Vide les listes d'évènements.
		 */
		C3D_API void Flush();
		/**
		 *\~english
		 *\brief		Puts an event in the corresponding array
		 *\param[in]	p_event	The event to put
		 *\~french
		 *\brief		Ajoute un évènement à la liste d'évènements correspondant à sont type
		 *\param[in]	p_event	L'évènement à ajouter
		 */
		C3D_API void PostEvent( FrameEventUPtr && p_event );
		/**
		 *\~english
		 *\brief		Applies all events of a given type, then discards them
		 *\param[in]	p_type	The type of events to fire
		 *\~french
		 *\brief		Traite tous les évènements d'un type donné
		 *\param[in]	p_type	Le type des évènements à traiter
		 *\return
		 */
		C3D_API bool FireEvents( eEVENT_TYPE p_type );

	protected:
		/**
		 *\~english
		 *\brief		Empties the event lists.
		 *\~french
		 *\brief		Vide les listes d'évènements.
		 */
		C3D_API virtual void DoFlush() {}

	protected:
		//!\~english The events arrays	\~french Les tableaux d'évènements
		std::array< FrameEventPtrArray,	eEVENT_TYPE_COUNT > m_events;
		//!\~english Mutex to make this class thread safe	\~french Mutex pour rendre cette classe thread safe
		std::recursive_mutex m_mutex;
	};
}

#endif
