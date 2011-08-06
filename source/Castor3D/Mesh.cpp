#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Mesh.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Cone.hpp"
#include "Castor3D/Cylinder.hpp"
#include "Castor3D/Icosaedron.hpp"
#include "Castor3D/Cube.hpp"
#include "Castor3D/Plane.hpp"
#include "Castor3D/Sphere.hpp"
#include "Castor3D/Torus.hpp"
#include "Castor3D/Projection.hpp"
#include "Castor3D/Pattern.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/SmoothingGroup.hpp"

using namespace Castor3D;

//*************************************************************************************************

Factory<MeshCategory, eMESH_TYPE>::obj_map Factory<MeshCategory, eMESH_TYPE>::m_mapRegistered;

//*************************************************************************************************

MeshCategory :: MeshCategory( eMESH_TYPE p_eMeshType)
	:	m_eMeshType( p_eMeshType)
{
}

MeshCategory :: ~MeshCategory()
{
}

void MeshCategory :: GeneratePoints()
{
}

MeshCategoryPtr MeshCategory :: Clone()
{
	return MeshCategoryPtr( new MeshCategory( m_eMeshType));
}

void MeshCategory :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
}

void MeshCategory :: ComputeNormals( bool p_bReverted)
{
	for (size_t i = 0 ; i < m_pMesh->GetNbSubmeshes() ; i++)
	{
		m_pMesh->GetSubmesh( i)->ComputeNormals();
	}
}

BEGIN_SERIALISE_MAP( MeshCategory, Serialisable)
	ADD_ELEMENT( m_eMeshType)
END_SERIALISE_MAP()

//*************************************************************************************************

Mesh :: Mesh( eMESH_TYPE p_eMeshType)
	:	Resource<Mesh>( cuEmptyString)
	,	m_modified( false)
	,	m_bOK( false)
{
	Factory<MeshCategory, eMESH_TYPE> l_factory;
	m_pMeshCategory = l_factory.Create( p_eMeshType);
	m_pMeshCategory->SetMesh( this);
}

Mesh :: Mesh( String const & p_name, eMESH_TYPE p_eMeshType)
	:	Resource<Mesh>( p_name)
	,	m_modified( false)
	,	m_bOK( false)
{
	Factory<MeshCategory, eMESH_TYPE> l_factory;
	m_pMeshCategory = l_factory.Create( p_eMeshType);
	m_pMeshCategory->SetMesh( this);
}

Mesh :: ~Mesh()
{
	Cleanup();
}

void Mesh :: Register()
{
	Factory<MeshCategory, eMESH_TYPE> l_factory;
	l_factory.Register(	eMESH_TYPE_CONE,		ConePtr( new Cone));
	l_factory.Register(	eMESH_TYPE_CUBE,		CubePtr( new Cube));
	l_factory.Register(	eMESH_TYPE_CUSTOM,		MeshCategoryPtr( new MeshCategory));
	l_factory.Register(	eMESH_TYPE_CYLINDER,	CylinderPtr( new Cylinder));
	l_factory.Register(	eMESH_TYPE_ICOSAEDRON,	IcosaedronPtr( new Icosaedron));
	l_factory.Register(	eMESH_TYPE_PLANE,		PlanePtr( new Plane));
	l_factory.Register(	eMESH_TYPE_PROJECTION,	ProjectionPtr( new Projection));
	l_factory.Register(	eMESH_TYPE_SPHERE,		SpherePtr( new Sphere));
	l_factory.Register(	eMESH_TYPE_TORUS,		TorusPtr( new Torus));
}

void Mesh :: Cleanup()
{
	CASTOR_TRACK;
//	vector::deleteAll( m_submeshes);
	m_submeshes.clear();
}

void Mesh :: Initialise( UIntArray p_arrayFaces, FloatArray p_arrayDimensions)
{
	m_pMeshCategory->Initialise( p_arrayFaces, p_arrayDimensions);
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

	Point3r l_min;
	Point3r l_max;
	l_min.copy( m_submeshes[0]->GetCubeBox().GetMin());
	l_max.copy( m_submeshes[0]->GetCubeBox().GetMax());

	for (size_t i = 1 ; i < m_submeshes.size() ; i++)
	{
		CubeBox const & l_box = m_submeshes[i]->GetCubeBox();

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
		l_nbFaces += m_submeshes[i]->GetNbPoints();
	}
	return l_nbFaces;
}

SubmeshPtr Mesh :: GetSubmesh( unsigned int p_index)const
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

	if (l_submesh)
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
	m_pMeshCategory->ComputeNormals();
}

void Mesh :: InitialiseBuffers()
{
	CASTOR_TRACK;
	for (unsigned int i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->InitialiseBuffers();
	}

	m_bOK = true;
}

void Mesh :: SetNormals( eNORMALS_MODE p_nm)
{
	CASTOR_TRACK;
	m_normalsMode = p_nm;
//	vector::cycle( m_submeshes, & Submesh::CreateNormalsBuffer, p_nm);
	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		m_submeshes[i]->SetNormals( p_nm);
	}

	ComputeContainers();
}

MeshPtr Mesh :: Clone( String const & p_name)
{
	MeshPtr l_clone( new Mesh( p_name, GetMeshType()));

	for (size_t i = 0 ; i < m_submeshes.size() ; i++)
	{
		l_clone->m_submeshes.push_back( m_submeshes[i]->Clone());
	}

	l_clone->ComputeContainers();

	return l_clone;
}

bool Mesh :: Subdivide( unsigned int p_index, SubdividerPtr p_pSubdivider, bool p_bThreaded)
{
	if (p_index >= m_submeshes.size())
	{
		return false;
	}

	m_bOK = false;

	m_submeshes[p_index]->Subdivide( p_pSubdivider, & m_sphere.GetCenter(), true, p_bThreaded);

	m_bOK = true;

	return true;
}

BEGIN_SERIALISE_MAP( Mesh, Serialisable)
	ADD_ELEMENT( m_strName)
	ADD_ELEMENT( * m_pMeshCategory)
	ADD_ELEMENT( m_submeshes)
END_SERIALISE_MAP()


BEGIN_POST_UNSERIALISE( Mesh, Serialisable)
END_POST_UNSERIALISE()

//*********************************************************************************************