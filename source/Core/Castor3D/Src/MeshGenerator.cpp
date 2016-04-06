#include "MeshGenerator.hpp"

#include "Engine.hpp"
#include "InitialiseEvent.hpp"
#include "Mesh.hpp"
#include "Submesh.hpp"

using namespace Castor;

namespace Castor3D
{
	MeshGenerator::MeshGenerator( eMESH_TYPE p_eMeshType )
		: m_eMeshType( p_eMeshType )
	{
	}

	MeshGenerator::~MeshGenerator()
	{
	}

	MeshGeneratorSPtr MeshGenerator::Create()
	{
		return std::make_shared< MeshGenerator >( eMESH_TYPE_CUSTOM );
	}

	void MeshGenerator::Generate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions )
	{
		DoGenerate( p_mesh, p_faces, p_dimensions );

		for ( auto && l_submesh : p_mesh )
		{
			p_mesh.GetEngine()->PostEvent( MakeInitialiseEvent( *l_submesh ) );
		}
	}

	void MeshGenerator::ComputeNormals( Mesh & p_mesh, bool p_reverted )
	{
		for ( auto && l_submesh : p_mesh )
		{
			l_submesh->ComputeNormals( p_reverted );
		}
	}

	void MeshGenerator::DoGenerate( Mesh & CU_PARAM_UNUSED( p_mesh ), UIntArray const & CU_PARAM_UNUSED( p_faces ), RealArray const & CU_PARAM_UNUSED( p_dimensions ) )
	{
	}
}
