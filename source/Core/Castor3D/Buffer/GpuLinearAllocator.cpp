#include "Castor3D/Buffer/GpuLinearAllocator.hpp"

namespace castor3d
{
	GpuLinearAllocator::GpuLinearAllocator( size_t count
		, uint32_t alignment )
		: m_alignSize{ alignment }
		, m_allocatedSize{ count * m_alignSize }
	{
		m_free.resize( count );
		VkDeviceSize offset{ count * m_alignSize };

		for ( auto & v : m_free )
		{
			offset -= m_alignSize;
			v = offset;
		}
	}

	VkDeviceSize GpuLinearAllocator::allocate( size_t size )
	{
		CU_Require( size <= m_alignSize );
		VkDeviceSize result{};

		if ( !m_free.empty() )
		{
			result = m_free.back();
			m_free.pop_back();
			return result;
		}

		return result;
	}

	void GpuLinearAllocator::deallocate( VkDeviceSize pointer )
	{
		CU_Require( pointer < m_allocatedSize );
		m_free.push_back( pointer );
	}
}
