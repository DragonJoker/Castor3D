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

	CU_DeclareCUSmartPtr( castor3d, MeshAnimation, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, MeshAnimationSubmesh, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, MeshMorphTarget, C3D_API );

	CU_DeclareVector( MeshAnimationSubmesh, MeshAnimationSubmesh );

	//@}
	//@}
	//@}
}

#endif
