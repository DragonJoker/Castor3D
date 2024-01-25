/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshSubmeshModule_H___
#define ___C3D_ModelMeshSubmeshModule_H___

#include "Castor3D/Model/Mesh/MeshModule.hpp"

#include "Castor3D/Limits.hpp"
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

	struct PassMasks
	{
		castor::Array< uint8_t, MaxPassLayers > data;
	};
	/**
	*\~english
	*\brief
	*	Lists the supported data kinds for a submesh.
	*\~french
	*\brief
	*	Liste les types de données supportés par un submesh.
	*/
	enum class SubmeshData
		: uint8_t
	{
		//!\~english	The submesh has indices.
		//!\~french		Le submesh a des indices.
		eIndex = 0,
		//!\~english	The submesh has positions.
		//!\~french		Le submesh a des positions.
		ePositions = 1,
		//!\~english	The submesh has normals.
		//!\~french		Le submesh a des normales.
		eNormals = 2,
		//!\~english	The submesh has tangents.
		//!\~french		Le submesh a des tangentes.
		eTangents = 3,
		//!\~english	The submesh has bitangents.
		//!\~french		Le submesh a des bitangentes.
		eBitangents = 4,
		//!\~english	Submesh has texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture.
		eTexcoords0 = 5,
		//!\~english	Submesh has a 2nd set of texture coordinates.
		//!\~french		Le submesh a un 2e set de coordonnées de texture.
		eTexcoords1 = 6,
		//!\~english	Submesh has a 3rd set of texture coordinates.
		//!\~french		Le submesh a un 3e set de coordonnées de texture.
		eTexcoords2 = 7,
		//!\~english	Submesh has a 4th set of texture coordinates.
		//!\~french		Le submesh a un 4e set de coordonnées de texture.
		eTexcoords3 = 8,
		//!\~english	Submesh has colours.
		//!\~french		Le submesh a des couleurs.
		eColours = 9,
		//!\~english	Minimum value for components not derived from BaseDataComponentT.
		//!\~french		Valeur minimale pour les composants n'héritant pas de BaseDataComponentT.
		eOtherMin = 10,
		//!\~english	Submesh has skin data.
		//!\~french		Le submesh a des données de skin.
		eSkin = eOtherMin,
		//!\~english	Submesh has subpasses masks.
		//!\~french		Le submesh a des masques de subpasses.
		ePassMasks = 11,
		//!\~english	Submesh has velocity data.
		//!\~french		Le submesh a des données de vélocité.
		eVelocity = 12,
		//!\~english	Submesh has meshlets.
		//!\~french		Le submesh a des meshlets.
		eMeshlets = 13,
		CU_ScopedEnumBounds( eIndex, eMeshlets ),
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
		case castor3d::SubmeshData::eBitangents:
			return cuT( "Bitangents" );
		case castor3d::SubmeshData::eTexcoords0:
			return cuT( "Texcoords0" );
		case castor3d::SubmeshData::eTexcoords1:
			return cuT( "Texcoords1" );
		case castor3d::SubmeshData::eTexcoords2:
			return cuT( "Texcoords2" );
		case castor3d::SubmeshData::eTexcoords3:
			return cuT( "Texcoords3" );
		case castor3d::SubmeshData::eColours:
			return cuT( "Colours" );
		case castor3d::SubmeshData::eSkin:
			return cuT( "Skin" );
		case castor3d::SubmeshData::ePassMasks:
			return cuT( "PassMasks" );
		case castor3d::SubmeshData::eVelocity:
			return cuT( "Velocity" );
		case castor3d::SubmeshData::eMeshlets:
			return cuT( "Meshlets" );
		default:
			CU_Failure( "Unsupported SubmeshData" );
			return cuT( "Unknown" );
		}
	}

	constexpr uint32_t getSize( SubmeshData value )
	{
		constexpr castor::Array< size_t, size_t( SubmeshData::eCount ) > sizes = { sizeof( uint32_t ) /* SubmeshData::eIndex */
			, sizeof( castor::Point4f ) /* SubmeshData::ePositions */
			, sizeof( castor::Point4f ) /* SubmeshData::eNormals */
			, sizeof( castor::Point4f ) /* SubmeshData::eTangents */
			, sizeof( castor::Point4f ) /* SubmeshData::eBitangents */
			, sizeof( castor::Point4f ) /* SubmeshData::eTexcoords0 */
			, sizeof( castor::Point4f ) /* SubmeshData::eTexcoords1 */
			, sizeof( castor::Point4f ) /* SubmeshData::eTexcoords2 */
			, sizeof( castor::Point4f ) /* SubmeshData::eTexcoords3 */
			, sizeof( castor::Point4f ) /* SubmeshData::eColours */
			, sizeof( VertexBoneData ) /* SubmeshData::eSkin */
			, sizeof( castor::Point4ui ) /* SubmeshData::ePassMasks */
			, sizeof( castor::Point4f ) /* SubmeshData::eVelocity */
			, sizeof( Meshlet ) /* SubmeshData::eMeshlets */ };
		return uint32_t( sizes[size_t( value )] );
	}

	using SubmeshComponentID = uint32_t;
	using SubmeshComponentCombineID = uint16_t;
	using SubmeshComponentFlag = uint32_t;

	constexpr SubmeshComponentFlag makeSubmeshComponentFlag( SubmeshComponentID componentId )noexcept
	{
		return uint32_t( componentId );
	}

	constexpr SubmeshComponentID splitSubmeshComponentFlag( SubmeshComponentFlag flag )noexcept
	{
		return SubmeshComponentID( flag );
	}

	using SubmeshComponentFlagsSet = castor::Set< SubmeshComponentFlag >;
	/**
	*\~english
	*\brief
	*	Lists the supported morphing kinds for a submesh.
	*\~french
	*\brief
	*	Liste les types de morphing supportés par un submesh.
	*/
	enum class MorphData
		: uint32_t
	{
		//!\~english	The submesh has morphing positions.
		//!\~french		Le submesh a des positions de morphing.
		ePositions = 0,
		//!\~english	The submesh has morphing normals.
		//!\~french		Le submesh a des normales de morphing.
		eNormals = 1,
		//!\~english	The submesh has morphing tangents.
		//!\~french		Le submesh a des tangentes de morphing.
		eTangents = 2,
		//!\~english	The submesh has morphing bitangents.
		//!\~french		Le submesh a des bitangentes de morphing.
		eBitangents = 3,
		//!\~english	The submesh has morphing texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture de morphing.
		eTexcoords0 = 4,
		//!\~english	The submesh has a 2nd set of morphing texture coordinates.
		//!\~french		Le submesh a un 2e set de coordonnées de texture de morphing.
		eTexcoords1 = 5,
		//!\~english	The submesh has a 3rd set of morphing texture coordinates.
		//!\~french		Le submesh a un 3e set de coordonnées de texture de morphing.
		eTexcoords2 = 6,
		//!\~english	The submesh has a 4th set of morphing texture coordinates.
		//!\~french		Le submesh a un 4e set de coordonnées de texture de morphing.
		eTexcoords3 = 7,
		//!\~english	Submesh has a morphing colours.
		//!\~french		Le submesh a des couleurs de morphing.
		eColours = 8,
		CU_ScopedEnumBounds( ePositions, eColours ),
	};

	inline castor::String getName( MorphData value )
	{
		switch ( value )
		{
		case castor3d::MorphData::ePositions:
			return cuT( "MorphPositions" );
		case castor3d::MorphData::eNormals:
			return cuT( "MorphNormals" );
		case castor3d::MorphData::eTangents:
			return cuT( "MorphTangents" );
		case castor3d::MorphData::eBitangents:
			return cuT( "MorphBitangents" );
		case castor3d::MorphData::eTexcoords0:
			return cuT( "MorphTexcoords0" );
		case castor3d::MorphData::eTexcoords1:
			return cuT( "MorphTexcoords1" );
		case castor3d::MorphData::eTexcoords2:
			return cuT( "MorphTexcoords2" );
		case castor3d::MorphData::eTexcoords3:
			return cuT( "MorphTexcoords3" );
		case castor3d::MorphData::eColours:
			return cuT( "MorphColours" );
		default:
			CU_Failure( "Unsupported MorphData" );
			return cuT( "Unknown" );
		}
	}
	/**
	*\~english
	*\brief
	*	Flags to determine what morphing is enabled on a submesh.
	*\~french
	*\brief
	*	Indicateurs pour déterminer le morphing utilisé sur un submesh.
	*/
	enum class MorphFlag
		: uint32_t
	{
		eNone = 0x00,
		//!\~english	The submesh has morphing positions.
		//!\~french		Le submesh a des positions de morphing.
		ePositions = 0x01 << size_t( MorphData::ePositions ),
		//!\~english	The submesh has morphing normals.
		//!\~french		Le submesh a des normales de morphing.
		eNormals = 0x01 << size_t( MorphData::eNormals ),
		//!\~english	The submesh has morphing tangents.
		//!\~french		Le submesh a des tangentes de morphing.
		eTangents = 0x01 << size_t( MorphData::eTangents ),
		//!\~english	The submesh has morphing tangents.
		//!\~french		Le submesh a des tangentes de morphing.
		eBitangents = 0x01 << size_t( MorphData::eBitangents),
		//!\~english	The submesh has morphing texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture de morphing.
		eTexcoords0 = 0x01 << size_t( MorphData::eTexcoords0 ),
		//!\~english	The submesh has a 2nd set of morphing texture coordinates.
		//!\~french		Le submesh a un 2e set de coordonnées de texture de morphing.
		eTexcoords1 = 0x01 << size_t( MorphData::eTexcoords1 ),
		//!\~english	The submesh has a 3rd set of morphing texture coordinates.
		//!\~french		Le submesh a un 3e set de coordonnées de texture de morphing.
		eTexcoords2 = 0x01 << size_t( MorphData::eTexcoords2 ),
		//!\~english	The submesh has a 4th set of morphing texture coordinates.
		//!\~french		Le submesh a un 4e set de coordonnées de texture de morphing.
		eTexcoords3 = 0x01 << size_t( MorphData::eTexcoords3 ),
		//!\~english	Submesh has morphing colours.
		//!\~french		Le submesh a des couleurs de morphing.
		eColours = 0x01 << size_t( MorphData::eColours ),
		//!\~english	All flags used in base pipeline flags hashing.
		//\~french		Tous les indicateurs utilisés dans le hash des indicateurs de pipeline.
		eAllBase = ( 0x0001 << int( MorphData::eCount ) ) - 1,
	};
	CU_ImplementFlags( MorphFlag )
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
	/**
	\~english
	\brief		Component for a submesh.
	\~french
	\brief		Composant pour un sous-maillage.
	*/
	class SubmeshComponent;

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
		castor::Point4fArray tangents{};
		//!\~english	The tangents buffer.
		//!\~french		Le tampon de tangentes.
		castor::Point3fArray bitangents{};
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
		//!\~english	The colours buffer.
		//!\~french		Le tampon de couleurs.
		castor::Point3fArray colours{};
		//!\~english	The subpasses masks buffer.
		//!\~french		Le tampon de masques de subpasses.
		castor::Vector< PassMasks > passMasks{};
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox boundingBox{};
	};

	CU_DeclareSmartPtr( castor3d, Submesh, C3D_API );

	//! Submesh pointer array
	CU_DeclareVector( SubmeshUPtr, SubmeshPtr );
	CU_DeclareMap( Submesh const *, castor::BoundingBox, SubmeshBoundingBox );
	CU_DeclareMap( Submesh const *, castor::BoundingSphere, SubmeshBoundingSphere );
	CU_DeclareMap( uint32_t, castor::Vector< float >, SubmeshTargetWeight );
	using SubmeshBoundingBoxList = castor::Vector< castor::Pair< Submesh const *, castor::BoundingBox > >;

	C3D_API Submesh * getComponentSubmesh( SubmeshComponent const & component );
	C3D_API castor::String const & getSubmeshComponentType( SubmeshComponent const & component );
	
	//@}
	//@}
	//@}
}

#endif
