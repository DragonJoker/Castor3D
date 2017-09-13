#include "Shader/GlShaderProgram.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlPushUniform.hpp"
#include "Shader/GlShaderObject.hpp"

#include <GlslSource.hpp>

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlShaderProgram::GlShaderProgram( OpenGl & p_gl, GlRenderSystem & renderSystem )
		: ShaderProgram( renderSystem )
		, Object( p_gl,
				  "GlShaderProgram",
				  std::bind( &OpenGl::CreateProgram, std::ref( p_gl ) ),
				  std::bind( &OpenGl::DeleteProgram, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGl::IsProgram, std::ref( p_gl ), std::placeholders::_1 )
				)
		, m_layout( p_gl, renderSystem )
	{
	}

	GlShaderProgram::~GlShaderProgram()
	{
	}

	void GlShaderProgram::cleanup()
	{
		m_layout.cleanup();
		doCleanup();
		ObjectType::destroy();
	}

	bool GlShaderProgram::initialise()
	{
		bool result = true;

		if ( m_status != ProgramStatus::eLinked )
		{
			ObjectType::create();
			result = doInitialise();

			if ( result )
			{
				m_layout.initialise( *this );
			}
		}

		return result;
	}

	bool GlShaderProgram::link()
	{
		doBindTransformLayout();
		ENSURE( getGlName() != GlInvalidIndex );
		int attached = 0;
		getOpenGl().GetProgramiv( getGlName(), GlShaderStatus::eAttachedShaders, &attached );
		Logger::logDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << attached );
		getOpenGl().LinkProgram( getGlName() );
		int linked = 0;
		getOpenGl().GetProgramiv( getGlName(), GlShaderStatus::eLink, &linked );
		Logger::logDebug( StringStream() << cuT( "GlShaderProgram::Link - Program link status : " ) << linked );
		m_linkerLog = doRetrieveLinkerLog();
		bool result = false;

		if ( linked && attached == int( m_activeShaders.size() ) && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::logWarning( cuT( "GlShaderProgram::link - " ) + m_linkerLog );
			}

			result = doLink();
		}
		else
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::logError( cuT( "GlShaderProgram::link - " ) + m_linkerLog );
			}

			if ( attached != int( m_activeShaders.size() ) )
			{
				Logger::logError( cuT( "GlShaderProgram::link - The linked shaders count doesn't match the active shaders count." ) );
			}

			m_status = ProgramStatus::eError;
		}

		return result;
	}

	void GlShaderProgram::bind()const
	{
		REQUIRE( getGlName() != GlInvalidIndex && m_status == ProgramStatus::eLinked );
		getOpenGl().UseProgram( getGlName() );
		doBind();
	}

	void GlShaderProgram::unbind()const
	{
		REQUIRE( getGlName() != GlInvalidIndex && m_status == ProgramStatus::eLinked );
		doUnbind();
		getOpenGl().UseProgram( 0 );
	}

	int GlShaderProgram::getAttributeLocation( String const & p_name )const
	{
		int iReturn = int( GlInvalidIndex );

		if ( getGlName() != GlInvalidIndex && getOpenGl().IsProgram( getGlName() ) )
		{
			iReturn = getOpenGl().GetAttribLocation( getGlName(), string::stringCast< char >( p_name ).c_str() );
		}

		return iReturn;
	}

	ShaderObjectSPtr GlShaderProgram::doCreateObject( ShaderType p_type )
	{
		ShaderObjectSPtr result = std::make_shared< GlShaderObject >( getOpenGl(), this, p_type );
		return result;
	}

	std::shared_ptr< PushUniform > GlShaderProgram::doCreateUniform( UniformType p_type, int p_occurences )
	{
		switch ( p_type )
		{
		case UniformType::eBool:
			return std::make_shared< GlPushUniform< UniformType::eBool > >( getOpenGl(), *this, p_occurences );

		case UniformType::eInt:
			return std::make_shared< GlPushUniform< UniformType::eInt > >( getOpenGl(), *this, p_occurences );

		case UniformType::eUInt:
			return std::make_shared< GlPushUniform< UniformType::eUInt > >( getOpenGl(), *this, p_occurences );

		case UniformType::eFloat:
			return std::make_shared< GlPushUniform< UniformType::eFloat > >( getOpenGl(), *this, p_occurences );

		case UniformType::eDouble:
			return std::make_shared< GlPushUniform< UniformType::eDouble > >( getOpenGl(), *this, p_occurences );

		case UniformType::eSampler:
			return std::make_shared< GlPushUniform< UniformType::eSampler > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec2b:
			return std::make_shared< GlPushUniform< UniformType::eVec2b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec3b:
			return std::make_shared< GlPushUniform< UniformType::eVec3b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec4b:
			return std::make_shared< GlPushUniform< UniformType::eVec4b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec2i:
			return std::make_shared< GlPushUniform< UniformType::eVec2i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec3i:
			return std::make_shared< GlPushUniform< UniformType::eVec3i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec4i:
			return std::make_shared< GlPushUniform< UniformType::eVec4i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec2ui:
			return std::make_shared< GlPushUniform< UniformType::eVec2ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec3ui:
			return std::make_shared< GlPushUniform< UniformType::eVec3ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec4ui:
			return std::make_shared< GlPushUniform< UniformType::eVec4ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec2f:
			return std::make_shared< GlPushUniform< UniformType::eVec2f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec3f:
			return std::make_shared< GlPushUniform< UniformType::eVec3f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec4f:
			return std::make_shared< GlPushUniform< UniformType::eVec4f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec2d:
			return std::make_shared< GlPushUniform< UniformType::eVec2d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec3d:
			return std::make_shared< GlPushUniform< UniformType::eVec3d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eVec4d:
			return std::make_shared< GlPushUniform< UniformType::eVec4d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4b > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4i > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4ui > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4f > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3d > >( getOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4d > >( getOpenGl(), *this, p_occurences );

		default:
			FAILURE( cuT( "Unsupported frame variable type" ) );
			return nullptr;
		}
	}

	String GlShaderProgram::doRetrieveLinkerLog()
	{
		String log;

		if ( getGlName() == GlInvalidIndex )
		{
			log = getOpenGl().getGlslErrorString( 2 );
		}
		else
		{
			int length = 0;
			getOpenGl().GetProgramiv( getGlName(), GlShaderStatus::eInfoLogLength, &length );

			if ( length > 1 )
			{
				std::vector< char > buffer( length );
				int written = 0;
				getOpenGl().GetProgramInfoLog( getGlName(), length, &written, buffer.data() );

				if ( written > 0 )
				{
					log = string::stringCast< xchar >( buffer.data(), buffer.data() + written );
				}
			}
		}

		return log;
	}

	void GlShaderProgram::doBindTransformLayout()
	{
		if ( m_declaration.size() > 0 )
		{
			std::vector< char const * > varyings;
			varyings.reserve( m_declaration.size() );

			for ( auto & element : m_declaration )
			{
				varyings.push_back( element.m_name.c_str() );
			}

			getOpenGl().TransformFeedbackVaryings( getGlName(), int( varyings.size() ), varyings.data(), GlAttributeLayout::eInterleaved );
		}
	}
}
