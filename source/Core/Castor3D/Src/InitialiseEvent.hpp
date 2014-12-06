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
	\remark		Initialises the member given when constructed.
	\~french
	\brief		Evènement d'initialisation
	\remark		Initialise le membre donné lors de la construction.
	*/
	template< class T >
	class C3D_API InitialiseEvent
		: public FrameEvent
	{
	private:
		//!\~english The object to initialise	\~french L'objet à initialiser
		T & m_tObject;
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		InitialiseEvent( InitialiseEvent const & p_copy )
			:	FrameEvent( p_copy )
			,	m_tObject( p_copy.m_tObject )
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
			std::swap( m_tObject, l_evt.m_tObject );
			std::swap( m_eType, l_evt.m_eType );
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
			:	FrameEvent( eEVENT_TYPE_PRE_RENDER )
			,	m_tObject( p_object )
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
		 *\remark		Initialises the object
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement
		 *\remark		Initialise l'objet
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		virtual bool Apply()
		{
			bool l_bReturn = true;

			try
			{
				m_tObject.Initialise();
			}
			catch ( Castor::Exception & p_exc )
			{
				Castor::String l_strText = cuT( "Encountered exception while initialising object : " ) + Castor::str_utils::from_str( p_exc.GetFullDescription() );
				Castor::Logger::LogError( l_strText );
				l_bReturn = false;
			}
			catch ( std::exception & p_exc )
			{
				Castor::String l_strText = cuT( "Encountered exception while initialising object : " ) + Castor::str_utils::from_str( p_exc.what() );
				Castor::Logger::LogError( l_strText );
				l_bReturn = false;
			}
			catch ( ... )
			{
				Castor::String l_strText = cuT( "Encountered unknown exception while initialising object" );
				Castor::Logger::LogError( l_strText );
				l_bReturn = false;
			}

			return l_bReturn;
		}
	};
}

#endif
