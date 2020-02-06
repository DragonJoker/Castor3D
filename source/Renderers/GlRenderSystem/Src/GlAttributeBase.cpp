#include "GlAttributeBase.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlAttributeBase::GlAttributeBase( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, String const & p_strAttribName, eGL_TYPE p_eGlType, int p_iCount )
		:	m_strAttribName( p_strAttribName )
		,	m_uiAttribLocation( uint32_t( eGL_INVALID_INDEX ) )
		,	m_iCount( p_iCount )
		,	m_uiOffset( 0 )
		,	m_iStride( 0 )
		,	m_eGlType( p_eGlType )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_gl( p_gl )
	{
	}

	GlAttributeBase::~GlAttributeBase()
	{
	}

	void GlAttributeBase::SetShader( ShaderProgramBaseSPtr p_pProgram )
	{
		m_pProgram = p_pProgram;
	}

	bool GlAttributeBase::Initialise()
	{
		bool l_bReturn = !m_pRenderSystem->UseShaders();

		if ( !m_pProgram.expired() && m_pProgram.lock()->GetStatus() == ePROGRAM_STATUS_LINKED && !l_bReturn )
		{
			m_uiAttribLocation = m_pProgram.lock()->GetAttributeLocation( m_strAttribName );

			if ( m_uiAttribLocation != eGL_INVALID_INDEX )
			{
				l_bReturn = true;
			}
		}

		return l_bReturn;
	}

	void GlAttributeBase::Cleanup()
	{
		m_uiAttribLocation = uint32_t( eGL_INVALID_INDEX );
		m_pProgram.reset();
	}

	bool GlAttributeBase::Bind( bool p_bNormalised )
	{
		bool l_bReturn = m_gl.EnableVertexAttribArray( m_uiAttribLocation );

		if ( m_eGlType == eGL_TYPE_INT )
		{
			l_bReturn &= m_gl.VertexAttribPointer( m_uiAttribLocation, m_iCount, m_eGlType, m_iStride, BUFFER_OFFSET( m_uiOffset ) );
		}
		else
		{
			l_bReturn &= m_gl.VertexAttribPointer( m_uiAttribLocation, m_iCount, m_eGlType, p_bNormalised, m_iStride, BUFFER_OFFSET( m_uiOffset ) );
		}

		return l_bReturn;
	}

	void GlAttributeBase::Unbind()
	{
		m_gl.DisableVertexAttribArray( m_uiAttribLocation );
	}
}
