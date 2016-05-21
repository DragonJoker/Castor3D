#include "BonedVertex.hpp"

#include "VertexBoneData.hpp"

using namespace Castor;

namespace Castor3D
{
	BonedVertex::BonedVertex( BufferElementGroup & p_group )
		: m_group( p_group )
	{
	}

	BonedVertex::BonedVertex( BonedVertex const & p_source )
		: m_group( p_source.m_group )
	{
	}

	BonedVertex & BonedVertex::operator=( BonedVertex const & p_source )
	{
		m_group = p_source.m_group;
		return *this;
	}

	BonedVertex::~BonedVertex()
	{
	}

	stVERTEX_BONE_DATA BonedVertex::GetBones()const
	{
		return *reinterpret_cast< stVERTEX_BONE_DATA const * >( m_group.const_ptr() );
	}

	void BonedVertex::SetBones( stVERTEX_BONE_DATA * p_data )
	{
		std::memcpy( m_group.ptr(), p_data, sizeof( stVERTEX_BONE_DATA ) );
	}

	void BonedVertex::SetBones( stVERTEX_BONE_DATA const & p_data )
	{
		std::memcpy( m_group.ptr(), &p_data, sizeof( stVERTEX_BONE_DATA ) );
	}

	stVERTEX_BONE_DATA BonedVertex::GetBones( BufferElementGroupSPtr p_group )
	{
		return *reinterpret_cast< stVERTEX_BONE_DATA const * >( p_group->const_ptr() );
	}

	stVERTEX_BONE_DATA BonedVertex::GetBones( BufferElementGroup const & p_group )
	{
		return *reinterpret_cast< stVERTEX_BONE_DATA const * >( p_group.const_ptr() );
	}

	void BonedVertex::SetBones( BufferElementGroupSPtr p_group, stVERTEX_BONE_DATA * p_data )
	{
		std::memcpy( p_group->ptr(), p_data, sizeof( stVERTEX_BONE_DATA ) );
	}

	void BonedVertex::SetBones( BufferElementGroupSPtr p_group, stVERTEX_BONE_DATA const & p_data )
	{
		std::memcpy( p_group->ptr(), &p_data, sizeof( stVERTEX_BONE_DATA ) );
	}

	void BonedVertex::SetBones( BufferElementGroup & p_group, stVERTEX_BONE_DATA * p_data )
	{
		std::memcpy( p_group.ptr(), p_data, sizeof( stVERTEX_BONE_DATA ) );
	}

	void BonedVertex::SetBones( BufferElementGroup & p_group, stVERTEX_BONE_DATA const & p_data )
	{
		std::memcpy( p_group.ptr(), &p_data, sizeof( stVERTEX_BONE_DATA ) );
	}
}
