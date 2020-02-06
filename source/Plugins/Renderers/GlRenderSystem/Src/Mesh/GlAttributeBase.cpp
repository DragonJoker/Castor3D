#include "Mesh/GlAttributeBase.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

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
		if ( m_program.GetStatus() == ProgramStatus::eLinked )
		{
			m_attributeLocation = static_cast< GlShaderProgram const & >( m_program ).GetAttributeLocation( m_attributeName );
		}
	}

	GlAttributeBase::~GlAttributeBase()
	{
		m_attributeLocation = uint32_t( GlInvalidIndex );
	}

	void GlAttributeBase::Bind( bool p_bNormalised )
	{
		GetOpenGl().EnableVertexAttribArray( m_attributeLocation );

		if ( m_glType == GlType::eFloat
			 || m_glType == GlType::eDouble
			 || m_glType == GlType::eHalfFloat )
		{
			GetOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, p_bNormalised, m_stride, BUFFER_OFFSET( m_offset ) );
		}
		else
		{
			GetOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, m_stride, BUFFER_OFFSET( m_offset ) );
		}
	}

	void GlAttributeBase::Unbind()
	{
		GetOpenGl().DisableVertexAttribArray( m_attributeLocation );
	}
}
