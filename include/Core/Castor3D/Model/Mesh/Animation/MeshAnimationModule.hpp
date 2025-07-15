/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshAnimationModule_H___
#define ___C3D_ModelMeshAnimationModule_H___

#include "Castor3D/Model/Mesh/MeshModule.hpp"

namespace c3d
{
	/**@name Model */
	//@{
	/**@name Mesh */
	//@{
	/**@name Animation */
	//@{

	class MeshAnimation;
	class MeshAnimationSubmesh;
	class MeshMorphTarget;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, MeshAnimation, C3D_API );
	CU_DeclareSmartPtr( c3d, MeshAnimationSubmesh, C3D_API );
	CU_DeclareSmartPtr( c3d, MeshMorphTarget, C3D_API );

	CU_DeclareVector( MeshAnimationSubmesh, MeshAnimationSubmesh );
	/** @endcond */

	//@}
	//@}
	//@}
}

#endif
