namespace castor3d
{
	//*********************************************************************************************

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

	//*********************************************************************************************

	template< typename AllocatorT >
	GpuBaseBufferT< AllocatorT >::GpuBaseBufferT( RenderDevice const & device
		, VkBufferUsageFlags usage
		, VkMemoryPropertyFlags memoryFlags
		, castor::String debugName
		, ashes::QueueShare sharingMode
		, AllocatorT allocator )
		: m_device{ device }
		, m_usage{ usage }
		, m_memoryFlags{ memoryFlags }
		, m_sharingMode{ std::move( sharingMode ) }
		, m_allocatedSize{ allocator.getTotalSize() }
		, m_buffer{ makeBufferBase( device
			, uint32_t( m_allocatedSize )
			, m_usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, m_memoryFlags
			, debugName
			, m_sharingMode ) }
		, m_allocator{ std::move( allocator ) }
	{
	}

	template< typename AllocatorT >
	bool GpuBaseBufferT< AllocatorT >::hasAvailable( VkDeviceSize size )const
	{
		size = ashes::getAlignedSize( size, m_allocator.getAlignSize() );
		return m_allocator.hasAvailable( size );
	}

	template< typename AllocatorT >
	MemChunk GpuBaseBufferT< AllocatorT >::allocate( VkDeviceSize size )
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
	void GpuBaseBufferT< AllocatorT >::deallocate( MemChunk const & mem )
	{
		m_allocator.deallocate( mem.offset );
	}

	template< typename AllocatorT >
	size_t GpuBaseBufferT< AllocatorT >::getMinAlignment()const
	{
		return m_allocator.getAlignSize();
	}

	//*********************************************************************************************
}
