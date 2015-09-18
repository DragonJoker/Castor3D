#include "GlShaderProgram.hpp"
#include "GlShaderObject.hpp"
#include "GlBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "GlOneFrameVariable.hpp"
#include "GlFrameVariableBuffer.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlShaderProgram::GlShaderProgram( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
		:	ShaderProgramBase( p_pRenderSystem, eSHADER_LANGUAGE_GLSL )
		,	m_programObject( 0 )
		,	m_gl( p_gl )
	{
		CreateObject( eSHADER_TYPE_VERTEX );
		CreateObject( eSHADER_TYPE_PIXEL );
	}

	GlShaderProgram::~GlShaderProgram()
	{
	}

	void GlShaderProgram::Cleanup()
	{
		ShaderProgramBase::Cleanup();

		if ( m_pRenderSystem->UseShaders() )
		{
			if ( m_programObject )
			{
				glUntrack( m_gl, this );
				m_gl.DeleteProgram( m_programObject );
				m_programObject = 0;
			}
		}
	}

	void GlShaderProgram::Initialise()
	{
		if ( m_eStatus != ePROGRAM_STATUS_LINKED )
		{
			if ( m_pRenderSystem->UseShaders() && !m_programObject )
			{
				m_programObject = m_gl.CreateProgram();
				glTrack( m_gl, GlShaderProgram, this );
			}

			ShaderProgramBase::Initialise();
		}
	}

	bool GlShaderProgram::Link()
	{
		bool l_return = false;
		int l_iLinked = 0;

		if ( m_pRenderSystem->UseShaders() && m_eStatus != ePROGRAM_STATUS_ERROR )
		{
			l_return = true;
			Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << uint32_t( m_activeShaders.size() ) );
			l_return &= m_gl.LinkProgram( m_programObject );
			l_return &= m_gl.GetProgramiv( m_programObject, eGL_SHADER_STATUS_LINK, &l_iLinked );
			Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Program link status : " ) << l_iLinked );
			RetrieveLinkerLog( m_linkerLog );

			if ( m_linkerLog.size() > 0 )
			{
				Logger::LogDebug( cuT( "GlShaderProgram::Link - Log : " ) + m_linkerLog );
			}

			if ( l_iLinked && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
			{
				ShaderProgramBase::Link();
			}
			else
			{
				m_eStatus = ePROGRAM_STATUS_ERROR;
			}

			l_return = m_eStatus == ePROGRAM_STATUS_LINKED;
		}

		return l_return;
	}

	void GlShaderProgram::RetrieveLinkerLog( String & strLog )
	{
		if ( !m_pRenderSystem->UseShaders() )
		{
			strLog = m_gl.GetGlslErrorString( 0 );
		}
		else
		{
			int l_iLength = 0;
			int l_iLength2 = 0;

			if ( m_programObject == 0 )
			{
				strLog = m_gl.GetGlslErrorString( 2 );
			}
			else
			{
				m_gl.GetProgramiv( m_programObject, eGL_SHADER_STATUS_INFO_LOG_LENGTH , &l_iLength );

				if ( l_iLength > 1 )
				{
					char * l_pTmp = new char[l_iLength];
					m_gl.GetProgramInfoLog( m_programObject, l_iLength, &l_iLength2, l_pTmp );
					strLog = str_utils::from_str( l_pTmp );
					delete [] l_pTmp;
				}
			}
		}
	}

	void GlShaderProgram::Bind( uint8_t p_byIndex, uint8_t p_byCount )
	{
		if ( m_pRenderSystem->UseShaders() && m_programObject != 0 && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			m_gl.UseProgram( m_programObject );
			m_pRenderSystem->GetPipeline()->UpdateFunctions( this );
			ShaderProgramBase::Bind( p_byIndex, p_byCount );
		}
	}

	void GlShaderProgram::Unbind()
	{
		if ( m_pRenderSystem->UseShaders() && m_programObject != 0 && m_bEnabled && m_eStatus == ePROGRAM_STATUS_LINKED )
		{
			uint32_t l_index = 0;

			for ( auto l_variableBuffer : m_listFrameVariableBuffers )
			{
				l_variableBuffer->Unbind( l_index++ );
			}

			ShaderProgramBase::Unbind();
			m_gl.UseProgram( 0 );
		}
	}

	int GlShaderProgram::GetAttributeLocation( String const & p_strName )const
	{
		int l_iReturn = eGL_INVALID_INDEX;

		if ( m_programObject != eGL_INVALID_INDEX && m_gl.IsProgram( m_programObject ) )
		{
			l_iReturn = m_gl.GetAttribLocation( m_programObject, str_utils::to_str( p_strName ).c_str() );
		}

		return l_iReturn;
	}

	ShaderObjectBaseSPtr GlShaderProgram::DoCreateObject( eSHADER_TYPE p_eType )
	{
		ShaderObjectBaseSPtr l_pReturn = std::make_shared< GlShaderObject >( m_gl, this, p_eType );
		return l_pReturn;
	}

	std::shared_ptr< OneTextureFrameVariable > GlShaderProgram::DoCreateTextureVariable( int p_iNbOcc )
	{
		return std::make_shared< GlOneFrameVariable< TextureBaseRPtr > >( m_gl, p_iNbOcc, this );
	}
}
