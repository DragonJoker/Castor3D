/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InterleavedVertex_H___
#define ___C3D_InterleavedVertex_H___

#include "ModelModule.hpp"

namespace castor3d
{
	template< typename T >
	struct InterleavedVertexT
	{
		InterleavedVertexT & position( castor::Point3< T > const & pos )
		{
			this->pos = pos;
			return *this;
		}

		InterleavedVertexT & normal( castor::Point3< T > const & nml )
		{
			this->nml = nml;
			return *this;
		}

		InterleavedVertexT & texcoord( castor::Point2< T > const & tex )
		{
			this->tex = castor::Point3< T >{ tex };
			return *this;
		}

		InterleavedVertexT & texcoord( castor::Point3< T > const & tex )
		{
			this->tex = tex;
			return *this;
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
}

#endif
