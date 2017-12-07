/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VERTEX_GROUP_H___
#define ___C3D_VERTEX_GROUP_H___

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		23/07/2012
	\~english
	\brief		Holds the all components of a vertex.
	\~french
	\brief		Contient toutes les composantes d'un sommet.
	*/
	template< typename T >
	struct InterleavedVertexT
	{
		//!\~english	The vertex coordinates.
		//!\~french		La position du sommet.
		std::array< T, 3 > m_pos;
		//!\~english	The vertex normal.
		//!\~french		La normale du sommet.
		std::array< T, 3 > m_nml;
		//!\~english	The vertex tangent.
		//!\~french		La tangente du sommet.
		std::array< T, 3 > m_tan;
		//!\~english	The vertex bitangent.
		//!\~french		La bitangente du sommet.
		std::array< T, 3 > m_bin;
		//!\~english	The vertex texture coordinates.
		//!\~french		La coordonnées de texture du sommet.
		std::array< T, 3 > m_tex;
	};

	template< typename T >
	using InterleavedVertexTArray = std::vector< InterleavedVertexT< T > >;

	using InterleavedVertex = InterleavedVertexT< real >;
	using InterleavedVertexArray = InterleavedVertexTArray< real >;
}

#endif
