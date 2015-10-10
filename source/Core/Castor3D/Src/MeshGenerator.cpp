#include "MeshGenerator.hpp"
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

	void MeshGenerator::Generate( Mesh & p_mesh, UIntArray const & CU_PARAM_UNUSED( p_faces ), RealArray const & CU_PARAM_UNUSED( p_dimensions ) )
	{
	}

	void MeshGenerator::ComputeNormals( Mesh & p_mesh, bool p_bReverted )
	{
		for ( auto && l_submesh : p_mesh )
		{
			l_submesh->ComputeNormals( p_bReverted );
		}
	}
}
