/*
See LICENSE file in root folder
*/
#ifndef ___C3D_INITIALISE_EVENT_H___
#define ___C3D_INITIALISE_EVENT_H___

#include "Castor3D/Castor3DPrerequisites.hpp"
#include "Castor3D/Event/Frame/FrameEvent.hpp"

#include <CastorUtils/Log/Logger.hpp>

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
		 *\param[in]	copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	copy	L'objet à copier
		 */
		InitialiseEvent( InitialiseEvent const & copy )
			: FrameEvent( copy )
			, m_object( copy.m_object )
		{
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	copy	The object to copy
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	copy	L'objet à copier
		 */
		InitialiseEvent & operator=( InitialiseEvent const & copy )
		{
			InitialiseEvent evt( copy );
			std::swap( m_object, evt.m_object );
			std::swap( m_type, evt.m_type );
			return *this;
		}

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	object	The object to initialise
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	object	L'objet à initialiser
		 */
		explicit InitialiseEvent( T & object )
			: FrameEvent( EventType::ePreRender )
			, m_object( object )
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
	 *\param[in]	object	The object to initialise
	 *\~french
	 *\brief		Fonction d'aide pour créer un éveènement d'initialisation
	 *\param[in]	object	L'objet à initialiser
	 */
	template< typename T >
	std::unique_ptr< InitialiseEvent< T > > makeInitialiseEvent( T & object )
	{
		return std::make_unique< InitialiseEvent< T > >( object );
	}
}

#endif
