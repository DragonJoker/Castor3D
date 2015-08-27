#include "Context.hpp"
#include "DepthStencilState.hpp"
#include "OneFrameVariable.hpp"
#include "RenderWindow.hpp"
#include "RenderSystem.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "Viewport.hpp"
#include "ShaderProgram.hpp"
#include "Vertex.hpp"
#include "Pipeline.hpp"
#include "Engine.hpp"
#include "ShaderManager.hpp"

using namespace Castor;

namespace Castor3D
{
	Context::Context()
		:	m_pWindow( NULL )
		,	m_pRenderSystem( NULL )
		,	m_bInitialised( false )
		,	m_bDeferredShadingSet( false )
		,	m_bMultiSampling( false )
	{
		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0, eELEMENT_TYPE_2FLOATS ),
		};
		real l_pBuffer[] =
		{
			 0,  0, 0, 0,
			 1,  1, 1, 1,
			 0,  1, 0, 1,
			 0,  0, 0, 0,
			 1,  0, 1, 0,
			 1,  1, 1, 1,
		};
		std::memcpy( m_pBuffer, l_pBuffer, sizeof( l_pBuffer ) );
		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
		uint32_t i = 0;
		std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
		{
			p_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_pDeclaration->GetStride()] );
		} );
	}

	Context::~Context()
	{
		std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
		{
			p_vertex.reset();
		} );
		m_pDeclaration.reset();
	}

	bool Context::Initialise( RenderWindow * p_window )
	{
		m_pWindow = p_window;
		m_pRenderSystem	= m_pWindow->GetEngine()->GetRenderSystem();
		m_bDeferredShadingSet = p_window->IsUsingDeferredRendering();
		ShaderManager & l_manager = m_pRenderSystem->GetEngine()->GetShaderManager();
		ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
		m_pBtoBShaderProgram = l_program;
		m_mapDiffuse = l_program->CreateFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
		m_bMultiSampling = p_window->IsMultisampling();
		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( m_pRenderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
		IndexBufferUPtr l_pIdxBuffer = std::make_unique< IndexBuffer >( m_pRenderSystem );
		uint32_t l_uiStride = m_pDeclaration->GetStride();
		l_pVtxBuffer->Resize( m_arrayVertex.size() * l_uiStride );
		m_arrayVertex[0]->LinkCoords( &l_pVtxBuffer->data()[0 * l_uiStride], l_uiStride );
		m_arrayVertex[1]->LinkCoords( &l_pVtxBuffer->data()[1 * l_uiStride], l_uiStride );
		m_arrayVertex[2]->LinkCoords( &l_pVtxBuffer->data()[2 * l_uiStride], l_uiStride );
		m_arrayVertex[3]->LinkCoords( &l_pVtxBuffer->data()[3 * l_uiStride], l_uiStride );
		m_arrayVertex[4]->LinkCoords( &l_pVtxBuffer->data()[4 * l_uiStride], l_uiStride );
		m_arrayVertex[5]->LinkCoords( &l_pVtxBuffer->data()[5 * l_uiStride], l_uiStride );
		m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr );
		m_pViewport = std::make_shared< Viewport >( m_pRenderSystem->GetEngine(), Size( 10, 10 ), eVIEWPORT_TYPE_2D );
		m_pViewport->SetLeft( real( 0.0 ) );
		m_pViewport->SetRight( real( 1.0 ) );
		m_pViewport->SetTop( real( 1.0 ) );
		m_pViewport->SetBottom( real( 0.0 ) );
		m_pViewport->SetNear( real( 0.0 ) );
		m_pViewport->SetFar( real( 1.0 ) );
		m_pDsStateBackground = m_pRenderSystem->GetEngine()->CreateDepthStencilState( cuT( "ContextBackgroundDSState" ) );
		m_pDsStateBackground->SetDepthTest( false );
		m_pDsStateBackground->SetDepthMask( eWRITING_MASK_ZERO );
		return DoInitialise();
	}

	void Context::Cleanup()
	{
		m_bInitialised = false;
		DoCleanup();
		SetCurrent();
		ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();

		if ( l_pProgram )
		{
			l_pProgram->Cleanup();
			l_pProgram.reset();
		}

		m_pGeometryBuffers->GetVertexBuffer().Cleanup();
		m_pGeometryBuffers->Cleanup();
		m_pGeometryBuffers.reset();
		m_pViewport.reset();
		EndCurrent();
	}

	void Context::SetCurrent()
	{
		DoSetCurrent();
		m_pRenderSystem->SetCurrentContext( this );
	}

	void Context::EndCurrent()
	{
		m_pRenderSystem->SetCurrentContext( NULL );
		DoEndCurrent();
	}

	void Context::CullFace( eFACE p_eCullFace )
	{
		DoCullFace( p_eCullFace );
	}

	void Context::SwapBuffers()
	{
		DoSwapBuffers();
	}

	void Context::SetClearColour( Castor::Colour const & p_clrClear )
	{
		m_clearColour = p_clrClear;
		DoSetClearColour( p_clrClear );
	}

	void Context::Clear( uint32_t p_uiTargets )
	{
		DoClear( p_uiTargets );
	}

	void Context::Bind( eBUFFER p_eBuffer, eFRAMEBUFFER_TARGET p_eTarget )
	{
		DoBind( p_eBuffer, p_eTarget );
	}

	void Context::SetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue )
	{
		DoSetAlphaFunc( p_eFunc, p_byValue );
	}

	void Context::BToBRender( Castor::Size const & p_size, TextureBaseSPtr p_pTexture, uint32_t p_uiComponents )
	{
		ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
		m_pViewport->SetSize( p_size );
		Clear( p_uiComponents );
		m_pViewport->Render();
		CullFace( eFACE_BACK );
		uint32_t l_id = p_pTexture->GetIndex();
		p_pTexture->SetIndex( 0 );

		if ( l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_mapDiffuse->SetValue( p_pTexture.get() );
			l_pProgram->Bind( 0, 1 );
			FrameVariableBufferSPtr l_matrices = l_pProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );

			if ( l_matrices )
			{
				m_pRenderSystem->GetPipeline()->ApplyProjection( *l_matrices );
			}
		}

		if ( p_pTexture->BindAt( 0 ) )
		{
			m_pGeometryBuffers->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, m_arrayVertex.size(), 0 );
			p_pTexture->UnbindFrom( 0 );
		}

		if ( l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			l_pProgram->Unbind();
		}
		
		p_pTexture->SetIndex( l_id );
	}
}
