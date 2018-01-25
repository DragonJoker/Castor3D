/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlCopyBufferToImageCommand.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Image/GlTexture.hpp"
#include "Image/GlTextureView.hpp"

#include <Image/ImageSubresourceRange.hpp>

namespace gl_renderer
{
	namespace
	{
		GlTextureType convert( renderer::TextureType type
			, uint32_t layer )
		{
			if ( type == renderer::TextureType::eCube )
			{
				return GlTextureType( GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer );
			}

			return gl_renderer::convert( type );
		}
	}

	CopyBufferToImageCommand::CopyBufferToImageCommand( renderer::BufferImageCopy const & copyInfo
		, renderer::BufferBase const & src
		, renderer::TextureView const & dst )
		: m_copyInfo{ copyInfo }
		, m_src{ static_cast< Buffer const & >( src ) }
		, m_dst{ static_cast< TextureView const & >( dst ) }
		, m_format{ getFormat( m_dst.getFormat() ) }
		, m_type{ getType( m_dst.getFormat() ) }
		, m_target{ convert( m_dst.getType() ) }
		, m_copyTarget{ convert( m_dst.getType(), m_copyInfo.imageSubresource.baseArrayLayer ) }
	{
	}

	void CopyBufferToImageCommand::apply()const
	{
		glLogCall( gl::BindTexture, m_target, m_dst.getImage() );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK, m_src.getBuffer() );

		switch ( m_target )
		{
		case GL_TEXTURE_1D:
			glLogCall( gl::TexSubImage1D
				, m_copyTarget
				, m_copyInfo.imageSubresource.mipLevel
				, m_copyInfo.imageOffset[0]
				, m_copyInfo.imageExtent[0]
				, m_format
				, m_type
				, nullptr );
			break;

		case GL_TEXTURE_2D:
		case GL_TEXTURE_CUBE_MAP:
			glLogCall( gl::TexSubImage2D
				, m_copyTarget
				, m_copyInfo.imageSubresource.mipLevel
				, m_copyInfo.imageOffset[0]
				, m_copyInfo.imageOffset[1]
				, m_copyInfo.imageExtent[0]
				, m_copyInfo.imageExtent[1]
				, m_format
				, m_type
				, nullptr );
			break;

		case GL_TEXTURE_3D:
			glLogCall( gl::TexSubImage3D
				, m_copyTarget
				, m_copyInfo.imageSubresource.mipLevel
				, m_copyInfo.imageOffset[0]
				, m_copyInfo.imageOffset[1]
				, m_copyInfo.imageOffset[2]
				, m_copyInfo.imageExtent[0]
				, m_copyInfo.imageExtent[1]
				, m_copyInfo.imageExtent[2]
				, m_format
				, m_type
				, nullptr );
			break;
		}

		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_UNPACK, 0u );
		glLogCall( gl::BindTexture, m_target, 0u );
	}

	CommandPtr CopyBufferToImageCommand::clone()const
	{
		return std::make_unique< CopyBufferToImageCommand >( *this );
	}
}
