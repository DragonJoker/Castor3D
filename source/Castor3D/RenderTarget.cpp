#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/RenderTarget.hpp"
#include "Castor3D/RenderTechnique.hpp"
#include "Castor3D/Texture.hpp"
#include "Castor3D/FrameBuffer.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Camera.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Pipeline.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/Context.hpp"
#include "Castor3D/Sampler.hpp"
#include "Castor3D/DepthStencilState.hpp"
#include "Castor3D/RasterState.hpp"
#include "Castor3D/BlendState.hpp"
#include "Castor3D/Parameter.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

TargetRenderer :: TargetRenderer( RenderSystem * p_pRenderSystem )
	:	Renderer< RenderTarget, TargetRenderer >( p_pRenderSystem )
{
}

TargetRenderer :: ~TargetRenderer()
{
}

//*************************************************************************************************

bool RenderTarget::stFRAME_BUFFER :: Create( RenderTarget * p_renderTarget )
{
	m_pRenderTarget	= p_renderTarget;

	m_pFrameBuffer	= m_pRenderTarget->CreateFrameBuffer();
	m_pColorTexture	= m_pRenderTarget->CreateDynamicTexture();
	m_pColorAttach	= m_pRenderTarget->CreateAttachment( m_pColorTexture );
	m_pDepthBuffer	= m_pFrameBuffer->CreateDepthRenderBuffer( m_pRenderTarget->GetDepthFormat() );
	m_pDepthAttach	= m_pRenderTarget->CreateAttachment( m_pDepthBuffer );

	SamplerSPtr l_pSampler = m_pRenderTarget->GetEngine()->CreateSampler( cuT( "DefaultRTSampler" ) );
	l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_ANISOTROPIC );
	l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_ANISOTROPIC );

	m_pColorTexture->SetSampler( l_pSampler );
	m_pColorTexture->SetRenderTarget( true );

	return true;
}

void RenderTarget::stFRAME_BUFFER :: Destroy()
{
	m_pColorTexture->Destroy();
	m_pDepthBuffer->Destroy();

	m_pFrameBuffer->Destroy();

	m_pDepthAttach.reset();
	m_pDepthBuffer.reset();
	m_pColorAttach.reset();
	m_pColorTexture.reset();
	m_pFrameBuffer.reset();

	m_pRenderTarget = NULL;
}

bool RenderTarget::stFRAME_BUFFER :: Initialise( Size const & p_size )
{
	bool l_bReturn = false;
	m_pColorTexture->SetDimension( eTEXTURE_DIMENSION_2D );
	m_pColorTexture->SetImage( p_size, m_pRenderTarget->GetPixelFormat() );
	Size l_size = m_pColorTexture->GetDimensions();
	m_pFrameBuffer->Create( 0 );
	m_pColorTexture->Create();
	m_pColorTexture->Initialise( 0 );
	m_pDepthBuffer->Create();
	m_pDepthBuffer->Initialise( l_size );

	if( m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
	{
		m_pColorAttach->Attach( eATTACHMENT_POINT_COLOUR0,	m_pFrameBuffer, eTEXTURE_TARGET_2D );
		m_pDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,	m_pFrameBuffer );
		m_pFrameBuffer->Unbind();
		l_bReturn = true;
	}

	return l_bReturn;
}

void RenderTarget::stFRAME_BUFFER :: Cleanup()
{
	m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
	m_pColorAttach->Detach();
	m_pDepthAttach->Detach();
	m_pFrameBuffer->Unbind();

	m_pColorTexture->Cleanup();
	m_pDepthBuffer->Cleanup();
}

//*************************************************************************************************

uint32_t RenderTarget::sm_uiCount = 0;

RenderTarget :: RenderTarget( Engine * p_pRoot, eTARGET_TYPE p_eTargetType )
	:	Renderable< RenderTarget, TargetRenderer >	( p_pRoot												)
	,	m_eTargetType								( p_eTargetType											)
	,	m_ePixelFormat								( ePIXEL_FORMAT_A8R8G8B8								)
	,	m_eDepthFormat								( ePIXEL_FORMAT_DEPTH24S8								)
	,	m_bInitialised								( false													)
	,	m_size										( Size( 100, 100 )										)
	,	m_pRenderTechnique							(														)
	,	m_bMultisampling							( false													)
	,	m_iSamplesCount								( 0														)
	,	m_bStereo									( false													)
	,	m_rIntraOcularDistance						( 0														)
	,	m_bDeferredRendering						( false													)
	,	m_uiIndex									( ++sm_uiCount											)
	,	m_strTechniqueName							( cuT( "direct"	)										)
{
	m_wpDepthStencilState	= p_pRoot->CreateDepthStencilState( cuT( "RenderTargetState_" ) + str_utils::to_string( m_uiIndex ) );
	m_wpRasteriserState		= p_pRoot->CreateRasteriserState( cuT( "RenderTargetState_" ) + str_utils::to_string( m_uiIndex ) );
}

RenderTarget :: ~RenderTarget()
{
}

void RenderTarget :: Initialise()
{
	m_fbLeftEye.Create( this );
	m_fbRightEye.Create( this );

	GetRenderer()->Initialise();

	if( !m_pRenderTechnique )
	{
		Parameters l_params;

		if( m_strTechniqueName == cuT( "msaa" ) )
		{
			m_bMultisampling = true;
			l_params.Add( cuT( "samples_count" ), m_iSamplesCount );
		}
		else if( m_strTechniqueName == cuT( "ssaa" ) )
		{
			l_params.Add( cuT( "samples_count" ), m_iSamplesCount );
		}
		else if( m_strTechniqueName == cuT( "deferred" ) )
		{
			m_bDeferredRendering = true;
		}

		m_pRenderTechnique = m_pEngine->CreateTechnique( m_strTechniqueName, *this, l_params );
	}

	m_fbLeftEye.Initialise( m_size );

	m_size = m_fbLeftEye.m_pColorTexture->GetDimensions();

	m_fbRightEye.Initialise( m_size );

	m_pRenderTechnique->Create();
	m_pRenderTechnique->Initialise();
	m_bInitialised = true;
}

void RenderTarget :: Cleanup()
{
	m_bInitialised = false;
	m_pRenderTechnique->Cleanup();

	m_fbLeftEye.Cleanup();
	m_fbRightEye.Cleanup();

	m_pRenderTechnique->Destroy();

	m_fbLeftEye.Destroy();
	m_fbRightEye.Destroy();

	GetRenderer()->Cleanup();

	m_pRenderTechnique.reset();
}

void RenderTarget :: Render( double p_dFrameTime )
{
	Engine *			l_pEngine		= GetEngine();
	SceneSPtr			l_pScene		= GetScene();
	RenderSystem *		l_pRenderSystem	= l_pEngine->GetRenderSystem();

	if( l_pScene && l_pScene->HasChanged() )
	{
		l_pScene->InitialiseGeometries();
	}

	if( m_bInitialised && l_pScene )
	{
		CameraSPtr l_pCamera = GetCamera();

		if( m_bStereo && m_rIntraOcularDistance > 0 && l_pRenderSystem->IsStereoAvailable() )
		{
			if( GetCameraLEye() && GetCameraREye() )
			{
				DoRender( m_fbLeftEye, GetCameraLEye(), p_dFrameTime );
				DoRender( m_fbRightEye, GetCameraREye(), p_dFrameTime );
			}
		}
		else if( l_pCamera )
		{
			DoRender( m_fbLeftEye, l_pCamera, p_dFrameTime );
		}
	}
}

DynamicTextureSPtr RenderTarget :: CreateDynamicTexture()const
{
	return m_pEngine->GetRenderSystem()->CreateDynamicTexture();
}

RenderBufferAttachmentSPtr RenderTarget :: CreateAttachment( RenderBufferSPtr p_pRenderBuffer )const
{
	RenderBufferAttachmentSPtr	l_pReturn;
	TargetRendererSPtr			l_pRenderer	= GetRenderer();

	if( l_pRenderer )
	{
		l_pReturn = l_pRenderer->CreateAttachment( p_pRenderBuffer );
	}

	return l_pReturn;
}

TextureAttachmentSPtr RenderTarget :: CreateAttachment( DynamicTextureSPtr p_pTexture )const
{
	TextureAttachmentSPtr	l_pReturn;
	TargetRendererSPtr		l_pRenderer	= GetRenderer();

	if( l_pRenderer )
	{
		l_pReturn = l_pRenderer->CreateAttachment( p_pTexture );
	}

	return l_pReturn;
}

FrameBufferSPtr RenderTarget :: CreateFrameBuffer()const
{
	FrameBufferSPtr		l_pReturn;
	TargetRendererSPtr	l_pRenderer	= GetRenderer();

	if( l_pRenderer )
	{
		l_pReturn = l_pRenderer->CreateFrameBuffer();
	}

	return l_pReturn;
}

eTOPOLOGY RenderTarget :: GetPrimitiveType()const
{
	return (GetCamera() ? GetCamera()->GetPrimitiveType() : eTOPOLOGY_COUNT);
}

eVIEWPORT_TYPE RenderTarget :: GetViewportType()const
{
	return (GetCamera() ? GetCamera()->GetViewportType() : eVIEWPORT_TYPE_COUNT);
}

void RenderTarget :: SetPrimitiveType( eTOPOLOGY val )
{
	if( GetCamera() )
	{
		GetCamera()->SetPrimitiveType( val);
	}
}

void RenderTarget :: SetViewportType( eVIEWPORT_TYPE val )
{
	if( GetCamera() )
	{
		GetCamera()->SetViewportType( val );
	}
}

void RenderTarget :: SetCamera( CameraSPtr p_pCamera )
{
	SceneNode *		l_pCamNode;
	SceneNodeSPtr	l_pLECamNode;
	SceneNodeSPtr	l_pRECamNode;
	String			l_strLENodeName;
	String			l_strRENodeName;
	String			l_strIndex = cuT( "_RT" ) + str_utils::to_string( m_uiIndex );
	SceneSPtr		l_pScene = GetScene();

	if( l_pScene )
	{
		if( GetCameraLEye() )
		{
			l_pScene->RemoveCamera( GetCameraLEye() );
		}

		if( GetCameraREye() )
		{
			l_pScene->RemoveCamera( GetCameraREye() );
		}
	}

	if( GetCamera() )
	{
		l_pCamNode		= GetCamera()->GetParent();
		l_strLENodeName	= l_pCamNode->GetName() + l_strIndex + cuT( "_LEye" );
		l_strRENodeName	= l_pCamNode->GetName() + l_strIndex + cuT( "_REye" );

		l_pCamNode->DetachChild( l_pCamNode->GetChild( l_strLENodeName ) );
		l_pCamNode->DetachChild( l_pCamNode->GetChild( l_strRENodeName ) );

		if( l_pScene )
		{
			l_pScene->RemoveNode( l_pScene->GetNode( l_strLENodeName ) );
			l_pScene->RemoveNode( l_pScene->GetNode( l_strRENodeName ) );
		}
	}

	m_pCamera = p_pCamera;

	if( p_pCamera )
	{
		l_pCamNode = p_pCamera->GetParent();

		if( l_pScene )
		{
			l_strLENodeName	= l_pCamNode->GetName() + l_strIndex + cuT( "_LEye" );
			l_strRENodeName	= l_pCamNode->GetName() + l_strIndex + cuT( "_REye" );

			l_pLECamNode = l_pScene->CreateSceneNode( l_strLENodeName, l_pScene->GetNode( l_pCamNode->GetName() ) );
			l_pRECamNode = l_pScene->CreateSceneNode( l_strRENodeName, l_pScene->GetNode( l_pCamNode->GetName() ) );

			l_pLECamNode->Translate( -m_rIntraOcularDistance / 2, 0, 0 );
			l_pRECamNode->Translate( m_rIntraOcularDistance / 2, 0, 0 );

			m_pCameraLEye = l_pScene->CreateCamera( p_pCamera->GetName() + l_strIndex + cuT( "_LEye" ), l_pLECamNode, p_pCamera->GetViewport() );
			m_pCameraREye = l_pScene->CreateCamera( p_pCamera->GetName() + l_strIndex + cuT( "_REye" ), l_pRECamNode, p_pCamera->GetViewport() );
		}
	}
}

void RenderTarget :: SetIntraOcularDistance( real p_rIod )
{
	if( GetCameraLEye() && GetCameraREye() )
	{
		SceneNode * l_pLECamNode = GetCameraLEye()->GetParent();
		SceneNode * l_pRECamNode = GetCameraREye()->GetParent();
		l_pLECamNode->Translate( m_rIntraOcularDistance / 2, 0, 0 );
		l_pRECamNode->Translate( -m_rIntraOcularDistance / 2, 0, 0 );
		l_pLECamNode->Translate( -p_rIod / 2, 0, 0 );
		l_pRECamNode->Translate( p_rIod / 2, 0, 0 );
	}

	m_rIntraOcularDistance = p_rIod;
}

void RenderTarget :: SetSize( Size const & p_size )
{
	m_size = p_size;
}

void RenderTarget :: Resize()
{
	m_fbLeftEye.m_pFrameBuffer->Resize( m_size );
	m_fbRightEye.m_pFrameBuffer->Resize( m_size );
}

void RenderTarget :: SetTechnique( Castor::String const & p_strName )
{
	m_strTechniqueName = p_strName;
	m_bDeferredRendering = p_strName == cuT( "deferred" );
	m_bMultisampling = p_strName == cuT( "msaa" );
}

void RenderTarget :: DoRender( RenderTarget::stFRAME_BUFFER & p_fb, CameraSPtr p_pCamera, double p_dFrameTime )
{
	Engine *			l_pEngine		= GetEngine();
	SceneSPtr			l_pScene		= GetScene();
	TargetRendererSPtr	l_pRenderer		= GetRenderer();
	RenderSystem *		l_pRenderSystem	= l_pEngine->GetRenderSystem();
	ContextRPtr			l_pContext		= l_pRenderSystem->GetCurrentContext();

	m_pCurrentFrameBuffer = p_fb.m_pFrameBuffer;
	m_pCurrentCamera = p_pCamera;
	l_pRenderer->BeginScene();

	if( m_pRenderTechnique->BeginRender() )
	{
		l_pContext->Clear( eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
		m_pRenderTechnique->Render( *l_pScene, *p_pCamera, GetPrimitiveType(), p_dFrameTime );
		m_pRenderTechnique->EndRender();
	}

	l_pRenderer->EndScene();
	m_pCurrentFrameBuffer.reset();
	m_pCurrentCamera.reset();
}

//*************************************************************************************************
