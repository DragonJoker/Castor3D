/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuInitialiseEvent_H___
#define ___C3D_GpuInitialiseEvent_H___

#include "CpuFunctorEvent.hpp"
#include "GpuFunctorEvent.hpp"

namespace castor3d
{
	/**
	 *\~english
	 *\brief		Helper function to create an initialise event.
	 *\param[in]	object	The object to initialise.
	 *\~french
	 *\brief		Fonction d'aide pour créer un éveènement d'initialisation.
	 *\param[in]	object	L'objet à initialiser.
	 */
	template< typename T >
	std::unique_ptr< CpuFunctorEvent > makeCpuInitialiseEvent( T & object )
	{
		return makeCpuFunctorEvent( EventType::ePreRender
			, [&object]()
			{
				object.initialise();
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
	std::unique_ptr< GpuFunctorEvent > makeGpuInitialiseEvent( T & object )
	{
		return makeGpuFunctorEvent( EventType::ePreRender
			, [&object]( RenderDevice const & device
				, QueueData const & queueData )
			{
				object.initialise( device );
			} );
	}
}

#endif
