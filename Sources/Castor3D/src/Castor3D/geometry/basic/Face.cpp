#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/basic/Face.h"
#include "geometry/basic/Vertex.h"

using namespace Castor3D;

unsigned long Face :: s_faceNumber = 0;

Face :: Face( const Point3r & p_v1, size_t p_uiIndex1, const Point3r & p_v2, size_t p_uiIndex2, const Point3r & p_v3, size_t p_uiIndex3)
{
	s_faceNumber++;
	m_vertex[0].SetCoords( p_v1);
	m_vertex[0].m_index = p_uiIndex1;
	m_vertex[1].SetCoords( p_v2);
	m_vertex[1].m_index = p_uiIndex2;
	m_vertex[2].SetCoords( p_v3);
	m_vertex[2].m_index = p_uiIndex3;
}

Face :: ~Face()
{
	s_faceNumber--;
	//m_vertex1,m_vertex2,m_vertex3 supprimés par le Submesh
}

Point3r Face :: GetFaceCenter()
{
	Point3r l_vertexAvrg;

	l_vertexAvrg += m_vertex[0].GetCoords();
	l_vertexAvrg += m_vertex[1].GetCoords();
	l_vertexAvrg += m_vertex[2].GetCoords();
	l_vertexAvrg /= real( 3.0);

	return l_vertexAvrg;
}

void Face :: SetVertexTexCoords( size_t p_iVertex, real x, real y)
{
	m_vertex[p_iVertex].SetTexCoord( x, y);
}

void Face :: SetVertexTexCoords( size_t p_iVertex, const Point2r & p_ptUV)
{
	m_vertex[p_iVertex].SetTexCoord( p_ptUV);
}

void Face :: SetVertexTexCoords( size_t p_iVertex, const real * p_pCoords)
{
	m_vertex[p_iVertex].SetTexCoord( p_pCoords);
}

void Face :: SetSmoothNormals()
{
	for (size_t i = 0 ; i < 3 ; i++)
	{
		m_vertex[i].SetNormal( m_smoothNormals[i]);
		m_vertex[i].SetTangent( m_smoothTangents[i]);
	}
}

void Face :: SetFlatNormals()
{
	for (size_t i = 0 ; i < 3 ; i++)
	{
		m_vertex[i].SetNormal( m_faceNormal);
		m_vertex[i].SetTangent( m_faceTangent);
	}
}

bool Face :: Write( File & p_file)const
{
	// on écrit la normale de la face
	if ( ! m_faceNormal.Write( p_file))
	{
		return false;
	}

	// on écrit les normales par vertex de la face
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[0].GetNormal().Write( p_file))
		{
			return false;
		}
	}

	// on écrit les coordonnées de texture de la face
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[0].GetTexCoord().Write( p_file))
		{
			return false;
		}
	}

	return true;
}

bool Face :: Read( File & p_file)
{
	// on lit la normale de la face
	if ( ! m_faceNormal.Read( p_file))
	{
		return false;
	}

	// on lit les normales par vertex de la face
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[0].GetNormal().Read( p_file))
		{
			return false;
		}
	}

	// on lit les coordonnées de texture par vertex de la face
	for (size_t i = 0 ; i < 3 ; i++)
	{
		if ( ! m_vertex[0].GetTexCoord().Read( p_file))
		{
			return false;
		}
	}

	return true;
}