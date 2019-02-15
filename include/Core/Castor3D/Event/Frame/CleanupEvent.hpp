/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CLEANUP_EVENT_H___
#define ___C3D_CLEANUP_EVENT_H___

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
		 *\param[in]	copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	copy	L'objet à copier
		 */
		CleanupEvent( CleanupEvent const & copy )
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
		CleanupEvent & operator=( CleanupEvent const & copy )
		{
			CleanupEvent evt( copy );
			std::swap( m_object, evt.m_object );
			std::swap( m_type, evt.m_type );
			return *this;
		}

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	object	The object to cleanup
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	object	L'objet à nettoyer
		 */
		explicit CleanupEvent( T & object )
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
		virtual bool apply()
		{
			m_object.cleanup();
			return true;
		}

	private:
		//!\~english The object to cleanup	\~french L'objet à nettoyer
		T & m_object;
	};
	/**
	 *\~english
	 *\brief		Helper function to create a cleanup event
	 *\param[in]	object	The object to cleanup
	 *\~french
	 *\brief		Fonction d'aide pour créer un éveènement de nettoyage
	 *\param[in]	object	L'objet à nettoyer
	 */
	template< typename T >
	std::unique_ptr< CleanupEvent< T > > makeCleanupEvent( T & object )
	{
		return std::make_unique< CleanupEvent< T > >( object );
	}
}

#endif
