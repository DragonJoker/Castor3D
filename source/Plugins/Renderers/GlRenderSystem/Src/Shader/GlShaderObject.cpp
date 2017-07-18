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
		bool result = false;
		String loadedSource = m_source.GetSource();

		if ( m_status != ShaderStatus::eError && !loadedSource.empty() )
		{
			result = true;

			if ( m_parent->GetRenderSystem()->GetGpuInformations().HasShaderType( m_type ) )
			{
				m_status = ShaderStatus::eNotCompiled;
				int compiled = 0;
				int iLength = int( loadedSource.size() );
				std::string tmp = string::string_cast< char >( loadedSource );
				std::vector< char > pszTmp( loadedSource.size() + 1 );
				char * buffer = pszTmp.data();
#if defined( CASTOR_COMPILER_MSVC )
				strncpy_s( buffer, loadedSource.size() + 1, tmp.c_str(), tmp.size() );
#else
				strncpy( buffer, tmp.c_str(), tmp.size() );
#endif
				GetOpenGl().ShaderSource( GetGlName(), 1, const_cast< const char ** >( &buffer ), &iLength );
				GetOpenGl().CompileShader( GetGlName() );
				GetOpenGl().GetShaderiv( GetGlName(), GlShaderStatus::eCompile, &compiled );
				Logger::LogDebug( StringStream() << cuT( "GlShaderObject:: Compile - Shader compilation status : " ) << compiled );

				if ( compiled )
				{
					m_status = ShaderStatus::eCompiled;
				}
				else
				{
					m_status = ShaderStatus::eError;
				}

				result = DoCheckErrors();
			}
			else
			{
				Logger::LogError( "GlShaderObject::Compile - Shader type not supported by currently loaded API." );
			}
		}
		else if ( loadedSource.empty() )
		{
			Logger::LogError( "GlShaderObject::Compile - No shader source." );
		}
		else
		{
			Logger::LogWarning( "GlShaderObject::Compile - Shader is already compiled." );
		}

		return result;
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
		String log;
		int infologLength = 0;
		int charsWritten = 0;
		GetOpenGl().GetShaderiv( GetGlName(), GlShaderStatus::eInfoLogLength, &infologLength );

		if ( infologLength > 0 )
		{
			std::vector< char > infoLog( infologLength + 1 );
			GetOpenGl().GetShaderInfoLog( GetGlName(), infologLength, &charsWritten, infoLog.data() );
			log = string::string_cast< xchar >( infoLog.data() );
		}

		if ( !log.empty() )
		{
			log = log.substr( 0, log.size() - 1 );
		}

		return log;
	}
}
