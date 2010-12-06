#include "PrecompiledHeader.h"

#include "camera/Ray.h"
#include "camera/Viewport.h"
#include "camera/Camera.h"
#include "scene/NodeBase.h"
#include "geometry/primitives/Geometry.h"
#include "geometry/basic/Vertex.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"

using namespace Castor3D;

Ray :: Ray( const Point<int, 2> & p_point, const Camera & p_camera)
{
	Viewport l_viewport = * p_camera.GetViewport();
	m_origin = p_camera.GetParent()->GetPosition();
	m_origin[2] += l_viewport.GetNearView();
	Quaternion l_camOrient = p_camera.GetParent()->GetOrientation();
	m_origin = l_camOrient * m_origin;

	m_direction = l_viewport.GetDirection( p_point);
	m_direction.Normalise();
}

Ray :: Ray( int p_x, int p_y, const Camera & p_camera)
{
	Viewport l_viewport = * p_camera.GetViewport();
	m_origin = p_camera.GetParent()->GetPosition();
	m_origin[2] += l_viewport.GetNearView();
	Quaternion l_camOrient = p_camera.GetParent()->GetOrientation();
	m_origin = l_camOrient * m_origin;

	Point<int, 2> l_point( p_x, p_y);
	m_direction = l_viewport.GetDirection( l_point);
	m_direction.Normalise();
}

Ray :: Ray( const Point3r & p_rOrigin, const Point3r & p_rDirection)
	:	m_origin( p_rOrigin),
		m_direction( p_rDirection)
{
}

Ray :: Ray( const Ray & p_ray)
	:	m_origin( p_ray.m_origin),
		m_direction( p_ray.m_direction)
{
	m_direction.Normalise();
}

Ray :: ~Ray()
{
}

real Ray :: Intersects( const Point3r & p_v1, const Point3r & p_v2, const Point3r & p_v3)
{
	real l_fReturn = -1.0f;

	Point3r e1( p_v2 - p_v1);
	Point3r e2( p_v1 - p_v3);
	Point3r e3( p_v3 - p_v2);

	Point3r h = m_direction ^ e2;

	real a = e1.Dot( h);

	if ( ! value::is_null( a))
	{
		real f = 1.0f / a;

		Point3r s( m_origin - p_v1);
		real u = f * s.Dot( h);

		if (u >= 0.0 && u <= 1.0)
		{
			Point3r q = s ^ e1;
			real v = f * m_direction.Dot( q);

			if (v >= 0.0 && u + v <= 1.0)
			{
				real t = f * e2.Dot( q);

				if ( ! value::is_null( t))
				{
					l_fReturn = t;
				}
			}
		}
	}

	return l_fReturn;
}

real Ray :: Intersects( const Face & p_face)
{
	return Intersects( p_face[0].GetCoords(), p_face[1].GetCoords(), p_face[2].GetCoords());
}

real Ray :: Intersects( const Point3r & p_vertex)
{
	Point3r u( m_origin - p_vertex);
	Point3r puv;

	if (ProjectVertex( u, puv) && puv.GetSquaredLength() < 0.000001)
	{
		return puv.GetLength();
	}

	return -1.0f;
}

real Ray :: Intersects( const ComboBox & p_box)
{
	Point3r l_v1( p_box.GetMin());
	Point3r l_v8( p_box.GetMax());
	Point3r l_v2( l_v8[0], l_v1[1], l_v1[2]);
	Point3r l_v3( l_v1[0], l_v8[1], l_v1[2]);
	Point3r l_v4( l_v8[0], l_v8[1], l_v1[2]);
	Point3r l_v5( l_v8[0], l_v1[1], l_v8[2]);
	Point3r l_v6( l_v1[0], l_v8[1], l_v8[2]);
	Point3r l_v7( l_v8[0], l_v8[1], l_v8[2]);

	bool l_foundOne = false;
	real l_dist, l_min = 1e6f;

	if ((l_dist = Intersects( l_v1, l_v2, l_v3)) >= 0)
	{
		l_foundOne = true;
		l_min = l_dist;
	}

	if ((l_dist = Intersects( l_v2, l_v4, l_v3)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v2, l_v6, l_v4)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v6, l_v8, l_v4)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v6, l_v5, l_v8)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v5, l_v7, l_v8)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v1, l_v7, l_v5)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v1, l_v3, l_v7)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v2, l_v1, l_v5)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v2, l_v5, l_v6)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v3, l_v4, l_v7)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v4, l_v7, l_v8)) >= 0)
	{
		l_foundOne = true;
		l_min = std::min( l_min, l_dist);
	}

	if (l_foundOne)
	{
		return l_min;
	}

	return -1.0f;
}

real Ray :: Intersects( const Sphere & p_sphere)
{
	real l_fReturn = -1.0f;

	// intersection rayon/sphere 
	Point3r l_vDist = p_sphere.GetCenter() - m_origin; 
	real B = m_direction.Dot( l_vDist);
	real D = B * B - l_vDist.Dot(l_vDist) + p_sphere.GetRadius() * p_sphere.GetRadius(); 

	if (D >= 0.0f)
	{
		real t0 = B - square_root( D);
		real t1 = B + square_root( D);

		if (t0 > 0.1f)
		{
			l_fReturn = t0;
		} 

		if (t1 > 0.1f && t1 < l_fReturn)
		{
			l_fReturn = t1;
		}
	}

	return l_fReturn;
}

real Ray :: Intersects( Geometry * p_pGeometry, FacePtr* p_ppFace, SubmeshPtr* p_ppSubmesh)
{
	Point3r l_vCenter = p_pGeometry->GetCenter()->operator +( p_pGeometry->GetParent()->GetPosition());
	Quaternion l_qOrientation = p_pGeometry->GetParent()->GetOrientation();
	Point3r l_vOrientedCenter = l_qOrientation * l_vCenter;

	MeshPtr l_pMesh = p_pGeometry->GetMesh();
	Sphere l_sphere( l_vCenter, l_pMesh->GetSphere().GetRadius());

	real l_fDistance = Intersects( l_sphere);
/*
	real l_faceDist = 10e6, l_vertexDist = 10e6;
	real l_curfaceDist, l_curvertexDist;
*/
	if (l_fDistance >= 0.0f)
	{
		l_fDistance = -1.0f;
		size_t l_nbSubmeshes = l_pMesh->GetNbSubmeshes();
		SubmeshPtr l_pSubmesh;

		for (size_t i = 0 ; i < l_nbSubmeshes ; i++)
		{
			l_pSubmesh = l_pMesh->GetSubmesh( i);
			Point3r l_submeshCenter = l_vCenter + l_pSubmesh->GetSphere().GetCenter();
			l_submeshCenter = l_qOrientation * l_submeshCenter;
			l_sphere.Load( l_submeshCenter, l_pSubmesh->GetSphere().GetRadius());

			if (p_ppSubmesh != NULL)
			{
				*p_ppSubmesh = l_pSubmesh;
			}
/*
			if (Intersects( l_sphere) >= 0.0f)
			{
				size_t l_nbSmoothGroups = l_pSubmesh->GetNbSmoothGroups();

				for (size_t j = 0 ; j < l_nbSmoothGroups ; j++)
				{
					SmoothingGroupPtr l_pGroup = l_pSubmesh->GetSmoothGroup( j);
					for (size_t k = 0 ; k < l_pGroup->m_faces.size() ; k++)
					{
						FacePtr l_pFace = l_pGroup->m_faces[k];

						if ((l_curfaceDist = Intersects( * l_pFace)) >= 0.0 && l_curfaceDist < l_faceDist)
						{
							if (p_ppFace != NULL)
							{
								*p_ppFace = l_pFace;
							}

							if (p_ppSubmesh != NULL)
							{
								*p_ppSubmesh = l_pSubmesh;
							}

							l_faceDist = l_curfaceDist;
							l_fDistance = l_curfaceDist;

							if ((l_curvertexDist = Intersects( * l_pFace->m_vertex1)) >= 0.0 && l_curvertexDist < l_fDistance)
							{
								l_fDistance = l_curvertexDist;
							}

							if ((l_curvertexDist = Intersects( * l_pFace->m_vertex2)) >= 0.0 && l_curvertexDist < l_fDistance)
							{
								l_fDistance = l_curvertexDist;
							}

							if ((l_curvertexDist = Intersects( * l_pFace->m_vertex2)) >= 0.0 && l_curvertexDist < l_fDistance)
							{
								l_fDistance = l_curvertexDist;
							}
						}
					}
				}
			}
*/
		}
	}

	return l_fDistance;
}

bool Ray :: ProjectVertex( const Point3r & p_vertex, Point3r & p_result)
{
	if (m_direction.Dot( p_vertex) >= 0.0)
	{
		p_result = m_direction * (m_direction.Dot( p_vertex) / m_direction.GetLength());
		return true;
	}

	return false;
}