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
		using Functor = std::function< void() >;

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
		CpuFunctorEvent( EventType type
			, Functor functor )
			: CpuFrameEvent( type )
			, m_functor( functor )
		{
		}
		/**
		 *\~english
		 *\brief		Applies the event
		 *\~french
		 *\brief		Traite l'évènement
		 */
		void apply()override
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
	inline std::unique_ptr< CpuFunctorEvent > makeCpuFunctorEvent( EventType type
		, CpuFunctorEvent::Functor functor )
	{
		return std::make_unique< CpuFunctorEvent >( type, functor );
	}
}

#endif
