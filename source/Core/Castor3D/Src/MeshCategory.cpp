#include "MeshCategory.hpp"
#include "Submesh.hpp"

using namespace Castor;

namespace Castor3D
{
	MeshCategory::MeshCategory( eMESH_TYPE p_eMeshType )
		:	m_eMeshType( p_eMeshType )
		,	m_pMesh( NULL )
	{
	}

	MeshCategory::~MeshCategory()
	{
	}

	MeshCategorySPtr MeshCategory::Create()
	{
		return std::make_shared< MeshCategory >( eMESH_TYPE_CUSTOM );
	}

	void MeshCategory::Generate()
	{
	}

	void MeshCategory::Initialise( UIntArray const & CU_PARAM_UNUSED( p_arrayFaces ), RealArray const & CU_PARAM_UNUSED( p_arrayDimensions ) )
	{
	}

	void MeshCategory::ComputeNormals( bool p_bReverted )
	{
		std::for_each( m_pMesh->Begin(), m_pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
		{
			p_pSubmesh->ComputeNormals( p_bReverted );
		} );
	}
}
