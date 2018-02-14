/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlClearDepthStencilCommand.hpp"

#include "Image/GlTextureView.hpp"

namespace gl_renderer
{
	ClearDepthStencilCommand::ClearDepthStencilCommand( renderer::TextureView const & image
		, renderer::DepthStencilClearValue const & value )
		: m_image{ static_cast< TextureView const & >( image ) }
		, m_value{ value }
		, m_format{ getFormat( m_image.getFormat() ) }
		, m_type{ getType( m_image.getFormat() ) }
	{
	}

	void ClearDepthStencilCommand::apply()const
	{
		glLogCommand( "ClearDepthStencilCommand" );

		if ( renderer::isDepthStencilFormat( m_image.getFormat() ) )
		{
			glLogCall( gl::ClearTexImage
				, m_image.getImage()
				, 0
				, m_format
				, m_type
				, &m_value.depth );
		}
		else if ( renderer::isStencilFormat( m_image.getFormat() ) )
		{
			glLogCall( gl::ClearTexImage
				, m_image.getImage()
				, 0
				, m_format
				, m_type
				, &m_value.stencil );
		}
		else if ( renderer::isDepthFormat( m_image.getFormat() ) )
		{
			glLogCall( gl::ClearTexImage
				, m_image.getImage()
				, 0
				, m_format
				, m_type
				, &m_value.depth );
		}
	}

	CommandPtr ClearDepthStencilCommand::clone()const
	{
		return std::make_unique< ClearDepthStencilCommand >( *this );
	}
}
