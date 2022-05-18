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
	template< SubmeshFlag SubmeshFlagT >
	class BaseDataComponentT;
	/**
	\~english
	\brief		The submesh component used for skinning.
	\~french
	\brief		Le composant de sous-maillage pour le skinning.
	*/
	class BonesComponent;
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
	using TangentsComponent = BaseDataComponentT< SubmeshFlag::eTangents >;
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

	CU_DeclareSmartPtr( Face );
	CU_DeclareSmartPtr( IndexMapping );
	CU_DeclareSmartPtr( InstantiationComponent );
	CU_DeclareSmartPtr( MorphComponent );
	CU_DeclareSmartPtr( SubmeshComponent );
	CU_DeclareSmartPtr( TriFaceMapping );

	//! Face array
	CU_DeclareVector( Face, Face );
	//! Face pointer array
	CU_DeclareVector( FaceSPtr, FacePtr );
	CU_DeclareMap( uint32_t, SubmeshComponentSPtr, SubmeshComponentID );

	//@}
	//@}
	//@}
	//@}
}

#endif
