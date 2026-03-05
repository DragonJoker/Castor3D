/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderVolumetricModule_H___
#define ___C3D_RenderVolumetricModule_H___

#include "Castor3D/Castor3DModule.hpp"

namespace c3d
{
	/**@name Render */
	//@{
	/**@name Volumetric */
	//@{
	static u32 constexpr FroxelIntegrateTileSize = 64u;
	/**
	*\~english
	*\brief
	*	Froxels configuration.
	*\~french
	*\brief
	*	Configuration des froxels.
	*/
	struct FroxelsConfig;
	/**
	*\~english
	*\brief
	*	The buffer containing the clusters.
	*\~french
	*\brief
	*	Le buffer contenant les clusters.
	*/
	class FrustumFroxels;
	/**
	*\~english
	*\brief
	*	Class in charge of rendering all volumetrics.
	*\~french
	*\brief
	*	Classe en charge du rendu des volumes.
	*/
	class VolumetricRendering;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, FrustumFroxels, C3D_API );
	/** @endcond */

	//@}
	//@}
}

#endif
