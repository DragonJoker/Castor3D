#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/camera/Ray.h"
#include "Castor3D/camera/Viewport.h"
#include "Castor3D/camera/Camera.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/geometry/primitives/Geometry.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/mesh/Mesh.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/basic/Face.h"

using namespace Castor3D;

Ray :: Ray( const Point2i & p_point, const Camera & p_camera)
{
	Viewport l_viewport = * p_camera.GetViewport();
	m_ptOrigin = p_camera.GetParent()->GetPosition();
	m_ptOrigin[2] += l_viewport.GetNearView();
	Quaternion l_camOrient = p_camera.GetParent()->GetOrientation();
	m_ptOrigin = l_camOrient * m_ptOrigin;

	m_ptDirection = l_viewport.GetDirection( p_point);
	m_ptDirection.Normalise();
}

Ray :: Ray( int p_iX, int p_iY, const Camera & p_camera)
{
	Viewport l_viewport = * p_camera.GetViewport();
	m_ptOrigin = p_camera.GetParent()->GetPosition();
	m_ptOrigin[2] += l_viewport.GetNearView();
	Quaternion l_camOrient = p_camera.GetParent()->GetOrientation();
	m_ptOrigin = l_camOrient * m_ptOrigin;

	Point2i l_point( p_iX, p_iY);
	m_ptDirection = l_viewport.GetDirection( l_point);
	m_ptDirection.Normalise();
}

Ray :: Ray( const Point3r & p_ptOrigin, const Point3r & p_ptDirection)
	:	m_ptOrigin( p_ptOrigin)
	,	m_ptDirection( p_ptDirection)
{
}

Ray :: Ray( const Ray & p_ray)
	:	m_ptOrigin( p_ray.m_ptOrigin)
	,	m_ptDirection( p_ray.m_ptDirection)
{
	m_ptDirection.Normalise();
}

Ray :: ~Ray()
{
}

real Ray :: Intersects( const Point3r & p_pt1, const Point3r & p_pt2, const Point3r & p_pt3)
{
	real l_fReturn = -1.0f;

	Point3r e1( p_pt2 - p_pt1);
	Point3r e2( p_pt1 - p_pt3);
	Point3r e3( p_pt3 - p_pt2);

	Point3r h = m_ptDirection ^ e2;

	real a = e1.Dot( h);

	if ( ! policy::is_null( a))
	{
		real f = 1.0f / a;

		Point3r s( m_ptOrigin - p_pt1);
		real u = f * s.Dot( h);

		if (u >= 0.0 && u <= 1.0)
		{
			Point3r q = s ^ e1;
			real v = f * m_ptDirection.Dot( q);

			if (v >= 0.0 && u + v <= 1.0)
			{
				real t = f * e2.Dot( q);

				if ( ! policy::is_null( t))
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
	Point3r u( m_ptOrigin - p_vertex);
	Point3r puv;

	if (ProjectVertex( u, puv) && puv.GetSquaredLength() < 0.000001)
	{
		return puv.GetLength();
	}

	return -1.0f;
}

real Ray :: Intersects( const CubeBox & p_box)
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

real Ray :: Intersects( const SphereBox & p_sphere)
{
	real l_fReturn = -1.0f;

	// intersection rayon/sphere 
	Point3r l_vDist = p_sphere.GetCenter() - m_ptOrigin; 
	real B = m_ptDirection.Dot( l_vDist);
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
	SphereBox l_sphere( l_vCenter, l_pMesh->GetSphere().GetRadius());

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

bool Ray :: ProjectVertex( const Point3r & p_point, Point3r & p_result)
{
	if (m_ptDirection.Dot( p_point) >= 0.0)
	{
		p_result = m_ptDirection * (m_ptDirection.Dot( p_point) / m_ptDirection.GetLength());
		return true;
	}

	return false;
}