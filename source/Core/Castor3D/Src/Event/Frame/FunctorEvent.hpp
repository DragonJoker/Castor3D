/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FUNCTOR_EVENT_H___
#define ___C3D_FUNCTOR_EVENT_H___

#include "Castor3DPrerequisites.hpp"
#include "Miscellaneous/Version.hpp"
#include "Event/Frame/FrameEvent.hpp"

#include <Log/Logger.hpp>

namespace castor3d
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
		inline FunctorEvent( FunctorEvent const & p_copy ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		inline FunctorEvent & operator=( FunctorEvent const & p_copy ) = delete;

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
		inline FunctorEvent( EventType p_type, Functor p_functor )
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
		 *\remarks		Executes the function
		 *\return		\p true if the event was applied successfully
		 *\~french
		 *\brief		Traite l'évènement
		 *\remarks		Exécute la fonction
		 *\return		\p true si l'évènement a été traité avec succès
		 */
		virtual bool apply()
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
	inline std::unique_ptr< FunctorEvent< Functor > > makeFunctorEvent( EventType p_type, Functor p_functor )
	{
		return std::make_unique< FunctorEvent< Functor > >( p_type, p_functor );
	}
}

#endif
