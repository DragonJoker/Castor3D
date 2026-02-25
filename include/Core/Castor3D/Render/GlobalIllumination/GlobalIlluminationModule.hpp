/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlobalIlluminationModule_H___
#define ___C3D_GlobalIlluminationModule_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

namespace c3d
{
	/**@name Render */
	//@{
	/**@name Global Illumination */
	//@{

	/**
	*\~english
	*\brief
	*	Enumerator of per light source supported global illumination algorithms.
	*\~french
	*\brief
	*	Enumération des algorithmes de global illumination par source lumineuse supportés.
	*/
	enum class GlobalIlluminationType
		: uint32_t
	{
		eNone = 0,
		// Reflective shadow maps.
		eRsm = 1,
		// Light Propagation Volumes without geometry injection.
		eLpv = 2,
		// Light Propagation Volumes with geometry injection.
		eLpvG = 3,
		// Layered Light Propagation Volumes without geometry injection.
		eLayeredLpv = 4,
		// Layered Light Propagation Volumes with geometry injection.
		eLayeredLpvG = 5,
		CU_ScopedEnumBounds( eNone, eLayeredLpvG ),
	};
	C3D_API String getName( GlobalIlluminationType giType );
	/**
	*\~english
	*\brief
	*	Lighting propagation volumes pass resulting buffers.
	*\~french
	*\brief
	*	Buffers résultant d'une passe de light propagation volumes.
	*/
	class LightVolumePassResult;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, LightVolumePassResult, C3D_API );

	CU_DeclareVector( LightVolumePassResultUPtr, LightVolumePassResult );
	/** @endcond */

	struct IndirectLightingData
	{
		LpvGridConfigUbo const * lpvConfigUbo{};
		LayeredLpvGridConfigUbo const * llpvConfigUbo{};
		VoxelizerUbo const * vctConfigUbo{};
		Texture const * rsmResult{};
		LightVolumePassResult const * lpvResult{};
		LightVolumePassResultArray const * llpvResult{};
		Texture const * vctFirstBounce{};
		Texture const * vctSecondaryBounce{};
	};

	//@}
	//@}
}

#endif
