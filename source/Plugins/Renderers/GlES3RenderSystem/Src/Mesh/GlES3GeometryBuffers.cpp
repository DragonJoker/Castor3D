#include "Mesh/GlES3GeometryBuffers.hpp"

#include "Common/OpenGlES3.hpp"
#include "Mesh/GlES3Attribute.hpp"
#include "Render/GlES3RenderSystem.hpp"
#include "Shader/GlES3ShaderProgram.hpp"

#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3GeometryBuffers::GlES3GeometryBuffers( OpenGlES3 & p_gl, Topology p_topology, ShaderProgram const & p_program )
		: GeometryBuffers( p_topology, p_program )
		, ObjectType( p_gl,
					  "GlES3VertexArrayObjects",
					  std::bind( &OpenGlES3::GenVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::DeleteVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGlES3::IsVertexArray, std::ref( p_gl ), std::placeholders::_1 ),
					  std::bind( &OpenGlES3::BindVertexArray, std::ref( p_gl ), std::placeholders::_1 )
					)
		, m_program{ p_program }
		, m_glTopology{ p_gl.Get( p_topology ) }
	{
	}

	GlES3GeometryBuffers::~GlES3GeometryBuffers()
	{
	}

	bool GlES3GeometryBuffers::Draw( uint32_t p_size, uint32_t p_index )const
	{
		ObjectType::Bind();
		glCheckTextureUnits();

		if ( m_indexBuffer )
		{
			GetOpenGlES3().DrawElements( m_glTopology, int( p_size ), GlES3Type::eUnsignedInt, BUFFER_OFFSET( p_index ) );
		}
		else
		{
			GetOpenGlES3().DrawArrays( m_glTopology, int( p_index ), int( p_size ) );
		}

		ObjectType::Unbind();
		return true;
	}

	bool GlES3GeometryBuffers::DrawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const
	{
		ObjectType::Bind();
		glCheckTextureUnits();

		if ( m_indexBuffer )
		{
			GetOpenGlES3().DrawElementsInstanced( m_glTopology, int( p_size ), GlES3Type::eUnsignedInt, BUFFER_OFFSET( p_index ), int( p_count ) );
		}
		else
		{
			GetOpenGlES3().DrawArraysInstanced( m_glTopology, int( p_index ), int( p_size ), int( p_count ) );
		}

		ObjectType::Unbind();
		return true;
	}

	bool GlES3GeometryBuffers::DoInitialise()
	{
		if ( m_program.GetStatus() != ProgramStatus::eLinked )
		{
			CASTOR_EXCEPTION( "Can't retrieve a program input layout from a non compiled shader." );
		}

		bool l_return = ObjectType::Create();

		if ( l_return )
		{
			GetOpenGlES3().BindVertexArray( GetGlES3Name() );

			for ( auto & l_buffer : m_buffers )
			{
				GlES3AttributePtrArray l_attributes;

				if ( DoCreateAttributes( m_program.GetLayout(), l_buffer.get().GetDeclaration(), l_attributes ) )
				{
					l_buffer.get().Bind();
					DoBindAttributes( l_attributes );
					m_attributes.insert( std::end( m_attributes ), std::begin( l_attributes ), std::end( l_attributes ) );
				}
			}

			if ( m_indexBuffer )
			{
				m_indexBuffer->Bind();
			}

			GetOpenGlES3().BindVertexArray( 0 );
		}

		return l_return;
	}

	void GlES3GeometryBuffers::DoCleanup()
	{
		m_attributes.clear();
		ObjectType::Destroy();
	}

	BufferDeclaration::const_iterator GlES3GeometryBuffers::DoFindElement( BufferDeclaration const & p_declaration, BufferElementDeclaration const & p_element )const
	{
		// First try to find an attribute with matching name.
		BufferDeclaration::const_iterator l_return = std::find_if( p_declaration.begin(), p_declaration.end(), [&p_element]( BufferElementDeclaration const & l_element )
		{
			return l_element.m_name == p_element.m_name;
		} );

		if ( l_return == p_declaration.end() )
		{
			// We try to find an element with the same ElementUsage as asked.
			BufferDeclaration::const_iterator l_return = std::find_if( p_declaration.begin(), p_declaration.end(), [&p_element]( BufferElementDeclaration const & l_element )
			{
				return l_element.m_usages == p_element.m_usages;
			} );
		}

		if ( l_return == p_declaration.end() )
		{
			// We try to find an element with an ElementUsage approaching the one asked.
			BufferDeclaration::const_iterator l_return = std::find_if( p_declaration.begin(), p_declaration.end(), [&p_element]( BufferElementDeclaration const & l_element )
			{
				return ( l_element.m_usages & p_element.m_usages ) != 0;
			} );
		}

		return l_return;
	}

	GlES3AttributeBaseSPtr GlES3GeometryBuffers::DoCreateAttribute( BufferElementDeclaration const & p_element, uint32_t p_offset, uint32_t p_divisor, BufferDeclaration const & p_declaration )
	{
		bool l_return = true;
		auto const & l_renderSystem = GetOpenGlES3().GetRenderSystem();
		GlES3AttributeBaseSPtr l_attribute;
		uint32_t l_stride = p_declaration.stride();

		switch ( p_element.m_dataType )
		{
		case ElementType::eFloat:
			l_attribute = std::make_shared< GlES3AttributeVec1r >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec2:
			l_attribute = std::make_shared< GlES3AttributeVec2r >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec3:
			l_attribute = std::make_shared< GlES3AttributeVec3r >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec4:
			l_attribute = std::make_shared< GlES3AttributeVec4r >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eColour:
			l_attribute = std::make_shared< GlES3AttributeVec1ui >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eInt:
			l_attribute = std::make_shared< GlES3AttributeVec1i >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec2:
			l_attribute = std::make_shared< GlES3AttributeVec2i >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec3:
			l_attribute = std::make_shared< GlES3AttributeVec3i >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec4:
			l_attribute = std::make_shared< GlES3AttributeVec4i >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUInt:
			l_attribute = std::make_shared< GlES3AttributeVec1ui >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec2:
			l_attribute = std::make_shared< GlES3AttributeVec2ui >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec3:
			l_attribute = std::make_shared< GlES3AttributeVec3ui >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec4:
			l_attribute = std::make_shared< GlES3AttributeVec4ui >( GetOpenGlES3(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eMat2:
			l_attribute = std::make_shared< GlES3MatAttribute< real, 2, 2 > >( GetOpenGlES3(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::eMat3:
			l_attribute = std::make_shared< GlES3MatAttribute< real, 3, 3 > >( GetOpenGlES3(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::eMat4:
			l_attribute = std::make_shared< GlES3MatAttribute< real, 4, 4 > >( GetOpenGlES3(), m_program, l_stride, p_element.m_name );
			break;

		default:
			assert( false && "Unsupported element type" );
			break;
		}

		if ( l_attribute )
		{
			l_attribute->SetOffset( p_offset );
		}

		return l_attribute;
	}

	bool GlES3GeometryBuffers::DoCreateAttributes( ProgramInputLayout const & p_layout, BufferDeclaration const & p_declaration, GlES3AttributePtrArray & p_attributes )
	{
		for ( auto & l_element : p_layout )
		{
			auto l_it = DoFindElement( p_declaration, l_element );

			if ( l_it != p_declaration.end() )
			{
				auto l_attribute = DoCreateAttribute( l_element, l_it->m_offset, l_it->m_divisor, p_declaration );

				if ( l_attribute )
				{
					p_attributes.push_back( l_attribute );
				}
			}
		}

		return !p_attributes.empty();
	}

	void GlES3GeometryBuffers::DoBindAttributes( GlES3AttributePtrArray const & p_attributes )const
	{
		for ( auto const & l_attribute : p_attributes )
		{
			if ( l_attribute->GetLocation() != GlES3InvalidIndex )
			{
				l_attribute->Bind( false );
			}
		}
	}
}
