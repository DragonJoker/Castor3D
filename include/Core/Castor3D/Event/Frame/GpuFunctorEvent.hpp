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
			: GpuFrameEvent( type )
			, m_functor( functor )
		{
		}

		~GpuFunctorEvent() = default;
		/**
		 *\~english
		 *\brief		Applies the event.
		 *\remarks		Executes the function.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Traite l'évènement.
		 *\remarks		Exécute la fonction.
		 *\param[in]	device	Le device GPU.
		 */
		void apply( RenderDevice const & device
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
	inline std::unique_ptr< GpuFunctorEvent > makeGpuFunctorEvent( EventType type
		, GpuFunctorEvent::Functor functor )
	{
		return std::make_unique< GpuFunctorEvent >( type, functor );
	}
}

#endif
