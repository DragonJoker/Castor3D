#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Context.hpp"
#include "Castor3D/RenderWindow.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Texture.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/Viewport.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/ShaderManager.hpp"

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

Context :: Context()
	:	m_pWindow				( NULL	)
	,	m_pRenderSystem			( NULL	)
	,	m_bInitialised			( false	)
	,	m_bDeferredShadingSet	( false	)
	,	m_bMultiSampling		( false	)
{
	BufferElementDeclaration	l_vertexDeclarationElements[] =
	{	BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS )
	,	BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_2FLOATS )
	};

	real l_pBuffer[] =
	{	0, 0, 0, 0, 0
	,	0, 1, 0, 0, 1
	,	1, 1, 0, 1, 1
	,	1, 0, 0, 1, 0
	};

	std::memcpy( m_pBuffer, l_pBuffer, sizeof( l_pBuffer ) );

	m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
	uint32_t i = 0;

	std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
	{
		p_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_pDeclaration->GetStride()] );
	} );
}

Context :: ~Context()
{
	std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
	{
		p_vertex.reset();
	} );

	m_pDeclaration.reset();
}

bool Context :: Initialise( RenderWindow * p_window )
{
	m_pWindow		= p_window;
	m_pRenderSystem	= m_pWindow->GetRenderer()->GetRenderSystem();

	m_bDeferredShadingSet = p_window->IsUsingDeferredRendering();
	m_pBtoBShaderProgram = m_pRenderSystem->GetEngine()->GetShaderManager().GetNewProgram();
	m_bMultiSampling = p_window->IsMultisampling();

	VertexBufferSPtr l_pVtxBuffer;
	IndexBufferSPtr l_pIdxBuffer;

	l_pVtxBuffer	= std::make_shared< VertexBuffer	>( m_pRenderSystem, &(*m_pDeclaration)[0], m_pDeclaration->Size() );
	l_pIdxBuffer	= std::make_shared< IndexBuffer		>( m_pRenderSystem );
	l_pVtxBuffer->SetShared( l_pVtxBuffer );
	l_pIdxBuffer->SetShared( l_pIdxBuffer );

	uint32_t l_uiStride = m_pDeclaration->GetStride();
	l_pVtxBuffer->Resize( 4 * l_uiStride );
	m_arrayVertex[0]->LinkCoords( &l_pVtxBuffer->data()[0 * l_uiStride], l_uiStride );
	m_arrayVertex[1]->LinkCoords( &l_pVtxBuffer->data()[1 * l_uiStride], l_uiStride );
	m_arrayVertex[2]->LinkCoords( &l_pVtxBuffer->data()[2 * l_uiStride], l_uiStride );
	m_arrayVertex[3]->LinkCoords( &l_pVtxBuffer->data()[3 * l_uiStride], l_uiStride );

	l_pIdxBuffer->AddElement( 0 );
	l_pIdxBuffer->AddElement( 2 );
	l_pIdxBuffer->AddElement( 1 );
	l_pIdxBuffer->AddElement( 0 );
	l_pIdxBuffer->AddElement( 3 );
	l_pIdxBuffer->AddElement( 2 );

	m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( l_pVtxBuffer, l_pIdxBuffer, nullptr );

	m_pViewport = std::make_shared< Viewport >( m_pRenderSystem->GetEngine(), Size( 10, 10 ), eVIEWPORT_TYPE_2D );
	m_pViewport->SetLeft(	real( 0.0 ) );
	m_pViewport->SetRight(	real( 1.0 ) );
	m_pViewport->SetTop(	real( 1.0 ) );
	m_pViewport->SetBottom(	real( 0.0 ) );
	m_pViewport->SetNear(	real( 0.0 ) );
	m_pViewport->SetFar(	real( 1.0 ) );

	return DoInitialise();
}

void Context::Cleanup()
{
	m_bInitialised = false;
	DoCleanup();
	SetCurrent();
	ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();

	if( l_pProgram )
	{
		l_pProgram->Cleanup();
		l_pProgram.reset();
	}

	m_pGeometryBuffers->GetIndexBuffer()->Cleanup();
	m_pGeometryBuffers->GetVertexBuffer()->Cleanup();
	m_pGeometryBuffers->Cleanup();
	m_pGeometryBuffers.reset();

	m_pViewport.reset();
	EndCurrent();
}

void Context :: SetCurrent()
{
	DoSetCurrent();
	m_pRenderSystem->SetCurrentContext( this );
}

void Context :: EndCurrent()
{
	m_pRenderSystem->SetCurrentContext( NULL );
	DoEndCurrent();
}

void Context :: CullFace( eFACE p_eCullFace )
{
	DoCullFace( p_eCullFace );
}

void Context :: SwapBuffers()
{
	DoSwapBuffers();
}

void Context :: SetClearColour( Castor::Colour const & p_clrClear )
{
	DoSetClearColour( p_clrClear );
}

void Context :: Clear( uint32_t p_uiTargets )
{
	DoClear( p_uiTargets );
}

void Context :: Bind( eBUFFER p_eBuffer, eFRAMEBUFFER_TARGET p_eTarget )
{
	DoBind( p_eBuffer, p_eTarget );
}

void Context :: SetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue )
{
	DoSetAlphaFunc( p_eFunc, p_byValue );
}

void Context :: BToBRender( Castor::Size const & p_size, TextureBaseSPtr p_pTexture, uint32_t p_uiComponents )
{
	Pipeline * l_pPipeline = m_pRenderSystem->GetPipeline();
	ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
	m_pViewport->SetSize( p_size );
	Clear( p_uiComponents );
	m_pViewport->Render();
	CullFace( eFACE_BACK );
	l_pPipeline->MatrixMode( eMTXMODE_MODEL );
	l_pPipeline->LoadIdentity();
	l_pPipeline->MatrixMode( eMTXMODE_VIEW );
	l_pPipeline->LoadIdentity();

	if( l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
	{
		OneTextureFrameVariableSPtr l_pMapDiffuse = l_pProgram->FindFrameVariable( cuT( "c3d_mapDiffuse" ), eSHADER_TYPE_PIXEL );

		if( l_pMapDiffuse )
		{
			l_pMapDiffuse->SetValue( p_pTexture.get() );
		}

		l_pProgram->Begin( 0, 1 );
		l_pPipeline->ApplyMatrices( *l_pProgram );
	}

	if( p_pTexture->Bind() )
	{
		m_pGeometryBuffers->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, m_pGeometryBuffers->GetIndexBuffer()->GetSize(), 0 );
		p_pTexture->Unbind();
	}

	if( l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
	{
		l_pProgram->End();
	}
}

//*************************************************************************************************