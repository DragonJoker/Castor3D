#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/MeshManager.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/ConicMesh.h"
#include "geometry/mesh/CylindricMesh.h"
#include "geometry/mesh/IcosaedricMesh.h"
#include "geometry/mesh/CubicMesh.h"
#include "geometry/mesh/PlaneMesh.h"
#include "geometry/mesh/SphericMesh.h"
#include "geometry/mesh/TorusMesh.h"
#include "geometry/mesh/ProjectionMesh.h"
#include "main/Root.h"

#include "geometry/basic/Face.h"
#include "geometry/basic/Arc.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"

#include "Log.h"

using namespace Castor3D;
using namespace General;
using namespace Templates;

MeshManager :: MeshManager()
{
}

MeshManager :: ~MeshManager()
{
	Clear();
}

void MeshManager :: Clear()
{
	map::deleteAll( m_meshMap);
}

Mesh * MeshManager :: CreateMesh( const String & p_name, UIntArray p_faces,
								  FloatArray p_size, Mesh::eTYPE p_type)
{
	Mesh * l_mesh = NULL;
	if (p_type == Mesh::eCone)
	{
		l_mesh = new ConicMesh( p_size[0], p_size[1], p_faces[0], p_name);
		AddElement( l_mesh);
	}
	else if (p_type == Mesh::eCylinder)
	{
		l_mesh = new CylindricMesh( p_size[0], p_size[1], p_faces[0], p_name);
		AddElement( l_mesh);
	}
	else if (p_type == Mesh::eSphere)
	{
		l_mesh = new SphericMesh( p_size[0], p_faces[0], p_name);
		AddElement( l_mesh);
	}
	else if (p_type == Mesh::eCube)
	{
		l_mesh = new CubicMesh( p_size[0], p_size[1], p_size[2], p_name);
		AddElement( l_mesh);
	}
	else if (p_type == Mesh::eTorus)
	{
		l_mesh = new TorusMesh( p_size[0], p_size[1], p_faces[0], p_faces[1], p_name);
		AddElement( l_mesh);
	}
	else if (p_type == Mesh::ePlane)
	{
		l_mesh = new PlaneMesh( p_size[0], p_size[1], p_faces[1], p_faces[0], p_name);
		AddElement( l_mesh);
	}
	else if (p_type == Mesh::eIcosaedron)
	{
		l_mesh = new IcosaedricMesh( p_size[0], p_faces[0], p_name);
		AddElement( l_mesh);
	}
	else if (p_type == Mesh::eProjection)
	{
		Castor3D::Arc * pArc = new Castor3D::Arc;
		pArc->AddVertex( new Vector3f( 0.0, 0.0, 0.0), 0);
		pArc->AddVertex( new Vector3f( 0.0, 1.0, 0.0), 1);
		pArc->AddVertex( new Vector3f( 1.0, 1.0, 0.0), 2);
		pArc->AddVertex( new Vector3f( 0.0, 2.0, 0.0), 3);
		pArc->AddVertex( new Vector3f( 2.0, 4.0, 0.0), 4);
		l_mesh = new ProjectionMesh( pArc, p_size[0], true, p_faces[0], p_name);
		AddElement( l_mesh);
	}
	else if (p_type == Mesh::eCustom)
	{
		l_mesh = new Mesh( p_name);
		AddElement( l_mesh);
	}
	else
	{
		Log::LogMessage( C3D_T( "Can't create mesh ") + p_name + C3D_T( " - Undefined mesh type"));
		return NULL;
	}

	return l_mesh;
}

bool MeshManager :: Write( const String & p_path)const
{
	size_t l_slashIndex = p_path.find_last_of( C3D_T( "//"));
	String l_path = p_path.substr( 0, l_slashIndex);
	TypeMap::const_iterator l_it = m_objectMap.begin();
	MeshLoader l_loader;
	while (l_it != m_objectMap.end())
	{
		if ( ! l_loader.SaveToFileIO( l_path, l_it->second))
		{
			return false;
		}
		++l_it;
	}
	return true;
}

bool MeshManager :: Read( const String & p_path)
{
	StringArray l_files;

	size_t l_slashIndex = p_path.find_last_of( C3D_T( "//"));
	String l_path = p_path.substr( 0, l_slashIndex);

	FileBase::ListDirectoryFiles( l_path, l_files);

	String l_fileName;
	String l_matName;
	String l_dotIndex;
	MeshLoader l_loader;
	Mesh * l_mesh;
	for (size_t i = 0 ; i < l_files.size() ; i++)
	{
		if (l_files[i].find( C3D_T( ".csmesh")) != String::npos)
		{
			l_mesh = l_loader.LoadFromFileIO( l_files[i]);
			if (l_mesh != NULL)
			{
				AddElement( l_mesh);
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

bool MeshManager :: _addElement( Mesh * p_element)
{
	const MeshPtrMap::iterator & ifind = m_meshMap.find( p_element->GetName());

	if (ifind != m_meshMap.end())
	{
		return false;
	}

	m_meshMap.insert( MeshPtrMap::value_type( p_element->GetName(), p_element));
	return true;
}