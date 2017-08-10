#include "Shader/GlShaderObject.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlShaderObject::GlShaderObject( OpenGl & gl
		, GlShaderProgram * parent
		, ShaderType type )
		: ShaderObject( parent, type )
		, Object( gl,
				  "GlShaderObject",
				  std::bind( &OpenGl::CreateShader, std::ref( gl ), gl.get( type ) ),
				  std::bind( &OpenGl::DeleteShader, std::ref( gl ), std::placeholders::_1 ),
				  std::bind( &OpenGl::IsShader, std::ref( gl ), std::placeholders::_1 )
				)
		, m_shaderProgram( nullptr )
	{
	}

	GlShaderObject::~GlShaderObject()
	{
	}

	bool GlShaderObject::create()
	{
		return ObjectType::create();
	}

	void GlShaderObject::destroy()
	{
		detach();

		if ( m_status == ShaderStatus::eCompiled )
		{
			ObjectType::destroy();
			m_status = ShaderStatus::eNotCompiled;
		}
	}

	bool GlShaderObject::compile()
	{
		bool result = false;
		String loadedSource = m_source.getSource();

		if ( m_status != ShaderStatus::eError && !loadedSource.empty() )
		{
			result = true;

			if ( m_parent->getRenderSystem()->getGpuInformations().hasShaderType( m_type ) )
			{
				m_status = ShaderStatus::eNotCompiled;
				int compiled = 0;
				int iLength = int( loadedSource.size() );
				std::string tmp = string::stringCast< char >( loadedSource );
				std::vector< char > pszTmp( loadedSource.size() + 1 );
				char * buffer = pszTmp.data();
#if defined( CASTOR_COMPILER_MSVC )
				strncpy_s( buffer, loadedSource.size() + 1, tmp.c_str(), tmp.size() );
#else
				strncpy( buffer, tmp.c_str(), tmp.size() );
#endif
				getOpenGl().ShaderSource( getGlName(), 1, const_cast< const char ** >( &buffer ), &iLength );
				getOpenGl().CompileShader( getGlName() );
				getOpenGl().GetShaderiv( getGlName(), GlShaderStatus::eCompile, &compiled );
				Logger::logDebug( StringStream() << cuT( "GlShaderObject:: Compile - Shader compilation status : " ) << compiled );

				if ( compiled )
				{
					m_status = ShaderStatus::eCompiled;
				}
				else
				{
					m_status = ShaderStatus::eError;
				}

				result = doCheckErrors();
			}
			else
			{
				Logger::logError( "GlShaderObject::Compile - Shader type not supported by currently loaded API." );
			}
		}
		else if ( loadedSource.empty() )
		{
			Logger::logError( "GlShaderObject::Compile - No shader source." );
		}
		else
		{
			Logger::logWarning( "GlShaderObject::Compile - Shader is already compiled." );
		}

		return result;
	}

	void GlShaderObject::detach()
	{
		if ( m_status == ShaderStatus::eCompiled && m_shaderProgram && m_parent->getRenderSystem()->getGpuInformations().hasShaderType( m_type ) )
		{
			getOpenGl().DetachShader( m_shaderProgram->getGlName(), getGlName() );
			m_shaderProgram = nullptr;
		}
	}

	void GlShaderObject::attachTo( ShaderProgram & program )
	{
		detach();

		if ( m_status == ShaderStatus::eCompiled && m_parent->getRenderSystem()->getGpuInformations().hasShaderType( m_type ) )
		{
			m_shaderProgram = &static_cast< GlShaderProgram & >( program );
			getOpenGl().AttachShader( m_shaderProgram->getGlName(), getGlName() );
		}
	}

	String GlShaderObject::doRetrieveCompilerLog()
	{
		String log;
		int infologLength = 0;
		int charsWritten = 0;
		getOpenGl().GetShaderiv( getGlName(), GlShaderStatus::eInfoLogLength, &infologLength );

		if ( infologLength > 0 )
		{
			std::vector< char > infoLog( infologLength + 1 );
			getOpenGl().GetShaderInfoLog( getGlName(), infologLength, &charsWritten, infoLog.data() );
			log = string::stringCast< xchar >( infoLog.data() );
		}

		if ( !log.empty() )
		{
			log = log.substr( 0, log.size() - 1 );
		}

		return log;
	}
}
