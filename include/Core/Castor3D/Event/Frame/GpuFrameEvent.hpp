/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuFrameEvent_H___
#define ___C3D_GpuFrameEvent_H___

#include "FrameListener.hpp"

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#pragma warning( pop )

namespace castor3d
{
	class GpuFrameEvent
	{
	public:
		C3D_API GpuFrameEvent( GpuFrameEvent const & rhs );
		C3D_API GpuFrameEvent( GpuFrameEvent && rhs );
		C3D_API GpuFrameEvent & operator=( GpuFrameEvent const & rhs );
		C3D_API GpuFrameEvent & operator=( GpuFrameEvent && rhs );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type	The event type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type	Le type d'évènement.
		 */
		C3D_API explicit GpuFrameEvent( EventType type );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~GpuFrameEvent() = default;
		/**
		 *\~english
		 *\brief		Applies the event.
		 *\param[in]	device		The GPU device.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\~french
		 *\brief		Traite l'évènement.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 */
		void apply( RenderDevice const & device
			, QueueData const & queueData )
		{
			if ( m_skip )
			{
				return;
			}

			doApply( device, queueData );
		}
		/**
		 *\~english
		 *\return		The event type.
		 *\~french
		 *\return		Le type de l'évènement.
		 */
		EventType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Sets the event to be skipped.
		 *\~french
		 *\brief		Définit que l'évènement doit être ignoré.
		 */
		void skip()
		{
			m_skip = true;
		}

	private:
		C3D_API virtual void doApply( RenderDevice const & device
			, QueueData const & queueData ) = 0;

	private:
		EventType m_type;
		std::atomic_bool m_skip{ false };

#if !defined( NDEBUG )

		//!\~english	The event creation stack trace.
		//!\~french		La pile d'appels lors de la création de l'évènement.
		castor::String m_stackTrace;

#endif
	};

	template< typename EventT, typename ... ParamsT >
	inline GpuFrameEventUPtr makeGpuFrameEvent( ParamsT && ... params )
	{
		return castor::makeUniqueDerived< GpuFrameEvent, EventT >( std::forward< ParamsT >( params )... );
	}
}

#endif
