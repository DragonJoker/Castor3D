/*
See LICENSE file in root folder
*/
#ifndef ___C3D_InterleavedVertex_H___
#define ___C3D_InterleavedVertex_H___

#include "ModelModule.hpp"

namespace c3d
{
	struct InterleavedVertex
	{
		InterleavedVertex()= default;

		explicit InterleavedVertex( Point3f pos
			, Point3f nml = {}
			, Point4f tan = {}
			, Point3f tex = {} )
			: pos{ c3d::move( pos ) }
			, nml{ c3d::move( nml ) }
			, tan{ c3d::move( tan ) }
			, tex{ c3d::move( tex ) }
		{
		}

		InterleavedVertex & position( Point3f const & val )
		{
			pos = val;
			return *this;
		}

		InterleavedVertex & normal( Point3f const & val )
		{
			nml = val;
			return *this;
		}

		InterleavedVertex & texcoord( Point2f const & val )
		{
			tex = Point3f{ val };
			return *this;
		}

		InterleavedVertex & texcoord( Point3f const & val )
		{
			tex = val;
			return *this;
		}

		//!\~english	The vertex coordinates.
		//!\~french		La position du sommet.
		Point3f pos{};
		//!\~english	The vertex normal.
		//!\~french		La normale du sommet.
		Point3f nml{};
		//!\~english	The vertex tangent.
		//!\~french		La tangente du sommet.
		Point4f tan{};
		//!\~english	The vertex texture coordinates.
		//!\~french		La coordonnées de texture du sommet.
		Point3f tex{};
	};
}

#endif
