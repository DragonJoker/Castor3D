#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "Log.h"

using namespace Castor3D;
//*********************************************************************************************

Mesh * MeshLoader :: LoadFromFile( const String & p_file)
{
	Mesh * l_pReturn = NULL;
	bool l_bResult = false;

	File l_file( p_file, File::eRead);
	size_t l_nameLength = 0;

	if (l_file.Read<size_t>( l_nameLength))
	{
		Char * l_name = new Char[l_nameLength+1];
		if (l_file.ReadArray<Char>( l_name, l_nameLength))
		{
			l_name[l_nameLength] = 0;

			Log::LogMessage( C3D_T( "Reading mesh ") + String( l_name));

			l_pReturn = new Mesh( l_name);
			delete [] l_name;

			Mesh::eTYPE l_meshType;
			if (l_file.Read<Mesh::eTYPE>( l_meshType))
			{
				l_pReturn->SetMeshType( l_meshType);

				size_t l_nbSubmeshes;
				if (l_file.Read<size_t>( l_nbSubmeshes))
				{
					l_bResult = true;

					for (size_t i = 0 ; i < l_nbSubmeshes && l_bResult ; i++)
					{
						if ( ! l_pReturn->CreateSubmesh( 0)->Read( l_file))
						{
							l_bResult = false;
						}
					}

					l_pReturn->CreateBuffers();
				}
			}
		}
	}

	if ( ! l_bResult)
	{
		delete l_pReturn;
		l_pReturn = NULL;
	}

	return l_pReturn;
}

Mesh * MeshLoader :: LoadFromExtFile( const String & p_file)
{
	Mesh * l_pReturn = NULL;
	StringArray l_arraySplitted = p_file.Split( ".");

	if (l_arraySplitted.size() > 1)
	{
		String l_strExt = l_arraySplitted[l_arraySplitted.size() - 1];

		if (l_strExt == "3ds")
		{
			l_pReturn = _loadFrom3DS( p_file);
		}
		else if (l_strExt == "ase")
		{
			l_pReturn = _loadFromAse( p_file);
		}
		else if (l_strExt == "obj")
		{
			l_pReturn = _loadFromObj( p_file);
		}
		else if (l_strExt == "ply")
		{
			l_pReturn = _loadFromPly( p_file);
		}
		else if (l_strExt == "md2")
		{
			l_pReturn = _loadFromMd2( p_file);
		}
		else if (l_strExt == "md3")
		{
			l_pReturn = _loadFromMd3( p_file);
		}
	}

	return l_pReturn;
}

bool MeshLoader :: SaveToFile( const String & p_file, Mesh * p_mesh)
{
	File l_file( p_file + C3D_T( "/") + p_mesh->GetName() + C3D_T( ".csmesh"), File::eWrite);

	//on écrit le nom du mesh
	size_t l_nameLength = p_mesh->GetName().size();
	if ( ! l_file.Write<size_t>( l_nameLength))
	{
		return false;
	}

	if ( ! l_file.WriteArray<Char>( p_mesh->GetName().c_str(), l_nameLength))
	{
		return false;
	}

	Mesh::eTYPE l_meshType = p_mesh->GetMeshType();
	if ( ! l_file.Write<Mesh::eTYPE>( l_meshType))
	{
		return false;
	}

	size_t l_nbSubmeshes = p_mesh->GetNbSubmeshes();
	if ( ! l_file.Write<size_t>( l_nbSubmeshes))
	{
		return false;
	}

	for (size_t i = 0 ; i < l_nbSubmeshes ; i++)
	{
		if ( ! p_mesh->GetSubmesh( i)->Write( l_file))
		{
			return false;
		}
	}

	return true;
}
Mesh * MeshLoader :: _loadFrom3DS( const String & p_file)
{
	Mesh * l_pReturn = NULL;



	return l_pReturn;
}

Mesh * MeshLoader :: _loadFromAse( const String & p_file)
{
	Mesh * l_pReturn = NULL;



	return l_pReturn;
}

Mesh * MeshLoader :: _loadFromObj( const String & p_file)
{
	Mesh * l_pReturn = NULL;



	return l_pReturn;
}

Mesh * MeshLoader :: _loadFromPly( const String & p_file)
{
	Mesh * l_pReturn = NULL;



	return l_pReturn;
}

Mesh * MeshLoader :: _loadFromMd2( const String & p_file)
{
	Mesh * l_pReturn = NULL;



	return l_pReturn;
}

Mesh * MeshLoader :: _loadFromMd3( const String & p_file)
{
	Mesh * l_pReturn = NULL;



	return l_pReturn;
}

//*********************************************************************************************

Mesh :: Mesh( const String & p_name)
	:	m_name( p_name),
		m_modified( false),
		m_box( NULL),
		m_sphere( NULL),
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
	vector::deleteAll( m_submeshes);

	delete m_box;
	m_box = NULL;

	delete m_sphere;
	m_sphere = NULL;
}

void Mesh :: ComputeContainers()
{
	delete m_box;
	m_box = NULL;

	delete m_sphere;
	m_sphere = NULL;

	if (m_submeshes.size() == 0)
	{
		return;
	}

	size_t i = 0;
	ComboBox * l_box = NULL;
	while (l_box == NULL && i < m_submeshes.size())
	{
		m_submeshes[i]->ComputeContainers();
		l_box = m_submeshes[i]->GetComboBox();
		i++;
	}

	Vector3f l_min;
	Vector3f l_max;
	if (l_box != NULL)
	{
		l_min = ( l_box->GetMin());
		l_max = ( l_box->GetMax());

		for ( ; i < m_submeshes.size() ; i++)
		{
			m_submeshes[i]->ComputeContainers();
			l_box = m_submeshes[i]->GetComboBox();

			if (l_box != NULL)
			{
				if (l_box->GetMin().x < l_min.x)
				{
					l_min.x = l_box->GetMin().x;
				}
				if (l_box->GetMin().y < l_min.y)
				{
					l_min.y = l_box->GetMin().y;
				}
				if (l_box->GetMin().z < l_min.z)
				{
					l_min.z = l_box->GetMin().z;
				}
				if (l_box->GetMax().x > l_max.x)
				{
					l_max.x = l_box->GetMax().x;
				}
				if (l_box->GetMax().y > l_max.y)
				{
					l_max.y = l_box->GetMax().y;
				}
				if (l_box->GetMax().z > l_max.z)
				{
					l_max.z = l_box->GetMax().z;
				}
			}
		}
	}

	m_box = new ComboBox( l_min, l_max);
	m_sphere = new Sphere( *m_box);
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
		l_nbFaces += m_submeshes[i]->m_vertex.size();
	}
	return l_nbFaces;
}

Submesh * Mesh :: GetSubmesh( unsigned int p_index)
{
	if (p_index < m_submeshes.size())
	{
		return m_submeshes[p_index];
	}
	return NULL;
}

Submesh * Mesh :: CreateSubmesh( unsigned int p_nbSmoothGroups)
{
	Submesh * l_submesh = new Submesh( Root::GetRenderSystem()->CreateSubmeshRenderer(), p_nbSmoothGroups);

	if (l_submesh != NULL)
	{
		m_submeshes.push_back( l_submesh);
	}

	return l_submesh;
}

void Mesh :: SetFlatNormals()
{
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->SetFlatNormals();
	}
}

void Mesh :: SetSmoothNormals()
{
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->SetSmoothNormals();
	}
}

void Mesh :: SetNormals()
{
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->SetNormals();
	}
}

void Mesh :: CreateBuffers()
{
	for (unsigned int i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->CreateBuffers();
	}

	m_bOK = true;
}

void Mesh :: CreateNormalsBuffers( NormalsMode p_nm)
{
	m_normalsMode = p_nm;
	vector::cycle( m_submeshes, & Submesh::CreateNormalsBuffer, p_nm);
	ComputeContainers();
}

Mesh * Mesh :: Clone( const String & p_name)
{
	Mesh * l_clone = new Mesh();
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

	m_submeshes[p_index]->Subdivide( p_mode, & m_sphere->GetCenter());

	m_bOK = true;

	return true;
}