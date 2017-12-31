#include "Mesh/GlAttributeBase.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlAttributeBase::GlAttributeBase( OpenGl & gl
		, ShaderProgram const & program
		, uint32_t stride
		, String const & attributeName
		, GlType glType
		, uint32_t count
		, uint32_t divisor )
		: Holder( gl )
		, m_program( program )
		, m_stride( stride )
		, m_attributeName( attributeName )
		, m_attributeLocation( GlInvalidIndex )
		, m_count( count )
		, m_divisor( divisor )
		, m_offset( 0u )
		, m_glType( glType )
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

	void GlAttributeBase::bind( bool normalised )
	{
		getOpenGl().EnableVertexAttribArray( m_attributeLocation );

		if ( m_glType == GlType::eFloat
			 || m_glType == GlType::eDouble
			 || m_glType == GlType::eHalfFloat )
		{
			getOpenGl().VertexAttribPointer( m_attributeLocation, m_count, m_glType, normalised, m_stride, BUFFER_OFFSET( m_offset ) );
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
