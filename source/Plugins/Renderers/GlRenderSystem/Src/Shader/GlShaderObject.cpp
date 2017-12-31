#include "Shader/GlShaderObject.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlPushUniform.hpp"
#include "Shader/GlShaderProgram.hpp"

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlShaderObject::GlShaderObject( OpenGl & gl
		, GlShaderProgram & parent
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

			if ( m_parent.getRenderSystem()->getGpuInformations().hasShaderType( m_type ) )
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
		if ( m_status == ShaderStatus::eCompiled && m_shaderProgram && m_parent.getRenderSystem()->getGpuInformations().hasShaderType( m_type ) )
		{
			getOpenGl().DetachShader( m_shaderProgram->getGlName(), getGlName() );
			m_shaderProgram = nullptr;
		}
	}

	void GlShaderObject::attachTo( ShaderProgram & program )
	{
		detach();

		if ( m_status == ShaderStatus::eCompiled && m_parent.getRenderSystem()->getGpuInformations().hasShaderType( m_type ) )
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

	std::shared_ptr< PushUniform > GlShaderObject::doCreateUniform( UniformType p_type, int p_occurences )
	{
		auto & program = static_cast< GlShaderProgram & >( m_parent );

		switch ( p_type )
		{
		case UniformType::eBool:
			return std::make_shared< GlPushUniform< UniformType::eBool > >( getOpenGl(), program, p_occurences );

		case UniformType::eInt:
			return std::make_shared< GlPushUniform< UniformType::eInt > >( getOpenGl(), program, p_occurences );

		case UniformType::eUInt:
			return std::make_shared< GlPushUniform< UniformType::eUInt > >( getOpenGl(), program, p_occurences );

		case UniformType::eFloat:
			return std::make_shared< GlPushUniform< UniformType::eFloat > >( getOpenGl(), program, p_occurences );

		case UniformType::eDouble:
			return std::make_shared< GlPushUniform< UniformType::eDouble > >( getOpenGl(), program, p_occurences );

		case UniformType::eSampler:
			return std::make_shared< GlPushUniform< UniformType::eSampler > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec2b:
			return std::make_shared< GlPushUniform< UniformType::eVec2b > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec3b:
			return std::make_shared< GlPushUniform< UniformType::eVec3b > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec4b:
			return std::make_shared< GlPushUniform< UniformType::eVec4b > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec2i:
			return std::make_shared< GlPushUniform< UniformType::eVec2i > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec3i:
			return std::make_shared< GlPushUniform< UniformType::eVec3i > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec4i:
			return std::make_shared< GlPushUniform< UniformType::eVec4i > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec2ui:
			return std::make_shared< GlPushUniform< UniformType::eVec2ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec3ui:
			return std::make_shared< GlPushUniform< UniformType::eVec3ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec4ui:
			return std::make_shared< GlPushUniform< UniformType::eVec4ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec2f:
			return std::make_shared< GlPushUniform< UniformType::eVec2f > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec3f:
			return std::make_shared< GlPushUniform< UniformType::eVec3f > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec4f:
			return std::make_shared< GlPushUniform< UniformType::eVec4f > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec2d:
			return std::make_shared< GlPushUniform< UniformType::eVec2d > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec3d:
			return std::make_shared< GlPushUniform< UniformType::eVec3d > >( getOpenGl(), program, p_occurences );

		case UniformType::eVec4d:
			return std::make_shared< GlPushUniform< UniformType::eVec4d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4b > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4i > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4ui > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4f > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat2x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat3x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3d > >( getOpenGl(), program, p_occurences );

		case UniformType::eMat4x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4d > >( getOpenGl(), program, p_occurences );

		default:
			FAILURE( cuT( "Unsupported frame variable type" ) );
			return nullptr;
		}
	}
}
