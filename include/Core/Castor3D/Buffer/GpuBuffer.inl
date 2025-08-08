#include "Castor3D/Render/Buffer.hpp"

namespace c3d
{
	//*********************************************************************************************

	template< typename AllocatorT >
	GpuBufferT< AllocatorT >::GpuBufferT( RenderSystem const & renderSystem
		, crg::ResourcesCache & resources
		, BufferUsageFlags usage
		, MemoryPropertyFlags memoryFlags
		, String const & debugName
		, ashes::QueueShare sharingMode
		, AllocatorT allocator )
		: GpuBufferBase{ renderSystem
		, resources
		, usage
		, memoryFlags
		, debugName
		, c3d::move( sharingMode )
		, allocator.getTotalSize() }
		, m_allocator{ c3d::move( allocator ) }
	{
	}

	template< typename AllocatorT >
	bool GpuBufferT< AllocatorT >::hasAvailable( DeviceSize size )const noexcept
	{
		size = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		return m_allocator.hasAvailable( size_t( size ) );
	}

	template< typename AllocatorT >
	DeviceSize GpuBufferT< AllocatorT >::getAvailable()const noexcept
	{
		return m_allocator.getAvailable();
	}

	template< typename AllocatorT >
	MemChunk GpuBufferT< AllocatorT >::allocate( DeviceSize size )
	{
		auto realSize = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		auto offset = m_allocator.allocate( size_t( realSize ) );
		return { offset
			, realSize, size
			, getSubView( offset, realSize ) };
	}

	template< typename AllocatorT >
	void GpuBufferT< AllocatorT >::deallocate( MemChunk const & mem )noexcept
	{
		m_allocator.deallocate( mem.offset );
	}

	template< typename AllocatorT >
	size_t GpuBufferT< AllocatorT >::getMinAlignment()const noexcept
	{
		return m_allocator.getAlignSize();
	}

	//*********************************************************************************************

	template< typename AllocatorT >
	GpuBaseBufferT< AllocatorT >::GpuBaseBufferT( RenderDevice const & device
		, crg::ResourcesCache & resources
		, BufferUsageFlags usage
		, MemoryPropertyFlags memoryFlags
		, String const & debugName
		, ashes::QueueShare sharingMode
		, AllocatorT allocator )
		: m_device{ device }
		, m_usage{ usage }
		, m_memoryFlags{ memoryFlags }
		, m_sharingMode{ c3d::move( sharingMode ) }
		, m_allocatedSize{ allocator.getTotalSize() }
		, m_buffer{ makeBufferBase( device, resources
			, uint32_t( m_allocatedSize )
			, m_usage | BufferUsageFlags::eTransferDst
			, m_memoryFlags
			, debugName ) }
		, m_allocator{ c3d::move( allocator ) }
	{
	}

	template< typename AllocatorT >
	GpuBaseBufferT< AllocatorT >::~GpuBaseBufferT()noexcept
	{
		m_buffer->destroy();
	}

	template< typename AllocatorT >
	DeviceSize GpuBaseBufferT< AllocatorT >::getAvailable()const noexcept
	{
		return m_allocator.getAvailable();
	}

	template< typename AllocatorT >
	bool GpuBaseBufferT< AllocatorT >::hasAvailable( DeviceSize size )const noexcept
	{
		size = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		return m_allocator.hasAvailable( size );
	}

	template< typename AllocatorT >
	MemChunk GpuBaseBufferT< AllocatorT >::allocate( DeviceSize size )noexcept
	{
		auto realSize = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		auto offset = m_allocator.allocate( realSize );
		return { offset, realSize, size
			, m_buffer->getSubView( offset, realSize ) };
	}

	template< typename AllocatorT >
	void GpuBaseBufferT< AllocatorT >::deallocate( MemChunk const & mem )noexcept
	{
		m_allocator.deallocate( mem.offset );
	}

	template< typename AllocatorT >
	size_t GpuBaseBufferT< AllocatorT >::getMinAlignment()const noexcept
	{
		return m_allocator.getAlignSize();
	}

	//*********************************************************************************************
}
