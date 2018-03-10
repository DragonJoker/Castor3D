/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlClearColourCommand.hpp"

#include "Image/GlTextureView.hpp"

namespace gl_renderer
{
	ClearColourCommand::ClearColourCommand( renderer::TextureView const & image
		, renderer::RgbaColour const & colour )
		: m_image{ static_cast< TextureView const & >( image ) }
		, m_colour{ colour }
		, m_internal{ getInternal( m_image.getFormat() ) }
		, m_format{ getFormat( m_internal ) }
		, m_type{ getType( m_internal ) }
	{
	}

	void ClearColourCommand::apply()const
	{
		glLogCommand( "ClearColourCommand" );

		if ( gl::ClearTexImage )
		{
			glLogCall( gl::ClearTexImage
				, m_image.getImage()
				, 0
				, m_format
				, m_type
				, m_colour.constPtr() );
		}
		else
		{
			std::cerr << "Unsupported command : ClearColourCommand" << std::endl;
		}
	}

	CommandPtr ClearColourCommand::clone()const
	{
		return std::make_unique< ClearColourCommand >( *this );
	}
}
