#include "GlAttributeBase.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlAttributeBase::GlAttributeBase( OpenGl & p_gl, GlRenderSystem * p_renderSystem, String const & p_attributeName, eGL_TYPE p_glType, int p_count )
		: m_attributeName( p_attributeName )
		, m_attributeLocation( uint32_t( eGL_INVALID_INDEX ) )
		, m_count( p_count )
		, m_offset( 0 )
		, m_stride( 0 )
		, m_glType( p_glType )
		, m_renderSystem( p_renderSystem )
		, Holder( p_gl )
	{
	}

	GlAttributeBase::~GlAttributeBase()
	{
	}

	void GlAttributeBase::SetShader( ShaderProgramBaseSPtr p_program )
	{
		m_program = p_program;
	}

	bool GlAttributeBase::Initialise()
	{
		bool l_return = false;
		ShaderProgramBaseSPtr l_program = m_program.lock();

		if ( l_program && l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_attributeLocation = std::static_pointer_cast< GlShaderProgram >( l_program )->GetAttributeLocation( m_attributeName );

			if ( m_attributeLocation != eGL_INVALID_INDEX )
			{
				l_return = true;
			}
		}

		return l_return;
	}

	void GlAttributeBase::Cleanup()
	{
		m_attributeLocation = uint32_t( eGL_INVALID_INDEX );
		m_program.reset();
	}

	bool GlAttributeBase::Bind( bool p_bNormalised )
	{
		bool l_return = GetOpenGl().EnableVertexAttribArray( m_attributeLocation );

		if ( m_glType == eGL_TYPE_INT )
		{
			l_return &= GetOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, m_stride, BUFFER_OFFSET( m_offset ) );
		}
		else
		{
			l_return &= GetOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, p_bNormalised, m_stride, BUFFER_OFFSET( m_offset ) );
		}

		return l_return;
	}

	void GlAttributeBase::Unbind()
	{
		GetOpenGl().DisableVertexAttribArray( m_attributeLocation );
	}
}
