/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CpuFunctorEvent_H___
#define ___C3D_CpuFunctorEvent_H___

#include "CpuFrameEvent.hpp"

namespace castor3d
{
	class CpuFunctorEvent
		: public CpuFrameEvent
	{
	public:
		using Functor = castor::Function< void() >;

	private:
		CpuFunctorEvent( CpuFunctorEvent const & copy ) = delete;
		CpuFunctorEvent & operator=( CpuFunctorEvent const & copy ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	type	The event type
		 *\param[in]	functor	The functor to execute
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	type	Le type d'évènement
		 *\param[in]	functor	Le foncteur à exécuter
		 */
		CpuFunctorEvent( CpuEventType type
			, Functor functor )
			: CpuFrameEvent{ type }
			, m_functor{ castor::move( functor ) }
		{
		}

	private:
		void doApply()override
		{
			m_functor();
		}

	private:
		Functor m_functor;
	};
	/**
	 *\~english
	 *\brief		Helper function to create a functor event
	 *\param[in]	type	The event type
	 *\param[in]	functor	The functor to execute
	 *\~french
	 *\brief		Constructeur
	 *\param[in]	type	Le type d'évènement
	 *\param[in]	functor	Le foncteur à exécuter
	 */
	inline CpuFrameEventUPtr makeCpuFunctorEvent( CpuEventType type
		, CpuFunctorEvent::Functor functor )
	{
		return castor::makeUniqueDerived< CpuFrameEvent, CpuFunctorEvent >( type, functor );
	}
	/**
	 *\~english
	 *\brief		Helper function to create a cleanup event
	 *\param[in]	object	The object to cleanup
	 *\~french
	 *\brief		Fonction d'aide pour créer un évènement de nettoyage
	 *\param[in]	object	L'objet à nettoyer
	 */
	template< typename T >
	inline CpuFrameEventUPtr makeCpuCleanupEvent( T & object )
	{
		return makeCpuFunctorEvent( CpuEventType::ePreGpuStep
			, [&object]()
			{
				object.cleanup();
			} );
	}
	/**
	 *\~english
	 *\brief		Helper function to create an initialise event.
	 *\param[in]	object	The object to initialise.
	 *\~french
	 *\brief		Fonction d'aide pour créer un éveènement d'initialisation.
	 *\param[in]	object	L'objet à initialiser.
	 */
	template< typename T >
	inline CpuFrameEventUPtr makeCpuInitialiseEvent( T & object )
	{
		return makeCpuFunctorEvent( CpuEventType::ePreGpuStep
			, [&object]()
			{
				object.initialise();
			} );
	}
	/**
	*\~english
	*name
	*	Cache functors.
	*\~french
	*name
	*	Foncteurs de cache.
	**/
	/**@{*/
	template< typename CacheT >
	struct CpuEventInitialiserT
	{
		using ElementT = typename CacheT::ElementT;

		explicit CpuEventInitialiserT( FrameListener & listener )
			: listener{ &listener }
		{
		}

		void operator()( ElementT & res )const
		{
			listener->postEvent( makeCpuInitialiseEvent( res ) );
		}

		FrameListener * listener;
	};

	template< typename CacheT >
	struct CpuEventCleanerT
	{
		using ElementT = typename CacheT::ElementT;

		explicit CpuEventCleanerT( FrameListener & listener )
			: listener{ &listener }
		{
		}

		void operator()( ElementT & res )const
		{
			listener->postEvent( makeCpuCleanupEvent( res ) );
		}

		FrameListener * listener;
	};
	/**@}*/
}

#endif
