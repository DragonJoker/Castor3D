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
		String l_loadedSource = m_source.GetSource();

		if ( m_status != ShaderStatus::eError && !l_loadedSource.empty() )
		{
			l_return = true;

			if ( m_parent->GetRenderSystem()->GetGpuInformations().HasShaderType( m_type ) )
			{
				m_status = ShaderStatus::eNotCompiled;
				int l_compiled = 0;
				int l_iLength = int( l_loadedSource.size() );
				std::string l_tmp = string::string_cast< char >( l_loadedSource );
				std::vector< char > l_pszTmp( l_loadedSource.size() + 1 );
				char * l_buffer = l_pszTmp.data();
#if defined( CASTOR_COMPILER_MSVC )
				strncpy_s( l_buffer, l_loadedSource.size() + 1, l_tmp.c_str(), l_tmp.size() );
#else
				strncpy( l_buffer, l_tmp.c_str(), l_tmp.size() );
#endif
				GetOpenGl().ShaderSource( GetGlName(), 1, const_cast< const char ** >( &l_buffer ), &l_iLength );
				GetOpenGl().CompileShader( GetGlName() );
				GetOpenGl().GetShaderiv( GetGlName(), GlShaderStatus::eCompile, &l_compiled );
				Logger::LogDebug( StringStream() << cuT( "GlShaderObject:: Compile - Shader compilation status : " ) << l_compiled );

				if ( l_compiled )
				{
					m_status = ShaderStatus::eCompiled;
				}
				else
				{
					m_status = ShaderStatus::eError;
				}

				l_return = DoCheckErrors();
			}
			else
			{
				Logger::LogError( "GlShaderObject::Compile - Shader type not supported by currently loaded API." );
			}
		}
		else if ( l_loadedSource.empty() )
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
