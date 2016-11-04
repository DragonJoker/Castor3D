#include "Shader/GlShaderObject.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlShaderObject::GlShaderObject( OpenGl & p_gl, GlShaderProgram * p_parent, ShaderType p_type )
		: ShaderObject( p_parent, p_type )
		, Object( p_gl,
				  "GlShaderObject",
				  std::bind( &OpenGl::CreateShader, std::ref( p_gl ), p_gl.Get( p_type ) ),
				  std::bind( &OpenGl::DeleteShader, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGl::IsShader, std::ref( p_gl ), std::placeholders::_1 )
				)
		, m_shaderProgram( nullptr )
	{
	}

	GlShaderObject::~GlShaderObject()
	{
	}

	bool GlShaderObject::Create()
	{
		return ObjectType::Create();
	}

	void GlShaderObject::Destroy()
	{
		Detach();

		if ( m_status == ShaderStatus::eCompiled )
		{
			ObjectType::Destroy();
			m_status = ShaderStatus::eNotCompiled;
		}
	}

	bool GlShaderObject::Compile()
	{
		bool l_return = false;
		String l_loadedSource;

		for ( size_t i = size_t( ShaderModel::eModel5 ); i >= size_t( ShaderModel::eModel1 ) && l_loadedSource.empty(); i-- )
		{
			if ( m_parent->GetRenderSystem()->GetGpuInformations().CheckSupport( ShaderModel( i ) ) )
			{
				l_loadedSource = m_arraySources[i];

				if ( !l_loadedSource.empty() )
				{
					m_eShaderModel = ShaderModel( i );
				}
			}
		}

		if ( m_status != ShaderStatus::eError && !l_loadedSource.empty() && l_loadedSource != m_loadedSource )
		{
			l_return = true;
			m_loadedSource = l_loadedSource;

			if ( m_parent->GetRenderSystem()->GetGpuInformations().HasShaderType( m_type ) )
			{
				m_status = ShaderStatus::eNotCompiled;
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
					l_return = GetOpenGl().GetShaderiv( GetGlName(), GlShaderStatus::eCompile, &l_compiled );

					if ( !l_return )
					{
						Logger::LogError( cuT( "GlShaderObject:: Compile - Shader compilation status retrieval failed." ) );
					}
					else
					{
						Logger::LogDebug( StringStream() << cuT( "GlShaderObject:: Compile - Shader compilation status : " ) << l_compiled );
					}
				}

				if ( l_return && l_compiled )
				{
					m_status = ShaderStatus::eCompiled;
				}
				else
				{
					m_status = ShaderStatus::eError;
					FAILURE( "Shader is not compiled." );
				}

				l_return = DoCheckErrors();
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
		if ( m_status == ShaderStatus::eCompiled && m_shaderProgram && m_parent->GetRenderSystem()->GetGpuInformations().HasShaderType( m_type ) )
		{
			GetOpenGl().DetachShader( m_shaderProgram->GetGlName(), GetGlName() );
			m_shaderProgram = nullptr;
		}
	}

	void GlShaderObject::AttachTo( ShaderProgram & p_program )
	{
		Detach();

		if ( m_status == ShaderStatus::eCompiled && m_parent->GetRenderSystem()->GetGpuInformations().HasShaderType( m_type ) )
		{
			m_shaderProgram = &static_cast< GlShaderProgram & >( p_program );
			GetOpenGl().AttachShader( m_shaderProgram->GetGlName(), GetGlName() );
		}
	}

	bool GlShaderObject::HasParameter( Castor::String const & p_name )
	{
		return GetParameter( p_name ) != GlInvalidIndex;
	}

	uint32_t GlShaderObject::GetParameter( Castor::String const & p_name )
	{
		uint32_t l_uiReturn = uint32_t( GlInvalidIndex );

		if ( m_status == ShaderStatus::eCompiled )
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

		if ( l_param != GlInvalidIndex )
		{
			GetOpenGl().SetUniformMatrix4x4v( l_param, 1, false, p_value.const_ptr() );
		}
	}

	void GlShaderObject::SetParameter( Castor::String const & p_name, Castor::Matrix3x3r const & p_value )
	{
		uint32_t l_param = GetParameter( p_name );

		if ( l_param != GlInvalidIndex )
		{
			GetOpenGl().SetUniformMatrix3x3v( l_param, 1, false, p_value.const_ptr() );
		}
	}

	String GlShaderObject::DoRetrieveCompilerLog()
	{
		String l_log;
		int l_infologLength = 0;
		int l_charsWritten = 0;
		GetOpenGl().GetShaderiv( GetGlName(), GlShaderStatus::eInfoLogLength, &l_infologLength );

		if ( l_infologLength > 0 )
		{
			std::vector< char > l_infoLog( l_infologLength + 1 );
			GetOpenGl().GetShaderInfoLog( GetGlName(), l_infologLength, &l_charsWritten, l_infoLog.data() );
			l_log = string::string_cast< xchar >( l_infoLog.data() );
		}

		if ( !l_log.empty() )
		{
			l_log = l_log.substr( 0, l_log.size() - 1 );
		}

		return l_log;
	}
}
