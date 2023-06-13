#include "Castor3D/Buffer/GpuBufferPool.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/FramePass.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Sync/Fence.hpp>

CU_ImplementSmartPtr( castor3d, GpuBufferPool )

namespace castor3d
{
	//*********************************************************************************************

	namespace gpupol
	{
		static crg::VkBufferArray makeVkArray( std::vector< ashes::BufferBase const * > const & buffers )
		{
			crg::VkBufferArray result;

			for ( auto buffer : buffers )
			{
				result.push_back( *buffer );
			}

			return result;
		}
	}

	//*********************************************************************************************

	void createUniformPassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > buffers
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		crg::VkBufferArray vkBuffers = gpupol::makeVkArray( buffers );
		pass.addUniformBuffer( { vkBuffers, name }
			, binding
			, offset
			, size );
	}

	void createInputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > buffers
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		crg::VkBufferArray vkBuffers = gpupol::makeVkArray( buffers );
		pass.addInputStorageBuffer( { vkBuffers, name }
			, binding
			, offset
			, size );
	}

	void createInOutStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > buffers
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		crg::VkBufferArray vkBuffers = gpupol::makeVkArray( buffers );
		pass.addInOutStorageBuffer( { vkBuffers, name }
			, binding
			, offset
			, size );
	}

	void createOutputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > buffers
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		crg::VkBufferArray vkBuffers = gpupol::makeVkArray( buffers );
		pass.addOutputStorageBuffer( { vkBuffers, name }
			, binding
			, offset
			, size );
	}

	void createClearableOutputStorageBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, std::vector< ashes::BufferBase const * > buffers
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		crg::VkBufferArray vkBuffers = gpupol::makeVkArray( buffers );
		pass.addClearableOutputStorageBuffer( { vkBuffers, name }
			, binding
			, offset
			, size );
	}

	void createUniformPassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		createUniformPassBinding( pass
			, binding
			, name
			, { &buffer }
			, offset
			, size );
	}

	void createInputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		createInputStoragePassBinding( pass
			, binding
			, name
			, { &buffer }
			, offset
			, size );
	}

	void createInOutStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		createInOutStoragePassBinding( pass
			, binding
			, name
			, { &buffer }
			, offset
			, size );
	}

	void createOutputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		createOutputStoragePassBinding( pass
			, binding
			, name
			, { &buffer }
			, offset
			, size );
	}

	void createClearableOutputStorageBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::BufferBase const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		createClearableOutputStorageBinding( pass
			, binding
			, name
			, { &buffer }
			, offset
			, size );
	}

	void createUniformPassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		pass.addUniformBuffer( { buffer, name }
			, binding
			, offset
			, size );
	}

	void createInputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		pass.addInputStorageBuffer( { buffer, name }
			, binding
			, offset
			, size );
	}

	void createInOutStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		pass.addInOutStorageBuffer( { buffer, name }
			, binding
			, offset
			, size );
	}

	void createOutputStoragePassBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		pass.addOutputStorageBuffer( { buffer, name }
			, binding
			, offset
			, size );
	}

	void createClearableOutputStorageBinding( crg::FramePass & pass
		, uint32_t binding
		, std::string const & name
		, ashes::Buffer< uint8_t > const & buffer
		, VkDeviceSize offset
		, VkDeviceSize size )
	{
		pass.addClearableOutputStorageBuffer( { buffer, name }
			, binding
			, offset
			, size );
	}

	//*********************************************************************************************

	GpuBufferPool::GpuBufferPool( RenderDevice const & device
		, castor::String debugName )
		: OwnedBy< RenderSystem >{ device.renderSystem }
		, m_debugName{ std::move( debugName ) }
		, m_minBlockSize{ uint32_t( device.renderSystem.getProperties().limits.minMemoryMapAlignment ) }
	{
	}

	void GpuBufferPool::cleanup()
	{
		m_buffers.clear();
	}

	void GpuBufferPool::upload( UploadData & uploader )
	{
		for ( auto & buffers : m_buffers )
		{
			for ( auto & buffer : buffers.second )
			{
				buffer->upload( uploader );
			}
		}
	}

	GpuBufferBase & GpuBufferPool::doGetBuffer( VkDeviceSize size
		, VkBufferUsageFlags target
		, VkMemoryPropertyFlags memory
		, MemChunk & chunk )
	{
		auto key = doMakeKey( target, memory );
		auto it = m_buffers.find( key );

		if ( it == m_buffers.end() )
		{
			it = m_buffers.emplace( key, BufferArray{} ).first;
		}

		auto itB = doFindBuffer( size, it->second );

		if ( itB == it->second.end() )
		{
			VkDeviceSize level = 21u;
			VkDeviceSize maxSize = VkDeviceSize( 1ull << level ) * m_minBlockSize;

			while ( size > maxSize && level <= 24 )
			{
				++level;
				maxSize = VkDeviceSize( 1ull << level ) * m_minBlockSize;
			}

			CU_Require( maxSize < std::numeric_limits< uint32_t >::max() );
			CU_Require( maxSize >= size );

			auto buffer = std::make_unique< GpuBuddyBuffer >( *getRenderSystem()
				, target
				, memory
				, m_debugName
				, ashes::QueueShare{}
				, GpuBufferBuddyAllocator{ uint32_t( level ), m_minBlockSize } );
			it->second.emplace_back( std::move( buffer ) );
			itB = std::next( it->second.begin()
				, ptrdiff_t( it->second.size() - 1u ) );
		}

		chunk = ( *itB )->allocate( size );
		return *( *itB ).get();
	}

	void GpuBufferPool::doPutBuffer( GpuBufferBase const & buffer
		, VkBufferUsageFlags target
		, VkMemoryPropertyFlags memory
		, MemChunk const & chunk )
	{
		auto key = doMakeKey( target, memory );
		auto it = m_buffers.find( key );
		CU_Require( it != m_buffers.end() );
		auto itB = std::find_if( it->second.begin()
			, it->second.end()
			, [&buffer]( std::unique_ptr< GpuBuddyBuffer > const & lookup )
			{
				return &lookup->getBuffer().getBuffer() == &buffer.getBuffer().getBuffer();
			} );
		CU_Require( itB != it->second.end() );
		( *itB )->deallocate( chunk );
	}

	GpuBufferPool::BufferArray::iterator GpuBufferPool::doFindBuffer( VkDeviceSize size
		, GpuBufferPool::BufferArray & array )
	{
		auto it = array.begin();

		while ( it != array.end() && !( *it )->hasAvailable( size ) )
		{
			++it;
		}

		return it;
	}

	uint32_t GpuBufferPool::doMakeKey( VkBufferUsageFlags target
		, VkMemoryPropertyFlags flags )
	{
		return ( target << 0u )
			| ( flags << 16u );
	}

	//*********************************************************************************************
}
