/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuFunctorEvent_H___
#define ___C3D_GpuFunctorEvent_H___

#include "GpuFrameEvent.hpp"

namespace castor3d
{
	class GpuFunctorEvent
		: public GpuFrameEvent
	{
	public:
		using Functor = std::function< void( RenderDevice const &, QueueData const & ) >;

	private:
		GpuFunctorEvent( GpuFunctorEvent const & copy ) = delete;
		GpuFunctorEvent & operator=( GpuFunctorEvent const & copy ) = delete;

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
		GpuFunctorEvent( EventType type
			, Functor functor )
			: GpuFrameEvent{ type }
			, m_functor{ functor }
		{
		}

	private:
		void doApply( RenderDevice const & device
			, QueueData const & queueData )override
		{
			m_functor( device, queueData );
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
	inline GpuFrameEventUPtr makeGpuFunctorEvent( EventType type
		, GpuFunctorEvent::Functor functor )
	{
		return castor::makeUniqueDerived< GpuFrameEvent, GpuFunctorEvent >( type, functor );
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
	inline GpuFrameEventUPtr makeGpuCleanupEvent( T & object )
	{
		return makeGpuFunctorEvent( EventType::ePreRender
			, [&object]( RenderDevice const & device
				, QueueData const & queueData )
			{
				object.cleanup( device );
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
	inline GpuFrameEventUPtr makeGpuInitialiseEvent( T & object )
	{
		return makeGpuFunctorEvent( EventType::ePreRender
			, [&object]( RenderDevice const & device
				, QueueData const & queueData )
			{
				object.initialise( device );
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
	struct GpuEventInitialiserT
	{
		using ElementT = typename CacheT::ElementT;

		explicit GpuEventInitialiserT( FrameListener & listener )
			: listener{ &listener }
		{
		}

		void operator()( ElementT & res )const
		{
			listener->postEvent( makeGpuInitialiseEvent( res ) );
		}

		FrameListener * listener;
	};

	template< typename CacheT >
	struct GpuEventCleanerT
	{
		using ElementT = typename CacheT::ElementT;

		explicit GpuEventCleanerT( FrameListener & listener )
			: listener{ &listener }
		{
		}

		void operator()( ElementT & res )const
		{
			listener->postEvent( makeGpuCleanupEvent( res ) );
		}

		FrameListener * listener;
	};
	/**@}*/
}

#endif
