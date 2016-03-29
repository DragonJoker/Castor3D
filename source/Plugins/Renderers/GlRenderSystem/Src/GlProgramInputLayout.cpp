#include "GlProgramInputLayout.hpp"

#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlProgramInputLayout::GlProgramInputLayout( OpenGl & p_gl, RenderSystem & p_renderSystem )
		: ProgramInputLayout( p_renderSystem )
		, Holder( p_gl )
	{
	}

	GlProgramInputLayout::~GlProgramInputLayout()
	{
	}

	bool GlProgramInputLayout::Initialise( ShaderProgram const & p_program )
	{
		auto l_attributes = DoListAttributes( p_program );
		bool l_return = true;

		for ( auto & l_element : l_attributes )
		{
			DoAddAttribute( l_element.second.m_name, l_element.second.m_dataType );
		}

		return l_return;
	}

	void GlProgramInputLayout::Cleanup()
	{
	}

	std::multimap< int, BufferElementDeclaration > GlProgramInputLayout::DoListAttributes( ShaderProgram const & p_program )
	{
		std::multimap< int, BufferElementDeclaration > l_attributes;
		GlShaderProgram const & l_program = static_cast< GlShaderProgram const & >( p_program );

		if ( GetOpenGl().HasExtension( ARB_program_interface_query, false ) )
		{
			int l_count = 0;
			GetOpenGl().GetProgramInterfaceInfos( l_program.GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, eGLSL_DATA_NAME_MAX_NAME_LENGTH, &l_count );
			std::vector< char > l_buffer( l_count );
			GetOpenGl().GetProgramInterfaceInfos( l_program.GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, eGLSL_DATA_NAME_ACTIVE_RESOURCES, &l_count );

			for ( int i = 0; i < l_count; ++i )
			{
				int l_length = 0;
				int l_value = 0;
				uint32_t l_prop = eGLSL_PROPERTY_TYPE;
				GetOpenGl().GetProgramResourceName( l_program.GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, i, uint32_t( l_buffer.size() ), &l_length, l_buffer.data() );
				std::string l_name( l_buffer.data(), l_length );
				Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( l_name ) );
				GetOpenGl().GetProgramResourceInfos( l_program.GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, i, 1, &l_prop, 1, &l_length, &l_value );
				int l_loc = GetOpenGl().GetAttribLocation( l_program.GetGlName(), l_buffer.data() );
				eELEMENT_TYPE l_elemType = GetOpenGl().Get( eGLSL_ATTRIBUTE_TYPE( l_value ) );
				string::to_lower_case( l_name );

				if ( l_elemType == eELEMENT_TYPE_4FLOATS && l_name == string::string_cast< char >( ShaderProgram::Position ) )
				{
					l_elemType = eELEMENT_TYPE_3FLOATS;
				}

				l_attributes.insert( { l_loc, BufferElementDeclaration{ l_buffer.data(), 0, l_elemType } } );
			}
		}
		else
		{
			int l_count = 0;
			GetOpenGl().GetProgramiv( l_program.GetGlName(), eGL_SHADER_STATUS_ACTIVE_ATTRIBUTE_MAX_LENGTH, &l_count );
			std::vector< char > l_buffer( l_count );
			GetOpenGl().GetProgramiv( l_program.GetGlName(), eGL_SHADER_STATUS_ACTIVE_ATTRIBUTES, &l_count );

			for ( int i = 0; i < l_count; ++i )
			{
				int l_arraySize = 0;
				uint32_t l_type = 0;
				int l_actualLength = 0;
				GetOpenGl().GetActiveAttrib( l_program.GetGlName(), i, uint32_t( l_buffer.size() ), &l_actualLength, &l_arraySize, &l_type, l_buffer.data() );
				std::string l_name( l_buffer.data(), l_actualLength );
				Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( l_name ) );
				eELEMENT_TYPE l_elemType = GetOpenGl().Get( eGLSL_ATTRIBUTE_TYPE( l_type ) );
				int l_loc = GetOpenGl().GetAttribLocation( l_program.GetGlName(), l_buffer.data() );
				string::to_lower_case( l_name );

				if ( l_elemType == eELEMENT_TYPE_4FLOATS && l_name == string::string_cast< char >( ShaderProgram::Position ) )
				{
					l_elemType = eELEMENT_TYPE_3FLOATS;
				}

				l_attributes.insert( { l_loc, BufferElementDeclaration{ l_buffer.data(), 0, l_elemType } } );
			}
		}

		return l_attributes;
	}
}
