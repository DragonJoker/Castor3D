#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "scene/Scene.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "importer/SMaxImporter.h"
#include "importer/AseImporter.h"
#include "importer/Md2Importer.h"
#include "importer/Md3Importer.h"
#include "importer/ObjImporter.h"
#include "importer/BspImporter.h"
#include "importer/PlyImporter.h"

using namespace Castor3D;
//*********************************************************************************************

MeshPtr MeshLoader :: LoadFromFile( const String & p_file)
{
	MeshPtr l_pReturn;
	bool l_bResult = false;

	File l_file( p_file, File::eRead);
	size_t l_nameLength = 0;
	l_bResult = l_file.Read( l_nameLength) == sizeof( size_t);
	String l_strName;

	if (l_bResult)
	{
		Char * l_name = new Char[l_nameLength + 1];
		l_bResult = l_file.ReadArray( l_name, l_nameLength) == l_nameLength * sizeof( Char);

		if (l_bResult)
		{
			l_name[l_nameLength] = 0;
			l_strName = l_name;
		}

		delete [] l_name;
	}

	if (l_bResult)
	{
		Logger::LogMessage( CU_T( "Reading mesh ") + l_strName);
		l_pReturn.reset( new Mesh( l_strName));
	}

	Mesh::eTYPE l_meshType;

	if (l_bResult)
	{
		l_bResult = l_file.Read( l_meshType) == sizeof( Mesh::eTYPE);
	}

	size_t l_nbSubmeshes;

	if (l_bResult)
	{
		l_pReturn->SetMeshType( l_meshType);
		l_bResult = l_file.Read( l_nbSubmeshes) == sizeof( size_t);
	}

	if (l_bResult)
	{
		for (size_t i = 0 ; i < l_nbSubmeshes && l_bResult ; i++)
		{
			if ( ! l_pReturn->CreateSubmesh( 0)->Read( l_file))
			{
				l_bResult = false;
			}
		}

//		l_pReturn->CreateBuffers();
	}

	if ( ! l_bResult)
	{
		l_pReturn.reset();
	}

	return l_pReturn;
}

void MeshLoader :: LoadFromExtFile( const String & p_file, Scene * p_pScene)
{
	StringArray l_arraySplitted = p_file.Split( ".");

	if (l_arraySplitted.size() > 1)
	{
		String l_strExt = l_arraySplitted[l_arraySplitted.size() - 1];

		if (l_strExt == "3ds")
		{
			_loadFrom3DS( p_file, p_pScene);
		}
		else if (l_strExt == "ase")
		{
			_loadFromAse( p_file, p_pScene);
		}
		else if (l_strExt == "obj")
		{
			_loadFromObj( p_file, p_pScene);
		}
		else if (l_strExt == "ply")
		{
			_loadFromPly( p_file, p_pScene);
		}
		else if (l_strExt == "md2")
		{
			_loadFromMd2( p_file, p_pScene);
		}
		else if (l_strExt == "md3")
		{
			_loadFromMd3( p_file, p_pScene);
		}
	}
}

bool MeshLoader :: SaveToFile( const String & p_file, MeshPtr p_mesh)
{
	File l_file( p_file + CU_T( "/") + p_mesh->GetName() + CU_T( ".cmsh"), File::eWrite);
	bool l_bReturn = false;

	//on écrit le nom du mesh
	size_t l_nameLength = p_mesh->GetName().size();
	l_bReturn = l_file.Write( l_nameLength) == sizeof( size_t);

	if (l_bReturn)
	{
		l_bReturn = l_file.WriteArray( p_mesh->GetName().c_str(), l_nameLength) == l_nameLength * sizeof( Char);
	}

	if (l_bReturn)
	{
		Mesh::eTYPE l_meshType = p_mesh->GetMeshType();
		l_bReturn = l_file.Write( l_meshType) == sizeof( Mesh::eTYPE);
	}

	size_t l_nbSubmeshes = p_mesh->GetNbSubmeshes();

	if (l_bReturn)
	{
		l_bReturn = l_file.Write( l_nbSubmeshes) == sizeof( size_t);
	}

	for (size_t i = 0 ; i < l_nbSubmeshes && l_bReturn ; i++)
	{
		l_bReturn = p_mesh->GetSubmesh( i)->Write( l_file);
	}

	return l_bReturn;
}

void MeshLoader :: _loadFrom3DS( const String & p_file, Scene * p_pScene)
{
	SMaxImporter l_importer;
	p_pScene->ImportExternal( p_file, & l_importer);
}

void MeshLoader :: _loadFromAse( const String & p_file, Scene * p_pScene)
{
	AseImporter l_importer;
	p_pScene->ImportExternal( p_file, & l_importer);
}

void MeshLoader :: _loadFromObj( const String & p_file, Scene * p_pScene)
{
	ObjImporter l_importer;
	p_pScene->ImportExternal( p_file, & l_importer);
}

void MeshLoader :: _loadFromPly( const String & p_file, Scene * p_pScene)
{
	PlyImporter l_importer;
	p_pScene->ImportExternal( p_file, & l_importer);
}

void MeshLoader :: _loadFromMd2( const String & p_file, Scene * p_pScene)
{
	Md2Importer l_importer;
	p_pScene->ImportExternal( p_file, & l_importer);
}

void MeshLoader :: _loadFromMd3( const String & p_file, Scene * p_pScene)
{
	Md3Importer l_importer;
	p_pScene->ImportExternal( p_file, & l_importer);
}

//*********************************************************************************************

Mesh :: Mesh( const String & p_name)
	:	m_name( p_name),
		m_modified( false),
		m_preferredSubdivMode( SMPNTriangles),
		m_bOK( false)
{
	m_meshType = Mesh::eCustom;
}

Mesh :: ~Mesh()
{
	m_name.clear();
	Cleanup();
}

void Mesh :: Cleanup()
{
//	vector::deleteAll( m_submeshes);
	m_submeshes.clear();
}

void Mesh :: ComputeContainers()
{
	if (m_submeshes.size() == 0)
	{
		return;
	}

	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->ComputeContainers();
	}

	Point3r l_min = m_submeshes[0]->GetComboBox().GetMin();
	Point3r l_max = m_submeshes[0]->GetComboBox().GetMax();

	for (size_t i = 1 ; i < m_submeshes.size() ; i++)
	{
		const ComboBox & l_box = m_submeshes[i]->GetComboBox();

		if (l_box.GetMin()[0] < l_min[0])
		{
			l_min[0] = l_box.GetMin()[0];
		}
		if (l_box.GetMin()[1] < l_min[1])
		{
			l_min[1] = l_box.GetMin()[1];
		}
		if (l_box.GetMin()[2] < l_min[2])
		{
			l_min[2] = l_box.GetMin()[2];
		}
		if (l_box.GetMax()[0] > l_max[0])
		{
			l_max[0] = l_box.GetMax()[0];
		}
		if (l_box.GetMax()[1] > l_max[1])
		{
			l_max[1] = l_box.GetMax()[1];
		}
		if (l_box.GetMax()[2] > l_max[2])
		{
			l_max[2] = l_box.GetMax()[2];
		}
	}

	m_box.Load( l_min, l_max);
	m_sphere.Load( m_box);
}

size_t Mesh :: GetNbFaces()const
{
	size_t l_nbFaces = 0;
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		l_nbFaces += m_submeshes[i]->GetNbFaces();
	}
	return l_nbFaces;
}

size_t Mesh :: GetNbVertex()const
{
	size_t l_nbFaces = 0;
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		l_nbFaces += m_submeshes[i]->GetVertices().size();
	}
	return l_nbFaces;
}

SubmeshPtr Mesh :: GetSubmesh( unsigned int p_index)
{
	SubmeshPtr l_pReturn;

	if (p_index < m_submeshes.size())
	{
		l_pReturn = m_submeshes[p_index];
	}

	return l_pReturn;
}

SubmeshPtr Mesh :: CreateSubmesh( unsigned int p_nbSmoothGroups)
{
	SubmeshPtr l_submesh( new Submesh( p_nbSmoothGroups));

	if ( ! l_submesh.null())
	{
		m_submeshes.push_back( l_submesh);
	}

	return l_submesh;
}

void Mesh :: ComputeFlatNormals()
{
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->ComputeFlatNormals();
	}
}

void Mesh :: ComputeSmoothNormals()
{
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->ComputeSmoothNormals();
	}
}

void Mesh :: ComputeNormals( bool p_bReverted)
{
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->ComputeNormals();
	}
}

void Mesh :: InitialiseBuffers()
{
	for (unsigned int i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->InitialiseBuffers();
	}

	m_bOK = true;
}

void Mesh :: SetNormals( eNORMALS_MODE p_nm)
{
	m_normalsMode = p_nm;
//	vector::cycle( m_submeshes, & Submesh::CreateNormalsBuffer, p_nm);
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->SetNormals( p_nm);
	}

	ComputeContainers();
}

MeshPtr Mesh :: Clone( const String & p_name)
{
	MeshPtr l_clone( new Mesh());
	l_clone->m_meshType = m_meshType;

	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		l_clone->m_submeshes.push_back( m_submeshes[i]->Clone());
	}
	l_clone->ComputeContainers();

	return l_clone;
}

bool Mesh :: Subdivide( unsigned int p_index, SubdivisionMode p_mode)
{
	if (p_index >= m_submeshes.size())
	{
		return false;
	}

	m_bOK = false;

	m_submeshes[p_index]->Subdivide( p_mode, & m_sphere.GetCenter());

	m_bOK = true;

	return true;
}