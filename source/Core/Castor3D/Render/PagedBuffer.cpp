#include "Castor3D/Render/PagedBuffer.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

#include <RenderGraph/ResourceHandler.hpp>

#include <ashespp/Buffer/Buffer.hpp>

CU_ImplementSmartPtr( c3d, PagedBuffer )

namespace c3d
{
	//*********************************************************************************************

	PagedBufferBase::PagedBufferBase( PagedBufferBase && rhs )noexcept
		: resources{ c3d::move( rhs.resources ) }
		, device{ c3d::move( rhs.device ) }
		, bufferId{ c3d::move( rhs.bufferId ) }
		, bufferViewId{ c3d::move( rhs.bufferViewId ) }
		, m_ownAttach{ c3d::move( rhs.m_ownAttach ) }
		, m_attach{ rhs.m_attach }
		, m_ownBuffer{ rhs.m_ownBuffer }
		, m_pages{ c3d::move( rhs.m_pages ) }
	{
		rhs.device = {};
		rhs.resources = {};
		rhs.m_ownBuffer = {};
	}

	PagedBufferBase & PagedBufferBase::operator=( PagedBufferBase && rhs )noexcept
	{
		resources = c3d::move( rhs.resources );
		device = c3d::move( rhs.device );
		bufferId = c3d::move( rhs.bufferId );
		bufferViewId = c3d::move( rhs.bufferViewId );
		m_ownAttach = c3d::move( rhs.m_ownAttach );
		m_attach = rhs.m_attach;
		m_ownBuffer = rhs.m_ownBuffer;
		m_pages = c3d::move( rhs.m_pages );

		rhs.device = {};
		rhs.resources = {};
		rhs.m_ownBuffer = {};

		return *this;
	}

	PagedBufferBase::PagedBufferBase( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, String const & name
		, BufferCreateFlags flags
		, DeviceSize size
		, uint32_t maxPages
		, BufferUsageFlags usage
		, MemoryPropertyFlags memory )
		: resources{ &presources }
		, device{ &pdevice }
		, bufferId{ resources->getHandler().createBufferId( crg::BufferData{ toUtf8( name )
			, flags, size, maxPages, usage, memory } ) }
		, bufferViewId{ resources->getHandler().createViewId( crg::BufferViewData{ bufferId.data->name
			, bufferId
			, { 0u, size * maxPages } } ) }
		, m_ownAttach{ makeRawUnique< crg::Attachment >( crg::Attachment::createDefault( bufferViewId ) ) }
		, m_attach{ m_ownAttach.get() }
		, m_ownBuffer{ true }
	{
	}

	PagedBufferBase::~PagedBufferBase()noexcept
	{
		CU_Require( m_pages.empty() || !m_ownBuffer );
	}

	void PagedBufferBase::create()
	{
		if ( !device || !resources || !m_pages.empty() || !m_ownBuffer )
			return;

		auto & context = device->makeContext();
		m_bufferPages = &resources->createBuffer( context, bufferId );
		auto page = m_bufferPages->getPage( 0u );
		m_pages.emplace_back( makeRawUnique< ashes::BufferBase >( **device
			, bufferId.data->name
			, page.buffer, page.memory
			, convert( bufferId.data->info ) ) );
	}

	void PagedBufferBase::destroy()noexcept
	{
		if ( !device || !resources || m_pages.empty() || !m_ownBuffer )
			return;

		resources->destroyBuffer( bufferId );
		m_pages.clear();
	}

	uint8_t * PagedBufferBase::doLockBuffer( uint32_t pageIndex )const
	{
		return getPageBuffer( pageIndex ).lock( 0u, getPageSize(), 0u );
	}

	void PagedBufferBase::doUnlock( uint32_t pageIndex )const
	{
		getPageBuffer( pageIndex ).unlock();
	}

	void PagedBufferBase::doFlush( uint32_t pageIndex )const
	{
		getPageBuffer( pageIndex ).flush( 0u, getPageSize() );
	}

	ashes::WriteDescriptorSet PagedBufferBase::doGetDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement )const
	{
		ashes::WriteDescriptorSet write{ uint32_t( dstBinding ), dstArrayElement
			, bufferId.data->maxPages
			, ( checkFlag( bufferId.data->info.usage, BufferUsageFlags::eUniformBuffer )
				? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				: VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ) };

		for ( u32 i = 0u; i < getAllocatedPageCount(); ++i )
			write.bufferInfo.push_back( VkDescriptorBufferInfo{ m_bufferPages->getBuffer( i ), 0u, getPageSize() } );
		for ( u32 i = getAllocatedPageCount(); i < getMaxPageCount(); ++i )
			write.bufferInfo.push_back( VkDescriptorBufferInfo{ m_bufferPages->getBuffer( 0 ), 0u, getPageSize() } );

		write.update( nullptr );
		return write;
	}

	//*********************************************************************************************

	PagedBuffer::PagedBuffer( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, String const & name
		, BufferCreateFlags flags
		, DeviceSize size, uint32_t maxPages
		, BufferUsageFlags usage
		, MemoryPropertyFlags memory )
		: PagedBufferBase{ pdevice, presources, name, flags, size, maxPages, usage, memory }
	{
	}

	//*********************************************************************************************
}
