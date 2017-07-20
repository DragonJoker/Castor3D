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

		bool result = ObjectType::Create();

		if ( result )
		{
			GetOpenGl().BindVertexArray( GetGlName() );

			for ( auto & buffer : m_buffers )
			{
				GlAttributePtrArray attributes;

				if ( DoCreateAttributes( m_program.GetLayout(), buffer.get().GetDeclaration(), attributes ) )
				{
					buffer.get().Bind();
					DoBindAttributes( attributes );
					m_attributes.insert( std::end( m_attributes ), std::begin( attributes ), std::end( attributes ) );
				}
			}

			if ( m_indexBuffer )
			{
				m_indexBuffer->Bind();
			}

			GetOpenGl().BindVertexArray( 0 );
		}

		return result;
	}

	void GlGeometryBuffers::DoCleanup()
	{
		m_attributes.clear();
		ObjectType::Destroy();
	}

	BufferDeclaration::const_iterator GlGeometryBuffers::DoFindElement( BufferDeclaration const & p_declaration, BufferElementDeclaration const & p_element )const
	{
		// First try to find an attribute with matching name.
		BufferDeclaration::const_iterator result = std::find_if( p_declaration.begin(), p_declaration.end(), [&p_element]( BufferElementDeclaration const & element )
		{
			return element.m_name == p_element.m_name;
		} );

		if ( result == p_declaration.end() )
		{
			// We try to find an element with the same ElementUsage as asked.
			BufferDeclaration::const_iterator result = std::find_if( p_declaration.begin(), p_declaration.end(), [&p_element]( BufferElementDeclaration const & element )
			{
				return element.m_usages == p_element.m_usages;
			} );
		}

		if ( result == p_declaration.end() )
		{
			// We try to find an element with an ElementUsage approaching the one asked.
			BufferDeclaration::const_iterator result = std::find_if( p_declaration.begin(), p_declaration.end(), [&p_element]( BufferElementDeclaration const & element )
			{
				return ( element.m_usages & p_element.m_usages ) != 0;
			} );
		}

		return result;
	}

	GlAttributeBaseSPtr GlGeometryBuffers::DoCreateAttribute( BufferElementDeclaration const & p_element, uint32_t p_offset, uint32_t p_divisor, BufferDeclaration const & p_declaration )
	{
		bool result = true;
		auto const & renderSystem = GetOpenGl().GetRenderSystem();
		GlAttributeBaseSPtr attribute;
		uint32_t stride = p_declaration.stride();

		switch ( p_element.m_dataType )
		{
		case ElementType::eFloat:
			attribute = std::make_shared< GlAttributeVec1r >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec2:
			attribute = std::make_shared< GlAttributeVec2r >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec3:
			attribute = std::make_shared< GlAttributeVec3r >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec4:
			attribute = std::make_shared< GlAttributeVec4r >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eColour:
			attribute = std::make_shared< GlAttributeVec1ui >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eInt:
			attribute = std::make_shared< GlAttributeVec1i >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec2:
			attribute = std::make_shared< GlAttributeVec2i >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec3:
			attribute = std::make_shared< GlAttributeVec3i >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec4:
			attribute = std::make_shared< GlAttributeVec4i >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUInt:
			attribute = std::make_shared< GlAttributeVec1ui >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec2:
			attribute = std::make_shared< GlAttributeVec2ui >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec3:
			attribute = std::make_shared< GlAttributeVec3ui >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec4:
			attribute = std::make_shared< GlAttributeVec4ui >( GetOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eMat2:
			attribute = std::make_shared< GlMatAttribute< real, 2, 2 > >( GetOpenGl(), m_program, stride, p_element.m_name );
			break;

		case ElementType::eMat3:
			attribute = std::make_shared< GlMatAttribute< real, 3, 3 > >( GetOpenGl(), m_program, stride, p_element.m_name );
			break;

		case ElementType::eMat4:
			attribute = std::make_shared< GlMatAttribute< real, 4, 4 > >( GetOpenGl(), m_program, stride, p_element.m_name );
			break;

		default:
			assert( false && "Unsupported element type" );
			break;
		}

		if ( attribute )
		{
			attribute->SetOffset( p_offset );
		}

		return attribute;
	}

	bool GlGeometryBuffers::DoCreateAttributes( ProgramInputLayout const & p_layout, BufferDeclaration const & p_declaration, GlAttributePtrArray & p_attributes )
	{
		for ( auto & element : p_layout )
		{
			auto it = DoFindElement( p_declaration, element );

			if ( it != p_declaration.end() )
			{
				auto attribute = DoCreateAttribute( element, it->m_offset, it->m_divisor, p_declaration );

				if ( attribute )
				{
					p_attributes.push_back( attribute );
				}
			}
		}

		return !p_attributes.empty();
	}

	void GlGeometryBuffers::DoBindAttributes( GlAttributePtrArray const & p_attributes )const
	{
		for ( auto const & attribute : p_attributes )
		{
			if ( attribute->GetLocation() != GlInvalidIndex )
			{
				attribute->Bind( false );
			}
		}
	}
}
