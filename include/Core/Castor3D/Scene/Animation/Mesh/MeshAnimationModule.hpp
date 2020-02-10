/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneAnimationMeshModule_H___
#define ___C3D_SceneAnimationMeshModule_H___

#include "Castor3D/Scene/Animation/AnimationModule.hpp"

namespace castor3d
{
	/**@name Scene */
	//@{
	/**@name Animation */
	//@{
	/**@name Mesh */
	//@{

	/**
	*\~english
	*\brief
	*	Mesh animation instance.
	*\~french
	*\brief
	*	Instance d'animation de maillage.
	*/
	class MeshAnimationInstance;
	/**
	*\~english
	*\brief
	*	Class which represents the moving things.
	*\remarks
	*	Manages translation, scaling, rotation of the thing.
	*\~french
	*\brief
	*	Classe de représentation de choses mouvantes.
	*\remarks
	*	Gère les translations, mises à l'échelle, rotations de la chose.
	*/
	class MeshAnimationInstanceSubmesh;

	CU_DeclareSmartPtr( MeshAnimationInstance );

	//! MeshAnimationInstanceSubmesh map, sorted by submesh ID.
	CU_DeclareMap( uint32_t, MeshAnimationInstanceSubmesh, MeshAnimationInstanceSubmesh );

	//@}
	//@}
	//@}
}

#endif
