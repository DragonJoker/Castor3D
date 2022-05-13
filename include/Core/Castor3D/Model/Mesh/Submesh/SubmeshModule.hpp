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
		eTexcoords0,
		//!\~english	Submesh has a 2nd set of texture coordinates.
		//!\~french		Le submesh a un 2e set de coordonnées de texture.
		eTexcoords1,
		//!\~english	Submesh has a 3rd set of texture coordinates.
		//!\~french		Le submesh a un 3e set de coordonnées de texture.
		eTexcoords2,
		//!\~english	Submesh has a 4th set of texture coordinates.
		//!\~french		Le submesh a un 4e set de coordonnées de texture.
		eTexcoords3,
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
		eMorphTexcoords0,
		//!\~english	The submesh has a 2nd set of morphing texture coordinates.
		//!\~french		Le submesh a un 2e set de coordonnées de texture de morphing.
		eMorphTexcoords1,
		//!\~english	The submesh has a 3rd set of morphing texture coordinates.
		//!\~french		Le submesh a un 3e set de coordonnées de texture de morphing.
		eMorphTexcoords2,
		//!\~english	The submesh has a 4th set of morphing texture coordinates.
		//!\~french		Le submesh a un 4e set de coordonnées de texture de morphing.
		eMorphTexcoords3,
		//!\~english	Submesh has bones data.
		//!\~french		Submesh a des données d'os.
		eBones,
	};

	inline castor::String getName( SubmeshData value )
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
		case castor3d::SubmeshData::eTexcoords0:
			return cuT( "Texcoords0" );
		case castor3d::SubmeshData::eTexcoords1:
			return cuT( "eTexcoords1" );
		case castor3d::SubmeshData::eTexcoords2:
			return cuT( "eTexcoords2" );
		case castor3d::SubmeshData::eTexcoords3:
			return cuT( "eTexcoords3" );
		case castor3d::SubmeshData::eMorphPositions:
			return cuT( "MorphPositions" );
		case castor3d::SubmeshData::eMorphNormals:
			return cuT( "MorphNormals" );
		case castor3d::SubmeshData::eMorphTangents:
			return cuT( "MorphTangents" );
		case castor3d::SubmeshData::eMorphTexcoords0:
			return cuT( "MorphTexcoords0" );
		case castor3d::SubmeshData::eMorphTexcoords1:
			return cuT( "MorphTexcoords1" );
		case castor3d::SubmeshData::eMorphTexcoords2:
			return cuT( "MorphTexcoords2" );
		case castor3d::SubmeshData::eMorphTexcoords3:
			return cuT( "MorphTexcoords3" );
		case castor3d::SubmeshData::eBones:
			return cuT( "Bones" );
		default:
			CU_Failure( "Unsupported SubmeshData" );
			return cuT( "Unknown" );
		}
	}

	inline constexpr uint32_t getSize( SubmeshData value )
	{
		constexpr size_t sizes[] = { sizeof( uint32_t ) /* SubmeshFlag::eIndex */
			, sizeof( castor::Point3f ) /* SubmeshFlag::ePositions */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eNormals */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eTangents */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eTexcoords0 */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eTexcoords1 */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eTexcoords2 */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eTexcoords3 */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphPositions */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphNormals */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTangents */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTexcoords0 */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTexcoords1 */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTexcoords2 */
			, sizeof( castor::Point3f ) /* SubmeshFlag::eMorphTexcoords3 */
			, sizeof( VertexBoneData ) /* SubmeshFlag::eBones */ };
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
		eTexcoords0 = 0x0001 << int( SubmeshData::eTexcoords0 ),
		//!\~english	Submesh has a 2nd set of texture coordinates.
		//!\~french		Le submesh a un 2e set de coordonnées de texture.
		eTexcoords1 = 0x0001 << int( SubmeshData::eTexcoords1 ),
		//!\~english	Submesh has a 3rd set of texture coordinates.
		//!\~french		Le submesh a un 3e set de coordonnées de texture.
		eTexcoords2 = 0x0001 << int( SubmeshData::eTexcoords2 ),
		//!\~english	Submesh has a 4th set of texture coordinates.
		//!\~french		Le submesh a un 4e set de coordonnées de texture.
		eTexcoords3 = 0x0001 << int( SubmeshData::eTexcoords3 ),
		//!\~english	Base components flags.
		//\~french		Indicateurs de composants de base.
		ePosNmlTanTex = ePositions | eNormals | eTangents | eTexcoords0,
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
		eMorphTexcoords0 = 0x0001 << int( SubmeshData::eMorphTexcoords0 ),
		//!\~english	Submesh has a 2nd set of morphing texture coordinates.
		//!\~french		Le submesh a un 2e set de coordonnées de texture de morphing.
		eMorphTexcoords1 = 0x0001 << int( SubmeshData::eMorphTexcoords1 ),
		//!\~english	Submesh has a 3rd set of morphing texture coordinates.
		//!\~french		Le submesh a un 3e set de coordonnées de texture de morphing.
		eMorphTexcoords2 = 0x0001 << int( SubmeshData::eMorphTexcoords2 ),
		//!\~english	Submesh has a 4th set of morphing texture coordinates.
		//!\~french		Le submesh a un 4e set de coordonnées de texture de morphing.
		eMorphTexcoords3 = 0x0001 << int( SubmeshData::eMorphTexcoords3 ),
		//!\~english	Submesh has bones data.
		//!\~french		Submesh a des données d'os.
		eBones = 0x0001 << int( SubmeshData::eBones ),
		//!\~english	All component flags.
		//\~french		Tous les indicateurs de composant.
		eAllComponents = 0x1FFFF,
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
		//!\~english	The positions buffer.
		//!\~french		Le tampon de positions.
		castor::Point3fArray positions{};
		//!\~english	The normals buffer.
		//!\~french		Le tampon de normales.
		castor::Point3fArray normals{};
		//!\~english	The tangents buffer.
		//!\~french		Le tampon de tangentes.
		castor::Point3fArray tangents{};
		//!\~english	The texture coordinates buffer.
		//!\~french		Le tampon de coordonnées de texture.
		castor::Point3fArray texcoords0{};
		//!\~english	The 2nd texture coordinates buffer.
		//!\~french		Le 2e tampon de coordonnées de texture.
		castor::Point3fArray texcoords1{};
		//!\~english	The 3rd texture coordinates buffer.
		//!\~french		Le 3e tampon de coordonnées de texture.
		castor::Point3fArray texcoords2{};
		//!\~english	The 4th texture coordinates buffer.
		//!\~french		Le 4e tampon de coordonnées de texture.
		castor::Point3fArray texcoords3{};
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
