#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/basic/Face.h"

using namespace Castor3D;

unsigned long Face :: s_faceNumber = 0;

Face :: Face( VertexPtr p_v1, VertexPtr p_v2, VertexPtr p_v3)
	:	m_vertex1( p_v1),
		m_vertex2( p_v2),
		m_vertex3( p_v3),
		m_vertex1Normal( NULL),
		m_vertex2Normal( NULL),
		m_vertex3Normal( NULL),
		m_vertex1Tangent( NULL),
		m_vertex2Tangent( NULL),
		m_vertex3Tangent( NULL)
{
	s_faceNumber++;
}

Face :: ~Face()
{
	s_faceNumber--;
	//m_vertex1,m_vertex2,m_vertex3 supprimés par le Submesh
}

Point3r Face :: GetFaceCenter()
{
	Point3r l_vertexMean;

	l_vertexMean += *m_vertex1;
	l_vertexMean += *m_vertex2;
	l_vertexMean += *m_vertex3;
	l_vertexMean /= real( 3.0);

	return l_vertexMean;
}

void Face :: SetTexCoordV1( real x, real y)
{
	m_vertex1TexCoord[0] = x;
	m_vertex1TexCoord[1] = y;
}

void Face :: SetTexCoordV1( const Point2r & p_ptUV)
{
	m_vertex1TexCoord[0] = p_ptUV[0];
	m_vertex1TexCoord[1] = p_ptUV[1];
}

void Face :: SetTexCoordV2( real x, real y)
{
	m_vertex2TexCoord[0] = x;
	m_vertex2TexCoord[1] = y;
}

void Face :: SetTexCoordV2( const Point2r & p_ptUV)
{
	m_vertex2TexCoord[0] = p_ptUV[0];
	m_vertex2TexCoord[1] = p_ptUV[1];
}

void Face :: SetTexCoordV3( real x, real y)
{
	m_vertex3TexCoord[0] = x;
	m_vertex3TexCoord[1] = y;
}

void Face :: SetTexCoordV3( const Point2r & p_ptUV)
{
	m_vertex3TexCoord[0] = p_ptUV[0];
	m_vertex3TexCoord[1] = p_ptUV[1];
}

bool Face :: Write( File & p_file)const
{
	// on écrit la normale de la face
	if ( ! m_faceNormal.Write( p_file))
	{
		return false;
	}

	// on écrit les normales par vertex de la face
	if ( ! m_vertex1Normal->Write( p_file))
	{
		return false;
	}
	if ( ! m_vertex2Normal->Write( p_file))
	{
		return false;
	}
	if ( ! m_vertex3Normal->Write( p_file))
	{
		return false;
	}

	// on écrit les coordonnées de texture de la face
	if ( ! m_vertex1TexCoord.Write( p_file))
	{
		return false;
	}
	if ( ! m_vertex2TexCoord.Write( p_file))
	{
		return false;
	}
	if ( ! m_vertex3TexCoord.Write( p_file))
	{
		return false;
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

	m_vertex1Normal = new Point3r;
	m_vertex2Normal = new Point3r;
	m_vertex3Normal = new Point3r;

	// on lit les normales par vertex de la face
	if ( ! m_vertex1Normal->Read( p_file))
	{
		return false;
	}
	if ( ! m_vertex2Normal->Read( p_file))
	{
		return false;
	}
	if ( ! m_vertex3Normal->Read( p_file))
	{
		return false;
	}

	// on lit les coordonnées de texture par vertex de la face
	if ( ! m_vertex1TexCoord.Read( p_file))
	{
		return false;
	}
	if ( ! m_vertex2TexCoord.Read( p_file))
	{
		return false;
	}
	if ( ! m_vertex3TexCoord.Read( p_file))
	{
		return false;
	}

	return true;
}