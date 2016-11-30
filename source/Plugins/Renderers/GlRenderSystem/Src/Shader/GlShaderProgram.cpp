#include "Shader/GlShaderProgram.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlFrameVariableBuffer.hpp"
#include "Shader/GlOneFrameVariable.hpp"
#include "Shader/GlPointFrameVariable.hpp"
#include "Shader/GlMatrixFrameVariable.hpp"
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
		bool l_return = DoBindTransformLayout();
		ENSURE( GetGlName() != GlInvalidIndex );
		int l_attached = 0;
		l_return &= GetOpenGl().GetProgramiv( GetGlName(), GlShaderStatus::eAttachedShaders, &l_attached );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << l_attached );
		l_return &= GetOpenGl().LinkProgram( GetGlName() );
		int l_linked = 0;
		l_return &= GetOpenGl().GetProgramiv( GetGlName(), GlShaderStatus::eLink, &l_linked );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Program link status : " ) << l_linked );
		m_linkerLog = DoRetrieveLinkerLog();

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

	FrameVariableBufferSPtr GlShaderProgram::DoCreateFrameVariableBuffer(
		String const & p_name,
		FlagCombination< ShaderTypeFlag > const & p_flags )
	{
		return std::make_shared< GlFrameVariableBuffer >( GetOpenGl(), p_name, *this, p_flags, *GetRenderSystem() );
	}

	std::shared_ptr< FrameVariable > GlShaderProgram::DoCreateVariable( FrameVariableType p_type, int p_occurences )
	{
		switch ( p_type )
		{
		case FrameVariableType::eBool:
			return std::make_shared< GlOneFrameVariable< bool > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eInt:
			return std::make_shared< GlOneFrameVariable< int > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eUInt:
			return std::make_shared< GlOneFrameVariable< unsigned int > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eFloat:
			return std::make_shared< GlOneFrameVariable< float > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eDouble:
			return std::make_shared< GlOneFrameVariable< double > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eSampler:
			return std::make_shared< GlOneFrameVariable< int > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec2b:
			return std::make_shared< GlPointFrameVariable< bool, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec3b:
			return std::make_shared< GlPointFrameVariable< bool, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec4b:
			return std::make_shared< GlPointFrameVariable< bool, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec2i:
			return std::make_shared< GlPointFrameVariable< int, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec3i:
			return std::make_shared< GlPointFrameVariable< int, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec4i:
			return std::make_shared< GlPointFrameVariable< int, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec2ui:
			return std::make_shared< GlPointFrameVariable< unsigned int, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec3ui:
			return std::make_shared< GlPointFrameVariable< unsigned int, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec4ui:
			return std::make_shared< GlPointFrameVariable< unsigned int, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec2f:
			return std::make_shared< GlPointFrameVariable< float, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec3f:
			return std::make_shared< GlPointFrameVariable< float, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec4f:
			return std::make_shared< GlPointFrameVariable< float, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec2d:
			return std::make_shared< GlPointFrameVariable< double, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec3d:
			return std::make_shared< GlPointFrameVariable< double, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eVec4d:
			return std::make_shared< GlPointFrameVariable< double, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x2b:
			return std::make_shared< GlMatrixFrameVariable< bool, 2, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x3b:
			return std::make_shared< GlMatrixFrameVariable< bool, 2, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x4b:
			return std::make_shared< GlMatrixFrameVariable< bool, 2, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x2b:
			return std::make_shared< GlMatrixFrameVariable< bool, 3, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x3b:
			return std::make_shared< GlMatrixFrameVariable< bool, 3, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x4b:
			return std::make_shared< GlMatrixFrameVariable< bool, 3, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x2b:
			return std::make_shared< GlMatrixFrameVariable< bool, 4, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x3b:
			return std::make_shared< GlMatrixFrameVariable< bool, 4, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x4b:
			return std::make_shared< GlMatrixFrameVariable< bool, 4, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x2i:
			return std::make_shared< GlMatrixFrameVariable< int, 2, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x3i:
			return std::make_shared< GlMatrixFrameVariable< int, 2, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x4i:
			return std::make_shared< GlMatrixFrameVariable< int, 2, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x2i:
			return std::make_shared< GlMatrixFrameVariable< int, 3, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x3i:
			return std::make_shared< GlMatrixFrameVariable< int, 3, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x4i:
			return std::make_shared< GlMatrixFrameVariable< int, 3, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x2i:
			return std::make_shared< GlMatrixFrameVariable< int, 4, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x3i:
			return std::make_shared< GlMatrixFrameVariable< int, 4, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x4i:
			return std::make_shared< GlMatrixFrameVariable< int, 4, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x2ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 2, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x3ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 2, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x4ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 2, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x2ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 3, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x3ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 3, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x4ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 3, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x2ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 4, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x3ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 4, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x4ui:
			return std::make_shared< GlMatrixFrameVariable< unsigned int, 4, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x2f:
			return std::make_shared< GlMatrixFrameVariable< float, 2, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x3f:
			return std::make_shared< GlMatrixFrameVariable< float, 2, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x4f:
			return std::make_shared< GlMatrixFrameVariable< float, 2, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x2f:
			return std::make_shared< GlMatrixFrameVariable< float, 3, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x3f:
			return std::make_shared< GlMatrixFrameVariable< float, 3, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x4f:
			return std::make_shared< GlMatrixFrameVariable< float, 3, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x2f:
			return std::make_shared< GlMatrixFrameVariable< float, 4, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x3f:
			return std::make_shared< GlMatrixFrameVariable< float, 4, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x4f:
			return std::make_shared< GlMatrixFrameVariable< float, 4, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x2d:
			return std::make_shared< GlMatrixFrameVariable< double, 2, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x3d:
			return std::make_shared< GlMatrixFrameVariable< double, 2, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat2x4d:
			return std::make_shared< GlMatrixFrameVariable< double, 2, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x2d:
			return std::make_shared< GlMatrixFrameVariable< double, 3, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x3d:
			return std::make_shared< GlMatrixFrameVariable< double, 3, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat3x4d:
			return std::make_shared< GlMatrixFrameVariable< double, 3, 4 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x2d:
			return std::make_shared< GlMatrixFrameVariable< double, 4, 2 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x3d:
			return std::make_shared< GlMatrixFrameVariable< double, 4, 3 > >( GetOpenGl(), p_occurences, *this );

		case FrameVariableType::eMat4x4d:
			return std::make_shared< GlMatrixFrameVariable< double, 4, 4 > >( GetOpenGl(), p_occurences, *this );

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

	bool GlShaderProgram::DoBindTransformLayout()
	{
		bool l_return = true;

		if ( m_declaration.size() > 0 )
		{
			std::vector< char const * > l_varyings;
			l_varyings.reserve( m_declaration.size() );

			for ( auto & l_element : m_declaration )
			{
				l_varyings.push_back( l_element.m_name.c_str() );
			}

			l_return = GetOpenGl().TransformFeedbackVaryings( GetGlName(), int( l_varyings.size() ), l_varyings.data(), GlAttributeLayout::eInterleaved );
		}

		return l_return;
	}
}
