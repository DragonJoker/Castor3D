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
		static InterleavedVertexT createP( castor::Point3< T > const & pos )
		{
			InterleavedVertexT result;
			result.pos = pos;
			return result;
		}

		static InterleavedVertexT createPN( castor::Point3< T > const & pos
			, castor::Point3< T > const & nml )
		{
			InterleavedVertexT result;
			result.pos = pos;
			result.nml = nml;
			return result;
		}

		static InterleavedVertexT createPT( castor::Point3< T > const & pos
			, castor::Point3< T > const & tex )
		{
			InterleavedVertexT result;
			result.pos = pos;
			result.tex = tex;
			return result;
		}

		static InterleavedVertexT createPT( castor::Point3< T > const & pos
			, castor::Point2< T > const & tex )
		{
			InterleavedVertexT result;
			result.pos = pos;
			result.tex = castor::Point3< T >{ tex };
			return result;
		}

		static InterleavedVertexT createPNT( castor::Point3< T > const & pos
			, castor::Point3< T > const & nml
			, castor::Point3< T > const & tex )
		{
			InterleavedVertexT result;
			result.pos = pos;
			result.nml = nml;
			result.tex = tex;
			return result;
		}

		static InterleavedVertexT createPNT( castor::Point3< T > const & pos
			, castor::Point3< T > const & nml
			, castor::Point2< T > const & tex )
		{
			InterleavedVertexT result;
			result.pos = pos;
			result.nml = nml;
			result.tex = castor::Point3< T >{ tex };
			return result;
		}

		//!\~english	The vertex coordinates.
		//!\~french		La position du sommet.
		castor::Point3< T > pos;
		//!\~english	The vertex normal.
		//!\~french		La normale du sommet.
		castor::Point3< T > nml;
		//!\~english	The vertex tangent.
		//!\~french		La tangente du sommet.
		castor::Point3< T > tan;
		//!\~english	The vertex texture coordinates.
		//!\~french		La coordonnées de texture du sommet.
		castor::Point3< T > tex;
	};

	template< typename T >
	using InterleavedVertexTArray = std::vector< InterleavedVertexT< T > >;

	using InterleavedVertex = InterleavedVertexT< float >;
	using InterleavedVertexArray = InterleavedVertexTArray< float >;
}

#endif
