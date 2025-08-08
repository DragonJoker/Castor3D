#include "Castor3D/Render/Buffer.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

#include <RenderGraph/ResourceHandler.hpp>

#include <ashespp/Buffer/Buffer.hpp>

CU_ImplementSmartPtr( c3d, Buffer )

namespace c3d
{
	//*********************************************************************************************

	BufferBase::BufferBase( BufferBase && rhs )noexcept
		: resources{ c3d::move( rhs.resources ) }
		, device{ c3d::move( rhs.device ) }
		, bufferId{ c3d::move( rhs.bufferId ) }
		, buffer{ c3d::move( rhs.buffer ) }
		, bufferViewId{ c3d::move( rhs.bufferViewId ) }
		, m_ownAttach{ c3d::move( rhs.m_ownAttach ) }
		, m_attach{ rhs.m_attach }
		, m_ownBuffer{ rhs.m_ownBuffer }
	{
		rhs.device = {};
		rhs.resources = {};
		rhs.buffer = {};
		rhs.m_ownBuffer = {};
	}

	BufferBase & BufferBase::operator=( BufferBase && rhs )noexcept
	{
		resources = c3d::move( rhs.resources );
		device = c3d::move( rhs.device );
		bufferId = c3d::move( rhs.bufferId );
		buffer = c3d::move( rhs.buffer );
		bufferViewId = c3d::move( rhs.bufferViewId );
		m_ownAttach = c3d::move( rhs.m_ownAttach );
		m_attach = rhs.m_attach;
		m_ownBuffer = rhs.m_ownBuffer;

		rhs.device = {};
		rhs.resources = {};
		rhs.buffer = {};
		rhs.m_ownBuffer = {};

		return *this;
	}

	BufferBase::BufferBase( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, String const & name
		, BufferCreateFlags flags
		, DeviceSize size
		, BufferUsageFlags usage
		, MemoryPropertyFlags memory )
		: resources{ &presources }
		, device{ &pdevice }
		, bufferId{ resources->getHandler().createBufferId( crg::BufferData{ toUtf8( name )
			, flags, size, usage, memory } ) }
		, bufferViewId{ resources->getHandler().createViewId( crg::BufferViewData{ bufferId.data->name
			, bufferId
			, { 0u, size } } ) }
		, m_ownAttach{ makeRawUnique< crg::Attachment >( crg::Attachment::createDefault( bufferViewId ) ) }
		, m_attach{ m_ownAttach.get() }
		, m_ownBuffer{ true }
	{
	}

	BufferBase::BufferBase( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, crg::BufferViewId view )
		: resources{ &presources }
		, device{ &pdevice }
		, bufferId{ view.data->buffer }
		, bufferViewId{ view }
		, m_ownAttach{ makeRawUnique< crg::Attachment >( crg::Attachment::createDefault( bufferViewId ) ) }
		, m_attach{ m_ownAttach.get() }
		, m_ownBuffer{ false }
	{
		auto & context = device->makeContext();
		VkDeviceMemory vkmemory{};
		VkBuffer  vkBuffer = resources->createBuffer( context, bufferId, vkmemory );
		buffer = makeRawUnique< ashes::BufferBase >( **device
			, bufferId.data->name
			, vkBuffer, vkmemory
			, convert( bufferId.data->info ) );
	}

	BufferBase::~BufferBase()noexcept
	{
		CU_Require( buffer == nullptr || !m_ownBuffer );
	}

	void BufferBase::create()
	{
		if ( !device || !resources || buffer || !m_ownBuffer )
			return;

		auto & context = device->makeContext();
		VkDeviceMemory vkmemory{};
		VkBuffer vkBuffer = resources->createBuffer( context, bufferId, vkmemory );
		buffer = makeRawUnique< ashes::BufferBase >( **device
			, bufferId.data->name
			, vkBuffer, vkmemory
			, convert( bufferId.data->info ) );
	}

	void BufferBase::destroy()noexcept
	{
		if ( !device || !resources || !buffer || !m_ownBuffer )
			return;

		resources->destroyBuffer( bufferId );
		buffer = nullptr;
	}

	crg::BufferViewId BufferBase::getSubView( DeviceSize offset, DeviceSize size )const
	{
		CU_Require( getSize() - offset >= size );
		return resources->getHandler().createViewId( crg::BufferViewData{ bufferId.data->name + "_" + std::to_string( offset )
			, bufferId, { offset, size } } );
	}

	uint8_t * BufferBase::doLockBuffer()const
	{
		return buffer->lock( getSubresourceRange( bufferViewId ).offset, getSize(), 0u );
	}

	void BufferBase::doUnlock()const
	{
		buffer->unlock();
	}

	void BufferBase::doFlush()const
	{
		auto & range = getSubresourceRange( bufferViewId );
		buffer->flush( range.offset, range.size );
	}

	//*********************************************************************************************

	Buffer::Buffer( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, String const & name
		, BufferCreateFlags flags
		, DeviceSize size
		, BufferUsageFlags usage
		, MemoryPropertyFlags memory )
		: BufferBase{ pdevice, presources, name, flags, size, usage, memory }
	{
	}

	Buffer::Buffer( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, crg::BufferViewId view )
		: BufferBase{ pdevice, presources, view }
	{
	}

	//*********************************************************************************************
}
