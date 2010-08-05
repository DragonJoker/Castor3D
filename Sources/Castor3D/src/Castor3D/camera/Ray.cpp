#include "PrecompiledHeader.h"

#include "camera/Ray.h"
#include "camera/Viewport.h"
#include "camera/Camera.h"
#include "scene/SceneNode.h"
#include "geometry/primitives/Geometry.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"

using namespace Castor3D;

Ray :: Ray( const Point2D<int> & p_point, const Camera & p_camera)
	:	m_origin( NULL),
		m_direction( NULL)
{
	Viewport l_viewport = *p_camera.GetViewport();
	m_origin = new Vector3f( *p_camera.GetPosition());
	m_origin->z += l_viewport.GetNearView();
	Quaternion l_camOrient = *p_camera.GetOrientation();
	*m_origin = l_camOrient * (*m_origin);

	m_direction = new Vector3f( l_viewport.GetDirection( p_point));
	m_direction->Normalise();
}

Ray :: Ray( int p_x, int p_y, const Camera & p_camera)
	:	m_origin( NULL),
		m_direction( NULL)
{
	Viewport l_viewport = *p_camera.GetViewport();
	m_origin = new Vector3f( *p_camera.GetPosition());
	m_origin->z += l_viewport.GetNearView();
	Quaternion l_camOrient = *p_camera.GetOrientation();
	*m_origin = l_camOrient * (*m_origin);

	Point2D<int> l_point( p_x, p_y);
	m_direction = new Vector3f( l_viewport.GetDirection( l_point));
	m_direction->Normalise();
}

Ray :: Ray( const Vector3f & p_rOrigin, const Vector3f & p_rDirection)
	:	m_origin( new Vector3f( p_rOrigin)),
		m_direction( new Vector3f( p_rDirection))
{
}

Ray :: Ray( const Ray & p_ray)
{
	m_origin = new Vector3f( *p_ray.m_origin);
	m_direction = new Vector3f( *p_ray.m_direction);
	m_direction->Normalise();
}

Ray :: ~Ray()
{
	delete m_origin;
	delete m_direction;
}

float Ray :: Intersects( const Vector3f & p_v1, const Vector3f & p_v2, const Vector3f & p_v3)
{
	float l_fReturn = -1.0f;

	Vector3f e1( p_v1, p_v2);
	Vector3f e2( p_v3, p_v1);
	Vector3f e3( p_v2, p_v3);

	Vector3f h = *m_direction ^ e2;

	float a = e1.dotProduct( h);

	if (a <= -0.00001 || a >= 0.00001)
	{
		float f = 1.0f / a;

		Vector3f s( p_v1, *m_origin);
		float u = f * s.dotProduct( h);

		if (u >= 0.0 && u <= 1.0)
		{
			Vector3f q = s ^ e1;
			float v = f * m_direction->dotProduct( q);

			if (v >= 0.0 && u + v <= 1.0)
			{
				float t = f * e2.dotProduct( q);

				if (t > 0.00001f)
				{
					l_fReturn = t;
				}
			}
		}
	}

	return l_fReturn;
}

float Ray :: Intersects( const Face & p_face)
{
	Vector3f * l_v1 = p_face.m_vertex1;
	Vector3f * l_v2 = p_face.m_vertex2;
	Vector3f * l_v3 = p_face.m_vertex3;
	return Intersects( *l_v1, *l_v2, *l_v3);
}

float Ray :: Intersects( const Vector3f & p_vertex)
{
	Vector3f u( p_vertex, * m_origin);
	Vector3f puv;

	if (ProjectVertex( u, puv) && puv.GetSquaredLength() < 0.000001)
	{
		return puv.GetLength();
	}

	return -1.0f;
}

float Ray :: Intersects( const ComboBox & p_box)
{
	Vector3f l_v1( p_box.GetMin());
	Vector3f l_v8( p_box.GetMax());
	Vector3f l_v2( l_v8.x, l_v1.y, l_v1.z);
	Vector3f l_v3( l_v1.x, l_v8.y, l_v1.z);
	Vector3f l_v4( l_v8.x, l_v8.y, l_v1.z);
	Vector3f l_v5( l_v8.x, l_v1.y, l_v8.z);
	Vector3f l_v6( l_v1.x, l_v8.y, l_v8.z);
	Vector3f l_v7( l_v8.x, l_v8.y, l_v8.z);

	bool l_foundOne = false;
	float l_dist, l_min = 1e6f;

	if ((l_dist = Intersects( l_v1, l_v2, l_v3)) >= 0)
	{
		l_foundOne = true;
		l_min = l_dist;
	}

	if ((l_dist = Intersects( l_v2, l_v4, l_v3)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v2, l_v6, l_v4)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v6, l_v8, l_v4)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v6, l_v5, l_v8)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v5, l_v7, l_v8)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v1, l_v7, l_v5)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v1, l_v3, l_v7)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v2, l_v1, l_v5)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v2, l_v5, l_v6)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v3, l_v4, l_v7)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if ((l_dist = Intersects( l_v4, l_v7, l_v8)) >= 0)
	{
		l_foundOne = true;
		l_min = min( l_min, l_dist);
	}

	if (l_foundOne)
	{
		return l_min;
	}

	return -1.0f;
}

float Ray :: Intersects( const Sphere & p_sphere)
{
	float l_fReturn = -1.0f;

	// intersection rayon/sphere 
	Vector3f l_vDist = p_sphere.GetCenter() - (* m_origin); 
	float B = m_direction->dotProduct( l_vDist);
	float D = B * B - l_vDist.dotProduct(l_vDist) + p_sphere.GetRadius() * p_sphere.GetRadius(); 

	if (D >= 0.0f)
	{
		float t0 = B - sqrtf( D);
		float t1 = B + sqrtf( D);

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

float Ray :: Intersects( Geometry * p_pGeometry, Face ** p_ppFace, Submesh ** p_ppSubmesh)
{
	Vector3f l_vCenter = p_pGeometry->GetCenter()->operator +( p_pGeometry->GetParent()->GetPosition());
	Quaternion l_qOrientation = p_pGeometry->GetParent()->GetOrientation();
	Vector3f l_vOrientedCenter = l_qOrientation * l_vCenter;

	Mesh * l_pMesh = p_pGeometry->GetMesh();
	Sphere l_sphere( l_vCenter, l_pMesh->GetSphere()->GetRadius());

	float l_fDistance = Intersects( l_sphere);
/*
	float l_faceDist = 10e6, l_vertexDist = 10e6;
	float l_curfaceDist, l_curvertexDist;
*/
	if (l_fDistance >= 0.0f)
	{
		l_fDistance = -1.0f;
		size_t l_nbSubmeshes = l_pMesh->GetNbSubmeshes();
		Submesh * l_pSubmesh = NULL;

		for (size_t i = 0 ; i < l_nbSubmeshes ; i++)
		{
			l_pSubmesh = l_pMesh->GetSubmesh( i);
			Vector3f l_submeshCenter = l_vCenter + l_pSubmesh->GetSphere()->GetCenter();
			l_submeshCenter = l_qOrientation * l_submeshCenter;
			l_sphere.Load( l_submeshCenter, l_pSubmesh->GetSphere()->GetRadius());

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
					SmoothingGroup * l_pGroup = l_pSubmesh->GetSmoothGroup( j);
					for (size_t k = 0 ; k < l_pGroup->m_faces.size() ; k++)
					{
						Face * l_pFace = l_pGroup->m_faces[k];

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

bool Ray :: ProjectVertex( const Vector3f & p_vertex, Vector3f & p_result)
{
	if (m_direction->dotProduct( p_vertex) >= 0.0)
	{
		p_result = (*m_direction) * (m_direction->dotProduct( p_vertex) / m_direction->GetLength());
		return true;
	}

	return false;
}