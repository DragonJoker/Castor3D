#include "GlGeometryBuffers.hpp"
#include "GlIndexBufferObject.hpp"
#include "GlVertexBufferObject.hpp"
#include "GlMatrixBufferObject.hpp"
#include "GlRenderSystem.hpp"
#include "GlShaderProgram.hpp"
#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlGeometryBuffers::GlGeometryBuffers( OpenGl & p_gl, eTOPOLOGY p_topology, ProgramInputLayout const & p_layout, VertexBuffer * p_vtx, IndexBuffer * p_idx, VertexBuffer * p_bones, MatrixBuffer * p_inst )
		: Castor3D::GeometryBuffers( p_topology, p_layout, p_vtx, p_idx, p_bones, p_inst )
		, ObjectType( p_gl,
					  "GlVertexArrayObjects",
					  std::bind( &OpenGl::GenVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::DeleteVertexArrays, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
					  std::bind( &OpenGl::IsVertexArray, std::ref( p_gl ), std::placeholders::_1 ),
					  std::bind( &OpenGl::BindVertexArray, std::ref( p_gl ), std::placeholders::_1 )
					  )
	{
		bool l_return = ObjectType::Create();

		if ( l_return )
		{
			l_return = GetOpenGl().BindVertexArray( GetGlName() );
		}

		if ( l_return )
		{
			m_vertexBuffer->Bind();
			m_layout.Bind();

			if ( m_indexBuffer )
			{
				m_indexBuffer->Bind();
			}

			if ( m_bonesBuffer )
			{
				m_bonesBuffer->Bind();
			}

			if ( m_matrixBuffer )
			{
				m_matrixBuffer->Bind( true );
			}

			GetOpenGl().BindVertexArray( 0 );
		}
	}

	GlGeometryBuffers::~GlGeometryBuffers()
	{
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
}
