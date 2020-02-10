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
	*	Class holding bone data.
	*\remark
	*	Holds weight for each vertice and matrix from mesh space to bone space.
	*\~french
	*\brief
	*	Classe contenant les données d'un bone.
	*\remark
	*	Contient les poids pour chaque vertice et la matrice de transformation de l'espace objet vers l'espace bone.
	*/
	class Bone;
	/**
	*\~english
	*\brief
	*	The skeleton, holds each bone
	*\~french
	*\brief
	*	Le squelette, contient chaque bone
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

	CU_DeclareSmartPtr( Bone );
	CU_DeclareSmartPtr( Skeleton );

	//! Skinned vertex data array
	CU_DeclareVector( VertexBoneData, VertexBoneData );
	//! Bone pointer array
	CU_DeclareVector( BoneSPtr, BonePtr );
	//! Bone pointer map sorted by name
	CU_DeclareMap( castor::String, BoneSPtr, BonePtrStr );

	//@}
	//@}
}

#endif
