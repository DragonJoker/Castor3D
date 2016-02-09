#include "GlGeometryBuffers.hpp"

#include "GlAttribute.hpp"
#include "GlIndexBufferObject.hpp"
#include "GlMatrixBufferObject.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "GlVertexBufferObject.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlGeometryBuffers::GlGeometryBuffers( OpenGl & p_gl, eTOPOLOGY p_topology, ShaderProgram const & p_program, VertexBuffer * p_vtx, IndexBuffer * p_idx, VertexBuffer * p_bones, MatrixBuffer * p_inst )
		: Castor3D::GeometryBuffers( p_topology, p_program, p_vtx, p_idx, p_bones, p_inst )
		, ObjectType( p_gl,
					  "GlVertexArrayObjects",
					  std::bind( &OpenGl::GenVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsVertexArray, std::ref( p_gl ), std::placeholders::_1 ),
					  std::bind( &OpenGl::BindVertexArray, std::ref( p_gl ), std::placeholders::_1 )
					  )
		, m_program( p_program )
	{
		bool l_return = ObjectType::Create();

		if ( l_return )
		{
			l_return = GetOpenGl().BindVertexArray( GetGlName() );
		}

		if ( l_return )
		{
			if ( DoCreateAttributes( p_program.GetLayout(), p_vtx->GetDeclaration(), m_vertexAttributes ) )
			{
				m_vertexBuffer->Bind();
				DoBindAttributes( m_vertexAttributes );
			}

			if ( m_indexBuffer )
			{
				m_indexBuffer->Bind();
			}

			if ( m_bonesBuffer )
			{
				if ( DoCreateAttributes( p_program.GetLayout(), p_bones->GetDeclaration(), m_bonesAttributes ) )
				{
					m_bonesBuffer->Bind();
					DoBindAttributes( m_bonesAttributes );
				}
			}

			if ( m_matrixBuffer )
			{
				m_matrixAttribute = std::make_shared< GlAttribute< real, 16 > >( p_gl, p_program, ShaderProgram::Transform );
				m_matrixBuffer->Bind( true );
				uint32_t l_location = m_matrixAttribute->GetLocation();

				if ( l_location != eGL_INVALID_INDEX )
				{
					for ( int i = 0; i < 4 && l_return; ++i )
					{
						l_return = GetOpenGl().EnableVertexAttribArray( l_location + i );
						l_return &= GetOpenGl().VertexAttribPointer( l_location + i, 4, eGL_TYPE_REAL, false, 16 * sizeof( real ), BUFFER_OFFSET( i * 4 * sizeof( real ) ) );
						l_return &= GetOpenGl().VertexAttribDivisor( l_location + i, 1 );
					}
				}
			}

			GetOpenGl().BindVertexArray( 0 );
		}
	}

	GlGeometryBuffers::~GlGeometryBuffers()
	{
		m_vertexAttributes.clear();
		m_matrixAttribute.reset();
		m_bonesAttributes.clear();
		ObjectType::Destroy();
	}

	bool GlGeometryBuffers::Draw( ShaderProgram const & p_program, uint32_t p_uiSize, uint32_t p_index )
	{
		eGL_PRIMITIVE l_eMode = GetOpenGl().Get( m_topology );

		if ( p_program.HasObject( eSHADER_TYPE_HULL ) )
		{
			l_eMode = eGL_PRIMITIVE_PATCHES;
			GetOpenGl().PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
		}

		if ( ObjectType::Bind() )
		{
			if ( m_indexBuffer )
			{
				GetOpenGl().DrawElements( l_eMode, int( p_uiSize ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_index ) );
			}
			else
			{
				GetOpenGl().DrawArrays( l_eMode, int( p_index ), int( p_uiSize ) );
			}

			ObjectType::Unbind();
		}

		return true;
	}

	bool GlGeometryBuffers::DrawInstanced( ShaderProgram const & p_program, uint32_t p_uiSize, uint32_t p_index, uint32_t p_count )
	{
		eGL_PRIMITIVE l_eMode = GetOpenGl().Get( m_topology );

		if ( p_program.HasObject( eSHADER_TYPE_HULL ) )
		{
			l_eMode = eGL_PRIMITIVE_PATCHES;
			GetOpenGl().PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
		}

		if ( m_matrixBuffer )
		{
			m_matrixBuffer->GetGpuBuffer()->Fill( m_matrixBuffer->data(), m_matrixBuffer->GetSize(), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}

		if ( ObjectType::Bind() )
		{
			if ( m_indexBuffer )
			{
				GetOpenGl().DrawElementsInstanced( l_eMode, int( p_uiSize ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_index ), int( p_count ) );
			}
			else
			{
				GetOpenGl().DrawArraysInstanced( l_eMode, int( p_index ), int( p_uiSize ), int( p_count ) );
			}

			ObjectType::Unbind();
		}

		return true;
	}

	BufferDeclaration::const_iterator GlGeometryBuffers::DoFindElement( BufferDeclaration const & p_declaration, BufferElementDeclaration const & p_element )const
	{
		// First try to find an attribute with matching name.
		BufferDeclaration::const_iterator l_return = std::find_if( p_declaration.begin(), p_declaration.end(), [&p_element]( BufferElementDeclaration const & l_element )
		{
			return l_element.m_name == p_element.m_name;
		} );

		// If not found, try to find it using the type
		if ( l_return == p_declaration.end() )
		{
			// TODO: Try to find the element using the type, and another thing not defined yet, that would restrain the lookup.
		}

		return l_return;
	}

	GlAttributeBaseSPtr GlGeometryBuffers::DoCreateAttribute( BufferElementDeclaration const & p_element, uint32_t p_offset, uint32_t p_stride )
	{
		bool l_return = true;
		auto const & l_renderSystem = GetOpenGl().GetRenderSystem();
		GlAttributeBaseSPtr l_attribute;

		switch ( p_element.m_dataType )
		{
		case eELEMENT_TYPE_1FLOAT:
			l_attribute = std::make_shared< GlAttribute1r >( GetOpenGl(), m_program, p_element.m_name );
			break;

		case eELEMENT_TYPE_2FLOATS:
			l_attribute = std::make_shared< GlAttribute2r >( GetOpenGl(), m_program, p_element.m_name );
			break;

		case eELEMENT_TYPE_3FLOATS:
			l_attribute = std::make_shared< GlAttribute3r >( GetOpenGl(), m_program, p_element.m_name );
			break;

		case eELEMENT_TYPE_4FLOATS:
			l_attribute = std::make_shared< GlAttribute4r >( GetOpenGl(), m_program, p_element.m_name );
			break;

		case eELEMENT_TYPE_COLOUR:
			l_attribute = std::make_shared< GlAttribute1ui >( GetOpenGl(), m_program, p_element.m_name );
			break;

		case eELEMENT_TYPE_1INT:
			l_attribute = std::make_shared< GlAttribute1i >( GetOpenGl(), m_program, p_element.m_name );
			break;

		case eELEMENT_TYPE_2INTS:
			l_attribute = std::make_shared< GlAttribute2i >( GetOpenGl(), m_program, p_element.m_name );
			break;

		case eELEMENT_TYPE_3INTS:
			l_attribute = std::make_shared< GlAttribute3i >( GetOpenGl(), m_program, p_element.m_name );
			break;

		case eELEMENT_TYPE_4INTS:
			l_attribute = std::make_shared< GlAttribute4i >( GetOpenGl(), m_program, p_element.m_name );
			break;

		default:
			assert( false && "Unsupported element type" );
			break;
		}

		if ( l_attribute )
		{
			l_attribute->SetOffset( p_offset );
			l_attribute->SetStride( p_stride );
		}

		return l_attribute;
	}

	bool GlGeometryBuffers::DoCreateAttributes( Castor3D::ProgramInputLayout const & p_layout, Castor3D::BufferDeclaration const & p_declaration, GlAttributePtrArray & p_attributes )
	{
		for ( auto & l_element : p_layout )
		{
			BufferDeclaration::const_iterator l_it = DoFindElement( p_declaration, l_element );

			if ( l_it != p_declaration.end() )
			{
				auto l_attribute = DoCreateAttribute( l_element, l_it->m_offset, p_declaration.GetStride() );

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
			if ( l_attribute->GetLocation() != eGL_INVALID_INDEX )
			{
				l_attribute->Bind( false );
			}
		}
	}
}
