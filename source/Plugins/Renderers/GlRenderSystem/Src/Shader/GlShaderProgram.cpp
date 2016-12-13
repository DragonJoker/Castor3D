#include "Shader/GlShaderProgram.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlUniformBuffer.hpp"
#include "Shader/GlPushUniform.hpp"
#include "Shader/GlShaderObject.hpp"

#include <GlslSource.hpp>

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlShaderProgram::GlShaderProgram( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
		: ShaderProgram( p_renderSystem )
		, Object( p_gl,
				  "GlShaderProgram",
				  std::bind( &OpenGl::CreateProgram, std::ref( p_gl ) ),
				  std::bind( &OpenGl::DeleteProgram, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGl::IsProgram, std::ref( p_gl ), std::placeholders::_1 )
				)
		, m_layout( p_gl, p_renderSystem )
	{
	}

	GlShaderProgram::~GlShaderProgram()
	{
	}

	void GlShaderProgram::Cleanup()
	{
		m_layout.Cleanup();
		DoCleanup();
		ObjectType::Destroy();
	}

	bool GlShaderProgram::Initialise()
	{
		bool l_return = true;

		if ( m_status != ProgramStatus::eLinked )
		{
			ObjectType::Create();
			l_return = DoInitialise();

			if ( l_return )
			{
				m_layout.Initialise( *this );
			}
		}

		return l_return;
	}

	bool GlShaderProgram::Link()
	{
		DoBindTransformLayout();
		ENSURE( GetGlName() != GlInvalidIndex );
		int l_attached = 0;
		GetOpenGl().GetProgramiv( GetGlName(), GlShaderStatus::eAttachedShaders, &l_attached );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << l_attached );
		GetOpenGl().LinkProgram( GetGlName() );
		int l_linked = 0;
		GetOpenGl().GetProgramiv( GetGlName(), GlShaderStatus::eLink, &l_linked );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Program link status : " ) << l_linked );
		m_linkerLog = DoRetrieveLinkerLog();
		bool l_return = false;

		if ( l_linked && l_attached == int( m_activeShaders.size() ) && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::LogWarning( cuT( "GlShaderProgram::Link - " ) + m_linkerLog );
			}

			l_return = DoLink();
		}
		else
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::LogError( cuT( "GlShaderProgram::Link - " ) + m_linkerLog );
			}

			if ( l_attached != int( m_activeShaders.size() ) )
			{
				Logger::LogError( cuT( "GlShaderProgram::Link - The linked shaders count doesn't match the active shaders count." ) );
			}

			m_status = ProgramStatus::eError;
		}

		return l_return;
	}

	void GlShaderProgram::Bind( bool p_bindUbo )const
	{
		if ( GetGlName() != GlInvalidIndex && m_status == ProgramStatus::eLinked )
		{
			GetOpenGl().UseProgram( GetGlName() );
			DoBind( p_bindUbo );
		}
	}

	void GlShaderProgram::Unbind()const
	{
		if ( GetGlName() != GlInvalidIndex && m_status == ProgramStatus::eLinked )
		{
			DoUnbind();
			GetOpenGl().UseProgram( 0 );
		}
	}

	int GlShaderProgram::GetAttributeLocation( String const & p_name )const
	{
		int l_iReturn = int( GlInvalidIndex );

		if ( GetGlName() != GlInvalidIndex && GetOpenGl().IsProgram( GetGlName() ) )
		{
			l_iReturn = GetOpenGl().GetAttribLocation( GetGlName(), string::string_cast< char >( p_name ).c_str() );
		}

		return l_iReturn;
	}

	ShaderObjectSPtr GlShaderProgram::DoCreateObject( ShaderType p_type )
	{
		ShaderObjectSPtr l_return = std::make_shared< GlShaderObject >( GetOpenGl(), this, p_type );
		return l_return;
	}

	UniformBufferSPtr GlShaderProgram::DoCreateUniformBuffer( String const & p_name, uint32_t p_index )
	{
		return std::make_shared< GlUniformBuffer >( GetOpenGl(), p_name, *GetRenderSystem(), p_index );
	}

	std::shared_ptr< PushUniform > GlShaderProgram::DoCreateUniform( UniformType p_type, int p_occurences )
	{
		switch ( p_type )
		{
		case UniformType::eBool:
			return std::make_shared< GlPushUniform< UniformType::eBool > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eInt:
			return std::make_shared< GlPushUniform< UniformType::eInt > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eUInt:
			return std::make_shared< GlPushUniform< UniformType::eUInt > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eFloat:
			return std::make_shared< GlPushUniform< UniformType::eFloat > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eDouble:
			return std::make_shared< GlPushUniform< UniformType::eDouble > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eSampler:
			return std::make_shared< GlPushUniform< UniformType::eSampler > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec2b:
			return std::make_shared< GlPushUniform< UniformType::eVec2b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec3b:
			return std::make_shared< GlPushUniform< UniformType::eVec3b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec4b:
			return std::make_shared< GlPushUniform< UniformType::eVec4b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec2i:
			return std::make_shared< GlPushUniform< UniformType::eVec2i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec3i:
			return std::make_shared< GlPushUniform< UniformType::eVec3i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec4i:
			return std::make_shared< GlPushUniform< UniformType::eVec4i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec2ui:
			return std::make_shared< GlPushUniform< UniformType::eVec2ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec3ui:
			return std::make_shared< GlPushUniform< UniformType::eVec3ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec4ui:
			return std::make_shared< GlPushUniform< UniformType::eVec4ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec2f:
			return std::make_shared< GlPushUniform< UniformType::eVec2f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec3f:
			return std::make_shared< GlPushUniform< UniformType::eVec3f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec4f:
			return std::make_shared< GlPushUniform< UniformType::eVec4f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec2d:
			return std::make_shared< GlPushUniform< UniformType::eVec2d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec3d:
			return std::make_shared< GlPushUniform< UniformType::eVec3d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eVec4d:
			return std::make_shared< GlPushUniform< UniformType::eVec4d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4b:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4b > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4i:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4i > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4ui:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4ui > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4f:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4f > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x2d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x3d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat2x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat2x4d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x2d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x3d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat3x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat3x4d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x2d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x2d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x3d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x3d > >( GetOpenGl(), *this, p_occurences );

		case UniformType::eMat4x4d:
			return std::make_shared< GlPushUniform< UniformType::eMat4x4d > >( GetOpenGl(), *this, p_occurences );

		default:
			FAILURE( cuT( "Unsupported frame variable type" ) );
			return nullptr;
		}
	}

	String GlShaderProgram::DoRetrieveLinkerLog()
	{
		String l_log;

		if ( GetGlName() == GlInvalidIndex )
		{
			l_log = GetOpenGl().GetGlslErrorString( 2 );
		}
		else
		{
			int l_length = 0;
			GetOpenGl().GetProgramiv( GetGlName(), GlShaderStatus::eInfoLogLength, &l_length );

			if ( l_length > 1 )
			{
				std::vector< char > l_buffer( l_length );
				int l_written = 0;
				GetOpenGl().GetProgramInfoLog( GetGlName(), l_length, &l_written, l_buffer.data() );

				if ( l_written > 0 )
				{
					l_log = string::string_cast< xchar >( l_buffer.data(), l_buffer.data() + l_written );
				}
			}
		}

		return l_log;
	}

	void GlShaderProgram::DoBindTransformLayout()
	{
		if ( m_declaration.size() > 0 )
		{
			std::vector< char const * > l_varyings;
			l_varyings.reserve( m_declaration.size() );

			for ( auto & l_element : m_declaration )
			{
				l_varyings.push_back( l_element.m_name.c_str() );
			}

			GetOpenGl().TransformFeedbackVaryings( GetGlName(), int( l_varyings.size() ), l_varyings.data(), GlAttributeLayout::eInterleaved );
		}
	}
}
