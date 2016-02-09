#include "GlAttributeBase.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlAttributeBase::GlAttributeBase( OpenGl & p_gl, ShaderProgram const & p_program, String const & p_attributeName, eGL_TYPE p_glType, uint32_t p_count, uint32_t p_divisor )
		: Holder( p_gl )
		, m_program( p_program )
		, m_attributeName( p_attributeName )
		, m_attributeLocation( eGL_INVALID_INDEX )
		, m_count( p_count )
		, m_divisor( p_divisor )
		, m_offset( 0u )
		, m_stride( 0u )
		, m_glType( p_glType )
	{
		if ( m_program.GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_attributeLocation = static_cast< GlShaderProgram const & >( m_program ).GetAttributeLocation( m_attributeName );
		}
	}

	GlAttributeBase::~GlAttributeBase()
	{
		m_attributeLocation = uint32_t( eGL_INVALID_INDEX );
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
