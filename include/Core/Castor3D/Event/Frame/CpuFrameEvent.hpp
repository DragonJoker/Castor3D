/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CpuFrameEvent_H___
#define ___C3D_CpuFrameEvent_H___

#include "FrameListener.hpp"

namespace castor3d
{
	class CpuFrameEvent
	{
	public:
		C3D_API CpuFrameEvent( CpuFrameEvent const & object ) = default;
		C3D_API CpuFrameEvent( CpuFrameEvent && object ) = default;
		C3D_API CpuFrameEvent & operator=( CpuFrameEvent const & object ) = default;
		C3D_API CpuFrameEvent & operator=( CpuFrameEvent && object ) = default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	type	The event type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	type	Le type d'évènement.
		 */
		C3D_API explicit CpuFrameEvent( EventType type );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~CpuFrameEvent() = default;
		/**
		 *\~english
		 *\brief		Applies the event.
		 *\remarks		Must be implemented by children classes.
		 *\~french
		 *\brief		Traite l'évènement.
		 *\remarks		doit être implémentée dans les classes filles.
		 */
		void apply()
		{
			if ( m_skip )
			{
				return;
			}

			doApply();
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
		 *\return		Définit que l'évènement doit être ignoré.
		 */
		void skip()
		{
			m_skip = true;
		}

	private:
		C3D_API virtual void doApply() = 0;

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
	CpuFrameEventUPtr makeCpuFrameEvent( ParamsT && ... params )
	{
		return castor::makeUniqueDerived< CpuFrameEvent, EventT >( std::forward< ParamsT >( params )... );
	}
}

#endif
