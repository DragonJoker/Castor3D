/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelMeshSubmeshModule_H___
#define ___C3D_ModelMeshSubmeshModule_H___

#include "Castor3D/Model/Mesh/MeshModule.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Graphics/BoundingSphere.hpp>

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
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x0000,
		//!\~english	The submesh has tangents.
		//!\~french		Le submesh a des tangentes.
		eTangents = 0x0001,
		//!\~english	Submesh has texture coordinates.
		//!\~french		Le submesh a des coordonnées de texture.
		eTexcoords = 0x0002,
		//!\~english	Submesh has a second set of texture coordinates.
		//!\~french		Le submesh a un second set de coordonnées de texture.
		eSecondaryUV = 0x0004,
		//!\~english	Submesh using instanciation.
		//!\~french		Submesh utilisant l'instanciation.
		eInstantiation = 0x0008,
		//!\~english	Submesh using skeleton animations.
		//!\~french		Submesh utilisant les animations par squelette.
		eSkinning = 0x0010,
		//!\~english	Submesh using per-vertex animations.
		//!\~french		Submesh utilisant les animations par sommet.
		eMorphing = 0x0020,
		//!\~english	Forces texcoords binding.
		//\~french		Force le binding des UV.
		eForceTexCoords = 0x0040,
		//!\~english	All flags.
		//\~french		Tous les indicateurs.
		eAll = 0x007F,
	};
	CU_ImplementFlags( SubmeshFlag )
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
		InterleavedVertexArray buffer;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox boundingBox;
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
