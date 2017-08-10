#include "Mesh/GlAttributeBase.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlAttributeBase::GlAttributeBase( OpenGl & p_gl, ShaderProgram const & p_program, uint32_t p_stride, String const & p_attributeName, GlType p_glType, uint32_t p_count, uint32_t p_divisor )
		: Holder( p_gl )
		, m_program( p_program )
		, m_stride( p_stride )
		, m_attributeName( p_attributeName )
		, m_attributeLocation( GlInvalidIndex )
		, m_count( p_count )
		, m_divisor( p_divisor )
		, m_offset( 0u )
		, m_glType( p_glType )
	{
		if ( m_program.getStatus() == ProgramStatus::eLinked )
		{
			m_attributeLocation = static_cast< GlShaderProgram const & >( m_program ).getAttributeLocation( m_attributeName );
		}
	}

	GlAttributeBase::~GlAttributeBase()
	{
		m_attributeLocation = uint32_t( GlInvalidIndex );
	}

	void GlAttributeBase::bind( bool p_bNormalised )
	{
		getOpenGl().EnableVertexAttribArray( m_attributeLocation );

		if ( m_glType == GlType::eFloat
			 || m_glType == GlType::eDouble
			 || m_glType == GlType::eHalfFloat )
		{
			getOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, p_bNormalised, m_stride, BUFFER_OFFSET( m_offset ) );
		}
		else
		{
			getOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, m_stride, BUFFER_OFFSET( m_offset ) );
		}
	}

	void GlAttributeBase::unbind()
	{
		getOpenGl().DisableVertexAttribArray( m_attributeLocation );
	}
}
