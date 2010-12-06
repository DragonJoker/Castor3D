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
#include "geometry/basic/Vertex.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Pattern.h"
#include "render_system/RenderSystem.h"



using namespace Castor3D;
using namespace Castor;
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
	MeshManager & l_pThis = GetSingleton();
	l_pThis.m_objectMap.clear();
	l_pThis.m_meshMap.clear();
}

MeshPtr MeshManager :: CreateMesh( const String & p_name, const UIntArray & p_faces,
								   const FloatArray & p_size, Mesh::eTYPE p_type)
{
	MeshManager & l_pThis = GetSingleton();
	MeshPtr l_mesh;

	switch (p_type)
	{
	case Mesh::eCone:
		l_mesh.reset( new ConicMesh( p_size[0], p_size[1], p_faces[0], p_name));
		l_pThis.AddElement( l_mesh);
		break;

	case Mesh::eCylinder:
		l_mesh.reset( new CylindricMesh( p_size[0], p_size[1], p_faces[0], p_name));
		l_pThis.AddElement( l_mesh);
		break;

	case Mesh::eSphere:
		l_mesh.reset( new SphericMesh( p_size[0], p_faces[0], p_name));
		l_pThis.AddElement( l_mesh);
		break;

	case Mesh::eCube:
		l_mesh.reset( new CubicMesh( p_size[0], p_size[1], p_size[2], p_name));
		l_pThis.AddElement( l_mesh);
		break;

	case Mesh::eTorus:
		l_mesh.reset( new TorusMesh( p_size[0], p_size[1], p_faces[0], p_faces[1], p_name));
		l_pThis.AddElement( l_mesh);
		break;

	case Mesh::ePlane:
		l_mesh.reset( new PlaneMesh( p_size[0], p_size[1], p_faces[1], p_faces[0], p_name));
		l_pThis.AddElement( l_mesh);
		break;

	case Mesh::eIcosaedron:
		l_mesh.reset( new IcosaedricMesh( p_size[0], p_faces[0], p_name));
		l_pThis.AddElement( l_mesh);
		break;

	case Mesh::eProjection:
		{
			Point3rPatternPtr l_pPattern( new Point3rPattern);
			l_pPattern->AddPoint( Point3r( 0.0, 0.0, 0.0), 0);
			l_pPattern->AddPoint( Point3r( 0.0, 1.0, 0.0), 1);
			l_pPattern->AddPoint( Point3r( 1.0, 1.0, 0.0), 2);
			l_pPattern->AddPoint( Point3r( 0.0, 2.0, 0.0), 3);
			l_pPattern->AddPoint( Point3r( 2.0, 4.0, 0.0), 4);
			l_mesh.reset( new ProjectionMesh( l_pPattern, Point3r( 0.0, 0.0, 1.0), true, p_size[0], p_faces[0], p_name));
			l_pThis.AddElement( l_mesh);
		}
		break;

	case Mesh::eCustom:
		l_mesh.reset( new Mesh( p_name));
		l_pThis.AddElement( l_mesh);
		break;

	default:
		Logger::LogMessage( CU_T( "Can't create mesh ") + p_name + CU_T( " - Undefined mesh type"));
	}

	return l_mesh;
}

bool MeshManager :: Write( const String & p_path)
{
	MeshManager & l_pThis = GetSingleton();
	size_t l_slashIndex = p_path.find_last_of( CU_T( "//"));
	String l_path = p_path.substr( 0, l_slashIndex);
	TypeMap::const_iterator l_it = l_pThis.m_objectMap.begin();
	MeshLoader l_loader;

	while (l_it != l_pThis.m_objectMap.end())
	{
		if ( ! l_loader.SaveToFile( l_path, l_it->second))
		{
			return false;
		}
		++l_it;
	}

	return true;
}

bool MeshManager :: Read( const String & p_path)
{
	MeshManager & l_pThis = GetSingleton();
	StringArray l_files;

	size_t l_slashIndex = p_path.find_last_of( CU_T( "//"));
	String l_path = p_path.substr( 0, l_slashIndex);

	File::ListDirectoryFiles( l_path, l_files);

	String l_fileName;
	String l_matName;
	String l_dotIndex;
	MeshLoader l_loader;
	MeshPtr l_mesh;

	for (size_t i = 0 ; i < l_files.size() ; i++)
	{
		if (l_files[i].find( CU_T( ".cmsh")) != String::npos)
		{
			l_mesh = MeshPtr( l_loader.LoadFromFile( l_files[i]));
			if (l_mesh.null())
			{
				l_pThis.AddElement( l_mesh);
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

bool MeshManager :: _addElement( MeshPtr p_element)
{
	MeshManager & l_pThis = GetSingleton();
	const MeshPtrStrMap::iterator & ifind = l_pThis.m_meshMap.find( p_element->GetName());

	if (ifind != l_pThis.m_meshMap.end())
	{
		return false;
	}

	l_pThis.m_meshMap.insert( MeshPtrStrMap::value_type( p_element->GetName(), p_element));
	return true;
}