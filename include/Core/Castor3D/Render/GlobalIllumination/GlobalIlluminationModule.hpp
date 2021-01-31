/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlobalIlluminationModule_H___
#define ___C3D_GlobalIlluminationModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Global Illumination */
	//@{

	/**
	*\~english
	*\brief
	*	Enumerator of supported global illumination algorithms.
	*\~french
	*\brief
	*	Enumération des algorithmes de global illumination supportés.
	*/
	enum class GlobalIlluminationType
		: uint8_t
	{
		eNone,
		// Reflective shadow maps.
		eRsm,
		// Voxel Cone Tracing.
		eVoxelConeTracing,
		// Light Propagation Volumes without geometry injection.
		eLpv,
		// Light Propagation Volumes with geometry injection.
		eLpvG,
		// Layered Light Propagation Volumes without geometry injection.
		eLayeredLpv,
		// Layered Light Propagation Volumes with geometry injection.
		eLayeredLpvG,
		CU_ScopedEnumBounds( eNone ),
	};
	castor::String getName( GlobalIlluminationType giType );

	//@}
	//@}
}

#endif
