/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlCopyImageCommand.hpp"

#include "Image/GlTexture.hpp"
#include "Image/GlTextureView.hpp"

#include <Image/ImageSubresourceRange.hpp>

namespace gl_renderer
{
	CopyImageCommand::CopyImageCommand( renderer::ImageCopy const & copyInfo
		, renderer::TextureView const & src
		, renderer::TextureView const & dst )
		: m_copyInfo{ copyInfo }
		, m_src{ static_cast< TextureView const & >( src ) }
		, m_dst{ static_cast< TextureView const & >( dst ) }
		, m_srcFormat{ getFormat( m_src.getFormat() ) }
		, m_srcType{ getType( m_src.getFormat() ) }
		, m_srcTarget{ convert( m_src.getType() ) }
		, m_dstFormat{ getFormat( m_dst.getFormat() ) }
		, m_dstType{ getType( m_dst.getFormat() ) }
		, m_dstTarget{ convert( m_dst.getType() ) }
	{
	}

	void CopyImageCommand::apply()const
	{
		glLogCommand( "CopyImageCommand" );
		glLogCall( gl::BindTexture, m_srcTarget, m_src.getImage() );
		glLogCall( gl::BindTexture, m_dstTarget, m_dst.getImage() );
		glLogCall( gl::CopyImageSubData
			, m_src.getImage()
			, m_srcTarget
			, m_copyInfo.srcSubresource.mipLevel
			, m_copyInfo.srcOffset[0]
			, m_copyInfo.srcOffset[1]
			, m_copyInfo.srcOffset[2]
			, m_dst.getImage()
			, m_dstTarget
			, m_copyInfo.dstSubresource.mipLevel
			, m_copyInfo.dstOffset[0]
			, m_copyInfo.dstOffset[1]
			, m_copyInfo.dstOffset[2]
			, m_copyInfo.extent[0]
			, m_copyInfo.extent[1]
			, m_copyInfo.extent[2] );
		glLogCall( gl::BindTexture, m_dstTarget, 0u );
		glLogCall( gl::BindTexture, m_srcTarget, 0u );
		m_dst.getTexture().generateMipmaps();
	}

	CommandPtr CopyImageCommand::clone()const
	{
		return std::make_unique< CopyImageCommand >( *this );
	}
}
