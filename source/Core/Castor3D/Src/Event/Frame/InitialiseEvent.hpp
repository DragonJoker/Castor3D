/*
See LICENSE file in root folder
*/
#ifndef ___C3D_INITIALISE_EVENT_H___
#define ___C3D_INITIALISE_EVENT_H___

#include "Castor3DPrerequisites.hpp"
#include "FrameEvent.hpp"

#include <Log/Logger.hpp>

namespace castor3d
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
			InitialiseEvent evt( p_copy );
			std::swap( m_object, evt.m_object );
			std::swap( m_type, evt.m_type );
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
		explicit InitialiseEvent( T & p_object )
			: FrameEvent( EventType::ePreRender )
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
		virtual bool apply()
		{
			m_object.initialise();
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
	std::unique_ptr< InitialiseEvent< T > > makeInitialiseEvent( T & p_object )
	{
		return std::make_unique< InitialiseEvent< T > >( p_object );
	}
}

#endif
