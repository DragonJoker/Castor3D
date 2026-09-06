/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RaytracingModule_H___
#define ___C3D_RaytracingModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Config/SmartPtr.hpp>

namespace c3d
{
	/**@name Render */
	//@{
	/**@name Raytracing */
	//@{

	enum class BLASInstanceFlag
	{
		eNone = 0x00,
		eForceOpaque = 0x01,
		eForceNonOpaque = 0x02
	};
	CU_ImplementFlags( BLASInstanceFlag )

	/**
	*\~english
	*\brief
	*	Bottom Level Acceleration Structure.
	*\~french
	*\brief
	*	Bottom Level Acceleration Structure..
	*/
	class BottomLevelAS;
	/**
	*\~english
	*\brief
	*	Top Level Acceleration Structure.
	*\~french
	*\brief
	*	Top Level Acceleration Structure.
	*/
	class TopLevelAS;
	/**
	*\~english
	*\brief
	*	Instance of a BLAS inside a TLAS.
	*\~french
	*\brief
	*	Instance d'un BLAS dans un TLAS.
	*/
	struct BLASInstance;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, BottomLevelAS, C3D_API );
	CU_DeclareSmartPtr( c3d, TopLevelAS, C3D_API );
	using BLASInstanceArray = Vector< BLASInstance >;
	/** @endcond */

	//@}
	//@}
}

#endif
