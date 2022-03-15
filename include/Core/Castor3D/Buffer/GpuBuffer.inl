namespace castor3d
{
	template< typename AllocatorT >
	GpuBufferT< AllocatorT >::GpuBufferT( RenderSystem const & renderSystem
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, castor::String debugName
		, ashes::QueueShare sharingMode
		, AllocatorT allocator
		, bool smallData )
		: GpuBufferBase{ renderSystem
			, usage
			, memoryFlags
			, std::move( debugName )
			, std::move( sharingMode )
			, allocator.getTotalSize()
			, smallData }
		, m_allocator{ std::move( allocator ) }
	{
	}

	template< typename AllocatorT >
	bool GpuBufferT< AllocatorT >::hasAvailable( VkDeviceSize size )const
	{
		size = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		return m_allocator.hasAvailable( size );
	}

	template< typename AllocatorT >
	MemChunk GpuBufferT< AllocatorT >::allocate( VkDeviceSize size )
	{
		auto realSize = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		return
		{
			m_allocator.allocate( realSize ),
			realSize,
			size,
		};
	}

	template< typename AllocatorT >
	void GpuBufferT< AllocatorT >::deallocate( MemChunk const & mem )
	{
		m_allocator.deallocate( mem.offset );
	}

	template< typename AllocatorT >
	size_t GpuBufferT< AllocatorT >::getMinAlignment()const
	{
		return m_allocator.getAlignSize();
	}
}
