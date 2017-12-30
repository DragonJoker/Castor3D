#include "BonedVertex.hpp"

#include "VertexBoneData.hpp"

using namespace castor;

namespace castor3d
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

	VertexBoneData BonedVertex::getBones()const
	{
		return *reinterpret_cast< VertexBoneData const * >( m_group.constPtr() );
	}

	void BonedVertex::setBones( VertexBoneData * p_data )
	{
		std::memcpy( m_group.ptr(), p_data, sizeof( VertexBoneData ) );
	}

	void BonedVertex::setBones( VertexBoneData const & p_data )
	{
		std::memcpy( m_group.ptr(), &p_data, sizeof( VertexBoneData ) );
	}

	VertexBoneData BonedVertex::getBones( BufferElementGroupSPtr p_group )
	{
		return *reinterpret_cast< VertexBoneData const * >( p_group->constPtr() );
	}

	VertexBoneData const & BonedVertex::getBones( BufferElementGroup const & p_group )
	{
		return *reinterpret_cast< VertexBoneData const * >( p_group.constPtr() );
	}

	VertexBoneData & BonedVertex::getBones( BufferElementGroup & p_group )
	{
		return *reinterpret_cast< VertexBoneData * >( p_group.ptr() );
	}

	VertexBoneData const & BonedVertex::getBones( void const * p_data )
	{
		return *reinterpret_cast< VertexBoneData const * >( p_data );
	}

	VertexBoneData & BonedVertex::getBones( void * p_data )
	{
		return *reinterpret_cast< VertexBoneData * >( p_data );
	}

	void BonedVertex::setBones( BufferElementGroupSPtr p_group, VertexBoneData * p_data )
	{
		std::memcpy( p_group->ptr(), p_data, sizeof( VertexBoneData ) );
	}

	void BonedVertex::setBones( BufferElementGroupSPtr p_group, VertexBoneData const & p_data )
	{
		std::memcpy( p_group->ptr(), &p_data, sizeof( VertexBoneData ) );
	}

	void BonedVertex::setBones( BufferElementGroup & p_group, VertexBoneData * p_data )
	{
		std::memcpy( p_group.ptr(), p_data, sizeof( VertexBoneData ) );
	}

	void BonedVertex::setBones( BufferElementGroup & p_group, VertexBoneData const & p_data )
	{
		std::memcpy( p_group.ptr(), &p_data, sizeof( VertexBoneData ) );
	}
}
