#include "Context.hpp"

#include "BlendStateManager.hpp"
#include "Buffer.hpp"
#include "DepthStencilStateManager.hpp"
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
	Context::Context( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_pWindow( NULL )
		, m_bInitialised( false )
		, m_bMultiSampling( false )
		, m_viewport( Viewport::Ortho( *GetOwner()->GetOwner(), 0, 1, 0, 1, 0, 1000 ) )
	{
		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0, eELEMENT_TYPE_2FLOATS ),
		};
		real l_pBuffer[] =
		{
			0, 0, 0, 0,
			1, 1, 1, 1,
			0, 1, 0, 1,
			0, 0, 0, 0,
			1, 0, 1, 0,
			1, 1, 1, 1,
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
		ShaderManager & l_manager = GetOwner()->GetOwner()->GetShaderManager();
		ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
		m_pBtoBShaderProgram = l_program;
		m_mapDiffuse = l_program->CreateFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
		m_bMultiSampling = p_window->IsMultisampling();
		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( *GetOwner()->GetOwner(), &( *m_pDeclaration )[0], m_pDeclaration->Size() );
		l_pVtxBuffer->Resize( m_arrayVertex.size() * m_pDeclaration->GetStride() );
		l_pVtxBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
		m_pGeometryBuffers = GetOwner()->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr );
		m_pDsStateBackground = GetOwner()->GetOwner()->GetDepthStencilStateManager().Create( cuT( "ContextBackgroundDSState" ) );
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
		m_pGeometryBuffers.reset();
		m_bMultiSampling = false;
		m_pBtoBShaderProgram.reset();
		m_pWindow = NULL;
	}

	void Context::SetCurrent()
	{
		DoSetCurrent();
		GetOwner()->SetCurrentContext( this );
	}

	void Context::EndCurrent()
	{
		GetOwner()->SetCurrentContext( NULL );
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

	void Context::SetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue )
	{
		DoSetAlphaFunc( p_eFunc, p_byValue );
	}

	void Context::BToBRender( Castor::Size const & p_size, TextureBaseSPtr p_pTexture )
	{
		ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
		m_viewport.SetSize( p_size );
		m_viewport.Render( GetOwner()->GetPipeline() );
		uint32_t l_id = p_pTexture->GetIndex();
		p_pTexture->SetIndex( 0 );

		if ( l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_mapDiffuse->SetValue( p_pTexture.get() );
			FrameVariableBufferSPtr l_matrices = l_pProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );

			if ( l_matrices )
			{
				GetOwner()->GetPipeline().ApplyProjection( *l_matrices );
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
