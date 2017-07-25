#include "Shader/GlProgramInputLayout.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"
#include "Buffer/GlBuffer.hpp"

#include <Shader/AtomicCounterBuffer.hpp>
#include <Shader/ShaderStorageBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlProgramInputLayout::GlProgramInputLayout( OpenGl & p_gl, RenderSystem & renderSystem )
		: ProgramInputLayout( renderSystem )
		, Holder( p_gl )
	{
	}

	GlProgramInputLayout::~GlProgramInputLayout()
	{
	}

	bool GlProgramInputLayout::Initialise( ShaderProgram const & p_program )
	{
		auto attributes = DoListAttributes( p_program );
		bool result = true;

		for ( auto & element : attributes )
		{
			DoAddAttribute( element.second.m_name, element.second.m_dataType );
		}

		DoListOther( p_program );
		return result;
	}

	void GlProgramInputLayout::Cleanup()
	{
	}

	namespace
	{
		template< typename FuncType >
		void GetProgramInterfaceInfos( OpenGl & p_gl, uint32_t p_name, GlslInterface p_interface, GlslProperty p_property, FuncType p_function )
		{
			int count = 0;
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eMaxNameLength, &count );
			std::vector< char > buffer( count );
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eActiveResources, &count );

			for ( int i = 0; i < count; ++i )
			{
				int length = 0;
				int value = 0;
				p_gl.GetProgramResourceName( p_name, p_interface, i, uint32_t( buffer.size() ), &length, buffer.data() );
				std::string name( buffer.data(), length );
				uint32_t prop = uint32_t( p_property );
				p_gl.GetProgramResourceInfos( p_name, p_interface, i, 1, &prop, 1, &length, &value );
				p_function( name, value );
			}
		}

		template< typename FuncType >
		void GetProgramBufferInfos( OpenGl & p_gl, uint32_t p_name, GlslInterface p_interface, GlslProperty p_property, FuncType p_function )
		{
			int count = 0;
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eMaxNameLength, &count );
			std::vector< char > buffer( count );
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eActiveResources, &count );

			for ( int i = 0; i < count; ++i )
			{
				int length = 0;
				int value = 0;
				int variables = 0;
				p_gl.GetProgramResourceName( p_name, p_interface, i, uint32_t( buffer.size() ), &length, buffer.data() );
				std::string name( buffer.data(), length );
				uint32_t prop = uint32_t( p_property );
				p_gl.GetProgramResourceInfos( p_name, p_interface, i, 1, &prop, 1, &length, &value );
				int index = p_gl.GetProgramResourceIndex( p_name, p_interface, name.c_str() );
				p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eMaxNumActiveVariables, &variables );
				p_function( name, value, index, variables );

			}
		}

		template< typename FuncType >
		void GetUnnamedProgramInterfaceInfos( OpenGl & p_gl, uint32_t p_name, GlslInterface p_interface, GlslProperty p_property, FuncType p_function )
		{
			int count = 0;
			p_gl.GetProgramInterfaceInfos( p_name, p_interface, GlslDataName::eActiveResources, &count );
			std::vector< int > values( count );
			std::vector< int > lengths( count );

			for ( int i = 0; i < count; ++i )
			{
				uint32_t prop = uint32_t( p_property );
				p_gl.GetProgramResourceInfos( p_name, p_interface, i, 1, &prop, 1, &lengths[i], &values[i] );
			}

			if ( count )
			{
				p_function( values );
			}
		}
	}

	std::multimap< int, BufferElementDeclaration > GlProgramInputLayout::DoListAttributes( ShaderProgram const & p_program )
	{
		std::multimap< int, BufferElementDeclaration > attributes;
		GlShaderProgram const & program = static_cast< GlShaderProgram const & >( p_program );

		if ( GetOpenGl().HasExtension( ARB_program_interface_query, false ) )
		{
			GetProgramInterfaceInfos(
				GetOpenGl(),
				program.GetGlName(),
				GlslInterface::eProgramInput,
				GlslProperty::eType,
				[this, &attributes, &program]( std::string p_name, int p_value )
				{
					Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( p_name ) );

					if ( p_name.find( cuT( "gl_" ) ) != 0 )
					{
						int loc = GetOpenGl().GetAttribLocation( program.GetGlName(), p_name.data() );
						ElementType elemType = GetOpenGl().Get( GlslAttributeType( p_value ) );
						string::to_lower_case( p_name );

						if ( elemType == ElementType::eVec4 && p_name == string::string_cast< char >( ShaderProgram::Position ) )
						{
							elemType = ElementType::eVec3;
						}

						attributes.insert( { loc, BufferElementDeclaration{ p_name, 0, elemType } } );
					}
				} );
		}
		else
		{
			int count = 0;
			GetOpenGl().GetProgramiv( program.GetGlName(), GlShaderStatus::eActiveAttributeMaxLength, &count );
			std::vector< char > buffer( count );
			GetOpenGl().GetProgramiv( program.GetGlName(), GlShaderStatus::eActiveAttributes, &count );

			for ( int i = 0; i < count; ++i )
			{
				int arraySize = 0;
				uint32_t type = 0;
				int actualLength = 0;
				GetOpenGl().GetActiveAttrib( program.GetGlName(), i, uint32_t( buffer.size() ), &actualLength, &arraySize, &type, buffer.data() );
				std::string name( buffer.data(), actualLength );
				Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( name ) );
				ElementType elemType = GetOpenGl().Get( GlslAttributeType( type ) );
				int loc = GetOpenGl().GetAttribLocation( program.GetGlName(), buffer.data() );
				string::to_lower_case( name );

				if ( elemType == ElementType::eVec4 && name == string::string_cast< char >( ShaderProgram::Position ) )
				{
					elemType = ElementType::eVec3;
				}

				attributes.insert( { loc, BufferElementDeclaration{ buffer.data(), 0, elemType } } );
			}
		}

		return attributes;
	}

	void GlProgramInputLayout::DoListOther( Castor3D::ShaderProgram const & p_program )
	{
		GlShaderProgram const & program = static_cast< GlShaderProgram const & >( p_program );

		if ( GetOpenGl().HasExtension( ARB_program_interface_query, false ) )
		{
			GetProgramBufferInfos(
				GetOpenGl(),
				program.GetGlName(),
				GlslInterface::eShaderStorageBlock,
				GlslProperty::eBufferBinding,
				[this, &program]( std::string p_name, int p_point, int p_index, int p_variables )
				{
					Logger::LogDebug( StringStream() << cuT( "   ShaderStorage block: " ) << string::string_cast< xchar >( p_name )
						<< cuT( ", at point " ) << p_point
						<< cuT( ", and index " ) << p_index
						<< cuT( ", active variables " ) << p_variables );
				} );

			GetUnnamedProgramInterfaceInfos(
				GetOpenGl(),
				program.GetGlName(),
				GlslInterface::eAtomicCounterBuffer,
				GlslProperty::eBufferBinding,
				[this, &program]( std::vector< int > const & p_value )
				{
					Logger::LogDebug( StringStream() << cuT( "   Atomic counter buffer" ) );
				} );

			GetProgramBufferInfos(
				GetOpenGl(),
				program.GetGlName(),
				GlslInterface::eUniformBlock,
				GlslProperty::eBufferBinding,
				[this, &program]( std::string p_name, int p_value, int p_index, int p_variables )
				{
					Logger::LogDebug( StringStream() << cuT( "   Uniform block: " ) << string::string_cast< xchar >( p_name )
						<< cuT( ", at point " ) << p_value
						<< cuT( ", and index " ) << p_index
						<< cuT( ", active variables " ) << p_variables );
				} );
		}
		else
		{
			int count = 0;
			GetOpenGl().GetProgramiv( program.GetGlName(), GlShaderStatus::eActiveUniforms, &count );

			for ( int i = 0; i < count; ++i )
			{
				int arraySize = 0;
				uint32_t type = 0;
				int actualLength = 0;
				std::vector< char > buffer( 256 );
				GetOpenGl().GetActiveUniform( program.GetGlName(), i, uint32_t( buffer.size() ), &actualLength, &arraySize, &type, buffer.data() );
				std::string name( buffer.data(), actualLength );
				Logger::LogDebug( StringStream() << cuT( "   Uniform: " ) << string::string_cast< xchar >( name ) );
			}
		}
	}
}
