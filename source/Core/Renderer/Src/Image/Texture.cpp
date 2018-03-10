/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Image/Texture.hpp"

#include "Buffer/StagingBuffer.hpp"
#include "Image/ImageSubresource.hpp"
#include "Image/SubresourceLayout.hpp"
#include "Image/TextureView.hpp"

namespace renderer
{
	Texture::Texture( Texture && rhs )
		: m_device{ rhs.m_device }
		, m_imageType{ rhs.m_imageType }
		, m_format{ rhs.m_format }
		, m_dimensions{ rhs.m_dimensions }
		, m_mipLevels{ rhs.m_mipLevels }
		, m_arrayLayers{ rhs.m_arrayLayers }
	{
	}

	Texture & Texture::operator=( Texture && rhs )
	{
		if ( &rhs != this )
		{
			m_imageType = rhs.m_imageType;
			m_format = rhs.m_format;
			m_dimensions = rhs.m_dimensions;
			m_mipLevels = rhs.m_mipLevels;
			m_arrayLayers = rhs.m_arrayLayers;
		}

		return *this;
	}

	Texture::Texture( Device const & device
		, TextureType type
		, Format format
		, Extent3D dimensions
		, uint32_t mipLevels
		, uint32_t arrayLayers )
		: m_device{ device }
		, m_imageType{ type }
		, m_format{ format }
		, m_dimensions{ dimensions }
		, m_mipLevels{ mipLevels }
		, m_arrayLayers{ arrayLayers }
	{
	}

	void Texture::bindMemory( DeviceMemoryPtr memory )
	{
		assert( !m_storage && "A resource can only be bound once to a device memory object." );
		m_storage = std::move( memory );
		doBindMemory();
	}

	Texture::Mapped Texture::lock( uint32_t offset
		, uint32_t size
		, MemoryMapFlags flags )const
	{
		assert( m_storage && "The resource is not bound to a device memory object." );
		Mapped mapped{};
		ImageSubresource subResource{};
		subResource.aspectMask = getAspectMask( getFormat() );
		SubresourceLayout subResourceLayout;
		m_device.getImageSubresourceLayout( *this, subResource, subResourceLayout );

		mapped.data = m_storage->lock( offset
			, size
			, flags );

		if ( mapped.data )
		{
			mapped.arrayPitch = subResourceLayout.arrayPitch;
			mapped.depthPitch = subResourceLayout.depthPitch;
			mapped.rowPitch = subResourceLayout.rowPitch;
			mapped.size = subResourceLayout.size;
			mapped.data += subResourceLayout.offset;
		}

		return mapped;
	}

	void Texture::invalidate( uint32_t offset
		, uint32_t size )const
	{
		assert( m_storage && "The resource is not bound to a device memory object." );
		return m_storage->invalidate( offset, size );
	}

	void Texture::flush( uint32_t offset
		, uint32_t size )const
	{
		assert( m_storage && "The resource is not bound to a device memory object." );
		return m_storage->flush( offset, size );
	}

	void Texture::unlock()const
	{
		assert( m_storage && "The resource is not bound to a device memory object." );
		return m_storage->unlock();
	}

	TextureViewPtr Texture::createView( TextureViewType type
		, Format format
		, uint32_t baseMipLevel
		, uint32_t levelCount
		, uint32_t baseArrayLayer
		, uint32_t layerCount
		, ComponentMapping const & mapping )const
	{
		return createView(
		{
			type,
			format,
			mapping,
			{
				getAspectMask( format ),
				baseMipLevel,
				levelCount,
				baseArrayLayer,
				layerCount
			}
		} );
	}
}
