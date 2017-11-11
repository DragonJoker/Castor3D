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
	GlGeometryBuffers::GlGeometryBuffers( OpenGl & gl
		, Topology topology
		, ShaderProgram const & program )
		: GeometryBuffers( topology, program )
		, ObjectType( gl,
					  "GlVertexArrayObjects",
					  std::bind( &OpenGl::GenVertexArrays, std::ref( gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteVertexArrays, std::ref( gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsVertexArray, std::ref( gl ), std::placeholders::_1 ),
					  std::bind( &OpenGl::BindVertexArray, std::ref( gl ), std::placeholders::_1 )
					)
		, m_program{ program }
		, m_glTopology{ gl.get( topology ) }
		, m_glIndirectArraysBuffer{ static_cast< GlRenderSystem & >( *program.getRenderSystem() )
			, gl
			, GlBufferTarget::eIndirect }
		, m_glIndirectElementsBuffer{ static_cast< GlRenderSystem & >( *program.getRenderSystem() )
			, gl
			, GlBufferTarget::eIndirect }
	{
	}

	GlGeometryBuffers::~GlGeometryBuffers()
	{
	}

	bool GlGeometryBuffers::draw( uint32_t size, uint32_t index )const
	{
		ObjectType::bind();
		glcheckTextureUnits();

		if ( m_indexBuffer )
		{
			getOpenGl().DrawElements( m_glTopology
				, int( size )
				, GlType::eUnsignedInt
				, BUFFER_OFFSET( index * sizeof( uint32_t ) ) );
		}
		else
		{
			getOpenGl().DrawArrays( m_glTopology
				, int( 0u )
				, int( size ) );
		}

		ObjectType::unbind();
		return true;
	}

	bool GlGeometryBuffers::drawInstanced( uint32_t size
		, uint32_t index
		, uint32_t count )const
	{
		ObjectType::bind();
		glcheckTextureUnits();

		if ( m_indexBuffer )
		{
			getOpenGl().DrawElementsInstanced( m_glTopology
				, int( size )
				, GlType::eUnsignedInt
				, BUFFER_OFFSET( index * sizeof( uint32_t ) )
				, int( count ) );
		}
		else
		{
			getOpenGl().DrawArraysInstanced( m_glTopology
				, int( 0u )
				, int( size )
				, int( count ) );
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

				if ( doCreateAttributes( m_program.getLayout()
					, buffer.get().getDeclaration()
					, buffer.get().getOffset()
					, attributes ) )
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

	void GlGeometryBuffers::doSetTopology( Topology p_value )
	{
		m_glTopology = getOpenGl().get( p_value );
	}

	BufferDeclaration::const_iterator GlGeometryBuffers::doFindElement( BufferDeclaration const & declaration
		, BufferElementDeclaration const & element )const
	{
		// First try to find an attribute with matching name.
		auto result = std::find_if( declaration.begin()
			, declaration.end()
			, [&element]( BufferElementDeclaration const & elem )
			{
				return elem.m_name == element.m_name;
			} );

		if ( result == declaration.end() )
		{
			// We try to find an element with the same ElementUsage as asked.
			result = std::find_if( declaration.begin()
				, declaration.end()
				, [&element]( BufferElementDeclaration const & elem )
				{
					return elem.m_usages == element.m_usages;
				} );
		}

		if ( result == declaration.end() )
		{
			// We try to find an element with an ElementUsage approaching the one asked.
			result = std::find_if( declaration.begin()
				, declaration.end()
				, [&element]( BufferElementDeclaration const & elem )
				{
					return ( elem.m_usages & element.m_usages ) != 0;
				} );
		}

		return result;
	}

	GlAttributeBaseSPtr GlGeometryBuffers::doCreateAttribute( BufferElementDeclaration const & element
		, uint32_t offset
		, uint32_t divisor
		, BufferDeclaration const & declaration )
	{
		bool result = true;
		auto const & renderSystem = getOpenGl().getRenderSystem();
		GlAttributeBaseSPtr attribute;
		uint32_t stride = declaration.stride();

		switch ( element.m_dataType )
		{
		case ElementType::eFloat:
			attribute = std::make_shared< GlAttributeVec1r >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eVec2:
			attribute = std::make_shared< GlAttributeVec2r >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eVec3:
			attribute = std::make_shared< GlAttributeVec3r >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eVec4:
			attribute = std::make_shared< GlAttributeVec4r >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eColour:
			attribute = std::make_shared< GlAttributeVec1ui >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eInt:
			attribute = std::make_shared< GlAttributeVec1i >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eIVec2:
			attribute = std::make_shared< GlAttributeVec2i >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eIVec3:
			attribute = std::make_shared< GlAttributeVec3i >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eIVec4:
			attribute = std::make_shared< GlAttributeVec4i >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eUInt:
			attribute = std::make_shared< GlAttributeVec1ui >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eUIVec2:
			attribute = std::make_shared< GlAttributeVec2ui >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eUIVec3:
			attribute = std::make_shared< GlAttributeVec3ui >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eUIVec4:
			attribute = std::make_shared< GlAttributeVec4ui >( getOpenGl(), m_program, stride, element.m_name, divisor );
			break;

		case ElementType::eMat2:
			attribute = std::make_shared< GlMatAttribute< real, 2, 2 > >( getOpenGl(), m_program, stride, element.m_name );
			break;

		case ElementType::eMat3:
			attribute = std::make_shared< GlMatAttribute< real, 3, 3 > >( getOpenGl(), m_program, stride, element.m_name );
			break;

		case ElementType::eMat4:
			attribute = std::make_shared< GlMatAttribute< real, 4, 4 > >( getOpenGl(), m_program, stride, element.m_name );
			break;

		default:
			assert( false && "Unsupported element type" );
			break;
		}

		if ( attribute )
		{
			attribute->setOffset( offset );
		}

		return attribute;
	}

	bool GlGeometryBuffers::doCreateAttributes( ProgramInputLayout const & layout
		, BufferDeclaration const & declaration
		, uint32_t offset
		, GlAttributePtrArray & attributes )
	{
		for ( auto & element : layout )
		{
			auto it = doFindElement( declaration, element );

			if ( it != declaration.end() )
			{
				auto attribute = doCreateAttribute( element
					, offset + it->m_offset
					, it->m_divisor
					, declaration );

				if ( attribute )
				{
					attributes.push_back( attribute );
				}
			}
		}

		return !attributes.empty();
	}

	void GlGeometryBuffers::doBindAttributes( GlAttributePtrArray const & attributes )const
	{
		for ( auto const & attribute : attributes )
		{
			if ( attribute->getLocation() != GlInvalidIndex )
			{
				attribute->bind( false );
			}
		}
	}

	void GlGeometryBuffers::doDrawElementsIndirect( uint32_t size
		, uint32_t index
		, uint32_t count )const
	{
		m_elementsCommand.baseInstance = 0u;
		m_elementsCommand.count = count;
		m_elementsCommand.firstIndex = index;
		m_elementsCommand.baseVertex = 0u;
		m_elementsCommand.primCount = size;
		m_glIndirectElementsBuffer.upload( 0u
			, sizeof( m_elementsCommand )
			, reinterpret_cast< uint8_t * >( &m_elementsCommand ) );
		m_glIndirectElementsBuffer.bind();
		getOpenGl().DrawElementsIndirect( m_glTopology, GlType::eUnsignedInt, BUFFER_OFFSET( 0u ) );
		m_glIndirectElementsBuffer.unbind();
	}

	void GlGeometryBuffers::doDrawArraysIndirect( uint32_t size
		, uint32_t index
		, uint32_t count )const
	{
		m_arraysCommand.baseInstance = 0u;
		m_arraysCommand.count = count;
		m_arraysCommand.first = index;
		m_arraysCommand.primCount = size;
		m_glIndirectArraysBuffer.upload( 0u
			, sizeof( m_arraysCommand )
			, reinterpret_cast< uint8_t * >( &m_arraysCommand ) );
		m_glIndirectArraysBuffer.bind();
		getOpenGl().DrawArraysIndirect( m_glTopology, BUFFER_OFFSET( 0u ) );
		m_glIndirectArraysBuffer.unbind();
	}
}
