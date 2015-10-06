#include "Context.hpp"

#include "Buffer.hpp"
#include "DepthStencilState.hpp"
#include "Engine.hpp"
#include "FrameVariableBuffer.hpp"
#include "MatrixFrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "Pipeline.hpp"
#include "RenderWindow.hpp"
#include "RenderSystem.hpp"
#include "ShaderManager.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Viewport.hpp"

using namespace Castor;

namespace Castor3D
{
	Context::Context()
		: m_pWindow( NULL )
		, m_renderSystem( NULL )
		, m_bInitialised( false )
		, m_bMultiSampling( false )
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

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_pDeclaration->GetStride()] );
		}
	}

	Context::~Context()
	{
		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}

		m_pDeclaration.reset();
	}

	bool Context::Initialise( RenderWindow * p_window )
	{
		m_pWindow = p_window;
		m_renderSystem	= m_pWindow->GetEngine()->GetRenderSystem();
		ShaderManager & l_manager = m_renderSystem->GetEngine()->GetShaderManager();
		ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
		m_pBtoBShaderProgram = l_program;
		m_mapDiffuse = l_program->CreateFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
		m_bMultiSampling = p_window->IsMultisampling();
		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( m_renderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
		l_pVtxBuffer->Resize( m_arrayVertex.size() * m_pDeclaration->GetStride() );
		l_pVtxBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
		m_pGeometryBuffers = m_renderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr );
		m_viewport = std::make_shared< Viewport >( m_renderSystem->GetEngine(), Size( 10, 10 ), eVIEWPORT_TYPE_2D );
		m_viewport->SetLeft( real( 0.0 ) );
		m_viewport->SetRight( real( 1.0 ) );
		m_viewport->SetTop( real( 1.0 ) );
		m_viewport->SetBottom( real( 0.0 ) );
		m_viewport->SetNear( real( 0.0 ) );
		m_viewport->SetFar( real( 1.0 ) );
		m_pDsStateBackground = m_renderSystem->GetEngine()->CreateDepthStencilState( cuT( "ContextBackgroundDSState" ) );
		m_pDsStateBackground->SetDepthTest( false );
		m_pDsStateBackground->SetDepthMask( eWRITING_MASK_ZERO );
		bool l_return = DoInitialise();

		if ( l_return )
		{
			SetCurrent();
			l_program->Initialise();
			m_pGeometryBuffers->Create();
			m_pGeometryBuffers->Initialise( l_program, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
			EndCurrent();
		}

		return l_return;
	}

	void Context::Cleanup()
	{
		m_bInitialised = false;
		SetCurrent();
		m_pGeometryBuffers->Cleanup();
		m_pGeometryBuffers->Destroy();
		ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();

		if ( l_pProgram )
		{
			l_pProgram->Cleanup();
		}

		EndCurrent();
		DoCleanup();
		m_pDsStateBackground.reset();
		m_viewport.reset();
		m_pGeometryBuffers.reset();
		m_bMultiSampling = false;
		m_pBtoBShaderProgram.reset();
		m_renderSystem = NULL;
		m_pWindow = NULL;
	}

	void Context::SetCurrent()
	{
		DoSetCurrent();
		m_renderSystem->SetCurrentContext( this );
	}

	void Context::EndCurrent()
	{
		m_renderSystem->SetCurrentContext( NULL );
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
		m_viewport->SetSize( p_size );
		Clear( p_uiComponents );
		m_viewport->Render();
		CullFace( eFACE_BACK );
		uint32_t l_id = p_pTexture->GetIndex();
		p_pTexture->SetIndex( 0 );

		if ( l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_mapDiffuse->SetValue( p_pTexture.get() );
			FrameVariableBufferSPtr l_matrices = l_pProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );

			if ( l_matrices )
			{
				m_renderSystem->GetPipeline().ApplyProjection( *l_matrices );
			}

			l_pProgram->Bind( 0, 1 );
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
