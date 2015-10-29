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
	Context::Context( RenderSystem & p_renderSystem, bool p_invertFinal )
		: OwnedBy< RenderSystem >( p_renderSystem )
		, m_window( NULL )
		, m_bInitialised( false )
		, m_bMultiSampling( false )
		, m_viewport( Viewport::Ortho( *GetOwner()->GetOwner(), 0, 1, 0, 1, 0, 1000 ) )
	{
		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0, eELEMENT_TYPE_2FLOATS ),
		};
		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );

		{
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
			uint32_t i = 0;

			for ( auto & l_vertex : m_arrayVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_pDeclaration->GetStride()] );
			}
		}
		{
			if ( p_invertFinal )
			{
				real l_pBuffer[] =
				{
					0, 0, 0, 1,
					1, 1, 1, 0,
					0, 1, 0, 0,
					0, 0, 0, 1,
					1, 0, 1, 1,
					1, 1, 1, 0,
				};

				std::memcpy( m_finalBuffer, l_pBuffer, sizeof( l_pBuffer ) );
			}
			else
			{
				std::memcpy( m_finalBuffer, m_pBuffer, sizeof( m_pBuffer ) );
			}

			uint32_t i = 0;

			for ( auto & l_vertex : m_finalVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_finalBuffer )[i++ * m_pDeclaration->GetStride()] );
			}
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
		m_window = p_window;
		ShaderManager & l_manager = GetOwner()->GetOwner()->GetShaderManager();
		ShaderProgramBaseSPtr l_program = l_manager.GetNewProgram();
		m_renderTextureProgram = l_program;
		m_mapDiffuse = l_program->CreateFrameVariable( ShaderProgramBase::MapDiffuse, eSHADER_TYPE_PIXEL );
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
		m_bMultiSampling = p_window->IsMultisampling();
		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( *GetOwner()->GetOwner(), &( *m_pDeclaration )[0], m_pDeclaration->Size() );
		l_pVtxBuffer->Resize( m_arrayVertex.size() * m_pDeclaration->GetStride() );
		l_pVtxBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
		m_pGeometryBuffers = GetOwner()->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr );
		l_pVtxBuffer = std::make_unique< VertexBuffer >( *GetOwner()->GetOwner(), &( *m_pDeclaration )[0], m_pDeclaration->Size() );
		l_pVtxBuffer->Resize( m_finalVertex.size() * m_pDeclaration->GetStride() );
		l_pVtxBuffer->LinkCoords( m_finalVertex.begin(), m_finalVertex.end() );
		m_finalGeometryBuffers = GetOwner()->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr );
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
			m_finalGeometryBuffers->Create();
			m_finalGeometryBuffers->Initialise( l_program, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
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
		m_finalGeometryBuffers->Cleanup();
		m_finalGeometryBuffers->Destroy();
		ShaderProgramBaseSPtr l_program = m_renderTextureProgram.lock();

		if ( l_program )
		{
			l_program->Cleanup();
		}

		EndCurrent();
		DoCleanup();
		m_pDsStateBackground.reset();
		m_finalGeometryBuffers.reset();
		m_pGeometryBuffers.reset();
		m_bMultiSampling = false;
		m_renderTextureProgram.reset();
		m_window = NULL;
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

	void Context::RenderTextureToCurrentBuffer( Castor::Size const & p_size, TextureBaseSPtr p_texture )
	{
		DoRenderTexture( p_size, p_texture, m_pGeometryBuffers );
	}

	void Context::RenderTextureToBackBuffer( Castor::Size const & p_size, TextureBaseSPtr p_texture )
	{
		DoRenderTexture( p_size, p_texture, m_finalGeometryBuffers );
	}

	void Context::DoRenderTexture( Castor::Size const & p_size, TextureBaseSPtr p_texture, GeometryBuffersSPtr p_geometryBuffers )
	{
		ShaderProgramBaseSPtr l_program = m_renderTextureProgram.lock();
		m_viewport.SetSize( p_size );
		m_viewport.Render( GetOwner()->GetPipeline() );
		uint32_t l_id = p_texture->GetIndex();
		p_texture->SetIndex( 0 );

		if ( l_program && l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_mapDiffuse->SetValue( p_texture.get() );
			FrameVariableBufferSPtr l_matrices = l_program->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );

			if ( l_matrices )
			{
				GetOwner()->GetPipeline().ApplyProjection( *l_matrices );
			}

			l_program->Bind( 0, 1 );
		}

		if ( p_texture->BindAt( 0 ) )
		{
			p_geometryBuffers->Draw( eTOPOLOGY_TRIANGLES, l_program, m_arrayVertex.size(), 0 );
			p_texture->UnbindFrom( 0 );
		}

		if ( l_program && l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			l_program->Unbind();
		}

		p_texture->SetIndex( l_id );
	}
}
