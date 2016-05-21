/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_CLEANUP_EVENT_H___
#define ___C3D_CLEANUP_EVENT_H___

#include "Castor3DPrerequisites.hpp"
#include "FrameEvent.hpp"

#include <Logger.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/04/2013
	\version	0.7.0
	\~english
	\brief		Cleaner event
	\remark		Cleans up the member given when constructed.
	\~french
	\brief		Evènement de nettoyage
	\remark		Nettoie le membre donné lors de la construction.
	*/
	template< class T >
	class CleanupEvent
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
		CleanupEvent( CleanupEvent const & p_copy )
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
		CleanupEvent & operator=( CleanupEvent const & p_copy )
		{
			CleanupEvent l_evt( p_copy );
			std::swap( m_object, l_evt.m_object );
			std::swap( m_type, l_evt.m_type );
			return *this;
		}

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_object	The object to cleanup
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_object	L'objet à nettoyer
		 */
		CleanupEvent( T & p_object )
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
		~CleanupEvent()
		{
		}
		/**
		 *\~english
		 *\brief		Applies the event
		 *\remarks		Cleans the object up
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement
		 *\remarks		Nettoie l'objet
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		virtual bool Apply()
		{
			m_object.Cleanup();
			return true;
		}

	private:
		//!\~english The object to cleanup	\~french L'objet à nettoyer
		T & m_object;
	};
	/**
	 *\~english
	 *\brief		Helper function to create a cleanup event
	 *\param[in]	p_object	The object to cleanup
	 *\~french
	 *\brief		Fonction d'aide pour créer un éveènement de nettoyage
	 *\param[in]	p_object	L'objet à nettoyer
	 */
	template< typename T >
	std::shared_ptr< CleanupEvent< T > > MakeCleanupEvent( T & p_object )
	{
		return std::make_shared< CleanupEvent< T > >( p_object );
	}
}

#endif
