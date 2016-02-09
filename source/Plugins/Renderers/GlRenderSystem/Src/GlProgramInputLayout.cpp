#include "GlProgramInputLayout.hpp"

#include "GlAttribute.hpp"
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
		m_program = p_program.shared_from_this();
		auto l_attributes = DoListAttributes( p_program );
		bool l_return = true;
		uint32_t l_offset = 0;

		for ( auto & l_element : l_attributes )
		{
			if ( l_return )
			{
				l_element.second.m_offset = l_offset;
				l_offset += Castor3D::GetSize( l_element.second.m_dataType );
				l_return = DoCreateAttribute( l_element.second );
			}
		}

		return l_return;
	}

	void GlProgramInputLayout::Cleanup()
	{
		for ( auto && l_attribute : m_attributes )
		{
			l_attribute->Cleanup();
		}
	}

	bool GlProgramInputLayout::Bind()const
	{
		bool l_return = true;

		for ( auto && l_attribute : m_attributes )
		{
			if ( l_return && l_attribute->GetLocation() != eGL_INVALID_INDEX )
			{
				l_return = l_attribute->Bind( false );
			}
		}

		return l_return;
	}

	void GlProgramInputLayout::Unbind()const
	{
		for ( auto && l_attribute : m_attributes )
		{
			if ( l_attribute->GetLocation() != eGL_INVALID_INDEX )
			{
				l_attribute->Unbind();
			}
		}
	}

	void GlProgramInputLayout::SetStride( uint32_t p_stride )
	{
		for ( auto && l_attribute : m_attributes )
		{
			l_attribute->SetStride( p_stride );
		}
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
				GetOpenGl().GetProgramResourceName( l_program.GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, i, l_buffer.size(), &l_length, l_buffer.data() );
				std::string l_name( l_buffer.data(), l_length );
				Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( l_name ) );
				GetOpenGl().GetProgramResourceInfos( l_program.GetGlName(), eGLSL_INTERFACE_PROGRAM_INPUT, i, 1, &l_prop, 1, &l_length, &l_value );
				int l_loc = GetOpenGl().GetAttribLocation( l_program.GetGlName(), l_buffer.data() );
				eELEMENT_TYPE l_elemType = GetOpenGl().Get( eGLSL_ATTRIBUTE_TYPE( l_value ) );
				string::to_lower_case( l_name );

				if ( l_elemType == eELEMENT_TYPE_4FLOATS && ( l_name.find( "vertex" ) != std::string::npos || l_name.find( "position" ) != std::string::npos ) )
				{
					l_elemType = eELEMENT_TYPE_3FLOATS;
				}

				l_attributes.insert( { l_loc, BufferElementDeclaration{ l_buffer.data(), l_elemType } } );
				DoAddAttribute( l_buffer.data(), l_elemType );
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
				GetOpenGl().GetActiveAttrib( l_program.GetGlName(), i, l_buffer.size(), &l_actualLength, &l_arraySize, &l_type, l_buffer.data() );
				std::string l_name( l_buffer.data(), l_actualLength );
				Logger::LogDebug( StringStream() << cuT( "   Attribute: " ) << string::string_cast< xchar >( l_name ) );
				eELEMENT_TYPE l_elemType = GetOpenGl().Get( eGLSL_ATTRIBUTE_TYPE( l_type ) );
				int l_loc = GetOpenGl().GetAttribLocation( l_program.GetGlName(), l_buffer.data() );
				string::to_lower_case( l_name );

				if ( l_elemType == eELEMENT_TYPE_4FLOATS && ( l_name.find( "vertex" ) != std::string::npos || l_name.find( "position" ) != std::string::npos ) )
				{
					l_elemType = eELEMENT_TYPE_3FLOATS;
				}

				l_attributes.insert( { l_loc, BufferElementDeclaration{ l_buffer.data(), l_elemType } } );
				DoAddAttribute( l_buffer.data(), l_elemType );
			}
		}

		return l_attributes;
	}

	bool GlProgramInputLayout::DoCreateAttribute( BufferElementDeclaration const & p_element )
	{
		bool l_return = true;
		auto l_renderSystem = static_cast< GlRenderSystem * >( GetRenderSystem() );
		GlAttributeBaseSPtr l_attribute;

		switch ( p_element.m_dataType )
		{
		case eELEMENT_TYPE_1FLOAT:
			l_attribute = std::make_shared< GlAttribute1r >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;

		case eELEMENT_TYPE_2FLOATS:
			l_attribute = std::make_shared< GlAttribute2r >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;

		case eELEMENT_TYPE_3FLOATS:
			l_attribute = std::make_shared< GlAttribute3r >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;

		case eELEMENT_TYPE_4FLOATS:
			l_attribute = std::make_shared< GlAttribute4r >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;

		case eELEMENT_TYPE_COLOUR:
			l_attribute = std::make_shared< GlAttribute1ui >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;

		case eELEMENT_TYPE_1INT:
			l_attribute = std::make_shared< GlAttribute1i >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;

		case eELEMENT_TYPE_2INTS:
			l_attribute = std::make_shared< GlAttribute2i >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;

		case eELEMENT_TYPE_3INTS:
			l_attribute = std::make_shared< GlAttribute3i >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;

		case eELEMENT_TYPE_4INTS:
			l_attribute = std::make_shared< GlAttribute4i >( GetOpenGl(), l_renderSystem, p_element.m_name );
			break;
		}

		l_attribute->SetOffset( p_element.m_offset );
		l_attribute->SetStride( GetStride() );
		l_attribute->SetShader( m_program.lock() );
		l_return = l_attribute->Initialise();

		if ( l_return )
		{
			m_attributes.push_back( l_attribute );
		}

		return l_return;
	}
}
