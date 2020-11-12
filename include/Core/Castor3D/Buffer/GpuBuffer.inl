namespace castor3d
{
	//*********************************************************************************************

	template< typename AllocatorT >
	GpuBufferT< AllocatorT >::GpuBufferT( RenderSystem const & renderSystem
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, castor::String const & debugName
		, ashes::QueueShare sharingMode
		, AllocatorT allocator )
		: GpuBufferBase{ renderSystem
			, usage
			, memoryFlags
			, debugName
			, castor::move( sharingMode )
			, allocator.getTotalSize() }
		, m_allocator{ castor::move( allocator ) }
	{
	}

	template< typename AllocatorT >
	bool GpuBufferT< AllocatorT >::hasAvailable( VkDeviceSize size )const noexcept
	{
		size = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		return m_allocator.hasAvailable( size_t( size ) );
	}

	template< typename AllocatorT >
	MemChunk GpuBufferT< AllocatorT >::allocate( VkDeviceSize size )
	{
		auto realSize = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		return
		{
			m_allocator.allocate( size_t( realSize ) ),
			realSize,
			size,
		};
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
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, castor::String const & debugName
		, ashes::QueueShare sharingMode
		, AllocatorT allocator )
		: m_device{ device }
		, m_usage{ usage }
		, m_memoryFlags{ memoryFlags }
		, m_sharingMode{ castor::move( sharingMode ) }
		, m_allocatedSize{ allocator.getTotalSize() }
		, m_buffer{ makeBufferBase( device
			, uint32_t( m_allocatedSize )
			, m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, m_memoryFlags
			, debugName
			, m_sharingMode ) }
		, m_allocator{ castor::move( allocator ) }
	{
	}

	template< typename AllocatorT >
	bool GpuBaseBufferT< AllocatorT >::hasAvailable( VkDeviceSize size )const noexcept
	{
		size = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		return m_allocator.hasAvailable( size );
	}

	template< typename AllocatorT >
	MemChunk GpuBaseBufferT< AllocatorT >::allocate( VkDeviceSize size )noexcept
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
