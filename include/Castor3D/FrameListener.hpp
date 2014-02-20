/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_FrameListener___
#define ___C3D_FrameListener___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API FrameEvent
	{
	protected:
		//!\~english	The event type	\~french	Le type d'évènement
		eEVENT_TYPE m_eType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType	The event type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType	Le type d'évènement
		 */
		FrameEvent( eEVENT_TYPE p_eType )
			:	m_eType	( p_eType	)
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_object	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_object	L'objet à copier
		 */
		FrameEvent( FrameEvent const & p_object )
			:	m_eType	( p_object.m_eType	)
		{
		}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_object	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_object	L'objet à déplacer
		 */
		FrameEvent( FrameEvent && p_object )
			:	m_eType	( std::move( p_object.m_eType )	)
		{
		}
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
		FrameEvent & operator =( FrameEvent const & p_object )
		{
			m_eType = p_object.m_eType;
			return *this;
		}
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
		FrameEvent & operator =( FrameEvent && p_object )
		{
			if( this != &p_object )
			{
				m_eType = p_object.m_eType;
				p_object.m_eType = eEVENT_TYPE_COUNT;
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~FrameEvent()
		{
		}
		/**
		 *\~english
		 *\brief		Applies the event
		 *\remark		Must be implemented by children classes
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement
		 *\remark		Doit être implémentée dans les classes filles
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		virtual bool Apply()=0;
		/**
		 *\~english
		 *\brief		Retrieves the event type
		 *\return		The event type
		 *\~french
		 *\brief		Récupère le type de l'évènement
		 *\return		Le type de l'évènement
		 */
		inline eEVENT_TYPE GetType() { return m_eType; }
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		03/03/2010
	\~english
	\brief		User event synchronisation class
	\remark		The manager of the frame events. It can add frame events and applies them at the wanted times
	\~french
	\brief		Classe de synchronisation des évènements
	\remark		Le gestionnaire des évènements de frame, on peut y ajouter des évènements à traiter, qui le seront au moment voulu (en fonction de leur eEVENT_TYPE)
	*/
	class C3D_API FrameListener
	{
	protected:
		//!\~english	The events arrays	\~french	Les tableaux d'évènements
		std::array< FrameEventPtrArray,	eEVENT_TYPE_COUNT > m_events;
		//!\~english	Mutex to make this class thread safe	\~french	Mutex pour rendre cette classe thread safe
		std::recursive_mutex m_mutex;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		FrameListener();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~FrameListener();
		/**
		 *\~english
		 *\brief		Puts an event in the corresponding array
		 *\param[in]	p_event	The event to put
		 *\~french
		 *\brief		Ajoute un évènement à la liste d'évènements correspondant à sont type
		 *\param[in]	p_event	L'évènement à ajouter
		 */
		void PostEvent( FrameEventSPtr p_event );
		/**
		 *\~english
		 *\brief		Applies all events of a given type, then discards them
		 *\param[in]	p_type	The type of events to fire
		 *\~french
		 *\brief		Traite tous les évènements d'un type donné
		 *\param[in]	p_type	Le type des évènements à traiter
		 *\return		
		 */
		bool FireEvents( eEVENT_TYPE p_type );
	};
}

#pragma warning( pop )

#endif
