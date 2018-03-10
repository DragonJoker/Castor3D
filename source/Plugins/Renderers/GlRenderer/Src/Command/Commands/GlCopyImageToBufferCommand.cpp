/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlCopyImageToBufferCommand.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Image/GlTexture.hpp"
#include "Image/GlTextureView.hpp"

#include <Image/ImageSubresourceRange.hpp>
#include <Miscellaneous/BufferImageCopy.hpp>

namespace gl_renderer
{
	namespace
	{
		std::vector< renderer::TextureViewPtr > createViews( renderer::Texture const & texture
			, renderer::BufferImageCopyArray copies )
		{
			std::vector< renderer::TextureViewPtr > result;
			renderer::TextureType type = texture.getType();
			renderer::TextureViewType viewType;

			if ( type == renderer::TextureType::e3D )
			{
				viewType = renderer::TextureViewType::e3D;
			}
			else if ( type == renderer::TextureType::e2D )
			{
				viewType = renderer::TextureViewType::e2D;
			}
			else if ( type == renderer::TextureType::e1D )
			{
				viewType = renderer::TextureViewType::e1D;
			}

			for ( auto & copy : copies )
			{
				result.emplace_back( texture.createView( viewType
					, texture.getFormat()
					, copy.imageSubresource.mipLevel
					, 1u
					, copy.imageSubresource.baseArrayLayer
					, copy.imageSubresource.layerCount ) );
			}

			return result;
		}
	}

	CopyImageToBufferCommand::CopyImageToBufferCommand( renderer::BufferImageCopyArray const & copyInfo
		, renderer::Texture const & src
		, renderer::BufferBase const & dst )
		: m_src{ static_cast< Texture const & >( src ) }
		, m_dst{ static_cast< Buffer const & >( dst ) }
		, m_copyInfo{ copyInfo }
		, m_internal{ getInternal( m_src.getFormat() ) }
		, m_format{ getFormat( m_internal ) }
		, m_type{ getType( m_internal ) }
		, m_target{ convert( m_src.getType(), 1u ) }
		, m_views{ createViews( m_src, m_copyInfo ) }
	{
	}

	CopyImageToBufferCommand::CopyImageToBufferCommand( CopyImageToBufferCommand const & rhs )
		: m_src{ rhs.m_src }
		, m_dst{ rhs.m_dst }
		, m_copyInfo{ rhs.m_copyInfo }
		, m_internal{ rhs.m_internal }
		, m_format{ rhs.m_format }
		, m_type{ rhs.m_type }
		, m_target{ rhs.m_target }
		, m_views{ createViews( m_src, m_copyInfo ) }
	{
	}

	void CopyImageToBufferCommand::apply()const
	{
		glLogCommand( "CopyImageToBufferCommand" );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_PACK, m_dst.getBuffer() );
		glLogCall( gl::BindTexture, m_target, m_src.getImage() );

		for ( size_t i = 0; i < m_views.size(); ++i )
		{
			applyOne( m_copyInfo[i]
				, *m_views[i] );
		}

		glLogCall( gl::BindTexture, m_target, 0u );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_PIXEL_PACK, 0u );
	}

	void CopyImageToBufferCommand::applyOne( renderer::BufferImageCopy const & copyInfo
		, renderer::TextureView const & view )const
	{
		glLogCall( gl::GetTexImage
			, m_target
			, copyInfo.imageSubresource.mipLevel
			, m_format
			, m_type
			, nullptr );
	}

	CommandPtr CopyImageToBufferCommand::clone()const
	{
		return std::make_unique< CopyImageToBufferCommand >( *this );
	}
}
