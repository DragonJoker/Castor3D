#include "BonedVertex.hpp"

using namespace Castor;

namespace Castor3D
{
	BonedVertex::BonedVertex( BufferElementGroup & p_group )
		:	Vertex( p_group )
	{
	}

	BonedVertex::BonedVertex( BonedVertex const & p_source )
		:	Vertex( p_source )
	{
	}

	BonedVertex::BonedVertex( Vertex const & p_source )
		:	Vertex( p_source )
	{
	}

	BonedVertex & BonedVertex::operator =( BonedVertex const & p_source )
	{
		Vertex::operator =( p_source );
		return *this;
	}

	BonedVertex & BonedVertex::operator =( Vertex const & p_source )
	{
		Vertex::operator =( p_source );
		return *this;
	}

	BonedVertex::~BonedVertex()
	{
	}

	stVERTEX_BONE_DATA BonedVertex::GetBones()const
	{
		return *reinterpret_cast< stVERTEX_BONE_DATA const * >( &m_group.const_ptr()[sm_uiOffsetBon] );
	}

	void BonedVertex::SetBones( stVERTEX_BONE_DATA * p_pData )
	{
		std::memcpy( &m_group.ptr()[sm_uiOffsetBon], p_pData, sizeof( stVERTEX_BONE_DATA ) );
	}

	void BonedVertex::SetBones( stVERTEX_BONE_DATA const & p_data )
	{
		std::memcpy( &m_group.ptr()[sm_uiOffsetBon], &p_data, sizeof( stVERTEX_BONE_DATA ) );
	}

	stVERTEX_BONE_DATA BonedVertex::GetBones( BufferElementGroupSPtr p_pGroup )
	{
		return *reinterpret_cast< stVERTEX_BONE_DATA const * >( &p_pGroup->const_ptr()[sm_uiOffsetBon] );
	}

	stVERTEX_BONE_DATA BonedVertex::GetBones( BufferElementGroup const & p_group )
	{
		return *reinterpret_cast< stVERTEX_BONE_DATA const * >( &p_group.const_ptr()[sm_uiOffsetBon] );
	}

	void BonedVertex::SetBones( BufferElementGroupSPtr p_pGroup, stVERTEX_BONE_DATA * p_pData )
	{
		std::memcpy( &p_pGroup->ptr()[sm_uiOffsetBon], p_pData, sizeof( stVERTEX_BONE_DATA ) );
	}

	void BonedVertex::SetBones( BufferElementGroupSPtr p_pGroup, stVERTEX_BONE_DATA const & p_data )
	{
		std::memcpy( &p_pGroup->ptr()[sm_uiOffsetBon], &p_data, sizeof( stVERTEX_BONE_DATA ) );
	}

	void BonedVertex::SetBones( BufferElementGroup & p_group, stVERTEX_BONE_DATA * p_pData )
	{
		std::memcpy( &p_group.ptr()[sm_uiOffsetBon], p_pData, sizeof( stVERTEX_BONE_DATA ) );
	}

	void BonedVertex::SetBones( BufferElementGroup & p_group, stVERTEX_BONE_DATA const & p_data )
	{
		std::memcpy( &p_group.ptr()[sm_uiOffsetBon], &p_data, sizeof( stVERTEX_BONE_DATA ) );
	}
}
