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
		, m_initialised( false )
		, m_bMultiSampling( false )
		, m_viewport( Viewport::Ortho( *GetRenderSystem()->GetEngine(), 0, 1, 0, 1, 0, 1 ) )
		, m_declaration( NULL, 0 )
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
		m_declaration = BufferDeclaration(
		{
			BufferElementDeclaration{ ShaderProgram::Position, eELEMENT_TYPE_2FLOATS },
			BufferElementDeclaration{ ShaderProgram::Texture, eELEMENT_TYPE_2FLOATS }
		} );
	}

	Context::~Context()
	{
		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	bool Context::Initialise( RenderWindow * p_window )
	{
		m_window = p_window;
		ShaderManager & l_manager = GetRenderSystem()->GetEngine()->GetShaderManager();
		ShaderProgramSPtr l_program = l_manager.GetNewProgram();
		m_renderTextureProgram = l_program;
		m_mapDiffuse = l_program->CreateFrameVariable( ShaderProgram::MapDiffuse, eSHADER_TYPE_PIXEL );
		l_manager.CreateMatrixBuffer( *l_program, MASK_SHADER_TYPE_VERTEX );
		m_bMultiSampling = p_window->IsMultisampling();
		m_pDsStateNoDepth = GetRenderSystem()->GetEngine()->GetDepthStencilStateManager().Create( cuT( "NoDepthState" ) );
		m_pDsStateNoDepth->SetDepthTest( false );
		m_pDsStateNoDepth->SetDepthMask( eWRITING_MASK_ZERO );
		m_pDsStateNoDepthWrite = GetRenderSystem()->GetEngine()->GetDepthStencilStateManager().Create( cuT( "NoDepthWriterState" ) );
		m_pDsStateNoDepthWrite->SetDepthMask( eWRITING_MASK_ZERO );
		bool l_return = DoInitialise();

		if ( l_return )
		{
			SetCurrent();
			l_program->Initialise();
			uint32_t i = 0;

			for ( auto & l_vertex : m_arrayVertex )
			{
				l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_declaration.GetStride()] );
			}

			m_vertexBuffer = std::make_unique< VertexBuffer >( *GetRenderSystem()->GetEngine(), m_declaration );
			m_vertexBuffer->Resize( m_arrayVertex.size() * m_declaration.GetStride() );
			m_vertexBuffer->LinkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
			m_vertexBuffer->Create();
			m_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
			m_geometryBuffers = GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, l_program->GetLayout(), m_vertexBuffer.get(), nullptr, nullptr, nullptr );
			m_pDsStateNoDepth->Initialise();
			m_pDsStateNoDepthWrite->Initialise();
			EndCurrent();
		}

		return l_return;
	}

	void Context::Cleanup()
	{
		m_initialised = false;
		SetCurrent();
		m_pDsStateNoDepth->Cleanup();
		m_pDsStateNoDepthWrite->Cleanup();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer->Destroy();
		m_vertexBuffer.reset();
		m_geometryBuffers.reset();
		ShaderProgramSPtr l_program = m_renderTextureProgram.lock();

		if ( l_program )
		{
			l_program->Cleanup();
		}

		EndCurrent();
		DoCleanup();
		m_pDsStateNoDepth.reset();
		m_pDsStateNoDepthWrite.reset();
		m_geometryBuffers.reset();
		m_bMultiSampling = false;
		m_renderTextureProgram.reset();
		m_window = NULL;
	}

	void Context::SetCurrent()
	{
		DoSetCurrent();
		GetRenderSystem()->SetCurrentContext( this );
	}

	void Context::EndCurrent()
	{
		GetRenderSystem()->SetCurrentContext( NULL );
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

	void Context::SetAlphaFunc( eALPHA_FUNC p_func, uint8_t p_value )
	{
		DoSetAlphaFunc( p_func, p_value );
	}

	void Context::RenderTexture( Castor::Size const & p_size, TextureSPtr p_texture )
	{
		m_mapDiffuse->SetValue( p_texture.get() );
		DoRenderTexture( p_size, p_texture, m_geometryBuffers, m_renderTextureProgram.lock() );
	}

	void Context::RenderTexture( Castor::Size const & p_size, TextureSPtr p_texture, ShaderProgramSPtr p_program )
	{
		DoRenderTexture( p_size, p_texture, m_geometryBuffers, p_program );
	}

	void Context::DoRenderTexture( Castor::Size const & p_size, TextureSPtr p_texture, GeometryBuffersSPtr p_geometryBuffers, ShaderProgramSPtr p_program )
	{
		ShaderProgramSPtr l_program = p_program;
		m_viewport.SetSize( p_size );
		m_viewport.Render( GetRenderSystem()->GetPipeline() );
		uint32_t l_id = p_texture->GetIndex();
		p_texture->SetIndex( 0 );

		if ( l_program && l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			FrameVariableBufferSPtr l_matrices = l_program->FindFrameVariableBuffer( ShaderProgram::BufferMatrix );

			if ( l_matrices )
			{
				GetRenderSystem()->GetPipeline().ApplyProjection( *l_matrices );
			}

			l_program->Bind();
		}

		if ( p_texture->BindAt( 0 ) )
		{
			p_geometryBuffers->Draw( *l_program, m_arrayVertex.size(), 0 );
			p_texture->UnbindFrom( 0 );
		}

		if ( l_program && l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			l_program->Unbind();
		}

		p_texture->SetIndex( l_id );
	}
}
