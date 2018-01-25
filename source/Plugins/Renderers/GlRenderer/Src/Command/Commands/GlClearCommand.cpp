/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlClearCommand.hpp"

#include "Image/GlTextureView.hpp"

namespace gl_renderer
{
	ClearCommand::ClearCommand( renderer::TextureView const & image
		, renderer::RgbaColour const & colour )
		: m_image{ static_cast< TextureView const & >( image ) }
		, m_colour{ colour }
		, m_format{ getFormat( m_image.getFormat() ) }
		, m_type{ getType( m_image.getFormat() ) }
	{
	}

	void ClearCommand::apply()const
	{
		glLogCall( gl::ClearTexImage
			, m_image.getImage()
			, 0
			, m_format
			, m_type
			, m_colour.constPtr() );
	}

	CommandPtr ClearCommand::clone()const
	{
		return std::make_unique< ClearCommand >( *this );
	}
}
