#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/primitives/MovableObject.h"
#include "scene/SceneNode.h"
#include "scene/Scene.h"

#include "Log.h"

using namespace Castor3D;

MovableObject :: MovableObject( SceneNode * p_sn, const String & p_name)
	:	m_name( p_name),
		m_matrix( new float[16]),
		m_sceneNode( p_sn)
{
	if (p_sn != NULL)
	{
		p_sn->AttachGeometry( this);
	}
}

MovableObject :: ~MovableObject()
{
	delete [] m_matrix;
}

void MovableObject :: Cleanup()
{
}

bool MovableObject :: Write( File & p_file)const
{
	size_t l_nameLength = m_name.size();

	if ( ! p_file.Write<size_t>( l_nameLength))
	{
		return false;
	}

	if ( ! p_file.WriteArray<Char>( m_name.c_str(), l_nameLength))
	{
		return false;
	}

	l_nameLength = m_sceneNode->GetName().size();

	if ( ! p_file.Write<size_t>( l_nameLength))
	{
		return false;
	}

	if ( ! p_file.WriteArray<Char>( m_sceneNode->GetName().c_str(), l_nameLength))
	{
		return false;
	}

	if ( ! WriteVertex( m_center, p_file))
	{
		return false;
	}

	if ( ! m_orientation.Write( p_file))
	{
		return false;
	}

	return true;
}

bool MovableObject :: Read( File & p_file, Scene * p_scene)
{
	size_t l_nameLength = 0;

	bool l_bReturn = p_file.Read<size_t>( l_nameLength) == sizeof( size_t);

	if (l_bReturn)
	{
		Char * l_name = new Char[l_nameLength+1];

		l_bReturn = p_file.ReadArray<Char>( l_name, l_nameLength) == l_nameLength * sizeof( Char);

		if (l_bReturn)
		{
			l_name[l_nameLength] = 0;
			m_name = l_name;
			delete [] l_name;
			Log::LogMessage( C3D_T( "Reading Geometry ") + m_name);
			l_nameLength = 0;
		}
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Read<size_t>( l_nameLength) == sizeof( size_t);
	}

	SceneNode * l_pNode = NULL;

	if (l_bReturn)
	{
		Char * l_nodeName = new Char[l_nameLength+1];

		l_bReturn = p_file.ReadArray<Char>( l_nodeName, l_nameLength) == sizeof( Char) * l_nameLength;

		if (l_bReturn)
		{
			l_nodeName[l_nameLength] = 0;

			l_pNode = p_scene->GetNode( l_nodeName);
			
			if (l_pNode != NULL)
			{
				l_pNode->AttachGeometry( this);
			}
			else
			{
				Log::LogMessage( C3D_T( "Node %s doesn't exist"), l_nodeName);
			}
		}

		delete [] l_nodeName;
	}

	if (l_bReturn)
	{
		l_bReturn = ReadVertex( m_center, p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = m_orientation.Read( p_file);
	}

	return l_bReturn;
}

float * MovableObject :: Get4x4RotationMatrix()
{
	m_orientation.ToRotationMatrix( m_matrix);
	return m_matrix;
}

void MovableObject :: Detach()
{
	if (m_sceneNode != NULL)
	{
		m_sceneNode->DetachGeometry( this);
		m_sceneNode = NULL;
	}
}