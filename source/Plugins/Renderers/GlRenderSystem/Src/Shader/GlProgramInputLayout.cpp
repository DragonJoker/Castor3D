#include "Shader/GlProgramInputLayout.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"
#include "Buffer/GlBuffer.hpp"

#include <Shader/AtomicCounterBuffer.hpp>
#include <Shader/ShaderStorageBuffer.hpp>

using namespace castor;
using namespace castor3d;

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

	bool GlProgramInputLayout::initialise( ShaderProgram const & p_program )
	{
		auto attributes = doListAttributes( p_program );
		bool result = true;

		for ( auto & element : attributes )
		{
			doAddAttribute( element.second.m_name, element.second.m_dataType );
		}

		doListOther( p_program );
		return result;
	}

	void GlProgramInputLayout::cleanup()
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
		void getProgramBufferInfos( OpenGl & p_gl, uint32_t p_name, GlslInterface p_interface, GlslProperty p_property, FuncType p_function )
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
		void getUnnamedProgramInterfaceInfos( OpenGl & p_gl, uint32_t p_name, GlslInterface p_interface, GlslProperty p_property, FuncType p_function )
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

	std::multimap< int, BufferElementDeclaration > GlProgramInputLayout::doListAttributes( ShaderProgram const & p_program )
	{
		std::multimap< int, BufferElementDeclaration > attributes;
		GlShaderProgram const & program = static_cast< GlShaderProgram const & >( p_program );

		if ( getOpenGl().hasExtension( ARB_program_interface_query, false ) )
		{
			GetProgramInterfaceInfos(
				getOpenGl(),
				program.getGlName(),
				GlslInterface::eProgramInput,
				GlslProperty::eType,
				[this, &attributes, &program]( std::string p_name, int p_value )
				{
					Logger::logDebug( StringStream() << cuT( "   Attribute: " ) << string::stringCast< xchar >( p_name ) );

					if ( p_name.find( cuT( "gl_" ) ) != 0 )
					{
						int loc = getOpenGl().GetAttribLocation( program.getGlName(), p_name.data() );
						ElementType elemType = getOpenGl().get( GlslAttributeType( p_value ) );
						string::toLowerCase( p_name );

						if ( elemType == ElementType::eVec4 && p_name == string::stringCast< char >( ShaderProgram::Position ) )
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
			getOpenGl().GetProgramiv( program.getGlName(), GlShaderStatus::eActiveAttributeMaxLength, &count );
			std::vector< char > buffer( count );
			getOpenGl().GetProgramiv( program.getGlName(), GlShaderStatus::eActiveAttributes, &count );

			for ( int i = 0; i < count; ++i )
			{
				int arraySize = 0;
				uint32_t type = 0;
				int actualLength = 0;
				getOpenGl().GetActiveAttrib( program.getGlName(), i, uint32_t( buffer.size() ), &actualLength, &arraySize, &type, buffer.data() );
				std::string name( buffer.data(), actualLength );
				Logger::logDebug( StringStream() << cuT( "   Attribute: " ) << string::stringCast< xchar >( name ) );
				ElementType elemType = getOpenGl().get( GlslAttributeType( type ) );
				int loc = getOpenGl().GetAttribLocation( program.getGlName(), buffer.data() );
				string::toLowerCase( name );

				if ( elemType == ElementType::eVec4 && name == string::stringCast< char >( ShaderProgram::Position ) )
				{
					elemType = ElementType::eVec3;
				}

				attributes.insert( { loc, BufferElementDeclaration{ buffer.data(), 0, elemType } } );
			}
		}

		return attributes;
	}

	void GlProgramInputLayout::doListOther( castor3d::ShaderProgram const & p_program )
	{
		GlShaderProgram const & program = static_cast< GlShaderProgram const & >( p_program );

		if ( getOpenGl().hasExtension( ARB_program_interface_query, false ) )
		{
			getProgramBufferInfos(
				getOpenGl(),
				program.getGlName(),
				GlslInterface::eShaderStorageBlock,
				GlslProperty::eBufferBinding,
				[this, &program]( std::string p_name, int p_point, int p_index, int p_variables )
				{
					Logger::logDebug( StringStream() << cuT( "   ShaderStorage block: " ) << string::stringCast< xchar >( p_name )
						<< cuT( ", at point " ) << p_point
						<< cuT( ", and index " ) << p_index
						<< cuT( ", active variables " ) << p_variables );
				} );

			getUnnamedProgramInterfaceInfos(
				getOpenGl(),
				program.getGlName(),
				GlslInterface::eAtomicCounterBuffer,
				GlslProperty::eBufferBinding,
				[this, &program]( std::vector< int > const & p_value )
				{
					Logger::logDebug( StringStream() << cuT( "   Atomic counter buffer" ) );
				} );

			getProgramBufferInfos(
				getOpenGl(),
				program.getGlName(),
				GlslInterface::eUniformBlock,
				GlslProperty::eBufferBinding,
				[this, &program]( std::string p_name, int p_value, int p_index, int p_variables )
				{
					Logger::logDebug( StringStream() << cuT( "   Uniform block: " ) << string::stringCast< xchar >( p_name )
						<< cuT( ", at point " ) << p_value
						<< cuT( ", and index " ) << p_index
						<< cuT( ", active variables " ) << p_variables );
				} );
		}
		else
		{
			int count = 0;
			getOpenGl().GetProgramiv( program.getGlName(), GlShaderStatus::eActiveUniforms, &count );

			for ( int i = 0; i < count; ++i )
			{
				int arraySize = 0;
				uint32_t type = 0;
				int actualLength = 0;
				std::vector< char > buffer( 256 );
				getOpenGl().GetActiveUniform( program.getGlName(), i, uint32_t( buffer.size() ), &actualLength, &arraySize, &type, buffer.data() );
				std::string name( buffer.data(), actualLength );
				Logger::logDebug( StringStream() << cuT( "   Uniform: " ) << string::stringCast< xchar >( name ) );
			}
		}
	}
}
