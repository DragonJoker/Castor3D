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

	VertexBoneData BonedVertex::GetBones()const
	{
		return *reinterpret_cast< VertexBoneData const * >( m_group.const_ptr() );
	}

	void BonedVertex::SetBones( VertexBoneData * p_data )
	{
		std::memcpy( m_group.ptr(), p_data, sizeof( VertexBoneData ) );
	}

	void BonedVertex::SetBones( VertexBoneData const & p_data )
	{
		std::memcpy( m_group.ptr(), &p_data, sizeof( VertexBoneData ) );
	}

	VertexBoneData BonedVertex::GetBones( BufferElementGroupSPtr p_group )
	{
		return *reinterpret_cast< VertexBoneData const * >( p_group->const_ptr() );
	}

	VertexBoneData const & BonedVertex::GetBones( BufferElementGroup const & p_group )
	{
		return *reinterpret_cast< VertexBoneData const * >( p_group.const_ptr() );
	}

	VertexBoneData & BonedVertex::GetBones( BufferElementGroup & p_group )
	{
		return *reinterpret_cast< VertexBoneData * >( p_group.ptr() );
	}

	VertexBoneData const & BonedVertex::GetBones( void const * p_data )
	{
		return *reinterpret_cast< VertexBoneData const * >( p_data );
	}

	VertexBoneData & BonedVertex::GetBones( void * p_data )
	{
		return *reinterpret_cast< VertexBoneData * >( p_data );
	}

	void BonedVertex::SetBones( BufferElementGroupSPtr p_group, VertexBoneData * p_data )
	{
		std::memcpy( p_group->ptr(), p_data, sizeof( VertexBoneData ) );
	}

	void BonedVertex::SetBones( BufferElementGroupSPtr p_group, VertexBoneData const & p_data )
	{
		std::memcpy( p_group->ptr(), &p_data, sizeof( VertexBoneData ) );
	}

	void BonedVertex::SetBones( BufferElementGroup & p_group, VertexBoneData * p_data )
	{
		std::memcpy( p_group.ptr(), p_data, sizeof( VertexBoneData ) );
	}

	void BonedVertex::SetBones( BufferElementGroup & p_group, VertexBoneData const & p_data )
	{
		std::memcpy( p_group.ptr(), &p_data, sizeof( VertexBoneData ) );
	}
}
