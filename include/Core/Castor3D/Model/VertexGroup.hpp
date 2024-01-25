/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InterleavedVertex_H___
#define ___C3D_InterleavedVertex_H___

#include "ModelModule.hpp"

namespace castor3d
{
	struct InterleavedVertex
	{
		InterleavedVertex()= default;

		explicit InterleavedVertex( castor::Point3f pos
			, castor::Point3f nml = {}
			, castor::Point4f tan = {}
			, castor::Point3f tex = {} )
			: pos{ castor::move( pos ) }
			, nml{ castor::move( nml ) }
			, tan{ castor::move( tan ) }
			, tex{ castor::move( tex ) }
		{
		}

		InterleavedVertex & position( castor::Point3f const & val )
		{
			pos = val;
			return *this;
		}

		InterleavedVertex & normal( castor::Point3f const & val )
		{
			nml = val;
			return *this;
		}

		InterleavedVertex & texcoord( castor::Point2f const & val )
		{
			tex = castor::Point3f{ val };
			return *this;
		}

		InterleavedVertex & texcoord( castor::Point3f const & val )
		{
			tex = val;
			return *this;
		}

		//!\~english	The vertex coordinates.
		//!\~french		La position du sommet.
		castor::Point3f pos{};
		//!\~english	The vertex normal.
		//!\~french		La normale du sommet.
		castor::Point3f nml{};
		//!\~english	The vertex tangent.
		//!\~french		La tangente du sommet.
		castor::Point4f tan{};
		//!\~english	The vertex texture coordinates.
		//!\~french		La coordonnées de texture du sommet.
		castor::Point3f tex{};
	};
}

#endif
