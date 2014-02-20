#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Mesh.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Cone.hpp"
#include "Castor3D/Cylinder.hpp"
#include "Castor3D/Icosahedron.hpp"
#include "Castor3D/Cube.hpp"
#include "Castor3D/Plane.hpp"
#include "Castor3D/Sphere.hpp"
#include "Castor3D/Torus.hpp"
#include "Castor3D/Projection.hpp"
#include "Castor3D/Animation.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

MeshCategory :: MeshCategory( eMESH_TYPE p_eMeshType )
	:	m_eMeshType	( p_eMeshType	)
	,	m_pMesh		( NULL			)
{
}

MeshCategory :: ~MeshCategory()
{
}

MeshCategorySPtr MeshCategory :: Create()
{
	return std::make_shared< MeshCategory >( eMESH_TYPE_CUSTOM );
}

void MeshCategory :: Generate()
{
}

void MeshCategory :: Initialise( UIntArray const & CU_PARAM_UNUSED( p_arrayFaces ), RealArray const & CU_PARAM_UNUSED( p_arrayDimensions ) )
{
}

void MeshCategory :: ComputeNormals( bool p_bReverted )
{
	std::for_each( m_pMesh->Begin(), m_pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
	{
		p_pSubmesh->ComputeNormals( p_bReverted );
	} );
}

//*************************************************************************************************

Mesh :: Mesh( Engine * p_pEngine, MeshFactory & p_factory, eMESH_TYPE p_eMeshType)
	:	Resource< Mesh >	( cuEmptyString	)
	,	m_modified			( false			)
	,	m_factory			( p_factory		)
	,	m_pEngine			( p_pEngine		)
{
	m_pMeshCategory = m_factory.Create( p_eMeshType );
	m_pMeshCategory->SetMesh( this);
}

Mesh :: Mesh( Engine * p_pEngine, MeshFactory & p_factory, String const & p_name, eMESH_TYPE p_eMeshType)
	:	Resource< Mesh >	( p_name		)
	,	m_modified			( false			)
	,	m_factory			( p_factory		)
	,	m_pEngine			( p_pEngine		)
{
	m_pMeshCategory = m_factory.Create( p_eMeshType );
	m_pMeshCategory->SetMesh( this);
}

Mesh :: ~Mesh()
{
	Cleanup();
}

void Mesh :: Register( MeshFactory & p_factory )
{
	p_factory.Register(	eMESH_TYPE_CONE,		Cone::Create			);
	p_factory.Register(	eMESH_TYPE_CUBE,		Cube::Create			);
	p_factory.Register(	eMESH_TYPE_CUSTOM,		MeshCategory::Create	);
	p_factory.Register(	eMESH_TYPE_CYLINDER,	Cylinder::Create		);
	p_factory.Register(	eMESH_TYPE_ICOSAHEDRON,	Icosahedron::Create		);
	p_factory.Register(	eMESH_TYPE_PLANE,		Plane::Create			);
	p_factory.Register(	eMESH_TYPE_PROJECTION,	Projection::Create		);
	p_factory.Register(	eMESH_TYPE_SPHERE,		Sphere::Create			);
	p_factory.Register(	eMESH_TYPE_TORUS,		Torus::Create			);
}

void Mesh :: Cleanup()
{
//	vector::deleteAll( m_submeshes);
	m_submeshes.clear();
}

void Mesh :: Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions)
{
	m_pMeshCategory->Initialise( p_arrayFaces, p_arrayDimensions );
}

void Mesh :: ComputeContainers()
{
	if (m_submeshes.size() == 0)
	{
		return;
	}

	uint32_t l_uiCount = GetSubmeshCount();

	for( uint32_t i = 0; i < l_uiCount; i++)
	{
		m_submeshes[i]->ComputeContainers();
	}

	Point3r l_min( m_submeshes[0]->GetCubeBox().GetMin());
	Point3r l_max( m_submeshes[0]->GetCubeBox().GetMax());

	for (uint32_t i = 1; i < l_uiCount; i++)
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

uint32_t Mesh :: GetNbFaces()const
{
	uint32_t l_nbFaces = 0;

	std::for_each( m_submeshes.begin(), m_submeshes.end(), [&]( SubmeshSPtr p_pSubmesh )
	{
		l_nbFaces += p_pSubmesh->GetFacesCount();
	} );

	return l_nbFaces;
}

uint32_t Mesh :: GetNbVertex()const
{
	uint32_t l_nbFaces = 0;

	std::for_each( m_submeshes.begin(), m_submeshes.end(), [&]( SubmeshSPtr p_pSubmesh )
	{
		l_nbFaces += p_pSubmesh->GetPointsCount();
	} );

	return l_nbFaces;
}

SubmeshSPtr Mesh :: GetSubmesh( uint32_t p_index)const
{
	SubmeshSPtr l_pReturn;

	if (p_index < m_submeshes.size())
	{
		l_pReturn = m_submeshes[p_index];
	}

	return l_pReturn;
}

SubmeshSPtr Mesh :: CreateSubmesh()
{
	SubmeshSPtr l_submesh = std::make_shared< Submesh >( this, m_pEngine, GetSubmeshCount() );

	if (l_submesh)
	{
		m_submeshes.push_back( l_submesh);
	}

	return l_submesh;
}

void Mesh :: DeleteSubmesh( SubmeshSPtr & p_pSubmesh )
{
	SubmeshPtrArray::iterator l_it = m_submeshes.begin();

	while( l_it != m_submeshes.end() )
	{
		if( *l_it == p_pSubmesh )
		{
			m_submeshes.erase( l_it );
			p_pSubmesh.reset();
			l_it = m_submeshes.end();
		}
		else
		{
			++l_it;
		}
	}
}

void Mesh :: ComputeNormals( bool p_bReverted )
{
	m_pMeshCategory->ComputeNormals( p_bReverted );
}

void Mesh :: GenerateBuffers()
{
	std::for_each( m_submeshes.begin(), m_submeshes.end(), [&]( SubmeshSPtr p_pSubmesh )
	{
		p_pSubmesh->GenerateBuffers();
	} );
}

MeshSPtr Mesh :: Clone( String const & p_name)
{
	MeshSPtr l_clone = std::make_shared< Mesh >( m_pEngine, m_factory, p_name, GetMeshType() );

	std::for_each( m_submeshes.begin(), m_submeshes.end(), [&]( SubmeshSPtr p_pSubmesh )
	{
		l_clone->m_submeshes.push_back( p_pSubmesh->Clone());
	} );

	l_clone->ComputeContainers();

	return l_clone;
}

void Mesh :: Ref()
{
	std::for_each( m_submeshes.begin(), m_submeshes.end(), [&]( SubmeshSPtr p_pSubmesh )
	{
		p_pSubmesh->Ref();
	} );
}

void Mesh :: UnRef()
{
	std::for_each( m_submeshes.begin(), m_submeshes.end(), [&]( SubmeshSPtr p_pSubmesh )
	{
		p_pSubmesh->UnRef();
	} );
}

//*********************************************************************************************
