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
		, m_program( p_program )
	{
	}

	GlGeometryBuffers::~GlGeometryBuffers()
	{
	}

	bool GlGeometryBuffers::Draw( uint32_t p_size, uint32_t p_index )const
	{
		eGL_PRIMITIVE l_eMode = GetOpenGl().Get( m_topology );

		if ( m_program.HasObject( ShaderType::Hull ) )
		{
			l_eMode = eGL_PRIMITIVE_PATCHES;
			GetOpenGl().PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
		}

		if ( ObjectType::Bind() )
		{
			if ( m_indexBuffer )
			{
				GetOpenGl().DrawElements( l_eMode, int( p_size ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_index ) );
			}
			else
			{
				GetOpenGl().DrawArrays( l_eMode, int( p_index ), int( p_size ) );
			}

			ObjectType::Unbind();
		}

		return true;
	}

	bool GlGeometryBuffers::DrawInstanced( uint32_t p_size, uint32_t p_index, uint32_t p_count )const
	{
		eGL_PRIMITIVE l_eMode = GetOpenGl().Get( m_topology );

		if ( m_program.HasObject( ShaderType::Hull ) )
		{
			l_eMode = eGL_PRIMITIVE_PATCHES;
			GetOpenGl().PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
		}

		if ( m_matrixBuffer )
		{
			m_matrixBuffer->GetGpuBuffer()->Fill( m_matrixBuffer->data(), m_matrixBuffer->GetSize(), BufferAccessType::Dynamic, BufferAccessNature::Draw );
		}

		if ( ObjectType::Bind() )
		{
			if ( m_indexBuffer )
			{
				GetOpenGl().DrawElementsInstanced( l_eMode, int( p_size ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_index ), int( p_count ) );
			}
			else
			{
				GetOpenGl().DrawArraysInstanced( l_eMode, int( p_index ), int( p_size ), int( p_count ) );
			}

			ObjectType::Unbind();
		}

		return true;
	}

	bool GlGeometryBuffers::DoInitialise()
	{
		if ( m_program.GetStatus() != ProgramStatus::Linked )
		{
			CASTOR_EXCEPTION( "Can't retrieve a program input layout from a non compiled shader." );
		}

		bool l_return = ObjectType::Create();

		if ( l_return )
		{
			l_return = GetOpenGl().BindVertexArray( GetGlName() );
		}

		if ( l_return )
		{
			if ( DoCreateAttributes( m_program.GetLayout(), m_vertexBuffer->GetDeclaration(), m_vertexAttributes ) )
			{
				m_vertexBuffer->Bind();
				DoBindAttributes( m_vertexAttributes );
			}

			if ( m_animationBuffer )
			{
				if ( DoCreateAttributes( m_program.GetLayout(), m_animationBuffer->GetDeclaration(), m_animationAttributes ) )
				{
					m_animationBuffer->Bind();
					DoBindAttributes( m_animationAttributes );
				}
			}

			if ( m_indexBuffer )
			{
				m_indexBuffer->Bind();
			}

			if ( m_bonesBuffer )
			{
				if ( DoCreateAttributes( m_program.GetLayout(), m_bonesBuffer->GetDeclaration(), m_bonesAttributes ) )
				{
					m_bonesBuffer->Bind();
					DoBindAttributes( m_bonesAttributes );
				}
			}

			if ( m_matrixBuffer )
			{
				if ( DoCreateAttributes( m_program.GetLayout(), m_matrixBuffer->GetDeclaration(), m_matrixAttributes ) )
				{
					m_matrixBuffer->Bind();
					DoBindAttributes( m_matrixAttributes );
				}
			}

			GetOpenGl().BindVertexArray( 0 );
		}

		return l_return;
	}

	void GlGeometryBuffers::DoCleanup()
	{
		m_vertexAttributes.clear();
		m_animationAttributes.clear();
		m_matrixAttributes.clear();
		m_bonesAttributes.clear();
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

	GlAttributeBaseSPtr GlGeometryBuffers::DoCreateAttribute( BufferElementDeclaration const & p_element, uint32_t p_offset, BufferDeclaration const & p_declaration )
	{
		bool l_return = true;
		auto const & l_renderSystem = GetOpenGl().GetRenderSystem();
		GlAttributeBaseSPtr l_attribute;

		switch ( p_element.m_dataType )
		{
		case ElementType::Float:
			l_attribute = std::make_shared< GlAttribute1r >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::Vec2:
			l_attribute = std::make_shared< GlAttribute2r >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::Vec3:
			l_attribute = std::make_shared< GlAttribute3r >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::Vec4:
			l_attribute = std::make_shared< GlAttribute4r >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::Colour:
			l_attribute = std::make_shared< GlAttribute1ui >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::Int:
			l_attribute = std::make_shared< GlAttribute1i >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::IVec2:
			l_attribute = std::make_shared< GlAttribute2i >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::IVec3:
			l_attribute = std::make_shared< GlAttribute3i >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::IVec4:
			l_attribute = std::make_shared< GlAttribute4i >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::Mat2:
			l_attribute = std::make_shared< GlMatAttribute< real, 2, 2 > >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::Mat3:
			l_attribute = std::make_shared< GlMatAttribute< real, 3, 3 > >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
			break;

		case ElementType::Mat4:
			l_attribute = std::make_shared< GlMatAttribute< real, 4, 4 > >( GetOpenGl(), m_program, p_declaration, p_element.m_name );
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
				auto l_attribute = DoCreateAttribute( l_element, l_it->m_offset, p_declaration );

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
