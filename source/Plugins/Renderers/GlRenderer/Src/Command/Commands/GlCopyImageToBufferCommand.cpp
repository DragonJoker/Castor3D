/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlCopyImageToBufferCommand.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Image/GlTextureView.hpp"

#include <Image/ImageSubresourceRange.hpp>
#include <Image/TextureView.hpp>

namespace gl_renderer
{
	CopyImageToBufferCommand::CopyImageToBufferCommand( renderer::BufferImageCopy const & copyInfo
		, renderer::TextureView const & src
		, renderer::BufferBase const & dst )
		: m_copyInfo{ copyInfo }
		, m_src{ static_cast< TextureView const & >( src ) }
		, m_dst{ static_cast< Buffer const & >( dst ) }
		, m_format{ getFormat( m_src.getFormat() ) }
		, m_type{ getType( m_src.getFormat() ) }
		, m_target{ convert( m_src.getType() ) }
	{
	}

	void CopyImageToBufferCommand::apply()const
	{
		glLogCall( gl::BindTexture, m_target, m_src.getImage() );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_PACK, m_dst.getBuffer() );
		glLogCall( gl::GetTexImage
			, m_target
			, m_copyInfo.imageSubresource.mipLevel
			, m_format
			, m_type
			, nullptr );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_PACK, 0u );
		glLogCall( gl::BindTexture, m_target, 0u );
	}

	CommandPtr CopyImageToBufferCommand::clone()const
	{
		return std::make_unique< CopyImageToBufferCommand >( *this );
	}
}
