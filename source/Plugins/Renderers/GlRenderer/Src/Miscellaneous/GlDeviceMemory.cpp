/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#include "Miscellaneous/GlDeviceMemory.hpp"

#include "Command/Commands/GlCopyBufferToImageCommand.hpp"
#include "Core/GlDevice.hpp"
#include "Image/GlTexture.hpp"

#include <Miscellaneous/MemoryRequirements.hpp>

namespace gl_renderer
{
	//************************************************************************************************

	namespace
	{
		class ImageMemory
			: public DeviceMemory::DeviceMemoryImpl
		{
		public:
			ImageMemory( renderer::MemoryRequirements const & requirements
				, renderer::MemoryPropertyFlags flags
				, Texture const & texture
				, GLuint boundTarget
				, renderer::ImageCreateInfo const & createInfo )
				: DeviceMemory::DeviceMemoryImpl{ requirements, flags, texture.getImage(), boundTarget }
				, m_texture{ &texture }
				, m_internal{ getInternal( m_texture->getFormat() ) }
				, m_format{ getFormat( m_internal ) }
				, m_type{ getType( m_internal ) }
			{
				m_texture = &texture;
				glLogCall( gl::BindTexture, m_boundTarget, m_boundResource );

				switch ( m_boundTarget )
				{
				case gl_renderer::GL_TEXTURE_1D:
					doSetImage1D( createInfo.extent.width
						, createInfo );
					break;
				case gl_renderer::GL_TEXTURE_2D:
					doSetImage2D( createInfo.extent.width
						, createInfo.extent.height
						, createInfo );
					break;
				case gl_renderer::GL_TEXTURE_3D:
					doSetImage3D( createInfo.extent.width
						, createInfo.extent.height
						, createInfo.extent.depth
						, createInfo );
					break;
				case gl_renderer::GL_TEXTURE_1D_ARRAY:
					doSetImage2D( createInfo.extent.width
						, createInfo.extent.height
						, createInfo );
					break;
				case gl_renderer::GL_TEXTURE_2D_ARRAY:
					doSetImage3D( createInfo.extent.width
						, createInfo.extent.height
						, createInfo.arrayLayers
						, createInfo );
					break;
				case gl_renderer::GL_TEXTURE_2D_MULTISAMPLE:
					doSetImage2DMS( createInfo.extent.width
						, createInfo.extent.height
						, createInfo );
					break;
				case gl_renderer::GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
					doSetImage3DMS( createInfo.extent.width
						, createInfo.extent.height
						, createInfo.arrayLayers
						, createInfo );
					break;
				default:
					break;
				}

				int levels = 0;
				gl::GetTexParameteriv( m_boundTarget, GL_TEXTURE_IMMUTABLE_LEVELS, &levels );
				assert( levels == createInfo.mipLevels );
				int format = 0;
				gl::GetTexParameteriv( m_boundTarget, GL_TEXTURE_IMMUTABLE_FORMAT, &format );
				assert( format != 0 );
				glLogCall( gl::BindTexture, m_boundTarget, 0 );

				// If the texture is visible to the host, we'll need a PBO to map it to RAM.
				if ( checkFlag( flags, renderer::MemoryPropertyFlag::eHostVisible ) )
				{
					glLogCall( gl::GenBuffers, 1u, &m_pbo );
					// Initialise Upload PBO.
					glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK, m_pbo );
					glLogCall( gl::BufferStorage, m_pbo, m_requirements.size, nullptr, GLbitfield( convert( flags ) ) );
					glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK, 0u );

					// Prepare update regions, layer by layer.
					uint32_t offset = 0;
					renderer::BufferImageCopy bufferCopyRegion = {};
					bufferCopyRegion.imageSubresource.aspectMask = getAspectMask( m_texture->getFormat() );
					bufferCopyRegion.imageSubresource.mipLevel = 0u;
					bufferCopyRegion.imageSubresource.layerCount = 1u;
					bufferCopyRegion.imageExtent = m_texture->getDimensions();
					bufferCopyRegion.levelSize = uint32_t( m_requirements.size / m_texture->getLayerCount() );

					for ( uint32_t layer = 0; layer < m_texture->getLayerCount(); layer++ )
					{
						bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
						bufferCopyRegion.bufferOffset = offset;
						m_updateRegions.push_back( bufferCopyRegion );
						offset += bufferCopyRegion.levelSize;
					}
				}
			}

			~ImageMemory()
			{
				if ( m_pbo != GL_INVALID_INDEX )
				{
					gl::DeleteBuffers( 1u, &m_pbo );
				}
			}

			uint8_t * lock( uint32_t offset
				, uint32_t size
				, renderer::MemoryMapFlags flags )const override
			{
				assert( checkFlag( m_flags, renderer::MemoryPropertyFlag::eHostVisible ) && "Unsupported action on a device local texture" );
				glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK, m_pbo );
				doSetupUpdateRegions( offset, size );
				auto result = glLogCall( gl::MapBufferRange, GL_BUFFER_TARGET_PIXEL_UNPACK, offset, size, m_mapFlags );
				return reinterpret_cast< uint8_t * >( result );
			}

			void flush( uint32_t offset
				, uint32_t size )const override
			{
				assert( checkFlag( m_flags, renderer::MemoryPropertyFlag::eHostVisible ) && "Unsupported action on a device local texture" );
				glLogCall( gl::FlushMappedBufferRange, GL_BUFFER_TARGET_PIXEL_UNPACK, offset, size );
			}

			void invalidate( uint32_t offset
				, uint32_t size )const override
			{
				assert( checkFlag( m_flags, renderer::MemoryPropertyFlag::eHostVisible ) && "Unsupported action on a device local texture" );
				glLogCall( gl::InvalidateBufferSubData, GL_BUFFER_TARGET_PIXEL_UNPACK, offset, size );
			}

			void unlock()const override
			{
				assert( checkFlag( m_flags, renderer::MemoryPropertyFlag::eHostVisible ) && "Unsupported action on a device local texture" );

				glLogCall( gl::BindTexture, m_boundTarget, m_boundResource );
				glLogCall( gl::UnmapBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK );

				for( size_t i = m_beginRegion; i < m_endRegion; ++i )
				{
					updateRegion( m_updateRegions[i] );
				}

				glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK, 0u );

				if ( m_texture->getMipmapLevels() > 1
					&& !renderer::isCompressedFormat( m_texture->getFormat() ) )
				{
					glLogCall( gl::MemoryBarrier, GL_MEMORY_BARRIER_TEXTURE_UPDATE );
					glLogCall( gl::GenerateMipmap, m_boundTarget );
				}

				glLogCall( gl::BindTexture, m_boundTarget, 0u );
			}

		private:
			void doSetImage1D( uint32_t width
				, renderer::ImageCreateInfo const & createInfo )
			{
				glLogCall( gl::TexStorage1D
					, m_boundTarget
					, GLsizei( createInfo.mipLevels )
					, gl_renderer::getInternal( createInfo.format )
					, width );
			}

			void doSetImage2D( uint32_t width
				, uint32_t height
				, renderer::ImageCreateInfo const & createInfo )
			{
				glLogCall( gl::TexStorage2D
					, m_boundTarget
					, GLsizei( createInfo.mipLevels )
					, gl_renderer::getInternal( createInfo.format )
					, width
					, height );
			}

			void doSetImage3D( uint32_t width
				, uint32_t height
				, uint32_t depth
				, renderer::ImageCreateInfo const & createInfo )
			{
				glLogCall( gl::TexStorage3D
					, m_boundTarget
					, GLsizei( createInfo.mipLevels )
					, gl_renderer::getInternal( createInfo.format )
					, width
					, height
					, depth );
			}

			void doSetImage2DMS( uint32_t width
				, uint32_t height
				, renderer::ImageCreateInfo const & createInfo )
			{
				glLogCall( gl::TexStorage2DMultisample
					, m_boundTarget
					, GLsizei( createInfo.samples )
					, gl_renderer::getInternal( createInfo.format )
					, width
					, height
					, GL_TRUE );
			}

			void doSetImage3DMS( uint32_t width
				, uint32_t height
				, uint32_t depth
				, renderer::ImageCreateInfo const & createInfo )
			{
				glLogCall( gl::TexStorage3DMultisample
					, m_boundTarget
					, GLsizei( createInfo.samples )
					, gl_renderer::getInternal( createInfo.format )
					, width
					, height
					, depth
					, GL_TRUE );
			}

			void doSetupUpdateRegions( uint32_t offset
				, uint32_t size )const
			{
				assert( !m_updateRegions.empty() && "Can't update this texture." );
				auto layerSize = m_updateRegions[0].levelSize;
				m_beginRegion = 0u;

				while ( offset >= layerSize )
				{
					++m_beginRegion;
					offset -= layerSize;
				}

				m_endRegion = m_beginRegion + 1u;

				while ( size > layerSize )
				{
					++m_endRegion;
					size -= layerSize;
				}

				if ( m_beginRegion >= m_updateRegions.size()
					|| m_endRegion > m_updateRegions.size() )
				{
					throw std::runtime_error{ "Invalid offset and/or size." };
				}
			}

			void updateRegion( renderer::BufferImageCopy const & copyInfo )const
			{
				if ( renderer::isCompressedFormat( m_texture->getFormat() ) )
				{
					switch ( m_boundTarget )
					{
					case GL_TEXTURE_1D:
						glLogCall( gl::CompressedTexSubImage1D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageExtent.width
							, m_internal
							, copyInfo.levelSize
							, BufferOffset( copyInfo.bufferOffset ) );
						break;

					case GL_TEXTURE_2D:
						glLogCall( gl::CompressedTexSubImage2D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageOffset.y
							, copyInfo.imageExtent.width
							, copyInfo.imageExtent.height
							, m_internal
							, copyInfo.levelSize
							, BufferOffset( copyInfo.bufferOffset ) );
						break;

					case GL_TEXTURE_3D:
						glLogCall( gl::CompressedTexSubImage3D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageOffset.y
							, copyInfo.imageOffset.z
							, copyInfo.imageExtent.width
							, copyInfo.imageExtent.height
							, copyInfo.imageExtent.depth
							, m_internal
							, copyInfo.levelSize
							, BufferOffset( copyInfo.bufferOffset ) );

					case GL_TEXTURE_1D_ARRAY:
						glLogCall( gl::CompressedTexSubImage2D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageSubresource.baseArrayLayer
							, copyInfo.imageExtent.width
							, copyInfo.imageSubresource.layerCount
							, m_internal
							, copyInfo.levelSize
							, BufferOffset( copyInfo.bufferOffset ) );
						break;

					case GL_TEXTURE_2D_ARRAY:
						glLogCall( gl::CompressedTexSubImage3D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageOffset.y
							, copyInfo.imageSubresource.baseArrayLayer
							, copyInfo.imageExtent.width
							, copyInfo.imageExtent.height
							, copyInfo.imageSubresource.layerCount
							, m_internal
							, copyInfo.levelSize
							, BufferOffset( copyInfo.bufferOffset ) );
						break;
					}
				}
				else
				{
					switch ( m_boundTarget )
					{
					case GL_TEXTURE_1D:
						glLogCall( gl::TexSubImage1D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageExtent.width
							, m_format
							, m_type
							, BufferOffset( copyInfo.bufferOffset ) );
						break;

					case GL_TEXTURE_2D:
						glLogCall( gl::TexSubImage2D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageOffset.y
							, copyInfo.imageExtent.width
							, copyInfo.imageExtent.height
							, m_format
							, m_type
							, BufferOffset( copyInfo.bufferOffset ) );
						break;

					case GL_TEXTURE_3D:
						glLogCall( gl::TexSubImage3D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageOffset.y
							, copyInfo.imageOffset.z
							, copyInfo.imageExtent.width
							, copyInfo.imageExtent.height
							, copyInfo.imageExtent.depth
							, m_format
							, m_type
							, BufferOffset( copyInfo.bufferOffset ) );
						break;

					case GL_TEXTURE_1D_ARRAY:
						glLogCall( gl::TexSubImage2D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageSubresource.baseArrayLayer
							, copyInfo.imageExtent.width
							, copyInfo.imageSubresource.layerCount
							, m_format
							, m_type
							, BufferOffset( copyInfo.bufferOffset ) );
						break;

					case GL_TEXTURE_2D_ARRAY:
						glLogCall( gl::TexSubImage3D
							, m_boundTarget
							, copyInfo.imageSubresource.mipLevel
							, copyInfo.imageOffset.x
							, copyInfo.imageOffset.y
							, copyInfo.imageSubresource.baseArrayLayer
							, copyInfo.imageExtent.width
							, copyInfo.imageExtent.height
							, copyInfo.imageSubresource.layerCount
							, m_format
							, m_type
							, BufferOffset( copyInfo.bufferOffset ) );
						break;
					}
				}
			}

		private:
			Texture const * m_texture;
			GlInternal m_internal;
			GlFormat m_format;
			GlType m_type;
			std::vector< renderer::BufferImageCopy > m_updateRegions;
			GLuint m_pbo{ GL_INVALID_INDEX };
			mutable size_t m_beginRegion{ 0u };
			mutable size_t m_endRegion{ 0u };
		};

		//************************************************************************************************

		class BufferMemory
			: public DeviceMemory::DeviceMemoryImpl
		{
		public:
			BufferMemory( renderer::MemoryRequirements const & requirements
				, renderer::MemoryPropertyFlags flags
				, GLuint boundResource
				, GLuint boundTarget )
				: DeviceMemory::DeviceMemoryImpl{ requirements, flags, boundResource, boundTarget }
			{
				glLogCall( gl::BindBuffer, m_boundTarget, m_boundResource );
				glLogCall( gl::BufferStorage, m_boundTarget, m_requirements.size, nullptr, GLbitfield( convert( flags ) ) );
				glLogCall( gl::BindBuffer, m_boundTarget, 0u );
			}

			uint8_t * lock( uint32_t offset
				, uint32_t size
				, renderer::MemoryMapFlags flags )const override
			{
				assert( checkFlag( m_flags, renderer::MemoryPropertyFlag::eHostVisible ) && "Unsupported action on a device local buffer" );
				glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_COPY_WRITE, m_boundResource );
				auto result = glLogCall( gl::MapBufferRange, GL_BUFFER_TARGET_COPY_WRITE, offset, size, m_mapFlags );
				return reinterpret_cast< uint8_t * >( result );
			}

			void flush( uint32_t offset
				, uint32_t size )const override
			{
				assert( checkFlag( m_flags, renderer::MemoryPropertyFlag::eHostVisible ) && "Unsupported action on a device local buffer" );
				glLogCall( gl::FlushMappedBufferRange, GL_BUFFER_TARGET_COPY_WRITE, offset, size );
			}

			void invalidate( uint32_t offset
				, uint32_t size )const override
			{
				assert( checkFlag( m_flags, renderer::MemoryPropertyFlag::eHostVisible ) && "Unsupported action on a device local buffer" );
				glLogCall( gl::InvalidateBufferSubData, GL_BUFFER_TARGET_COPY_WRITE, offset, size );
			}

			void unlock()const override
			{
				assert( checkFlag( m_flags, renderer::MemoryPropertyFlag::eHostVisible ) && "Unsupported action on a device local buffer" );
				glLogCall( gl::UnmapBuffer, GL_BUFFER_TARGET_COPY_WRITE );
				glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_COPY_WRITE, 0u );
			}

		private:
			mutable GLenum m_copyTarget;
		};
	}

	//************************************************************************************************

	DeviceMemory::DeviceMemoryImpl::DeviceMemoryImpl( renderer::MemoryRequirements const & requirements
		, renderer::MemoryPropertyFlags flags
		, GLuint boundResource
		, GLuint boundTarget )
		: m_requirements{ requirements }
		, m_mapFlags{ 0u }
		, m_boundResource{ boundResource }
		, m_boundTarget{ boundTarget }
		, m_flags{ flags }
	{
		if ( checkFlag( flags, renderer::MemoryPropertyFlag::eHostVisible ) )
		{
			m_mapFlags |= GL_MEMORY_MAP_READ_BIT | GL_MEMORY_MAP_WRITE_BIT | GL_MEMORY_MAP_FLUSH_EXPLICIT_BIT;
		}

		if ( checkFlag( flags, renderer::MemoryPropertyFlag::eHostCoherent ) )
		{
			m_mapFlags |= GL_MEMORY_MAP_COHERENT_BIT;
		}
	}

	//************************************************************************************************

	DeviceMemory::DeviceMemory( Device const & device
		, renderer::MemoryRequirements const & requirements
		, renderer::MemoryPropertyFlags flags )
		: renderer::DeviceMemory{ device, flags }
		, m_device{ device }
		, m_requirements{ requirements }
	{
	}

	DeviceMemory::~DeviceMemory()
	{
	}

	void DeviceMemory::bindToBuffer( GLuint resource, GLenum target )
	{
		assert( !m_impl && "Memory object was already bound to a resource object" );
		m_impl = std::make_unique< BufferMemory >( m_requirements, m_flags, resource, target );
	}

	void DeviceMemory::bindToImage( Texture const & texture
		, GLenum target
		, renderer::ImageCreateInfo const & createInfo )
	{
		assert( !m_impl && "Memory object was already bound to a resource object" );
		m_impl = std::make_unique< ImageMemory >( m_requirements, m_flags, texture, target, createInfo );
	}

	uint8_t * DeviceMemory::lock( uint32_t offset
		, uint32_t size
		, renderer::MemoryMapFlags flags )const
	{
		assert( m_impl && "Memory object was not bound to a resource object" );
		return m_impl->lock( offset, size, flags );
	}

	void DeviceMemory::flush( uint32_t offset
		, uint32_t size )const
	{
		assert( m_impl && "Memory object was not bound to a resource object" );
		m_impl->flush( offset, size );
	}

	void DeviceMemory::invalidate( uint32_t offset
		, uint32_t size )const
	{
		assert( m_impl && "Memory object was not bound to a resource object" );
		m_impl->invalidate( offset, size );
	}

	void DeviceMemory::unlock()const
	{
		assert( m_impl && "Memory object was not bound to a resource object" );
		m_impl->unlock();
	}

	//************************************************************************************************
}
