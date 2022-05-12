/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshSubmeshModule_H___
#define ___C3D_ModelMeshSubmeshModule_H___

#include "Castor3D/Model/Mesh/MeshModule.hpp"

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
	*	Flags to determine what a submesh is using.
	*\~french
	*\brief
	*	Indicateurs pour déterminer ce qu'un submesh utilise.
	*/
	enum class SubmeshFlag
		: uint32_t
	{
		//!\~english	The submesh has indices (always).
		//!\~french		Le submesh a des indices (toujours).
		eIndex = 0x0000,
		//!\~english	The submesh has positions.
		//!\~french		Le submesh a des positions.
		ePositions = 0x0001,
		//!\~english	The submesh has normals.
		//!\~french		Le submesh a des normales.
		eNormals = 0x0002,
		//!\~english	The submesh has tangents.
		//!\~french		Le submesh a des tangentes.
		eTangents = 0x0004,
		//!\~english	Submesh has texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture.
		eTexcoords = 0x0008,
		//!\~english	Base components flags.
		//\~french		Indicateurs de composants de base.
		ePosNmlTanTex = ePositions | eNormals | eTangents | eTexcoords,
		//!\~english	The submesh has morphing positions.
		//!\~french		Le submesh a des positions de morphing.
		eMorphPositions = 0x0010,
		//!\~english	The submesh has morphing normals.
		//!\~french		Le submesh a des normales de morphing.
		eMorphNormals = 0x0020,
		//!\~english	The submesh has morphing tangents.
		//!\~french		Le submesh a des tangentes de morphing.
		eMorphTangents = 0x0040,
		//!\~english	The submesh has morphing texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture de morphing.
		eMorphTexcoords = 0x0080,
		//!\~english	Submesh has bones data.
		//!\~french		Submesh a des données d'os.
		eBones = 0x0100,
		//!\~english	Submesh has a second set of texture coordinates.
		//!\~french		Le submesh a un second set de coordonnées de texture.
		eSecondaryUV = 0x0200,
		//!\~english	All component flags.
		//\~french		Tous les indicateurs de composant.
		eAllComponents = 0x07FF,
	};
	CU_ImplementFlags( SubmeshFlag )

	static constexpr uint32_t getIndex( SubmeshFlag value )
	{
		return uint32_t( castor::getBitSize( uint32_t( value ) ) );
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
