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
#ifndef ___C3D_FUNCTOR_EVENT_H___
#define ___C3D_FUNCTOR_EVENT_H___

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
	\brief		Functor event
	\remark		Executes a function when processed
	\~french
	\brief		Evènement foncteur
	\remark		Excécute une fonction lorsqu'il est traité
	*/
	template< class Functor >
	class FunctorEvent
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
		inline FunctorEvent( FunctorEvent const & p_copy )
			: FrameEvent( p_copy )
			, m_functor( p_copy.m_functor )
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
		inline FunctorEvent & operator=( FunctorEvent const & p_copy )
		{
			FunctorEvent l_evt( p_copy );
			std::swap( m_functor, l_evt.m_functor );
			std::swap( m_eType, l_evt.m_eType );
			return *this;
		}

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type		The event type
		 *\param[in]	p_functor	The functor to execute
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type		Le type d'évènement
		 *\param[in]	p_functor	Le foncteur à exécuter
		 */
		inline FunctorEvent( eEVENT_TYPE p_type, Functor p_functor )
			: FrameEvent( p_type )
			, m_functor( p_functor )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~FunctorEvent()
		{
		}
		/**
		 *\~english
		 *\brief		Applies the event
		 *\remark		Executes the function
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement
		 *\remark		Exécute la fonction
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		virtual bool Apply()
		{
			m_functor();
			return true;
		}

	private:
		//!\~english The functor to execute	\~french Le foncteur à exécuter
		Functor m_functor;
	};
	/**
	 *\~english
	 *\brief		Helper function to create a functor event
	 *\param[in]	p_type		The event type
	 *\param[in]	p_functor	The functor to execute
	 *\~french
	 *\brief		Constructeur
	 *\param[in]	p_type		Le type d'évènement
	 *\param[in]	p_functor	Le foncteur à exécuter
	 */
	template< typename Functor >
	inline std::shared_ptr< FunctorEvent< Functor > > MakeFunctorEvent( eEVENT_TYPE p_type, Functor p_functor )
	{
		return std::make_shared< FunctorEvent< Functor > >( p_type, p_functor );
	}
}

#endif
