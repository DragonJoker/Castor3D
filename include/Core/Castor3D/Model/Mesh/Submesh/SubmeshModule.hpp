/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshSubmeshModule_H___
#define ___C3D_ModelMeshSubmeshModule_H___

#include "Castor3D/Model/Mesh/MeshModule.hpp"

#include "Castor3D/Model/Skeleton/VertexBoneData.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Graphics/BoundingSphere.hpp>
#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <cstdint>

namespace castor3d
{
	/**@name Model */
	//@{
	/**@name Mesh */
	//@{
	/**@name Submesh */
	//@{

	/**
	*\~english
	*\brief
	*	Lists the supported data kinds for a submesh.
	*\~french
	*\brief
	*	Liste les types de données supportés par un submesh.
	*/
	enum class SubmeshData
		: uint32_t
	{
		//!\~english	The submesh has indices.
		//!\~french		Le submesh a des indices.
		eIndex,
		//!\~english	The submesh has positions.
		//!\~french		Le submesh a des positions.
		ePositions,
		//!\~english	The submesh has normals.
		//!\~french		Le submesh a des normales.
		eNormals,
		//!\~english	The submesh has tangents.
		//!\~french		Le submesh a des tangentes.
		eTangents,
		//!\~english	Submesh has texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture.
		eTexcoords,
		//!\~english	The submesh has morphing positions.
		//!\~french		Le submesh a des positions de morphing.
		eMorphPositions,
		//!\~english	The submesh has morphing normals.
		//!\~french		Le submesh a des normales de morphing.
		eMorphNormals,
		//!\~english	The submesh has morphing tangents.
		//!\~french		Le submesh a des tangentes de morphing.
		eMorphTangents,
		//!\~english	The submesh has morphing texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture de morphing.
		eMorphTexcoords,
		//!\~english	Submesh has bones data.
		//!\~french		Submesh a des données d'os.
		eBones,
		//!\~english	Submesh has a second set of texture coordinates.
		//!\~french		Le submesh a un second set de coordonnées de texture.
		eSecondaryUV,
	};

	static castor::String getName( SubmeshData value )
	{
		switch ( value )
		{
		case castor3d::SubmeshData::eIndex:
			return cuT( "Index" );
		case castor3d::SubmeshData::ePositions:
			return cuT( "Positions" );
		case castor3d::SubmeshData::eNormals:
			return cuT( "Normals" );
		case castor3d::SubmeshData::eTangents:
			return cuT( "Tangents" );
		case castor3d::SubmeshData::eTexcoords:
			return cuT( "Texcoords" );
		case castor3d::SubmeshData::eMorphPositions:
			return cuT( "MorphPositions" );
		case castor3d::SubmeshData::eMorphNormals:
			return cuT( "MorphNormals" );
		case castor3d::SubmeshData::eMorphTangents:
			return cuT( "MorphTangents" );
		case castor3d::SubmeshData::eMorphTexcoords:
			return cuT( "MorphTexcoords" );
		case castor3d::SubmeshData::eBones:
			return cuT( "Bones" );
		case castor3d::SubmeshData::eSecondaryUV:
			return cuT( "SecondaryUV" );
		default:
			CU_Failure( "Unsupported SubmeshData" );
			return cuT( "Unknown" );
		}
	}

	static constexpr uint32_t getSize( SubmeshData value )
	{
		constexpr size_t sizes[] = { sizeof( uint32_t ) /* SubmeshFlag::eIndex */
			, sizeof( castor::Point3f ) /* SubmeshFlag::ePositions */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eNormals */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eTangents */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eTexcoords */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphPositions */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphNormals */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTangents */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTexcoords */
			, sizeof( VertexBoneData ) /* SubmeshFlag::eBones */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eSecondaryUV */ };
		return uint32_t( sizes[size_t( value )] );
	}
	/**
	*\~english
	*\brief
	*	Flags to determine what a submesh is using.
	*\~french
	*\brief
	*	Indicateurs pour déterminer ce qu'un submesh utilise.
	*/
	enum class SubmeshFlag
		: uint32_t
	{
		eNone = 0x0000,
		//!\~english	The submesh has indices (always).
		//!\~french		Le submesh a des indices (toujours).
		eIndex = 0x0001 << int( SubmeshData::eIndex ),
		//!\~english	The submesh has positions.
		//!\~french		Le submesh a des positions.
		ePositions = 0x0001 << int( SubmeshData::ePositions ),
		//!\~english	The submesh has normals.
		//!\~french		Le submesh a des normales.
		eNormals = 0x0001 << int( SubmeshData::eNormals ),
		//!\~english	The submesh has tangents.
		//!\~french		Le submesh a des tangentes.
		eTangents = 0x0001 << int( SubmeshData::eTangents ),
		//!\~english	Submesh has texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture.
		eTexcoords = 0x0001 << int( SubmeshData::eTexcoords ),
		//!\~english	Base components flags.
		//\~french		Indicateurs de composants de base.
		ePosNmlTanTex = ePositions | eNormals | eTangents | eTexcoords,
		//!\~english	The submesh has morphing positions.
		//!\~french		Le submesh a des positions de morphing.
		eMorphPositions = 0x0001 << int( SubmeshData::eMorphPositions ),
		//!\~english	The submesh has morphing normals.
		//!\~french		Le submesh a des normales de morphing.
		eMorphNormals = 0x0001 << int( SubmeshData::eMorphNormals ),
		//!\~english	The submesh has morphing tangents.
		//!\~french		Le submesh a des tangentes de morphing.
		eMorphTangents = 0x0001 << int( SubmeshData::eMorphTangents ),
		//!\~english	The submesh has morphing texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture de morphing.
		eMorphTexcoords = 0x0001 << int( SubmeshData::eMorphTexcoords ),
		//!\~english	Submesh has bones data.
		//!\~french		Submesh a des données d'os.
		eBones = 0x0001 << int( SubmeshData::eBones ),
		//!\~english	Submesh has a second set of texture coordinates.
		//!\~french		Le submesh a un second set de coordonnées de texture.
		eSecondaryUV = 0x0001 << int( SubmeshData::eSecondaryUV ),
		//!\~english	All component flags.
		//\~french		Tous les indicateurs de composant.
		eAllComponents = 0x0FFF,
	};
	CU_ImplementFlags( SubmeshFlag )

	static constexpr SubmeshData getData( SubmeshFlag value )
	{
		return SubmeshData( castor::getBitSize( uint32_t( value ) ) - 1u );
	}

	static constexpr uint32_t getIndex( SubmeshFlag value )
	{
		return uint32_t( getData( value ) );
	}
	/**
	*\~english
	*\brief
	*	The submesh representation.
	*\remarks
	*	A submesh holds its buffers (vertex, normals and texture) and its combobox.
	*\~french
	*\brief
	*	Representation d'un sous-maillage.
	*\remarks
	*	Un sous-maillage est sous partie d'un maillage. Il possede ses propres tampons (vertex, normales et texture coords) et ses combobox.
	*/
	class Submesh;

	struct SubmeshAnimationBuffer
	{
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		castor::Point3fArray positions;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		castor::Point3fArray normals;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		castor::Point3fArray tangents;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		castor::Point3fArray texcoords;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox boundingBox{};
	};

	CU_DeclareSmartPtr( Submesh );

	//! Submesh pointer array
	CU_DeclareVector( SubmeshSPtr, SubmeshPtr );
	CU_DeclareMap( castor::String, SubmeshSPtr, SubmeshPtrStr );
	CU_DeclareMap( Submesh const *, castor::BoundingBox, SubmeshBoundingBox );
	CU_DeclareMap( Submesh const *, castor::BoundingSphere, SubmeshBoundingSphere );
	CU_DeclareMap( uint32_t, SubmeshAnimationBuffer, SubmeshAnimationBuffer );
	using SubmeshBoundingBoxList = std::vector< std::pair< Submesh const *, castor::BoundingBox > >;
	
	//@}
	//@}
	//@}
}

#endif
