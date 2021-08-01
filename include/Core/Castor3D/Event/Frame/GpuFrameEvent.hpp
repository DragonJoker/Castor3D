/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuFrameEvent_H___
#define ___C3D_GpuFrameEvent_H___

#include "FrameEventModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	class GpuFrameEvent
	{
	public:
		C3D_API GpuFrameEvent( GpuFrameEvent const & object ) = default;
		C3D_API GpuFrameEvent( GpuFrameEvent && object ) = default;
		C3D_API GpuFrameEvent & operator=( GpuFrameEvent const & object ) = default;
		C3D_API GpuFrameEvent & operator=( GpuFrameEvent && object ) = default;
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
		 *\remarks		Must be implemented by children classes.
		 *\param[in]	device	The GPU device.
		 *\~french
		 *\brief		Traite l'évènement.
		 *\remarks		doit être implémentée dans les classes filles.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API virtual void apply( RenderDevice const & device
			, QueueData const & queueData ) = 0;
		/**
		 *\~english
		 *\return		The event type.
		 *\~french
		 *\return		Le type de l'évènement.
		 */
		EventType getType()
		{
			return m_type;
		}

	protected:
		//!\~english	The event type.
		//!\~french		Le type d'évènement.
		EventType m_type;

#if !defined( NDEBUG )

		//!\~english	The event creation stack trace.
		//!\~french		La pile d'appels lors de la création de l'évènement.
		castor::String m_stackTrace;

#endif
	};
}

#endif
