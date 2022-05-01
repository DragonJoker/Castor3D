/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelSkeletonModule_H___
#define ___C3D_ModelSkeletonModule_H___

#include "Castor3D/Model/ModelModule.hpp"

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Skeleton */
	//@{

	static const uint32_t MaxBonesPerVertex = 8;
	/**
	*\~english
	*\brief
	*	A node in the skeleton.
	*\~french
	*\brief
	*	Un noeud dans le squelette.
	*/
	class SkeletonNode;
	/**
	*\~english
	*\brief
	*	Class holding bone data.
	*\~french
	*\brief
	*	Classe contenant les données d'un bone.
	*/
	class BoneNode;
	/**
	*\~english
	*\brief
	*	The skeleton, holds each skeleton node.
	*\~french
	*\brief
	*	Le squelette, contient chaque noeud de squelette.
	*/
	class Skeleton;
	/**
	*\~english
	*\brief
	*	Holds bone data for one vertex
	*\~french
	*\brief
	*	Contient les données de bones pour un vertice
	*/
	struct VertexBoneData;

	CU_DeclareSmartPtr( BoneNode );
	CU_DeclareSmartPtr( SkeletonNode );
	CU_DeclareSmartPtr( Skeleton );

	//! Skinned vertex data array
	CU_DeclareVector( VertexBoneData, VertexBoneData );
	//! Bone pointer array
	CU_DeclareVector( SkeletonNodeUPtr, SkeletonNodePtr );

	//@}
	//@}
}

#endif
