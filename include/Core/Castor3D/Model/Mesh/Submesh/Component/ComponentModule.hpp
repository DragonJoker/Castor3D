/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshSubmeshComponentModule_H___
#define ___C3D_ModelMeshSubmeshComponentModule_H___

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Mesh */
	//@{
	/**@name Submesh */
	//@{
	/**@name Component */
	//@{

	/**
	\~english
	\brief		Submesh component holding base submesh data.
	\~french
	\brief		Composant de sous-maillage détenant des données basiques d'un sous-maillage.
	*/
	template< SubmeshFlag SubmeshFlagT, typename DataT = castor::Point3f >
	class BaseDataComponentT;
	/**
	\~english
	\brief		Submesh component holding subpasses masks.
	\~french
	\brief		Composant de sous-maillage contenant les masques des subpasses.
	*/
	class PassMasksComponent;
	/**
	\~english
	\brief		The submesh component used for skinning.
	\~french
	\brief		Le composant de sous-maillage pour le skinning.
	*/
	class SkinComponent;
	/**
	\~english
	\brief		Face implementation
	\remark		A face is constituted from 3 vertices
	\~french
	\brief		Implémentation d'un face
	\remark		Une face est constituée de 3 vertices
	*/
	class Face;
	/**
	\~english
	\brief		Holds the 3 vertex indices
	\~french
	\brief		Contient simplement les index des 3 vertex d'une face
	*/
	struct FaceIndices;
	/**
	\~english
	\brief		Holds face informations.
	\~french
	\brief		Contient des informations sur une face.
	*/
	struct FaceInfos;
	/**
	\~english
	\brief		A submesh component for faces.
	\~french
	\brief		Un composant de sous-maillage pour les faces.
	*/
	class IndexMapping;
	/**
	\~english
	\brief		The transformation matrix and material index for one instance.
	\~french
	\brief		La matrice de transformation et l'index de matériau pour une instance.
	*/
	struct InstantiationData;
	/**
	\~english
	\brief		The submesh component used for hardware instantiation.
	\~french
	\brief		Le composant de sous-maillage pour l'instantiation matérielle.
	*/
	class InstantiationComponent;
	/**
	\~english
	\brief		Line implementation.
	\~french
	\brief		Implémentation d'un ligne.
	*/
	class Line;
	/**
	\~english
	\brief		Holds the 2 vertex indices of a ligne.
	\~french
	\brief		Contient les index des 2 sommets d'une ligne.
	*/
	struct LineIndices;
	/**
	\~english
	\brief		The submesh component for lines.
	\~french
	\brief		Composant de sous-maillage pour les lignes.
	*/
	class LinesMapping;
	/**
	\~english
	\brief		The submesh component used for meshlets.
	\~french
	\brief		Le composant de sous-maillage pour les meshlets.
	*/
	class MeshletComponent;
	/**
	\~english
	\brief		The submesh component used for morphing.
	\~french
	\brief		Le composant de sous-maillage pour le morphing.
	*/
	class MorphComponent;
	/**
	\~english
	\brief		Component for a submesh.
	\~french
	\brief		Composant pour un sous-maillage.
	*/
	class SubmeshComponent;
	/**
	\~english
	\brief		The submesh component for triangular faces.
	\~french
	\brief		Composant de sous-maillage pour les faces triangulaires.
	*/
	class TriFaceMapping;
	/**
	\~english
	\brief		The submesh component holding normals.
	\~french
	\brief		Le composant de sous-maillage contenant les normales.
	*/
	using NormalsComponent = BaseDataComponentT< SubmeshFlag::eNormals >;
	/**
	\~english
	\brief		The submesh component holding positions.
	\~french
	\brief		Le composant de sous-maillage contenant les positions.
	*/
	using PositionsComponent = BaseDataComponentT< SubmeshFlag::ePositions >;
	/**
	\~english
	\brief		The submesh component holding tangents.
	\~french
	\brief		Le composant de sous-maillage contenant les tangents.
	*/
	using TangentsComponent = BaseDataComponentT< SubmeshFlag::eTangents, castor::Point4f >;
	/**
	\~english
	\brief		The submesh component holding texture coordinates.
	\~french
	\brief		Le composant de sous-maillage contenant les coordonnées de texture.
	*/
	using Texcoords0Component = BaseDataComponentT< SubmeshFlag::eTexcoords0 >;
	/**
	\~english
	\brief		The submesh component holding texture coordinates.
	\~french
	\brief		Le composant de sous-maillage contenant les coordonnées de texture.
	*/
	using Texcoords1Component = BaseDataComponentT< SubmeshFlag::eTexcoords1 >;
	/**
	\~english
	\brief		The submesh component holding texture coordinates.
	\~french
	\brief		Le composant de sous-maillage contenant les coordonnées de texture.
	*/
	using Texcoords2Component = BaseDataComponentT< SubmeshFlag::eTexcoords2 >;
	/**
	\~english
	\brief		The submesh component holding texture coordinates.
	\~french
	\brief		Le composant de sous-maillage contenant les coordonnées de texture.
	*/
	using Texcoords3Component = BaseDataComponentT< SubmeshFlag::eTexcoords3 >;
	/**
	\~english
	\brief		The submesh component holding colours.
	\~french
	\brief		Le composant de sous-maillage contenant les couleurs.
	*/
	using ColoursComponent = BaseDataComponentT< SubmeshFlag::eColours >;

	CU_DeclareSmartPtr( castor3d, IndexMapping, C3D_API );
	CU_DeclareSmartPtr( castor3d, InstantiationComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, LinesMapping, C3D_API );
	CU_DeclareSmartPtr( castor3d, MeshletComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, MorphComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, PassMasksComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SkinComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SubmeshComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, TriFaceMapping, C3D_API );

	//! Face array
	CU_DeclareVector( Face, Face );
	CU_DeclareMap( uint32_t, SubmeshComponentUPtr, SubmeshComponentID );

	//@}
	//@}
	//@}
	//@}
}

namespace castor
{
	template< castor3d::SubmeshFlag SubmeshFlagT, typename DataT >
	struct Deleter< castor3d::BaseDataComponentT< SubmeshFlagT, DataT > >
	{
		void operator()( castor3d::BaseDataComponentT< SubmeshFlagT, DataT > * pointer )noexcept
		{
			delete pointer;
		}
	};
}
namespace castor3d
{
	template< castor3d::SubmeshFlag SubmeshFlagT, typename DataT >
	using BaseDataComponentUPtrT = castor::UniquePtr< BaseDataComponentT< SubmeshFlagT, DataT > >;
	template< castor3d::SubmeshFlag SubmeshFlagT, typename DataT >
	using BaseDataComponentRPtrT = BaseDataComponentT< SubmeshFlagT, DataT > *;
}

#endif
