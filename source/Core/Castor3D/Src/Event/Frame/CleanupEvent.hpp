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
#ifndef ___C3D_CLEANUP_EVENT_H___
#define ___C3D_CLEANUP_EVENT_H___

#include "Castor3DPrerequisites.hpp"
#include "FrameEvent.hpp"

#include <Log/Logger.hpp>

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
		explicit CleanupEvent( T & p_object )
			: FrameEvent( EventType::PreRender )
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
	std::unique_ptr< CleanupEvent< T > > MakeCleanupEvent( T & p_object )
	{
		return std::make_unique< CleanupEvent< T > >( p_object );
	}
}

#endif
