#include "Mesh/GlGeometryBuffers.hpp"

#include "Common/OpenGl.hpp"
#include "Mesh/GlAttribute.hpp"
#include "Render/GlRenderSystem.hpp"
#include "Shader/GlShaderProgram.hpp"

#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>

using namespace castor3d;
using namespace castor;

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
		, m_glTopology{ p_gl.get( p_topology ) }
	{
	}

	GlGeometryBuffers::~GlGeometryBuffers()
	{
	}

	bool GlGeometryBuffers::draw( uint32_t p_size, uint32_t p_index )const
	{
		ObjectType::bind();
		glcheckTextureUnits();

		if ( m_indexBuffer )
		{
			getOpenGl().DrawElements( m_glTopology, int( p_size ), GlType::eUnsignedInt, BUFFER_OFFSET( p_index ) );
		}
		else
		{
			getOpenGl().DrawArrays( m_glTopology, int( p_index ), int( p_size ) );
		}

		ObjectType::unbind();
		return true;
	}

	bool GlGeometryBuffers::drawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const
	{
		ObjectType::bind();
		glcheckTextureUnits();

		if ( m_indexBuffer )
		{
			getOpenGl().DrawElementsInstanced( m_glTopology, int( p_size ), GlType::eUnsignedInt, BUFFER_OFFSET( p_index ), int( p_count ) );
		}
		else
		{
			getOpenGl().DrawArraysInstanced( m_glTopology, int( p_index ), int( p_size ), int( p_count ) );
		}

		ObjectType::unbind();
		return true;
	}

	bool GlGeometryBuffers::doInitialise()
	{
		if ( m_program.getStatus() != ProgramStatus::eLinked )
		{
			CASTOR_EXCEPTION( "Can't retrieve a program input layout from a non compiled shader." );
		}

		bool result = ObjectType::create();

		if ( result )
		{
			getOpenGl().BindVertexArray( getGlName() );

			for ( auto & buffer : m_buffers )
			{
				GlAttributePtrArray attributes;

				if ( doCreateAttributes( m_program.getLayout(), buffer.get().getDeclaration(), attributes ) )
				{
					buffer.get().bind();
					doBindAttributes( attributes );
					m_attributes.insert( std::end( m_attributes ), std::begin( attributes ), std::end( attributes ) );
				}
			}

			if ( m_indexBuffer )
			{
				m_indexBuffer->bind();
			}

			getOpenGl().BindVertexArray( 0 );
		}

		return result;
	}

	void GlGeometryBuffers::doCleanup()
	{
		m_attributes.clear();
		ObjectType::destroy();
	}

	BufferDeclaration::const_iterator GlGeometryBuffers::doFindElement( BufferDeclaration const & p_declaration, BufferElementDeclaration const & p_element )const
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

	GlAttributeBaseSPtr GlGeometryBuffers::doCreateAttribute( BufferElementDeclaration const & p_element, uint32_t p_offset, uint32_t p_divisor, BufferDeclaration const & p_declaration )
	{
		bool result = true;
		auto const & renderSystem = getOpenGl().getRenderSystem();
		GlAttributeBaseSPtr attribute;
		uint32_t stride = p_declaration.stride();

		switch ( p_element.m_dataType )
		{
		case ElementType::eFloat:
			attribute = std::make_shared< GlAttributeVec1r >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec2:
			attribute = std::make_shared< GlAttributeVec2r >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec3:
			attribute = std::make_shared< GlAttributeVec3r >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eVec4:
			attribute = std::make_shared< GlAttributeVec4r >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eColour:
			attribute = std::make_shared< GlAttributeVec1ui >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eInt:
			attribute = std::make_shared< GlAttributeVec1i >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec2:
			attribute = std::make_shared< GlAttributeVec2i >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec3:
			attribute = std::make_shared< GlAttributeVec3i >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eIVec4:
			attribute = std::make_shared< GlAttributeVec4i >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUInt:
			attribute = std::make_shared< GlAttributeVec1ui >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec2:
			attribute = std::make_shared< GlAttributeVec2ui >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec3:
			attribute = std::make_shared< GlAttributeVec3ui >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eUIVec4:
			attribute = std::make_shared< GlAttributeVec4ui >( getOpenGl(), m_program, stride, p_element.m_name, p_divisor );
			break;

		case ElementType::eMat2:
			attribute = std::make_shared< GlMatAttribute< real, 2, 2 > >( getOpenGl(), m_program, stride, p_element.m_name );
			break;

		case ElementType::eMat3:
			attribute = std::make_shared< GlMatAttribute< real, 3, 3 > >( getOpenGl(), m_program, stride, p_element.m_name );
			break;

		case ElementType::eMat4:
			attribute = std::make_shared< GlMatAttribute< real, 4, 4 > >( getOpenGl(), m_program, stride, p_element.m_name );
			break;

		default:
			assert( false && "Unsupported element type" );
			break;
		}

		if ( attribute )
		{
			attribute->setOffset( p_offset );
		}

		return attribute;
	}

	bool GlGeometryBuffers::doCreateAttributes( ProgramInputLayout const & p_layout, BufferDeclaration const & p_declaration, GlAttributePtrArray & p_attributes )
	{
		for ( auto & element : p_layout )
		{
			auto it = doFindElement( p_declaration, element );

			if ( it != p_declaration.end() )
			{
				auto attribute = doCreateAttribute( element, it->m_offset, it->m_divisor, p_declaration );

				if ( attribute )
				{
					p_attributes.push_back( attribute );
				}
			}
		}

		return !p_attributes.empty();
	}

	void GlGeometryBuffers::doBindAttributes( GlAttributePtrArray const & p_attributes )const
	{
		for ( auto const & attribute : p_attributes )
		{
			if ( attribute->getLocation() != GlInvalidIndex )
			{
				attribute->bind( false );
			}
		}
	}
}
