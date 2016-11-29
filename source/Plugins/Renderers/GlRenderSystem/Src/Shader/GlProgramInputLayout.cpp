#include "Shader/GlProgramInputLayout.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlFrameVariableBuffer.hpp"
#include "Shader/GlShaderProgram.hpp"
#include "Buffer/GlBuffer.hpp"

#include <Shader/ShaderStorageBuffer.hpp>

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

		DoListOther( p_program );
		return l_return;
	}

	void GlProgramInputLayout::Cleanup()
	{
	}

	namespace
	{
		template< typename FuncType >
		void GetProgramInterfaceInfos( OpenGl & p_gl, uint32_t p_name, GlslInterface p_interface, GlslProperty p_property, FuncType p_function )
		{
			int l_count = 0;
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eMaxNameLength, &l_count );
			std::vector< char > l_buffer( l_count );
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eActiveResources, &l_count );

			for ( int i = 0; i < l_count; ++i )
			{
				int l_length = 0;
				int l_value = 0;
				p_gl.GetProgramResourceName( p_name, p_interface, i, uint32_t( l_buffer.size() ), &l_length, l_buffer.data() );
				std::string l_name( l_buffer.data(), l_length );
				uint32_t l_prop = uint32_t( p_property );
				p_gl.GetProgramResourceInfos( p_name, p_interface, i, 1, &l_prop, 1, &l_length, &l_value );
				p_function( l_name, l_value );
			}
		}

		template< typename FuncType >
		void GetProgramBufferInfos( OpenGl & p_gl, uint32_t p_name, GlslInterface p_interface, GlslProperty p_property, FuncType p_function )
		{
			int l_count = 0;
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eMaxNameLength, &l_count );
			std::vector< char > l_buffer( l_count );
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eActiveResources, &l_count );

			for ( int i = 0; i < l_count; ++i )
			{
				int l_length = 0;
				int l_value = 0;
				p_gl.GetProgramResourceName( p_name, p_interface, i, uint32_t( l_buffer.size() ), &l_length, l_buffer.data() );
				std::string l_name( l_buffer.data(), l_length );
				uint32_t l_prop = uint32_t( p_property );
				p_gl.GetProgramResourceInfos( p_name, p_interface, i, 1, &l_prop, 1, &l_length, &l_value );
				int l_index = p_gl.GetProgramResourceIndex( p_name, p_interface, l_name.c_str() );
				p_function( l_name, l_value, l_index );
			}
		}

		template< typename FuncType >
		void GetUnnamedProgramInterfaceInfos( OpenGl & p_gl, uint32_t p_name, GlslInterface p_interface, GlslProperty p_property, FuncType p_function )
		{
			int l_count = 0;
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eActiveResources, &l_count );
			std::vector< int > l_values( l_count );
			std::vector< int > l_lengths( l_count );

			for ( int i = 0; i < l_count; ++i )
			{
				uint32_t l_prop = uint32_t( p_property );
				p_gl.GetProgramResourceInfos( p_name, p_interface, i, 1, &l_prop, 1, &l_lengths[i], &l_values[i] );
			}

			if ( l_count )
			{
				p_function( l_values );
			}
		}
	}

	std::multimap< int, BufferElementDeclaration > GlProgramInputLayout::DoListAttributes( ShaderProgram const & p_program )
	{
		std::multimap< int, BufferElementDeclaration > l_attributes;
		GlShaderProgram const & l_program = static_cast< GlShaderProgram const & >( p_program );

		if ( GetOpenGl().HasExtension( ARB_program_interface_query, false ) )
		{
			GetProgramInterfaceInfos(
				GetOpenGl(),
				l_program.GetGlName(),
				GlslInterface::eProgramInput,
				GlslProperty::eType,
				[this, &l_attributes, &l_program]( std::string p_name, int p_value )
				{
					Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( p_name ) );

					if ( p_name.find( cuT( "gl_" ) ) != 0 )
					{
						int l_loc = GetOpenGl().GetAttribLocation( l_program.GetGlName(), p_name.data() );
						ElementType l_elemType = GetOpenGl().Get( GlslAttributeType( p_value ) );
						string::to_lower_case( p_name );

						if ( l_elemType == ElementType::eVec4 && p_name == string::string_cast< char >( ShaderProgram::Position ) )
						{
							l_elemType = ElementType::eVec3;
						}

						l_attributes.insert( { l_loc, BufferElementDeclaration{ p_name, 0, l_elemType } } );
					}
				} );
		}
		else
		{
			int l_count = 0;
			GetOpenGl().GetProgramiv( l_program.GetGlName(), GlShaderStatus::eActiveAttributeMaxLength, &l_count );
			std::vector< char > l_buffer( l_count );
			GetOpenGl().GetProgramiv( l_program.GetGlName(), GlShaderStatus::eActiveAttributes, &l_count );

			for ( int i = 0; i < l_count; ++i )
			{
				int l_arraySize = 0;
				uint32_t l_type = 0;
				int l_actualLength = 0;
				GetOpenGl().GetActiveAttrib( l_program.GetGlName(), i, uint32_t( l_buffer.size() ), &l_actualLength, &l_arraySize, &l_type, l_buffer.data() );
				std::string l_name( l_buffer.data(), l_actualLength );
				Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( l_name ) );
				ElementType l_elemType = GetOpenGl().Get( GlslAttributeType( l_type ) );
				int l_loc = GetOpenGl().GetAttribLocation( l_program.GetGlName(), l_buffer.data() );
				string::to_lower_case( l_name );

				if ( l_elemType == ElementType::eVec4 && l_name == string::string_cast< char >( ShaderProgram::Position ) )
				{
					l_elemType = ElementType::eVec3;
				}

				l_attributes.insert( { l_loc, BufferElementDeclaration{ l_buffer.data(), 0, l_elemType } } );
			}
		}

		return l_attributes;
	}

	void GlProgramInputLayout::DoListOther( Castor3D::ShaderProgram const & p_program )
	{
		GlShaderProgram const & l_program = static_cast< GlShaderProgram const & >( p_program );

		if ( GetOpenGl().HasExtension( ARB_program_interface_query, false ) )
		{
			GetProgramBufferInfos(
				GetOpenGl(),
				l_program.GetGlName(),
				GlslInterface::eShaderStorageBlock,
				GlslProperty::eBufferBinding,
				[this, &l_program]( std::string p_name, int p_point, int p_index )
				{
					Logger::LogDebug( StringStream() << cuT( "   ShaderStorage block: " ) << string::string_cast< xchar >( p_name ) );
				} );

			GetProgramBufferInfos(
				GetOpenGl(),
				l_program.GetGlName(),
				GlslInterface::eUniformBlock,
				GlslProperty::eBufferBinding,
				[this, &l_program]( std::string p_name, int p_value, int p_index )
				{
					Logger::LogDebug( StringStream() << cuT( "   Uniform block: " ) << string::string_cast< xchar >( p_name ) );
				} );
		}
		else
		{
			int l_count = 0;
			GetOpenGl().GetProgramiv( l_program.GetGlName(), GlShaderStatus::eActiveUniforms, &l_count );

			for ( int i = 0; i < l_count; ++i )
			{
				int l_arraySize = 0;
				uint32_t l_type = 0;
				int l_actualLength = 0;
				std::vector< char > l_buffer( 256 );
				GetOpenGl().GetActiveUniform( l_program.GetGlName(), i, uint32_t( l_buffer.size() ), &l_actualLength, &l_arraySize, &l_type, l_buffer.data() );
				std::string l_name( l_buffer.data(), l_actualLength );
				Logger::LogDebug( StringStream() << cuT( "   Uniform: " ) << string::string_cast< xchar >( l_name ) );
			}
		}
	}
}
