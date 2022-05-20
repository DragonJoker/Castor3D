/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshAnimationModule_H___
#define ___C3D_ModelMeshAnimationModule_H___

#include "Castor3D/Model/Mesh/MeshModule.hpp"

namespace castor3d
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

	CU_DeclareSmartPtr( MeshAnimation );
	CU_DeclareSmartPtr( MeshAnimationSubmesh );
	CU_DeclareSmartPtr( MeshMorphTarget );

	CU_DeclareVector( MeshAnimationSubmesh, MeshAnimationSubmesh );

	//@}
	//@}
	//@}
}

#endif
