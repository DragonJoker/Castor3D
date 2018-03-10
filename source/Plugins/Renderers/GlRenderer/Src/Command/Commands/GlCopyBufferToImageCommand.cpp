/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlCopyBufferToImageCommand.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Image/GlTexture.hpp"
#include "Image/GlTextureView.hpp"

#include <Image/ImageSubresourceRange.hpp>

#define BufferOffset( n ) ( ( uint8_t * )nullptr + ( n ) )

namespace gl_renderer
{
	CopyBufferToImageCommand::CopyBufferToImageCommand( renderer::BufferImageCopyArray const & copyInfo
		, renderer::BufferBase const & src
		, renderer::Texture const & dst )
		: m_copyInfo{ copyInfo }
		, m_src{ static_cast< Buffer const & >( src ) }
		, m_dst{ static_cast< Texture const & >( dst ) }
		, m_internal{ getInternal( m_dst.getFormat() ) }
		, m_format{ getFormat( m_internal ) }
		, m_type{ getType( m_internal ) }
		, m_copyTarget{ convert( m_dst.getType(), m_dst.getLayerCount() ) }
	{
	}

	void CopyBufferToImageCommand::apply()const
	{
		glLogCommand( "CopyBufferToImageCommand" );

		for (const auto & copyInfo : m_copyInfo)
		{
			applyOne( copyInfo );
		}
	}

	CommandPtr CopyBufferToImageCommand::clone()const
	{
		return std::make_unique< CopyBufferToImageCommand >( *this );
	}

	void CopyBufferToImageCommand::applyOne( renderer::BufferImageCopy const & copyInfo )const
	{
		glLogCall( gl::BindTexture, m_copyTarget, m_dst.getImage() );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK, m_src.getBuffer() );

		if ( renderer::isCompressedFormat( m_dst.getFormat() ) )
		{
			switch ( m_copyTarget )
			{
			case GL_TEXTURE_1D:
				glLogCall( gl::CompressedTexSubImage1D
					, m_copyTarget
					, copyInfo.imageSubresource.mipLevel
					, copyInfo.imageOffset.x
					, copyInfo.imageExtent.width
					, m_internal
					, copyInfo.levelSize
					, BufferOffset( copyInfo.bufferOffset ) );
				break;

			case GL_TEXTURE_2D:
				glLogCall( gl::CompressedTexSubImage2D
					, m_copyTarget
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
					, m_copyTarget
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
					, m_copyTarget
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
					, m_copyTarget
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
			switch ( m_copyTarget )
			{
			case GL_TEXTURE_1D:
				glLogCall( gl::TexSubImage1D
					, m_copyTarget
					, copyInfo.imageSubresource.mipLevel
					, copyInfo.imageOffset.x
					, copyInfo.imageExtent.width
					, m_format
					, m_type
					, BufferOffset( copyInfo.bufferOffset ) );
				break;

			case GL_TEXTURE_2D:
				glLogCall( gl::TexSubImage2D
					, m_copyTarget
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
					, m_copyTarget
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
					, m_copyTarget
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
					, m_copyTarget
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

		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK, 0u );
		glLogCall( gl::BindTexture, m_copyTarget, 0u );
	}
}
