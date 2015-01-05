#include "Submesh.hpp"
#include "InitialiseEvent.hpp"
#include "Vertex.hpp"
#include "Buffer.hpp"
#include "Material.hpp"
#include "Face.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "ShaderProgram.hpp"
#include "Engine.hpp"
#include "ShaderManager.hpp"
#include "Skeleton.hpp"

#include <BlockTracker.hpp>

using namespace Castor;

namespace Castor3D
{
	SubmeshRenderer::SubmeshRenderer( RenderSystem * p_pRenderSystem )
		:	Renderer<Submesh, SubmeshRenderer>( p_pRenderSystem )
		,	m_eCurDrawType( eTOPOLOGY( -1 ) )
		,	m_ePrvDrawType( eTOPOLOGY( -1 ) )
		,	m_bInitialised( false )
		,	m_bDirty( false )
	{
	}

	SubmeshRenderer::~SubmeshRenderer()
	{
		Cleanup();
	}

	void SubmeshRenderer::Cleanup()
	{
		if ( m_pGeometryBuffers )
		{
			m_pGeometryBuffers->GetVertexBuffer().Cleanup();
			m_pGeometryBuffers->GetVertexBuffer().Destroy();

			if ( m_pGeometryBuffers->HasIndexBuffer() )
			{
				m_pGeometryBuffers->GetIndexBuffer().Cleanup();
				m_pGeometryBuffers->GetIndexBuffer().Destroy();
			}

			if ( m_pGeometryBuffers->HasMatrixBuffer() )
			{
				m_pGeometryBuffers->GetMatrixBuffer().Cleanup();
				m_pGeometryBuffers->GetMatrixBuffer().Destroy();
			}

			m_pGeometryBuffers->Cleanup();
		}

		m_bInitialised = false;
	}

	void SubmeshRenderer::Initialise()
	{
		m_pGeometryBuffers->GetVertexBuffer().Create();
		m_pGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );

		if ( m_pGeometryBuffers->HasIndexBuffer() )
		{
			m_pGeometryBuffers->GetIndexBuffer().Create();
			m_pGeometryBuffers->GetIndexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
		}

		if ( m_pGeometryBuffers->HasMatrixBuffer() )
		{
			m_pGeometryBuffers->GetMatrixBuffer().Create();
			m_pGeometryBuffers->GetMatrixBuffer().Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
		}

		m_pGeometryBuffers->Initialise();
		m_bInitialised = true;
	}

	void SubmeshRenderer::Draw( eTOPOLOGY p_eMode, Pass const & p_pass )
	{
		if ( p_eMode != m_eCurDrawType )
		{
			m_eCurDrawType = p_eMode;
		}

		if ( DoPrepareBuffers( p_pass ) )
		{
			ShaderProgramBaseSPtr l_pProgram = p_pass.GetShader< ShaderProgramBase >();
			uint32_t l_uiSize = m_pGeometryBuffers->GetVertexBuffer().GetSize() / m_pGeometryBuffers->GetVertexBuffer().GetDeclaration().GetStride();

			if ( m_pGeometryBuffers->HasIndexBuffer() )
			{
				l_uiSize = m_pGeometryBuffers->GetIndexBuffer().GetSize();
			}

			uint32_t l_count = GetTarget()->GetRefCount( p_pass.GetParent() );

			if ( l_count > 1 )
			{
				if ( m_pRenderSystem->HasInstancing() )
				{
					m_pGeometryBuffers->DrawInstanced( m_eCurDrawType, l_pProgram, l_uiSize, 0, l_count );
				}
				else
				{
					m_pGeometryBuffers->Draw( m_eCurDrawType, l_pProgram, l_uiSize, 0 );
				}
			}
			else
			{
				m_pGeometryBuffers->Draw( m_eCurDrawType, l_pProgram, l_uiSize, 0 );
			}
		}
	}

	void SubmeshRenderer::CreateBuffers()
	{
		std::vector< BufferElementDeclaration >	l_vertexDeclarationElements;
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_NORMAL,		eELEMENT_TYPE_3FLOATS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_TANGENT,		eELEMENT_TYPE_3FLOATS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BITANGENT,	eELEMENT_TYPE_3FLOATS ) );
		l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_3FLOATS ) );

		if ( m_target && m_target->GetSkeleton() )
		{
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BONE_IDS,		eELEMENT_TYPE_4INTS ) );
			l_vertexDeclarationElements.push_back( BufferElementDeclaration( 0, eELEMENT_USAGE_BONE_WEIGHTS,	eELEMENT_TYPE_4FLOATS ) );
		}

		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( m_pRenderSystem, &l_vertexDeclarationElements[0], uint32_t( l_vertexDeclarationElements.size() ) );
		IndexBufferUPtr l_pIdxBuffer = std::make_unique< IndexBuffer >( m_pRenderSystem );
		MatrixBufferUPtr l_pMtxBuffer = std::make_unique< MatrixBuffer >( m_pRenderSystem );
		m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), std::move( l_pIdxBuffer ), std::move( l_pMtxBuffer ) );
	}

	bool SubmeshRenderer::DoPrepareBuffers( Pass const & p_pass )
	{
		ShaderProgramBaseSPtr l_pProgram = p_pass.GetShader< ShaderProgramBase >();
		bool l_bUseShader = l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED;

		if ( m_eCurDrawType != m_ePrvDrawType || !m_pGeometryBuffers )
		{
			m_ePrvDrawType = m_eCurDrawType;
			m_pGeometryBuffers->GetVertexBuffer().Cleanup();

			if ( l_bUseShader )
			{
				m_pGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
			}
			else
			{
				m_pGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
			}

			if ( m_pGeometryBuffers->HasIndexBuffer() )
			{
				m_pGeometryBuffers->GetIndexBuffer().Cleanup();
				m_pGeometryBuffers->GetIndexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STREAM, eBUFFER_ACCESS_NATURE_DRAW );
			}

			if ( m_pGeometryBuffers->HasMatrixBuffer() )
			{
				m_pGeometryBuffers->GetMatrixBuffer().Cleanup();
				m_pGeometryBuffers->GetMatrixBuffer().Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
			}

			if ( m_pGeometryBuffers )
			{
				m_pGeometryBuffers->Cleanup();
			}

			m_pGeometryBuffers->Initialise();
		}
		else if ( m_bDirty )
		{
			m_pGeometryBuffers->Initialise();
		}

		if ( l_bUseShader )
		{
			m_pRenderSystem->GetPipeline()->ApplyMatrices( *l_pProgram );
		}
		else
		{
			l_bUseShader = true;
		}

		return l_bUseShader;
	}
}
