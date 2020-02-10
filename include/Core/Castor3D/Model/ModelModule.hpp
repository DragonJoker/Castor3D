/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelModule_H___
#define ___C3D_ModelModule_H___

#include "Castor3D/Castor3DModule.hpp"

namespace castor3d
{
	/**@name Model */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\date 		23/07/2012
	\~english
	\brief		Holds the all components of a vertex.
	\~french
	\brief		Contient toutes les composantes d'un sommet.
	*/
	template< typename T >
	struct InterleavedVertexT;
	/*!
	\author		Sylvain DOREMUS
	\version	0.11.0
	\date		17/01/2018
	\~english
	\brief		Representation of a vertex inside a submesh.
	\~fench
	\brief		Représentation d'un sommet, dans un sous-maillage.
	*/
	struct SubmeshVertex;

	//! SubmeshVertex array.
	CU_DeclareVector( SubmeshVertex, SubmeshVertex );

	template< typename T >
	using InterleavedVertexTArray = std::vector< InterleavedVertexT< T > >;

	using InterleavedVertex = InterleavedVertexT< float >;
	using InterleavedVertexArray = InterleavedVertexTArray< float >;

	//@}
}

#endif
