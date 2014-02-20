#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/RenderTechnique.hpp"
#include "Castor3D/RenderTarget.hpp"
#include "Castor3D/BlendState.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/FrameBuffer.hpp"
#include "Castor3D/Texture.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Camera.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Viewport.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/FrameVariable.hpp"
#include "Castor3D/Context.hpp"
#include "Castor3D/DepthStencilState.hpp"
#include "Castor3D/RasterState.hpp"
#include "Castor3D/Parameter.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/ShaderManager.hpp"

using namespace Castor3D;
using namespace Castor;

#define DEBUG_BUFFERS	0

//*************************************************************************************************

TechniqueFactory :: TechniqueFactory()
{
}

TechniqueFactory :: ~TechniqueFactory()
{
}

RenderTechniqueBaseSPtr TechniqueFactory :: Create( String const & p_strName, RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
{
	RenderTechniqueBaseSPtr l_pReturn;
	RenderTechniquePtrStrMapIt l_it = m_mapRegistered.find( p_strName );

	if( l_it != m_mapRegistered.end() )
	{
		l_pReturn = l_it->second->Clone( p_renderTarget, p_pRenderSystem, p_params );
	}

	return l_pReturn;
}

//*************************************************************************************************

RenderTechniqueBase :: RenderTechniqueBase()
	:	m_pRenderTarget	( NULL	)
	,	m_pRenderSystem	( NULL	)
{
}

RenderTechniqueBase :: RenderTechniqueBase( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & CU_PARAM_UNUSED( p_params ) )
	:	m_pRenderTarget	( &p_renderTarget				)
	,	m_pRenderSystem	( p_pRenderSystem				)
	,	m_pEngine		( p_pRenderSystem->GetEngine()	)
{
	m_pFrameBuffer		= m_pRenderTarget->CreateFrameBuffer();

	m_pColorBuffer		= m_pRenderSystem->CreateDynamicTexture();
	m_pDepthBuffer		= m_pFrameBuffer->CreateDepthRenderBuffer(	ePIXEL_FORMAT_DEPTH24	);

	m_pColorBuffer->SetRenderTarget( true );

	m_pColorAttach		= m_pRenderTarget->CreateAttachment( m_pColorBuffer );
	m_pDepthAttach		= m_pRenderTarget->CreateAttachment( m_pDepthBuffer );

	m_wp2DBlendState	= m_pEngine->CreateBlendState( cuT( "RT_OVERLAY_BLEND" ) );
	m_wp2DDepthStencilState = m_pEngine->CreateDepthStencilState( cuT( "RT_OVERLAY_DS" ) );
}

RenderTechniqueBase :: ~RenderTechniqueBase()
{
}

bool RenderTechniqueBase :: Create()
{
	bool l_bReturn = true;

	l_bReturn &= m_pFrameBuffer->Create( 0 );
	l_bReturn &= m_pColorBuffer->Create();
	l_bReturn &= m_pDepthBuffer->Create();

	if( l_bReturn )
	{
		l_bReturn = DoCreate();
	}

	return l_bReturn;
}

void RenderTechniqueBase :: Destroy()
{
	DoDestroy();

	m_pColorBuffer->Destroy();
	m_pDepthBuffer->Destroy();
	m_pFrameBuffer->Destroy();
}

bool RenderTechniqueBase :: Initialise()
{
	m_size.set( m_pRenderTarget->GetCamera()->GetWidth(), m_pRenderTarget->GetCamera()->GetHeight() );

	m_pColorBuffer->SetDimension(	eTEXTURE_DIMENSION_2D );
	m_pColorBuffer->SetImage(	m_size, ePIXEL_FORMAT_A8R8G8B8	);

	m_size = m_pColorBuffer->GetDimensions();
	m_rect.set( 0, 0, m_size.width(), m_size.height() );

	bool l_bReturn = m_pColorBuffer->Initialise( 0 );

	if( l_bReturn )
	{
		l_bReturn = m_pDepthBuffer->Initialise( m_size );
	}

	if( l_bReturn )
	{
		l_bReturn = m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

		if( l_bReturn )
		{
			l_bReturn = m_pColorAttach->Attach( eATTACHMENT_POINT_COLOUR0,	m_pFrameBuffer, eTEXTURE_TARGET_2D );

			if( l_bReturn )
			{
				l_bReturn = m_pDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,	m_pFrameBuffer );
			}

			m_pFrameBuffer->Unbind();
		}
	}

	if( l_bReturn )
	{
		BlendStateSPtr l_pState = m_wp2DBlendState.lock();
		l_pState->EnableAlphaToCoverage( false );
		l_pState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
		l_pState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
		l_pState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
		l_pState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
		l_pState->EnableBlend( true );
		l_bReturn = l_pState->Initialise();
	}

	if( l_bReturn )
	{
		DepthStencilStateSPtr l_pState = m_wp2DDepthStencilState.lock();
		l_pState->SetDepthTest( false );
		l_bReturn = l_pState->Initialise();
	}

	if( l_bReturn )
	{
		l_bReturn = DoInitialise();
	}

	return l_bReturn;
}

void RenderTechniqueBase :: Cleanup()
{
	BlendStateSPtr l_pBlendState = m_wp2DBlendState.lock();
	l_pBlendState->Cleanup();

	m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
	m_pColorAttach->Detach();
	m_pDepthAttach->Detach();
	m_pFrameBuffer->Unbind();

//	m_pFrameBuffer->DetachAll();

	m_pColorBuffer->Cleanup();
	m_pDepthBuffer->Cleanup();
}

bool RenderTechniqueBase :: BeginRender()
{
	return DoBeginRender();
}

bool RenderTechniqueBase :: Render( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
{
	m_pRenderSystem->PushScene( &p_scene );
	return DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
}

void RenderTechniqueBase :: EndRender()
{
	DoEndRender();
#if !DX_DEBUG
	m_pColorBuffer->Bind();
	m_pColorBuffer->GenerateMipmaps();
	m_pColorBuffer->Unbind();

#if DEBUG_BUFFERS
	if( m_pColorAttach->DownloadBuffer( m_pColorBuffer->GetBuffer() ) )
	{
		Image l_image( cuT( "ColorBuffer" ), *m_pColorBuffer->GetBuffer() );
		Image::BinaryLoader()( const_cast< const Image & >( l_image ), cuT( "ColorBuffer.bmp" ) );
	}
#endif

	BlendStateSPtr l_pBlendState = m_wp2DBlendState.lock();
	DepthStencilStateSPtr l_pDepthStencilState = m_wp2DDepthStencilState.lock();
	m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	l_pBlendState->Apply();
	l_pDepthStencilState->Apply();
	m_pEngine->RenderOverlays( *m_pRenderSystem->GetTopScene() );
	m_pFrameBuffer->Unbind();
	m_pRenderSystem->PopScene();

//	m_pFrameBuffer->StretchInto( m_pRenderTarget->GetFrameBuffer(), m_rect, Rect( Position(), m_pRenderTarget->GetSize() ), eBUFFER_COMPONENT_COLOUR, eINTERPOLATION_MODE_LINEAR );
	m_pFrameBuffer->RenderToBuffer( m_pRenderTarget->GetFrameBuffer(), m_pRenderTarget->GetSize(), eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH, m_pRenderTarget->GetDepthStencilState(), m_pRenderTarget->GetRasteriserState() );
#endif
}

bool RenderTechniqueBase :: DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
{
	p_camera.Render();
	p_scene.Render( p_ePrimitives, p_dFrameTime, p_camera );
	p_camera.EndRender();
	return true;
}

//*************************************************************************************************

DirectRenderTechnique :: DirectRenderTechnique()
	:	RenderTechniqueBase	()
{
}

DirectRenderTechnique :: DirectRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	:	RenderTechniqueBase	( p_renderTarget, p_pRenderSystem, p_params	)
{
	Logger::LogMessage( "Using Direct rendering" );
}

DirectRenderTechnique :: ~DirectRenderTechnique()
{
}

RenderTechniqueBaseSPtr DirectRenderTechnique :: Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
{
	// No make_shared because ctor is protected;
	return RenderTechniqueBaseSPtr( new DirectRenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
}

bool DirectRenderTechnique :: DoCreate()
{
	return true;
}

void DirectRenderTechnique :: DoDestroy()
{
}

bool DirectRenderTechnique :: DoInitialise()
{
	return true;
}

void DirectRenderTechnique :: DoCleanup()
{
}

bool DirectRenderTechnique :: DoBeginRender()
{
#if DX_DEBUG
	return true;
#else
	bool l_bReturn = m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );

	if( l_bReturn )
	{
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
	}

	return l_bReturn;
#endif
}

void DirectRenderTechnique :: DoEndRender()
{
#if !DX_DEBUG
	m_pFrameBuffer->Unbind();
#endif
}

//*************************************************************************************************

MsaaRenderTechnique :: MsaaRenderTechnique()
	:	RenderTechniqueBase	(	)
	,	m_iSamplesCount		( 0	)
{
}

MsaaRenderTechnique :: MsaaRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	:	RenderTechniqueBase	( p_renderTarget, p_pRenderSystem, p_params	)
	,	m_iSamplesCount		( 0											)
{
	if( p_params.Get( cuT( "samples_count" ), m_iSamplesCount ) && m_iSamplesCount > 1 )
	{
		Logger::LogMessage( "Using MSAA, %d samples", m_iSamplesCount );
		m_pMsFrameBuffer	= m_pRenderTarget->CreateFrameBuffer();

		m_pMsColorBuffer	= m_pFrameBuffer->CreateColourRenderBuffer(	ePIXEL_FORMAT_A8R8G8B8	);
		m_pMsDepthBuffer	= m_pFrameBuffer->CreateDepthRenderBuffer(	ePIXEL_FORMAT_DEPTH24	);

		m_pMsColorAttach	= m_pRenderTarget->CreateAttachment( m_pMsColorBuffer );
		m_pMsDepthAttach	= m_pRenderTarget->CreateAttachment( m_pMsDepthBuffer );

		RasteriserStateSPtr l_pRasteriser = m_pEngine->CreateRasteriserState( cuT( "MSAA_RT" ) );
		l_pRasteriser->SetMultisample( true );
		m_wpMsRasteriserState = l_pRasteriser;

		m_pBoundFrameBuffer = m_pMsFrameBuffer;
	}
	else
	{
		Logger::LogWarning( "Using classic rendering through MSAA Render Technique" );
		m_iSamplesCount = 0;
		m_pBoundFrameBuffer = m_pFrameBuffer;
	}
}

MsaaRenderTechnique :: ~MsaaRenderTechnique()
{
}

RenderTechniqueBaseSPtr MsaaRenderTechnique :: Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
{
	// No make_shared because ctor is protected;
	return RenderTechniqueBaseSPtr( new MsaaRenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
}

bool MsaaRenderTechnique :: DoCreate()
{
	bool l_bReturn = true;

	if( m_iSamplesCount )
	{
		l_bReturn &= m_pMsFrameBuffer->Create( m_iSamplesCount );

		m_pMsColorBuffer->SetSamplesCount( m_iSamplesCount );
		m_pMsDepthBuffer->SetSamplesCount( m_iSamplesCount );

		l_bReturn &= m_pMsColorBuffer->Create();
		l_bReturn &= m_pMsDepthBuffer->Create();
	}

	return l_bReturn;
}

void MsaaRenderTechnique :: DoDestroy()
{
	if( m_iSamplesCount )
	{
		m_pMsColorBuffer->Destroy();
		m_pMsDepthBuffer->Destroy();
		m_pMsFrameBuffer->Destroy();
	}
}

bool MsaaRenderTechnique :: DoInitialise()
{
	bool l_bReturn = true;

	if( m_iSamplesCount )
	{
		if( l_bReturn )
		{
			l_bReturn = m_pMsColorBuffer->Initialise( m_size );
		}

		if( l_bReturn )
		{
			l_bReturn = m_pMsDepthBuffer->Initialise( m_size );
		}

		if( l_bReturn )
		{
			l_bReturn = m_pMsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

			if( l_bReturn )
			{
				l_bReturn = m_pMsColorAttach->Attach( eATTACHMENT_POINT_COLOUR0,	m_pMsFrameBuffer );

				if( l_bReturn )
				{
					l_bReturn = m_pMsDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,		m_pMsFrameBuffer );
				}

				m_pMsFrameBuffer->Unbind();
			}
		}
	}

	return l_bReturn;
}

void MsaaRenderTechnique :: DoCleanup()
{
	if( m_iSamplesCount )
	{
		m_pMsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_pMsColorAttach->Detach();
		m_pMsDepthAttach->Detach();
		m_pMsFrameBuffer->Unbind();
		m_pMsFrameBuffer->DetachAll();
		m_pMsColorBuffer->Cleanup();
		m_pMsDepthBuffer->Cleanup();
	}
}

bool MsaaRenderTechnique :: DoBeginRender()
{
	bool l_bReturn = m_pBoundFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );

	m_pRenderTarget->GetDepthStencilState()->Apply();

	if( l_bReturn && !m_iSamplesCount )
	{
		m_pRenderTarget->GetRasteriserState()->Apply();
	}
	else
	{
		m_wpMsRasteriserState.lock()->Apply();
	}

	return l_bReturn;
}

void MsaaRenderTechnique :: DoEndRender()
{
	m_pBoundFrameBuffer->Unbind();

	if( m_iSamplesCount )
	{
		m_pMsFrameBuffer->BlitInto( m_pFrameBuffer, m_rect, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH );
	}
}

//*************************************************************************************************

SsaaRenderTechnique :: SsaaRenderTechnique()
	:	RenderTechniqueBase	(	)
	,	m_iSamplesCount		( 0	)
{
}

SsaaRenderTechnique :: SsaaRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	:	RenderTechniqueBase	( p_renderTarget, p_pRenderSystem, p_params	)
	,	m_iSamplesCount		( 0											)
{
	if( p_params.Get( cuT( "samples_count" ), m_iSamplesCount ) && m_iSamplesCount > 1 )
	{
		Logger::LogMessage( "Using SSAA, %d samples", m_iSamplesCount );
		m_pSsFrameBuffer	= m_pRenderTarget->CreateFrameBuffer();

		m_pSsColorBuffer	= m_pSsFrameBuffer->CreateColourRenderBuffer(	ePIXEL_FORMAT_A8R8G8B8	);
		m_pSsColorAttach	= m_pRenderTarget->CreateAttachment( m_pSsColorBuffer );
		m_pSsDepthBuffer	= m_pSsFrameBuffer->CreateDepthRenderBuffer(	ePIXEL_FORMAT_DEPTH24	);
		m_pSsDepthAttach	= m_pRenderTarget->CreateAttachment( m_pSsDepthBuffer );

		m_pBoundFrameBuffer = m_pSsFrameBuffer;
	}
	else
	{
		Logger::LogWarning( "Using classic rendering through SSAA Render Technique" );
		m_iSamplesCount = 0;
		m_pBoundFrameBuffer = m_pFrameBuffer;
	}
}

SsaaRenderTechnique :: ~SsaaRenderTechnique()
{
}

RenderTechniqueBaseSPtr SsaaRenderTechnique :: Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
{
	// No make_shared because ctor is protected;
	return RenderTechniqueBaseSPtr( new SsaaRenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
}

bool SsaaRenderTechnique :: DoCreate()
{
	bool l_bReturn = true;

	if( m_iSamplesCount )
	{
		l_bReturn &= m_pSsFrameBuffer->Create( 0 );
		l_bReturn &= m_pSsColorBuffer->Create();
		l_bReturn &= m_pSsDepthBuffer->Create();
	}

	return l_bReturn;
}

void SsaaRenderTechnique :: DoDestroy()
{
	if( m_iSamplesCount )
	{
		m_pSsColorBuffer->Destroy();
		m_pSsDepthBuffer->Destroy();
		m_pSsFrameBuffer->Destroy();
	}
}

bool SsaaRenderTechnique :: DoInitialise()
{
	bool l_bReturn = true;
	m_sizeSsaa = m_size;
	m_rectSsaa = m_rect;

	if( m_iSamplesCount )
	{
		m_sizeSsaa = Size( m_sizeSsaa.width() * m_iSamplesCount, m_sizeSsaa.height() * m_iSamplesCount );
		m_rectSsaa.set( 0, 0, m_sizeSsaa.width(), m_sizeSsaa.height() );

		l_bReturn = m_pSsColorBuffer->Initialise( m_sizeSsaa );

		if( l_bReturn )
		{
			l_bReturn = m_pSsDepthBuffer->Initialise( m_sizeSsaa );
		}

		if( l_bReturn )
		{
			l_bReturn = m_pSsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

			if( l_bReturn )
			{
				l_bReturn = m_pSsColorAttach->Attach( eATTACHMENT_POINT_COLOUR0,	m_pSsFrameBuffer );

				if( l_bReturn )
				{
					l_bReturn = m_pSsDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,		m_pSsFrameBuffer );
				}

				m_pSsFrameBuffer->Unbind();
			}
		}
	}

	return l_bReturn;
}

void SsaaRenderTechnique :: DoCleanup()
{
	if( m_iSamplesCount )
	{
		m_pSsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_pSsColorAttach->Detach();
		m_pSsDepthAttach->Detach();
		m_pSsFrameBuffer->Unbind();
		m_pSsColorBuffer->Cleanup();
		m_pSsDepthBuffer->Cleanup();
	}
}

bool SsaaRenderTechnique :: DoBeginRender()
{
	bool l_bReturn = m_pBoundFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );

	if( l_bReturn )
	{
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
	}

	return l_bReturn;
}

void SsaaRenderTechnique :: DoEndRender()
{
	m_pBoundFrameBuffer->Unbind();

	if( m_iSamplesCount )
	{
		m_pSsFrameBuffer->BlitInto( m_pFrameBuffer, m_rect, eBUFFER_COMPONENT_COLOUR );
	}
}

//*************************************************************************************************

String g_strNames[] =
{	cuT( "c3d_mapPosition"	)
,	cuT( "c3d_mapDiffuse"	)
,	cuT( "c3d_mapNormals"	)
,	cuT( "c3d_mapTangent"	)
,	cuT( "c3d_mapBitangent"	)
,	cuT( "c3d_mapSpecular"	)
};

DeferredShadingRenderTechnique :: DeferredShadingRenderTechnique()
	:	RenderTechniqueBase()
{
}

DeferredShadingRenderTechnique :: DeferredShadingRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	:	RenderTechniqueBase( p_renderTarget, p_pRenderSystem, p_params )
{
	Logger::LogMessage( cuT( "Using deferred shading" ) );
	m_pDsFrameBuffer	= m_pRenderTarget->CreateFrameBuffer();

	for( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
	{
		m_pDsTextures[i]	= m_pRenderSystem->CreateDynamicTexture();
		m_pDsTextures[i]->SetRenderTarget( true );
		m_pDsTexAttachs[i]	= m_pRenderTarget->CreateAttachment( m_pDsTextures[i] );
	}

	m_pDsBufDepth		= m_pFrameBuffer->CreateDepthRenderBuffer( ePIXEL_FORMAT_DEPTH24 );
	m_pDsBufDepth		= m_pDsFrameBuffer->CreateDepthRenderBuffer( ePIXEL_FORMAT_DEPTH24 );
	m_pDsDepthAttach	= m_pRenderTarget->CreateAttachment( m_pDsBufDepth );

	m_pDsShaderProgram = m_pEngine->GetShaderManager().GetNewProgram();

	BufferElementDeclaration	l_vertexDeclarationElements[] =
	{	BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_2FLOATS )
	,	BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_2FLOATS )
	};

	stVERTEX l_pBuffer[] =
	{	{ { 0, 0 }, { 0, 0 } }
	,	{ { 0, 1 }, { 0, 1 } }
	,	{ { 1, 1 }, { 1, 1 } }
	,	{ { 0, 0 }, { 0, 0 } }
	,	{ { 1, 1 }, { 1, 1 } }
	,	{ { 1, 0 }, { 1, 0 } }
	};

	std::memcpy( m_pBuffer, l_pBuffer, sizeof( l_pBuffer ) );
	m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
	uint32_t i = 0;

	std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_pVertex )
	{
		p_pVertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_pDeclaration->GetStride()] );
	} );

	VertexBufferSPtr l_pVtxBuffer = std::make_shared< VertexBuffer >( m_pRenderSystem, l_vertexDeclarationElements );
	l_pVtxBuffer->SetShared( l_pVtxBuffer );

	uint32_t l_uiStride = m_pDeclaration->GetStride();
	l_pVtxBuffer->Resize( uint32_t( m_arrayVertex.size() * l_uiStride ) );

	for( uint32_t i = 0; i < m_arrayVertex.size(); ++i )
	{
		m_arrayVertex[i]->LinkCoords( &l_pVtxBuffer->data()[i * l_uiStride], l_uiStride );
	}

	m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( l_pVtxBuffer, nullptr, nullptr );

	m_pViewport = std::make_shared< Viewport >( m_pRenderSystem->GetEngine(), Size( 1, 1 ), eVIEWPORT_TYPE_2D );
	m_pViewport->SetLeft(	real( 0.0 ) );
	m_pViewport->SetRight(	real( 1.0 ) );
	m_pViewport->SetTop(	real( 0.0 ) );
	m_pViewport->SetBottom(	real( 1.0 ) );
	m_pViewport->SetNear(	real( 0.0 ) );
	m_pViewport->SetFar(	real( 1.0 ) );
}

DeferredShadingRenderTechnique :: ~DeferredShadingRenderTechnique()
{
	m_pViewport.reset();
	m_pGeometryBuffers.reset();
	m_pDsShaderProgram.reset();
	m_pDeclaration.reset();
}

RenderTechniqueBaseSPtr DeferredShadingRenderTechnique :: Clone( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
{
	// No make_shared because ctor is protected;
	return RenderTechniqueBaseSPtr( new DeferredShadingRenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
}

bool DeferredShadingRenderTechnique :: DoCreate()
{
	bool l_bReturn = true;

	m_pRenderSystem->GetMainContext()->SetDeferredShading( true );
	l_bReturn = m_pDsFrameBuffer->Create( 0 );

	for( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
	{
		l_bReturn = m_pDsTextures[i]->Create();
	}

	l_bReturn &= m_pDsBufDepth->Create();

	if( l_bReturn )
	{
		for( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
		{
			m_pDsShaderProgram->CreateFrameVariable( g_strNames[i], eSHADER_TYPE_PIXEL )->SetValue( m_pDsTextures[i].get() );
		}

		m_pGeometryBuffers->GetVertexBuffer()->Create();

		for( int i = 0; i < eSHADER_MODEL_COUNT; i++ )
		{
			eSHADER_MODEL l_eModel = eSHADER_MODEL( i );

			if( m_pRenderSystem->CheckSupport( l_eModel ) )
			{
				m_pDsShaderProgram->SetSource( eSHADER_TYPE_VERTEX,	l_eModel, m_pDsShaderProgram->GetVertexShaderSource( ePROGRAM_FLAG_DEFERRED, true ) );
				m_pDsShaderProgram->SetSource( eSHADER_TYPE_PIXEL,	l_eModel, m_pDsShaderProgram->GetPixelShaderSource( eTEXTURE_CHANNEL_LGHTPASS ) );
			}
		}
	}

	return l_bReturn;
}

void DeferredShadingRenderTechnique :: DoDestroy()
{
	m_pGeometryBuffers->GetVertexBuffer()->Destroy();

	for( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
	{
		m_pDsTextures[i]->Destroy();
	}

	m_pDsBufDepth->Destroy();
	m_pDsFrameBuffer->Destroy();
}

bool DeferredShadingRenderTechnique :: DoInitialise()
{
	bool l_bReturn = true;

	for( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
	{
		m_pDsTextures[i]->SetDimension(	eTEXTURE_DIMENSION_2D );

		if( i == eDS_TEXTURE_POSITION )
		{
			m_pDsTextures[i]->SetImage(	m_size, ePIXEL_FORMAT_ARGB32F	);
		}
		else
		{
			m_pDsTextures[i]->SetImage(	m_size, ePIXEL_FORMAT_A8R8G8B8	);
		}

		m_pDsTextures[i]->Initialise( i + 1 );
	}

	m_pDsBufDepth->Initialise( m_size );

	if( m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
	{
		for( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
		{
			l_bReturn &= m_pDsTexAttachs[i]->Attach( eATTACHMENT_POINT( eATTACHMENT_POINT_COLOUR0 + i ),	m_pDsFrameBuffer, eTEXTURE_TARGET_2D );
		}

		l_bReturn &= m_pDsDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,	m_pDsFrameBuffer );
		m_pDsFrameBuffer->Unbind();
	}

	m_pDsShaderProgram->Initialise();
	m_pDsShaderProgram->GetSceneBuffer()->GetVariable( ShaderProgramBase::CameraPos, m_pShaderCamera );

	m_pGeometryBuffers->GetVertexBuffer()->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, m_pDsShaderProgram );
	m_pGeometryBuffers->Initialise();

	return l_bReturn;
}

void DeferredShadingRenderTechnique :: DoCleanup()
{
	m_pShaderCamera.reset();
	m_pGeometryBuffers->Cleanup();

	m_pDsShaderProgram->Cleanup();

	m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

	for( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
	{
		m_pDsTexAttachs[i]->Detach();
	}

	m_pDsDepthAttach->Detach();
	m_pDsFrameBuffer->Unbind();

	for( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
	{
		m_pDsTextures[i]->Cleanup();
	}

	m_pDsBufDepth->Cleanup();
}

bool DeferredShadingRenderTechnique :: DoBeginRender()
{
	bool l_bReturn = true;

	if( l_bReturn )
	{
		l_bReturn = m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	return l_bReturn;
}

void DeferredShadingRenderTechnique :: DoEndRender()
{
	Size const & l_size = m_pRenderTarget->GetSize();
	Size l_halfSize( l_size.width() / 2, l_size.height() / 2 );
	Pipeline * l_pPipeline = m_pRenderSystem->GetPipeline();
	ContextRPtr l_pContext = m_pRenderSystem->GetCurrentContext();

	m_pDsFrameBuffer->Unbind();

	if( m_pViewport )
	{
		bool l_bReturn = true;
		m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
		m_pRenderTarget->GetDepthStencilState()->Apply();
		m_pRenderTarget->GetRasteriserState()->Apply();
		m_pRenderTarget->GetRenderer()->BeginScene();

		l_pContext->Clear( eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
		m_pViewport->SetSize( m_size );
		m_pViewport->Render();
		l_pPipeline->MatrixMode( eMTXMODE_MODEL );
		l_pPipeline->LoadIdentity();
		l_pPipeline->MatrixMode( eMTXMODE_VIEW );
		l_pPipeline->LoadIdentity();

		if( m_pShaderCamera )
		{
			Point3r l_position = m_pRenderTarget->GetCamera()->GetParent()->GetDerivedPosition();
			m_pShaderCamera->SetValue( l_position );
			m_pDsShaderProgram->Begin( 0, 1 );
			l_pPipeline->ApplyMatrices( *m_pDsShaderProgram );

			for( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
			{
				l_bReturn = m_pDsTextures[i]->Bind();
			}

			if( l_bReturn )
			{
				m_pGeometryBuffers->Draw( eTOPOLOGY_TRIANGLES, m_pDsShaderProgram, uint32_t( m_arrayVertex.size() ), 0 );

				for( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
				{
					m_pDsTextures[i]->Unbind();
				}
			}

			m_pDsShaderProgram->End();
		}

		m_pRenderTarget->GetRenderer()->EndScene();
		m_pFrameBuffer->Unbind();

#if DEBUG_BUFFERS
		for( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			if( m_pDsTexAttachs[i]->DownloadBuffer( m_pDsTextures[i]->GetBuffer() ) )
			{
				Image l_image( g_strNames[i], *m_pDsTextures[i]->GetBuffer() );
				Image::BinaryLoader()( const_cast< const Image & >( l_image ), g_strNames[i] + cuT( ".bmp" ) );
			}
		}
#endif
	}
}

//*************************************************************************************************
