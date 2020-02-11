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

	template< typename T >
	struct SubmeshAnimationBufferT
	{
		InterleavedVertexTArray< T > m_buffer;
		float m_timeIndex;
	};
	using SubmeshAnimationBuffer = SubmeshAnimationBufferT< float >;

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
