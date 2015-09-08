#include "GlShaderObject.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlShaderObject::GlShaderObject( OpenGl & p_gl, GlShaderProgram * p_pParent, eSHADER_TYPE p_eType )
		:	ShaderObjectBase( p_pParent, p_eType )
		,	m_shaderObject( 0 )
		,	m_pShaderProgram( NULL )
		,	m_gl( p_gl )
	{
	}

	GlShaderObject::~GlShaderObject()
	{
	}

	void GlShaderObject::DestroyProgram()
	{
		Detach();

		if ( m_eStatus == eSHADER_STATUS_COMPILED )
		{
			glUntrack( m_gl, this );
			m_gl.DeleteShader( m_shaderObject );
			m_eStatus = eSHADER_STATUS_NOTCOMPILED;
		}
	}

	void GlShaderObject::RetrieveCompilerLog( String & p_strCompilerLog )
	{
		int infologLength = 0;
		int charsWritten  = 0;
		m_gl.GetShaderiv( m_shaderObject, eGL_SHADER_STATUS_INFO_LOG_LENGTH, & infologLength );

		if ( infologLength > 0 )
		{
			char * infoLog = new char[infologLength];
			m_gl.GetShaderInfoLog( m_shaderObject, infologLength, & charsWritten, infoLog );
			p_strCompilerLog = string::from_str( infoLog );
			delete [] infoLog;
		}

		if ( p_strCompilerLog.size() > 0 )
		{
			p_strCompilerLog = p_strCompilerLog.substr( 0, p_strCompilerLog.size() - 1 );
		}
	}

	void GlShaderObject::CreateProgram()
	{
		if ( m_pParent->GetRenderSystem()->UseShaders() && m_pParent->GetRenderSystem()->HasShaderType( m_eType ) )
		{
			m_shaderObject = m_gl.CreateShader( m_gl.Get( m_eType ) );
			glTrack( m_gl, GlShaderObject, this );
		}
	}

	bool GlShaderObject::Compile()
	{
		bool l_return = false;
		m_strLoadedSource.clear();

		for ( int i = eSHADER_MODEL_5; i >= eSHADER_MODEL_1 && m_strLoadedSource.empty(); i-- )
		{
			if ( m_pParent->GetRenderSystem()->CheckSupport( eSHADER_MODEL( i ) ) )
			{
				m_strLoadedSource = m_arraySources[i];

				if ( !m_strLoadedSource.empty() )
				{
					m_eShaderModel = eSHADER_MODEL( i );
				}
			}
		}

		if ( m_pParent->GetRenderSystem()->UseShaders() && m_eStatus != eSHADER_STATUS_ERROR && !m_strLoadedSource.empty() )
		{
			l_return = true;

			if ( m_pParent->GetRenderSystem()->HasShaderType( m_eType ) )
			{
				m_eStatus = eSHADER_STATUS_NOTCOMPILED;
				int l_iCompiled = 0;
				int l_iLength = int( m_strLoadedSource.size() );
				std::string l_strTmp = string::to_str( m_strLoadedSource );
				std::vector< char > l_pszTmp( m_strLoadedSource.size() + 1 );
				char * l_buffer = l_pszTmp.data();
#if defined( _MSC_VER )
				strncpy_s( l_buffer, m_strLoadedSource.size() + 1, l_strTmp.c_str(), l_strTmp.size() );
#else
				strncpy( l_buffer, l_strTmp.c_str(), l_strTmp.size() );
#endif
				l_return &= m_gl.ShaderSource( m_shaderObject, 1, const_cast< const char ** >( &l_buffer ), & l_iLength );
				l_return &= m_gl.CompileShader( m_shaderObject );
				l_return &= m_gl.GetShaderiv( m_shaderObject, eGL_SHADER_STATUS_COMPILE, & l_iCompiled );

				if ( l_iCompiled != 0 )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
				else
				{
					m_eStatus = eSHADER_STATUS_ERROR;
				}

				RetrieveCompilerLog( m_compilerLog );

				if ( m_compilerLog.size() > 0 )
				{
					Logger::LogInfo( m_compilerLog );

					if ( m_eStatus == eSHADER_STATUS_ERROR )
					{
						Logger::LogInfo( m_strLoadedSource );
						m_strLoadedSource.clear();
					}
				}

				l_return = m_eStatus == eSHADER_STATUS_COMPILED;
			}
		}

		return l_return;
	}

	void GlShaderObject::Detach()
	{
		if ( m_eStatus == eSHADER_STATUS_COMPILED && m_pShaderProgram && m_pParent->GetRenderSystem()->HasShaderType( m_eType ) )
		{
			m_gl.DetachShader( m_pShaderProgram->GetGlProgram(), m_shaderObject );
			m_pShaderProgram = NULL;
			// if you get an error here, you deleted the Program object first and then
			// the ShaderObject! Always delete ShaderPrograms last!
		}
	}

	void GlShaderObject::AttachTo( ShaderProgramBase & p_program )
	{
		Detach();

		if ( m_eStatus == eSHADER_STATUS_COMPILED && m_pParent->GetRenderSystem()->HasShaderType( m_eType ) )
		{
			m_pShaderProgram = &static_cast< GlShaderProgram & >( p_program );
			m_gl.AttachShader( m_pShaderProgram->GetGlProgram(), m_shaderObject );

			//if( m_eType == eSHADER_TYPE_GEOMETRY )
			//{
			//	int l_iTmp;
			//	m_gl.GetIntegerv( eGL_GETINTEGER_PARAM_MAX_GEOMETRY_OUTPUT_VERTICES,	&l_iTmp );
			//	m_gl.ProgramParameteri( m_pShaderProgram->GetGlProgram(), eGL_PROGRAM_PARAM_GEOMETRY_INPUT_TYPE,	m_gl.Get( m_eInputType )						);
			//	m_gl.ProgramParameteri( m_pShaderProgram->GetGlProgram(), eGL_PROGRAM_PARAM_GEOMETRY_OUTPUT_TYPE,	m_gl.Get( m_eOutputType )						);
			//	m_gl.ProgramParameteri( m_pShaderProgram->GetGlProgram(), eGL_PROGRAM_PARAM_GEOMETRY_VERTICES_OUT,	std::min< int >( m_uiOutputVtxCount, l_iTmp )	);
			//}
		}
	}

	bool GlShaderObject::HasParameter( Castor::String const & p_strName )
	{
		return GetParameter( p_strName ) != eGL_INVALID_INDEX;
	}

	uint32_t GlShaderObject::GetParameter( Castor::String const & p_strName )
	{
		uint32_t l_uiReturn = uint32_t( eGL_INVALID_INDEX );

		if ( m_eStatus == eSHADER_STATUS_COMPILED )
		{
			UIntStrMap::iterator l_it = m_mapParamsByName.find( p_strName );

			if ( l_it == m_mapParamsByName.end() )
			{
				uint32_t l_uiProgram = m_pShaderProgram->GetGlProgram();
				m_mapParamsByName.insert( std::make_pair( p_strName, m_gl.GetUniformLocation( l_uiProgram, string::to_str( p_strName ).c_str() ) ) );
				l_it = m_mapParamsByName.find( p_strName );
			}

			l_uiReturn = l_it->second;
		}

		return l_uiReturn;
	}

	void GlShaderObject::SetParameter( Castor::String const & p_strName, Castor::Matrix4x4r const & p_mtxValue )
	{
		uint32_t l_uiParam = GetParameter( p_strName );

		if ( l_uiParam != eGL_INVALID_INDEX )
		{
			m_gl.SetUniformMatrix4x4v( l_uiParam, 1, false, p_mtxValue.const_ptr() );
		}
	}

	void GlShaderObject::SetParameter( Castor::String const & p_strName, Castor::Matrix3x3r const & p_mtxValue )
	{
		uint32_t l_uiParam = GetParameter( p_strName );

		if ( l_uiParam != eGL_INVALID_INDEX )
		{
			m_gl.SetUniformMatrix3x3v( l_uiParam, 1, false, p_mtxValue.const_ptr() );
		}
	}
}
