#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/mesh/MeshManager.h"
#include "Castor3D/geometry/mesh/Mesh.h"
#include "Castor3D/geometry/mesh/Cone.h"
#include "Castor3D/geometry/mesh/Cylinder.h"
#include "Castor3D/geometry/mesh/Icosaedron.h"
#include "Castor3D/geometry/mesh/Cube.h"
#include "Castor3D/geometry/mesh/Plane.h"
#include "Castor3D/geometry/mesh/Sphere.h"
#include "Castor3D/geometry/mesh/Torus.h"
#include "Castor3D/geometry/mesh/Projection.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/basic/Pattern.h"
#include "Castor3D/render_system/RenderSystem.h"



using namespace Castor3D;
using namespace Castor;
using namespace Templates;

MeshManager :: MeshManager( SceneManager * p_pParent)
	:	m_pParent( p_pParent)
{
}

MeshManager :: ~MeshManager()
{
	Clear();
}

void MeshManager :: Clear()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	m_objectMap.clear();
}

MeshPtr MeshManager :: CreateMesh( const String & p_name, const UIntArray & p_faces,
								   const FloatArray & p_size, Mesh::eTYPE p_type)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	MeshPtr l_mesh;

	switch (p_type)
	{
	case Mesh::eCone:
		l_mesh.reset( new Cone( this, p_size[0], p_size[1], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case Mesh::eCylinder:
		l_mesh.reset( new Cylinder( this, p_size[0], p_size[1], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case Mesh::eSphere:
		l_mesh.reset( new Sphere( this, p_size[0], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case Mesh::eCube:
		l_mesh.reset( new Cube( this, p_size[0], p_size[1], p_size[2], p_name));
		AddElement( l_mesh);
		break;

	case Mesh::eTorus:
		l_mesh.reset( new Torus( this, p_size[0], p_size[1], p_faces[0], p_faces[1], p_name));
		AddElement( l_mesh);
		break;

	case Mesh::ePlane:
		l_mesh.reset( new Plane( this, p_size[0], p_size[1], p_faces[1], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case Mesh::eIcosaedron:
		l_mesh.reset( new Icosaedron( this, p_size[0], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case Mesh::eProjection:
		{
			IdPoint3rPatternPtr l_pPattern( new IdPoint3rPattern);
			l_pPattern->AddElement( IdPoint3r( 0.0, 0.0, 0.0), 0);
			l_pPattern->AddElement( IdPoint3r( 0.0, 1.0, 0.0), 1);
			l_pPattern->AddElement( IdPoint3r( 1.0, 1.0, 0.0), 2);
			l_pPattern->AddElement( IdPoint3r( 0.0, 2.0, 0.0), 3);
			l_pPattern->AddElement( IdPoint3r( 2.0, 4.0, 0.0), 4);
			l_mesh.reset( new Projection( this, l_pPattern, Point3r( 0.0, 0.0, 1.0), true, p_size[0], p_faces[0], p_name));
			AddElement( l_mesh);
		}
		break;

	case Mesh::eCustom:
		l_mesh.reset( new Mesh( this, p_name));
		AddElement( l_mesh);
		break;

	default:
		Logger::LogMessage( CU_T( "Can't create mesh ") + p_name + CU_T( " - Undefined mesh type"));
	}

	return l_mesh;
}

bool MeshManager :: Write( const String & p_path)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	size_t l_slashIndex = p_path.find_last_of( CU_T( "//"));
	String l_path = p_path.substr( 0, l_slashIndex);
	TypeMap::const_iterator l_it = m_objectMap.begin();
	MeshLoader l_loader;

	while (l_it != m_objectMap.end())
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
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
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
			l_mesh = MeshPtr( l_loader.LoadFromFile( this, l_files[i]));
			if (l_mesh == NULL)
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

bool MeshManager :: _addElement( MeshPtr p_element)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	const TypeMap::iterator & ifind = m_objectMap.find( p_element->GetName());

	if (ifind != m_objectMap.end())
	{
		return false;
	}

	m_objectMap.insert( TypeMap::value_type( p_element->GetName(), p_element));
	return true;
}