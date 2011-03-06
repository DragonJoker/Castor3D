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
								   const FloatArray & p_size, eMESH_TYPE p_type)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	MeshPtr l_mesh;

	switch (p_type)
	{
	case eCone:
		l_mesh.reset( new Cone( this, p_size[0], p_size[1], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case eCylinder:
		l_mesh.reset( new Cylinder( this, p_size[0], p_size[1], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case eSphere:
		l_mesh.reset( new Sphere( this, p_size[0], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case eCube:
		l_mesh.reset( new Cube( this, p_size[0], p_size[1], p_size[2], p_name));
		AddElement( l_mesh);
		break;

	case eTorus:
		l_mesh.reset( new Torus( this, p_size[0], p_size[1], p_faces[0], p_faces[1], p_name));
		AddElement( l_mesh);
		break;

	case ePlane:
		l_mesh.reset( new Plane( this, p_size[0], p_size[1], p_faces[1], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case eIcosaedron:
		l_mesh.reset( new Icosaedron( this, p_size[0], p_faces[0], p_name));
		AddElement( l_mesh);
		break;

	case eProjection:
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

	case eCustom:
		l_mesh.reset( new Mesh( this, p_name));
		AddElement( l_mesh);
		break;

	default:
		Logger::LogMessage( CU_T( "Can't create mesh ") + p_name + CU_T( " - Undefined mesh type"));
	}

	return l_mesh;
}

bool MeshManager :: Save( File & p_file)const
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	Path l_path = p_file.GetFileFullPath();
	bool l_bReturn = true;
	TypeMap::const_iterator l_it = m_objectMap.begin();

	if (l_path.GetExtension() == "cmsh")
	{
		l_bReturn = p_file.Write( m_objectMap.size()) == sizeof( size_t);

		while (l_it != m_objectMap.end() && l_bReturn)
		{
			l_bReturn = l_it->second->Save( p_file);
			++l_it;
		}
	}
	else
	{
		l_path = l_path.GetPath() / l_path.GetFileName() + CU_T( ".cmsh");
		File l_file( l_path, File::eWrite | File::eBinary);

		l_bReturn = l_file.Write( m_objectMap.size()) == sizeof( size_t);

		while (l_it != m_objectMap.end() && l_bReturn)
		{
			l_bReturn = l_it->second->Save( l_file);
			++l_it;
		}
	}


	return l_bReturn;
}

bool MeshManager :: Load( File & p_file)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	size_t l_uiSize;
	bool l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		MeshPtr l_mesh = MeshPtr( new Mesh( this));
		l_bReturn = l_mesh->Load( p_file);

		if (l_bReturn)
		{
			AddElement( l_mesh);
		}
	}

	return l_bReturn;
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