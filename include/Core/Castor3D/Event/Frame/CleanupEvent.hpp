/*
See LICENSE file in root folder
*/
#ifndef ___C3D_CleanupEvent_H___
#define ___C3D_CleanupEvent_H___

#include "CpuFunctorEvent.hpp"
#include "GpuFunctorEvent.hpp"

namespace castor3d
{
	/**
	 *\~english
	 *\brief		Helper function to create a cleanup event
	 *\param[in]	object	The object to cleanup
	 *\~french
	 *\brief		Fonction d'aide pour créer un évènement de nettoyage
	 *\param[in]	object	L'objet à nettoyer
	 */
	template< typename T >
	std::unique_ptr< CpuFunctorEvent > makeCpuCleanupEvent( T & object )
	{
		return makeCpuFunctorEvent( EventType::ePreRender
			, [&object]()
			{
				object.cleanup();
			} );
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
	std::unique_ptr< GpuFunctorEvent > makeGpuCleanupEvent( T & object )
	{
		return makeGpuFunctorEvent( EventType::ePreRender
			, [&object]( RenderDevice const & device
				, QueueData const & queueData )
			{
				object.cleanup( device );
			} );
	}
}

#endif
