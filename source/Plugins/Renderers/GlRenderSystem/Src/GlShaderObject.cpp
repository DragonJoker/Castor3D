#include "GlShaderObject.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

#include <RenderSystem.hpp>

#include <Logger.hpp>
#include <StreamPrefixManipulators.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlShaderObject::GlShaderObject( OpenGl & p_gl, GlShaderProgram * p_parent, eSHADER_TYPE p_type )
		: ShaderObjectBase( p_parent, p_type )
		, Object( p_gl,
				  "GlShaderObject",
				  std::bind( &OpenGl::CreateShader, std::ref( p_gl ), p_gl.Get( p_type ) ),
				  std::bind( &OpenGl::DeleteShader, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGl::IsShader, std::ref( p_gl ), std::placeholders::_1 )
				  )
		, m_shaderProgram( NULL )
	{
	}

	GlShaderObject::~GlShaderObject()
	{
	}

	void GlShaderObject::CreateProgram()
	{
		ObjectType::Create();
	}

	void GlShaderObject::DestroyProgram()
	{
		Detach();

		if ( m_status == eSHADER_STATUS_COMPILED )
		{
			ObjectType::Destroy();
			m_status = eSHADER_STATUS_NOTCOMPILED;
		}
	}

	void GlShaderObject::RetrieveCompilerLog( String & p_compilerLog )
	{
		int l_infologLength = 0;
		int l_charsWritten  = 0;
		GetOpenGl().GetShaderiv( GetGlName(), eGL_SHADER_STATUS_INFO_LOG_LENGTH, &l_charsWritten );

		if ( l_infologLength > 0 )
		{
			char * infoLog = new char[l_infologLength];
			GetOpenGl().GetShaderInfoLog( GetGlName(), l_infologLength, &l_charsWritten, infoLog );
			p_compilerLog = string::string_cast< xchar >( infoLog );
			delete [] infoLog;
		}

		if ( p_compilerLog.size() > 0 )
		{
			p_compilerLog = p_compilerLog.substr( 0, p_compilerLog.size() - 1 );
		}
	}

	bool GlShaderObject::Compile()
	{
		bool l_return = false;
		String l_loadedSource;

		for ( int i = eSHADER_MODEL_5; i >= eSHADER_MODEL_1 && l_loadedSource.empty(); i-- )
		{
			if ( m_parent->GetOwner()->CheckSupport( eSHADER_MODEL( i ) ) )
			{
				l_loadedSource = m_arraySources[i];

				if ( !l_loadedSource.empty() )
				{
					m_eShaderModel = eSHADER_MODEL( i );
				}
			}
		}

		if ( m_status != eSHADER_STATUS_ERROR && !l_loadedSource.empty() && l_loadedSource != m_loadedSource )
		{
			l_return = true;
			m_loadedSource = l_loadedSource;

			if ( m_parent->GetOwner()->HasShaderType( m_type ) )
			{
				m_status = eSHADER_STATUS_NOTCOMPILED;
				int l_compiled = 0;
				int l_iLength = int( m_loadedSource.size() );
				std::string l_tmp = string::string_cast< char >( m_loadedSource );
				std::vector< char > l_pszTmp( m_loadedSource.size() + 1 );
				char * l_buffer = l_pszTmp.data();
#if defined( _MSC_VER )
				strncpy_s( l_buffer, m_loadedSource.size() + 1, l_tmp.c_str(), l_tmp.size() );
#else
				strncpy( l_buffer, l_tmp.c_str(), l_tmp.size() );
#endif
				l_return = GetOpenGl().ShaderSource( GetGlName(), 1, const_cast< const char ** >( &l_buffer ), &l_iLength );

				if ( l_return )
				{
					l_return = GetOpenGl().CompileShader( GetGlName() );

					if ( !l_return )
					{
						Logger::LogError( cuT( "GlShaderObject:: Compile - Shader source was not compiled." ) );
					}
				}
				else
				{
					Logger::LogError( cuT( "GlShaderObject:: Compile - Shader source was not loaded." ) );
				}

				if ( l_return )
				{
					l_return = GetOpenGl().GetShaderiv( GetGlName(), eGL_SHADER_STATUS_COMPILE, &l_compiled );

					if ( !l_return )
					{
						Logger::LogError( cuT( "GlShaderObject:: Compile - Shader compilation status retrieval failed." ) );
					}
					else
					{
						Logger::LogDebug( StringStream() << cuT( "GlShaderObject:: Compile - Shader compilation status : " ) << l_compiled );
					}
				}

				if ( l_return && l_compiled != 0 )
				{
					m_status = eSHADER_STATUS_COMPILED;
				}
				else
				{
					m_status = eSHADER_STATUS_ERROR;
				}

				RetrieveCompilerLog( m_compilerLog );

				if ( !m_compilerLog.empty() )
				{
					if ( m_status == eSHADER_STATUS_ERROR )
					{
						Logger::LogError( m_compilerLog );
					}
					else
					{
						Logger::LogInfo( m_compilerLog );
					}

					StringStream l_source;
					l_source << format::line_prefix();
					l_source << m_loadedSource;
					Logger::LogDebug( l_source.str() );
					m_loadedSource.clear();
				}
				else if ( m_status == eSHADER_STATUS_ERROR )
				{
					Logger::LogError( cuT( "GlShaderObject:: Compile - Compilaton failed with an unknown error." ) );
					StringStream l_source;
					l_source << format::line_prefix();
					l_source << m_loadedSource;
					Logger::LogDebug( l_source.str() );
					m_loadedSource.clear();
				}

				l_return = m_status == eSHADER_STATUS_COMPILED;
			}
			else
			{
				Logger::LogError( "GlShaderObject::Compile - Shader type not supported by currently loaded API." );
			}
		}
		else if ( m_loadedSource.empty() )
		{
			Logger::LogError( "GlShaderObject::Compile - No shader source." );
		}
		else
		{
			Logger::LogWarning( "GlShaderObject::Compile - Shader is already compiled." );
		}

		return l_return;
	}

	void GlShaderObject::Detach()
	{
		if ( m_status == eSHADER_STATUS_COMPILED && m_shaderProgram && m_parent->GetOwner()->HasShaderType( m_type ) )
		{
			GetOpenGl().DetachShader( m_shaderProgram->GetGlName(), GetGlName() );
			m_shaderProgram = NULL;
			// if you get an error here, you deleted the Program object first and then
			// the ShaderObject! Always delete ShaderPrograms last!
		}
	}

	void GlShaderObject::AttachTo( ShaderProgramBase & p_program )
	{
		Detach();

		if ( m_status == eSHADER_STATUS_COMPILED && m_parent->GetOwner()->HasShaderType( m_type ) )
		{
			m_shaderProgram = &static_cast< GlShaderProgram & >( p_program );
			GetOpenGl().AttachShader( m_shaderProgram->GetGlName(), GetGlName() );

			//if( m_type == eSHADER_TYPE_GEOMETRY )
			//{
			//	int l_iTmp;
			//	GetOpenGl().GetIntegerv( eGL_GETINTEGER_PARAM_MAX_GEOMETRY_OUTPUT_VERTICES,	&l_iTmp );
			//	GetOpenGl().ProgramParameteri( m_shaderProgram->GetGlName(), eGL_PROGRAM_PARAM_GEOMETRY_INPUT_TYPE, GetOpenGl().Get( m_eInputType ) );
			//	GetOpenGl().ProgramParameteri( m_shaderProgram->GetGlName(), eGL_PROGRAM_PARAM_GEOMETRY_OUTPUT_TYPE, GetOpenGl().Get( m_eOutputType ) );
			//	GetOpenGl().ProgramParameteri( m_shaderProgram->GetGlName(), eGL_PROGRAM_PARAM_GEOMETRY_VERTICES_OUT, std::min< int >( m_uiOutputVtxCount, l_iTmp ) );
			//}
		}
	}

	bool GlShaderObject::HasParameter( Castor::String const & p_name )
	{
		return GetParameter( p_name ) != eGL_INVALID_INDEX;
	}

	uint32_t GlShaderObject::GetParameter( Castor::String const & p_name )
	{
		uint32_t l_uiReturn = uint32_t( eGL_INVALID_INDEX );

		if ( m_status == eSHADER_STATUS_COMPILED )
		{
			UIntStrMap::iterator l_it = m_mapParamsByName.find( p_name );

			if ( l_it == m_mapParamsByName.end() )
			{
				uint32_t l_program = m_shaderProgram->GetGlName();
				m_mapParamsByName.insert( std::make_pair( p_name, GetOpenGl().GetUniformLocation( l_program, string::string_cast< char >( p_name ).c_str() ) ) );
				l_it = m_mapParamsByName.find( p_name );
			}

			l_uiReturn = l_it->second;
		}

		return l_uiReturn;
	}

	void GlShaderObject::SetParameter( Castor::String const & p_name, Castor::Matrix4x4r const & p_value )
	{
		uint32_t l_param = GetParameter( p_name );

		if ( l_param != eGL_INVALID_INDEX )
		{
			GetOpenGl().SetUniformMatrix4x4v( l_param, 1, false, p_value.const_ptr() );
		}
	}

	void GlShaderObject::SetParameter( Castor::String const & p_name, Castor::Matrix3x3r const & p_value )
	{
		uint32_t l_param = GetParameter( p_name );

		if ( l_param != eGL_INVALID_INDEX )
		{
			GetOpenGl().SetUniformMatrix3x3v( l_param, 1, false, p_value.const_ptr() );
		}
	}
}
