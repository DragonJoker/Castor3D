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
#ifndef ___C3D_INITIALISE_EVENT_H___
#define ___C3D_INITIALISE_EVENT_H___

#include "Castor3DPrerequisites.hpp"
#include "Version.hpp"
#include "FrameEvent.hpp"

#include <Logger.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/04/2013
	\version	0.7.0
	\~english
	\brief		Initialiser event
	\remarks	Initialises the member given when constructed.
	\~french
	\brief		Evènement d'initialisation
	\remarks	Initialise le membre donné lors de la construction.
	*/
	template< class T >
	class InitialiseEvent
		: public FrameEvent
	{
	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		InitialiseEvent( InitialiseEvent const & p_copy )
			: FrameEvent( p_copy )
			, m_object( p_copy.m_object )
		{
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		InitialiseEvent & operator=( InitialiseEvent const & p_copy )
		{
			InitialiseEvent l_evt( p_copy );
			std::swap( m_object, l_evt.m_object );
			std::swap( m_type, l_evt.m_type );
			return *this;
		}

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_object	The object to initialise
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_object	L'objet à initialiser
		 */
		InitialiseEvent( T & p_object )
			: FrameEvent( eEVENT_TYPE_PRE_RENDER )
			, m_object( p_object )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~InitialiseEvent()
		{
		}
		/**
		 *\~english
		 *\brief		Applies the event
		 *\remarks		Initialises the object
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement
		 *\remarks		Initialise l'objet
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		virtual bool Apply()
		{
			m_object.Initialise();
			return true;
		}

	private:
		//!\~english The object to initialise	\~french L'objet à initialiser
		T & m_object;
	};
	/**
	 *\~english
	 *\brief		Helper function to create an initialise event
	 *\param[in]	p_object	The object to initialise
	 *\~french
	 *\brief		Fonction d'aide pour créer un éveènement d'initialisation
	 *\param[in]	p_object	L'objet à initialiser
	 */
	template< typename T >
	std::shared_ptr< InitialiseEvent< T > > MakeInitialiseEvent( T & p_object )
	{
		return std::make_shared< InitialiseEvent< T > >( p_object );
	}
}

#endif
