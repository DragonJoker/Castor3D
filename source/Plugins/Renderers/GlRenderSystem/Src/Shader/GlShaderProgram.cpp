#include "Shader/GlShaderProgram.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
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
		ShaderObjectSPtr result = std::make_shared< GlShaderObject >( getOpenGl(), *this, p_type );
		return result;
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
