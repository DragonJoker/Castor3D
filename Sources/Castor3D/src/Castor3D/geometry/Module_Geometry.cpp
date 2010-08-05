#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/basic/Face.h"

using namespace Castor3D;

Vector3f * Castor3D :: GetFaceCenter( Face * p_face)
{
	Vector3f * pVertexMean = new Vector3f();

	pVertexMean->operator+=( *p_face->m_vertex1);
	pVertexMean->operator+=( *p_face->m_vertex2);
	pVertexMean->operator+=( *p_face->m_vertex3);
	pVertexMean->operator /= ( 3);

	return pVertexMean;
}

void Castor3D :: SetTexCoordV1( Face * p_face, float x, float y)
{
	p_face->m_vertex1TexCoord.x = x;
	p_face->m_vertex1TexCoord.y = y;
}

void Castor3D :: SetTexCoordV2( Face * p_face, float x, float y)
{
	p_face->m_vertex2TexCoord.x = x;
	p_face->m_vertex2TexCoord.y = y;
}

void Castor3D :: SetTexCoordV3( Face * p_face, float x, float y)
{
	p_face->m_vertex3TexCoord.x = x;
	p_face->m_vertex3TexCoord.y = y;
}

bool Castor3D :: WriteFace( Face * p_face, File & p_file)
{
	// on écrit la normale de la face
	if ( ! p_file.WriteArray<float>( p_face->m_faceNormal.const_ptr(), 3))
	{
		return false;
	}

	// on écrit les normales par vertex de la face
	if ( ! p_file.WriteArray<float>( p_face->m_vertex1Normal->const_ptr(), 3))
	{
		return false;
	}
	if ( ! p_file.WriteArray<float>( p_face->m_vertex2Normal->const_ptr(), 3))
	{
		return false;
	}
	if ( ! p_file.WriteArray<float>( p_face->m_vertex3Normal->const_ptr(), 3))
	{
		return false;
	}

	// on écrit les coordonnées de texture de la face
	if ( ! p_file.WriteArray<float>( p_face->m_vertex1TexCoord.const_ptr(), 3))
	{
		return false;
	}
	if ( ! p_file.WriteArray<float>( p_face->m_vertex2TexCoord.const_ptr(), 3))
	{
		return false;
	}
	if ( ! p_file.WriteArray<float>( p_face->m_vertex3TexCoord.const_ptr(), 3))
	{
		return false;
	}

	return true;
}

bool Castor3D :: ReadFace( Face * p_face, File & p_file)
{
	// on lit la normale de la face
	if ( ! p_file.ReadArray<float>( p_face->m_faceNormal.ptr(), 3))
	{
		return false;
	}

	// on lit les normales par vertex de la face
	if ( ! p_file.ReadArray<float>( p_face->m_vertex1Normal->ptr(), 3))
	{
		return false;
	}
	if ( ! p_file.ReadArray<float>( p_face->m_vertex2Normal->ptr(), 3))
	{
		return false;
	}
	if ( ! p_file.ReadArray<float>( p_face->m_vertex3Normal->ptr(), 3))
	{
		return false;
	}

	// on lit les coordonnées de texture par vertex de la face
	if ( ! p_file.ReadArray<float>( p_face->m_vertex1TexCoord.ptr(), 3))
	{
		return false;
	}
	if ( ! p_file.ReadArray<float>( p_face->m_vertex2TexCoord.ptr(), 3))
	{
		return false;
	}
	if ( ! p_file.ReadArray<float>( p_face->m_vertex3TexCoord.ptr(), 3))
	{
		return false;
	}

	return true;
}

bool Castor3D :: WriteVertex( const Vector3f & p_vertex, File & p_file)
{
	if ( ! p_file.Write<float>( p_vertex.x))
	{
		return false;
	}
	if ( ! p_file.Write<float>( p_vertex.y))
	{
		return false;
	}
	if ( ! p_file.Write<float>( p_vertex.z))
	{
		return false;
	}
	return true;
}

bool Castor3D :: WriteVertex( const Vector3f * p_vertex, File & p_file)
{
	return WriteVertex( * p_vertex, p_file);
}

bool Castor3D :: ReadVertex( Vector3f & p_vertex, File & p_file)
{
	if ( ! p_file.Read<float>( p_vertex.x))
	{
		return false;
	}
	if ( ! p_file.Read<float>( p_vertex.y))
	{
		return false;
	}
	if ( ! p_file.Read<float>( p_vertex.z))
	{
		return false;
	}
	return true;
}

bool Castor3D :: ReadVertex( Vector3f * p_vertex, File & p_file)
{
	return ReadVertex( * p_vertex, p_file);
}