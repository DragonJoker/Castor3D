#include "Mesh/GlES3AttributeBase.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3RenderSystem.hpp"
#include "Shader/GlES3ShaderProgram.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3AttributeBase::GlES3AttributeBase( OpenGlES3 & p_gl, ShaderProgram const & p_program, uint32_t p_stride, String const & p_attributeName, GlES3Type p_glType, uint32_t p_count, uint32_t p_divisor )
		: Holder( p_gl )
		, m_program( p_program )
		, m_stride( p_stride )
		, m_attributeName( p_attributeName )
		, m_attributeLocation( GlES3InvalidIndex )
		, m_count( p_count )
		, m_divisor( p_divisor )
		, m_offset( 0u )
		, m_glType( p_glType )
	{
		if ( m_program.GetStatus() == ProgramStatus::eLinked )
		{
			m_attributeLocation = static_cast< GlES3ShaderProgram const & >( m_program ).GetAttributeLocation( m_attributeName );
		}
	}

	GlES3AttributeBase::~GlES3AttributeBase()
	{
		m_attributeLocation = uint32_t( GlES3InvalidIndex );
	}

	void GlES3AttributeBase::Bind( bool p_bNormalised )
	{
		GetOpenGlES3().EnableVertexAttribArray( m_attributeLocation );

		if ( m_glType == GlES3Type::eFloat
			 || m_glType == GlES3Type::eDouble
			 || m_glType == GlES3Type::eHalfFloat )
		{
			GetOpenGlES3().VertexAttribPointer( m_attributeLocation, m_count, m_glType, p_bNormalised, m_stride, BUFFER_OFFSET( m_offset ) );
		}
		else
		{
			GetOpenGlES3().VertexAttribPointer( m_attributeLocation, m_count, m_glType, m_stride, BUFFER_OFFSET( m_offset ) );
		}
	}

	void GlES3AttributeBase::Unbind()
	{
		GetOpenGlES3().DisableVertexAttribArray( m_attributeLocation );
	}
}
