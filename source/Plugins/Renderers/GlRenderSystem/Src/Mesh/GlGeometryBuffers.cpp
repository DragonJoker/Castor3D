#include "Mesh/GlGeometryBuffers.hpp"

#include "Common/OpenGl.hpp"
#include "Mesh/GlAttribute.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlGeometryBuffers::GlGeometryBuffers( OpenGl & p_gl, Topology p_topology, ShaderProgram const & p_program )
		: GeometryBuffers( p_topology, p_program )
		, ObjectType( p_gl,
					  "GlVertexArrayObjects",
					  std::bind( &OpenGl::GenVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsVertexArray, std::ref( p_gl ), std::placeholders::_1 ),
					  std::bind( &OpenGl::BindVertexArray, std::ref( p_gl ), std::placeholders::_1 )
					)
		, m_program{ p_program }
		, m_glTopology{ p_gl.Get( p_topology ) }
	{
	}

	GlGeometryBuffers::~GlGeometryBuffers()
	{
	}

	bool GlGeometryBuffers::Draw( uint32_t p_size, uint32_t p_index )const
	{
		ObjectType::Bind();
		glCheckTextureUnits();

		if ( m_indexBuffer )
		{
			GetOpenGl().DrawElements( m_glTopology, int( p_size ), GlType::eUnsignedInt, BUFFER_OFFSET( p_index ) );
		}
		else
		{
			GetOpenGl().DrawArrays( m_glTopology, int( p_index ), int( p_size ) );
		}

		ObjectType::Unbind();
		return true;
	}

	bool GlGeometryBuffers::DrawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const
	{
		ObjectType::Bind();
		glCheckTextureUnits();

		if ( m_indexBuffer )
		{
			GetOpenGl().DrawElementsInstanced( m_glTopology, int( p_size ), GlType::eUnsignedInt, BUFFER_OFFSET( p_index ), int( p_count ) );
		}
		else
		{
			GetOpenGl().DrawArraysInstanced( m_glTopology, int( p_index ), int( p_size ), int( p_count ) );
		}

		ObjectType::Unbind();
		return true;
	}

	bool GlGeometryBuffers::DoInitialise()
	{
		if ( m_program.GetStatus() != ProgramStatus::eLinked )
		{
			CASTOR_EXCEPTION( "Can't retrieve a program input layout from a non compiled shader." );
		}

		bool l_return = ObjectType::Create();

		if ( l_return )
		{
			GetOpenGl().BindVertexArray( GetGlName() );

			for ( auto & l_buffer : m_buffers )
			{
				GlAttributePtrArray l_attributes;

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

			GetOpenGl().BindVertexArray( 0 );
		}

		return l_return;
	}

	void GlGeometryBuffers::DoCleanup()
	{
		m_attributes.clear();
		ObjectType::Destroy();
	}

	BufferDeclaration::const_iterator GlGeometryBuffers::DoFindElement( BufferDeclaration const & p_declaration, BufferElementDeclaration const & p_element )const
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

	GlAttributeBaseSPtr GlGeometryBuffers::DoCreateAttribute( BufferElementDeclaration const & p_element, uint32_t p_offset, uint32_t p_divisor, BufferDeclaration const & p_declaration )
	{
		bool l_return = true;
		auto const & l_renderSystem = GetOpenGl().GetRenderSystem();
		GlAttributeBaseSPtr l_attribute;
		uint32_t l_stride = p_declaration.stride();

		switch ( p_element.m_dataType )
		{
		case ElementType::eFloat:
			l_attribute = std::make_shared< GlAttributeVec1r >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec2:
			l_attribute = std::make_shared< GlAttributeVec2r >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec3:
			l_attribute = std::make_shared< GlAttributeVec3r >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec4:
			l_attribute = std::make_shared< GlAttributeVec4r >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eColour:
			l_attribute = std::make_shared< GlAttributeVec1ui >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eInt:
			l_attribute = std::make_shared< GlAttributeVec1i >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec2:
			l_attribute = std::make_shared< GlAttributeVec2i >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec3:
			l_attribute = std::make_shared< GlAttributeVec3i >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec4:
			l_attribute = std::make_shared< GlAttributeVec4i >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUInt:
			l_attribute = std::make_shared< GlAttributeVec1ui >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec2:
			l_attribute = std::make_shared< GlAttributeVec2ui >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec3:
			l_attribute = std::make_shared< GlAttributeVec3ui >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec4:
			l_attribute = std::make_shared< GlAttributeVec4ui >( GetOpenGl(), m_program, l_stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eMat2:
			l_attribute = std::make_shared< GlMatAttribute< real, 2, 2 > >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::eMat3:
			l_attribute = std::make_shared< GlMatAttribute< real, 3, 3 > >( GetOpenGl(), m_program, l_stride, p_element.m_name );
			break;

		case ElementType::eMat4:
			l_attribute = std::make_shared< GlMatAttribute< real, 4, 4 > >( GetOpenGl(), m_program, l_stride, p_element.m_name );
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

	bool GlGeometryBuffers::DoCreateAttributes( ProgramInputLayout const & p_layout, BufferDeclaration const & p_declaration, GlAttributePtrArray & p_attributes )
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

	void GlGeometryBuffers::DoBindAttributes( GlAttributePtrArray const & p_attributes )const
	{
		for ( auto const & l_attribute : p_attributes )
		{
			if ( l_attribute->GetLocation() != GlInvalidIndex )
			{
				l_attribute->Bind( false );
			}
		}
	}
}
