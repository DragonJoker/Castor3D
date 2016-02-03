#include "GlShaderProgram.hpp"

#include "GlShaderObject.hpp"
#include "GlBuffer.hpp"
#include "GlRenderSystem.hpp"
#include "GlOneFrameVariable.hpp"
#include "GlFrameVariableBuffer.hpp"
#include "OpenGl.hpp"

#include "GlslSource.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	namespace
	{
		uint32_t GetOffset( eELEMENT_TYPE p_type )
		{
			switch ( p_type )
			{
			case eELEMENT_TYPE_1FLOAT:
				return 4;
				break;

			case eELEMENT_TYPE_2FLOATS:
				return 8;
				break;

			case eELEMENT_TYPE_3FLOATS:
				return 12;
				break;

			case eELEMENT_TYPE_4FLOATS:
				return 16;
				break;

			case eELEMENT_TYPE_COLOUR:
				return 4;
				break;

			case eELEMENT_TYPE_1INT:
				return 4;
				break;

			case eELEMENT_TYPE_2INTS:
				return 8;
				break;

			case eELEMENT_TYPE_3INTS:
				return 12;
				break;

			case eELEMENT_TYPE_4INTS:
				return 16;
				break;

			default:
				assert( false && "Unsupported element type" );
				break;
			}

			return 0;
		}
	}

	GlShaderProgram::GlShaderProgram( OpenGl & p_gl, GlRenderSystem & p_renderSystem )
		: ShaderProgramBase( p_renderSystem, eSHADER_LANGUAGE_GLSL )
		, Object( p_gl,
				  "GlShaderProgram",
				  std::bind( &OpenGl::CreateProgram, std::ref( p_gl ) ),
				  std::bind( &OpenGl::DeleteProgram, std::ref( p_gl ), std::placeholders::_1 ),
				  std::bind( &OpenGl::IsProgram, std::ref( p_gl ), std::placeholders::_1 )
				)
	{
		CreateObject( eSHADER_TYPE_VERTEX );
		CreateObject( eSHADER_TYPE_PIXEL );
	}

	GlShaderProgram::~GlShaderProgram()
	{
	}

	void GlShaderProgram::Cleanup()
	{
		DoCleanup();
		ObjectType::Destroy();
	}

	bool GlShaderProgram::Initialise()
	{
		bool l_return = true;

		if ( m_status != ePROGRAM_STATUS_LINKED )
		{
			ObjectType::Create();
			l_return = DoInitialise();
		}

		return l_return;
	}

	bool GlShaderProgram::Link()
	{
		bool l_return = false;
		int l_iLinked = 0;
		ENSURE( GetGlName() != eGL_INVALID_INDEX );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Programs attached : " ) << uint32_t( m_activeShaders.size() ) );
		l_return &= GetOpenGl().LinkProgram( GetGlName() );
		l_return &= GetOpenGl().GetProgramiv( GetGlName(), eGL_SHADER_STATUS_LINK, &l_iLinked );
		Logger::LogDebug( StringStream() << cuT( "GlShaderProgram::Link - Program link status : " ) << l_iLinked );
		m_linkerLog = DoRetrieveLinkerLog();

		if ( l_iLinked && m_linkerLog.find( cuT( "ERROR" ) ) == String::npos )
		{
			if ( !m_linkerLog.empty() )
			{
				Logger::LogWarning( cuT( "GlShaderProgram::Link - " ) + m_linkerLog );
			}

			l_return = DoLink();

			if ( l_return )
			{
				if ( GetOpenGl().HasExtension( ARB_program_interface_query, false ) )
				{
					int l_count = 0;
					GetOpenGl().GetProgramInterfaceInfos( GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, eGLSL_DATA_NAME_MAX_NAME_LENGTH, &l_count );
					std::vector< char > l_buffer( l_count );
					GetOpenGl().GetProgramInterfaceInfos( GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, eGLSL_DATA_NAME_ACTIVE_RESOURCES, &l_count );
					uint32_t l_offset = 0;

					for ( int i = 0; i < l_count; ++i )
					{
						int l_length = 0;
						int l_value;
						uint32_t l_prop = eGLSL_PROPERTY_TYPE;
						GetOpenGl().GetProgramResourceName( GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, i, l_buffer.size(), &l_length, l_buffer.data() );
						std::string l_name( l_buffer.data(), l_length );
						Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( l_name ) );
						GetOpenGl().GetProgramResourceInfos( GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, i, 1, &l_prop, 1, &l_length, &l_value );
						eELEMENT_TYPE l_elemType = GetOpenGl().Get( eGLSL_ATTRIBUTE_TYPE( l_value ) );
						string::to_lower_case( l_name );

						if ( l_elemType == eELEMENT_TYPE_4FLOATS && ( l_name.find( "vertex" ) != std::string::npos || l_name.find( "position" ) != std::string::npos ) )
						{
							l_elemType = eELEMENT_TYPE_3FLOATS;
						}

						m_vertexLayout.AddAttribute( l_buffer.data(), l_elemType, l_offset );
						l_offset += GetOffset( l_elemType );
					}
				}
				else
				{
					int l_count = 0;
					GetOpenGl().GetProgramiv( GetGlName(), eGL_SHADER_STATUS_ACTIVE_ATTRIBUTE_MAX_LENGTH, &l_count );
					std::vector< char > l_buffer( l_count );
					GetOpenGl().GetProgramiv( GetGlName(), eGL_SHADER_STATUS_ACTIVE_ATTRIBUTES, &l_count );
					uint32_t l_offset = 0;

					for ( int i = 0; i < l_count; ++i )
					{
						int l_arraySize = 0;
						uint32_t l_type = 0;
						int l_actualLength = 0;
						GetOpenGl().GetActiveAttrib( GetGlName(), i, l_buffer.size(), &l_actualLength, &l_arraySize, &l_type, l_buffer.data() );
						std::string l_name( l_buffer.data(), l_actualLength );
						Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( l_name ) );
						eELEMENT_TYPE l_elemType = GetOpenGl().Get( eGLSL_ATTRIBUTE_TYPE( l_type ) );
						string::to_lower_case( l_name );

						if ( l_elemType == eELEMENT_TYPE_4FLOATS && ( l_name.find( "vertex" ) != std::string::npos || l_name.find( "position" ) != std::string::npos ) )
						{
							l_elemType = eELEMENT_TYPE_3FLOATS;
						}

						m_vertexLayout.AddAttribute( l_buffer.data(), l_elemType, l_offset );
						l_offset += GetOffset( l_elemType );
					}
				}
			}
		}
		else
		{
			Logger::LogError( cuT( "GlShaderProgram::Link - " ) + m_linkerLog );
			m_status = ePROGRAM_STATUS_ERROR;
		}

		return l_return;
	}

	void GlShaderProgram::Bind( bool p_bindUbo )
	{
		if ( GetGlName() != eGL_INVALID_INDEX && m_status == ePROGRAM_STATUS_LINKED )
		{
			GetOpenGl().UseProgram( GetGlName() );
			DoBind( p_bindUbo );
		}
	}

	void GlShaderProgram::Unbind()
	{
		if ( GetGlName() != eGL_INVALID_INDEX && m_status == ePROGRAM_STATUS_LINKED )
		{
			uint32_t l_index = 0;

			for ( auto l_variableBuffer : m_listFrameVariableBuffers )
			{
				l_variableBuffer->Unbind( l_index++ );
			}

			DoUnbind();
			GetOpenGl().UseProgram( 0 );
		}
	}

	int GlShaderProgram::GetAttributeLocation( String const & p_name )const
	{
		int l_iReturn = eGL_INVALID_INDEX;

		if ( GetGlName() != eGL_INVALID_INDEX && GetOpenGl().IsProgram( GetGlName() ) )
		{
			l_iReturn = GetOpenGl().GetAttribLocation( GetGlName(), string::string_cast< char >( p_name ).c_str() );
		}

		return l_iReturn;
	}

	ShaderObjectBaseSPtr GlShaderProgram::DoCreateObject( eSHADER_TYPE p_type )
	{
		ShaderObjectBaseSPtr l_return = std::make_shared< GlShaderObject >( GetOpenGl(), this, p_type );
		return l_return;
	}

	std::shared_ptr< OneTextureFrameVariable > GlShaderProgram::DoCreateTextureVariable( int p_occurences )
	{
		return std::make_shared< GlOneFrameVariable< TextureBaseRPtr > >( GetOpenGl(), p_occurences, this );
	}

	String GlShaderProgram::DoRetrieveLinkerLog()
	{
		String l_log;

		if ( GetGlName() == eGL_INVALID_INDEX )
		{
			l_log = GetOpenGl().GetGlslErrorString( 2 );
		}
		else
		{
			int l_length = 0;
			GetOpenGl().GetProgramiv( GetGlName(), eGL_SHADER_STATUS_INFO_LOG_LENGTH, &l_length );

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
}
