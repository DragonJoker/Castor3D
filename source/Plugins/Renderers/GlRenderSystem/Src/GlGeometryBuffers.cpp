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
	GlGeometryBuffers::GlGeometryBuffers( OpenGl & p_gl, VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer )
		:	GeometryBuffers( std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ) )
		,	m_uiIndex( eGL_INVALID_INDEX )
		,	m_gl( p_gl )
	{
	}

	GlGeometryBuffers::~GlGeometryBuffers()
	{
	}

	bool GlGeometryBuffers::Draw( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex )
	{
		eGL_PRIMITIVE l_eMode = m_gl.Get( p_ePrimitiveType );

		if ( p_pProgram && p_pProgram->HasObject( eSHADER_TYPE_HULL ) )
		{
			l_eMode = eGL_PRIMITIVE_PATCHES;
			m_gl.PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
		}

		if ( Bind() )
		{
			if ( m_pIndexBuffer )
			{
				m_gl.DrawElements( l_eMode, int( p_uiSize ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_uiIndex ) );
			}
			else
			{
				m_gl.DrawArrays( l_eMode, int( p_uiIndex ), int( p_uiSize ) );
			}

			Unbind();
		}

		return true;
	}

	bool GlGeometryBuffers::DrawInstanced( eTOPOLOGY p_eTopology, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount )
	{
		eGL_PRIMITIVE l_eMode = m_gl.Get( p_eTopology );

		if ( p_pProgram->HasObject( eSHADER_TYPE_HULL ) )
		{
			l_eMode = eGL_PRIMITIVE_PATCHES;
			m_gl.PatchParameter( eGL_PATCH_PARAMETER_VERTICES, 3 );
		}

		if ( m_pMatrixBuffer )
		{
			m_pMatrixBuffer->GetGpuBuffer()->Fill( m_pMatrixBuffer->data(), m_pMatrixBuffer->GetSize(), eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
		}

		if ( Bind() )
		{
			if ( m_pIndexBuffer )
			{
				m_gl.DrawElementsInstanced( l_eMode, int( p_uiSize ), eGL_TYPE_UNSIGNED_INT, BUFFER_OFFSET( p_uiIndex ), int( p_uiCount ) );
			}
			else
			{
				m_gl.DrawArraysInstanced( l_eMode, int( p_uiIndex ), int( p_uiSize ), int( p_uiCount ) );
			}

			Unbind();
		}

		return true;
	}

	bool GlGeometryBuffers::Initialise()
	{
		bool l_return = false;
#if !C3DGL_LIMIT_TO_2_1

		if ( m_gl.HasVao() )
		{
			m_pfnBind = PFnBind( [&]()
			{
				bool l_return = m_uiIndex != eGL_INVALID_INDEX;

				if ( l_return )
				{
					l_return = m_gl.BindVertexArray( m_uiIndex );
				}

				return l_return;
			} );
			m_pfnUnbind = PFnUnbind( [&]()
			{
				m_gl.BindVertexArray( 0 );
			} );

			if ( m_uiIndex == eGL_INVALID_INDEX )
			{
				m_gl.GenVertexArrays( 1, &m_uiIndex );
				glTrack( m_gl, GlGeometryBuffers, this );
			}

			if ( m_uiIndex != eGL_INVALID_INDEX )
			{
				if ( m_pVertexBuffer )
				{
					l_return = m_gl.BindVertexArray( m_uiIndex );

					if ( l_return )
					{
						m_pVertexBuffer->Bind();

						if ( m_pIndexBuffer )
						{
							m_pIndexBuffer->Bind();
						}

						if ( m_pMatrixBuffer )
						{
							m_pMatrixBuffer->Bind( 2 );
						}

						m_gl.BindVertexArray( 0 );
					}
				}
			}
		}
		else
#endif
		{
			m_pfnBind = PFnBind( [&]()
			{
				return GeometryBuffers::Bind();
			} );
			m_pfnUnbind = PFnUnbind( [&]()
			{
				GeometryBuffers::Unbind();
			} );
			m_uiIndex = 0;
			l_return = true;
		}

		return l_return;
	}

	void GlGeometryBuffers::Cleanup()
	{
#if !C3DGL_LIMIT_TO_2_1

		if ( m_gl.HasVao() )
		{
			if ( m_uiIndex != eGL_INVALID_INDEX )
			{
				glUntrack( m_gl, this );
				m_gl.DeleteVertexArrays( 1, &m_uiIndex );
			}
		}

#endif
		m_uiIndex = eGL_INVALID_INDEX;
	}

	bool GlGeometryBuffers::Bind()
	{
		return m_pfnBind();
	}

	void GlGeometryBuffers::Unbind()
	{
		m_pfnUnbind();
	}
}
