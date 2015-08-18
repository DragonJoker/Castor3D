﻿#include "RenderTarget.hpp"

#include "Engine.hpp"
#include "RenderTechnique.hpp"
#include "DynamicTexture.hpp"
#include "FrameBuffer.hpp"
#include "ColourRenderBuffer.hpp"
#include "DepthStencilRenderBuffer.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "Engine.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Pipeline.hpp"
#include "Buffer.hpp"
#include "Context.hpp"
#include "Sampler.hpp"
#include "DepthStencilState.hpp"
#include "RasteriserState.hpp"
#include "BlendState.hpp"
#include "Parameter.hpp"
#include "RenderSystem.hpp"

#include <Logger.hpp>
#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderTarget::TextLoader::TextLoader( String const & p_tabs, File::eENCODING_MODE p_eEncodingMode )
		:	Loader< RenderTarget, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
		,	m_tabs( p_tabs )
	{
	}

	bool RenderTarget::TextLoader::operator()( RenderTarget const & p_target, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "RenderTarget::Write" ) );
		bool l_bReturn = p_file.WriteText( m_tabs + cuT( "render_target\n" ) + m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_bReturn && p_target.GetScene() )
		{
			l_bReturn = p_file.WriteText( m_tabs + cuT( "\tscene \"" ) + p_target.GetScene()->GetName() + cuT( "\"\n" ) ) > 0;
		}

		if ( l_bReturn && p_target.GetCamera() )
		{
			l_bReturn = p_file.WriteText( m_tabs + cuT( "\tcamera \"" ) + p_target.GetCamera()->GetName() + cuT( "\"\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.Print( 256, ( m_tabs + cuT( "\tsize %d %d\n" ) ).c_str(), p_target.GetSize().width(), p_target.GetSize().height() ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( m_tabs + cuT( "\tformat " ) + Castor::PF::GetFormatName( p_target.GetPixelFormat() ) + cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( m_tabs + cuT( "\tdepth " ) + Castor::PF::GetFormatName( p_target.GetDepthFormat() ) + cuT( "\n" ) ) > 0;
		}

		if ( l_bReturn && p_target.GetTechnique()->GetName() == cuT( "MSAA" ) )
		{
			l_bReturn = p_file.WriteText( m_tabs + cuT( "\tmsaa true\n" ) ) > 0;
		}

		if ( l_bReturn && p_target.GetTechnique()->GetName() == cuT( "SSAA" ) )
		{
			l_bReturn = p_file.WriteText( m_tabs + cuT( "\tssaa true\n" ) ) > 0;
		}

		if ( l_bReturn && p_target.GetTechnique()->GetName() == cuT( "deferred" ) )
		{
			l_bReturn = p_file.WriteText( m_tabs + cuT( "\tdeferred true\n" ) ) > 0;
		}

		if ( l_bReturn && p_target.IsUsingStereo() )
		{
			l_bReturn = p_file.Print( 256, ( m_tabs + cuT( "\tstereo %.2f\n" ) ).c_str(), p_target.GetIntraOcularDistance() ) > 0;
		}

		p_file.WriteText( m_tabs + cuT( "}\n" ) );
		return l_bReturn;
	}

	//*************************************************************************************************

	RenderTarget::BinaryParser::BinaryParser( Path const & p_path )
		:	Castor3D::BinaryParser< RenderTarget >( p_path )
	{
	}

	bool RenderTarget::BinaryParser::Fill( RenderTarget const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_TARGET );

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetScene()->GetName(), eCHUNK_TYPE_TARGET_SCENE, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetCamera()->GetName(), eCHUNK_TYPE_TARGET_CAMERA, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetSize(), eCHUNK_TYPE_TARGET_SIZE, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetPixelFormat(), eCHUNK_TYPE_TARGET_FORMAT, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetDepthFormat(), eCHUNK_TYPE_TARGET_DEPTH, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetTechnique()->GetName(), eCHUNK_TYPE_TARGET_TECHNIQUE, l_chunk );
		}

		if ( l_bReturn && ( p_obj.GetTechnique()->GetName() == cuT( "MSAA" ) || p_obj.GetTechnique()->GetName() == cuT( "SSAA" ) ) )
		{
			l_bReturn = DoFillChunk( p_obj.GetSamplesCount(), eCHUNK_TYPE_TARGET_SAMPLES, l_chunk );
		}

		if ( l_bReturn && p_obj.IsUsingStereo() )
		{
			float l_dist = float( p_obj.GetIntraOcularDistance() );
			l_bReturn = DoFillChunk( l_dist, eCHUNK_TYPE_TARGET_STEREO, l_chunk );
		}

		if ( l_bReturn )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_bReturn;
	}

	bool RenderTarget::BinaryParser::Parse( RenderTarget & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		String l_name;
		String l_camName;
		SceneSPtr l_scene;
		Size l_size;
		ePIXEL_FORMAT l_format;
		uint32_t l_samples = 0;
		float l_dist = 0;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_TARGET_SCENE:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						l_scene = p_obj.GetEngine()->GetSceneManager().find( l_name );
						p_obj.SetScene( l_scene );

						if ( l_scene && !l_camName.empty() )
						{
							p_obj.SetCamera( l_scene->GetCamera( l_camName ) );
						}
					}

					break;

				case eCHUNK_TYPE_TARGET_CAMERA:
					l_bReturn = DoParseChunk( l_camName, l_chunk );

					if ( l_bReturn && l_scene )
					{
						p_obj.SetCamera( l_scene->GetCamera( l_camName ) );
					}

					break;

				case eCHUNK_TYPE_TARGET_SIZE:
					l_bReturn = DoParseChunk( l_size, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetSize( l_size );
					}

					break;

				case eCHUNK_TYPE_TARGET_FORMAT:
					l_bReturn = DoParseChunk( l_format, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetPixelFormat( l_format );
					}

					break;

				case eCHUNK_TYPE_TARGET_DEPTH:
					l_bReturn = DoParseChunk( l_format, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetDepthFormat( l_format );
					}

					break;

				case eCHUNK_TYPE_TARGET_TECHNIQUE:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetTechnique( l_name );
					}

					break;

				case eCHUNK_TYPE_TARGET_SAMPLES:
					l_bReturn = DoParseChunk( l_samples, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetSamplesCount( l_samples );
					}

					break;

				case eCHUNK_TYPE_TARGET_STEREO:
					l_bReturn = DoParseChunk( l_dist, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetIntraOcularDistance( real( l_dist ) );
					}

					break;

				default:
					l_bReturn = false;
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	bool RenderTarget::stFRAME_BUFFER::Create( RenderTarget * p_renderTarget )
	{
		m_pRenderTarget = p_renderTarget;
		m_pFrameBuffer = m_pRenderTarget->CreateFrameBuffer();
		m_pColorTexture = m_pRenderTarget->CreateDynamicTexture();
		m_pColorAttach = m_pRenderTarget->CreateAttachment( m_pColorTexture );
		m_pDepthBuffer = m_pFrameBuffer->CreateDepthStencilRenderBuffer( m_pRenderTarget->GetDepthFormat() );
		m_pDepthAttach = m_pRenderTarget->CreateAttachment( m_pDepthBuffer );
		SamplerSPtr l_pSampler = m_pRenderTarget->GetEngine()->CreateSampler( RenderTarget::DefaultSamplerName );
		l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_ANISOTROPIC );
		l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_ANISOTROPIC );
		m_pColorTexture->SetSampler( l_pSampler );
		m_pColorTexture->SetRenderTarget( true );
		return true;
	}

	void RenderTarget::stFRAME_BUFFER::Destroy()
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

	bool RenderTarget::stFRAME_BUFFER::Initialise( uint32_t p_index, Size const & p_size )
	{
		bool l_bReturn = false;
		m_pColorTexture->SetDimension( eTEXTURE_DIMENSION_2D );
		m_pColorTexture->SetImage( p_size, m_pRenderTarget->GetPixelFormat() );
		Size l_size = m_pColorTexture->GetDimensions();
		m_pFrameBuffer->Create( 0 );
		m_pColorTexture->Create();
		m_pColorTexture->Initialise( p_index );
		m_pDepthBuffer->Create();
		m_pDepthBuffer->Initialise( l_size );

		if ( m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
		{
			m_pColorAttach->Attach( eATTACHMENT_POINT_COLOUR0,	m_pFrameBuffer, eTEXTURE_TARGET_2D );
			m_pDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,	m_pFrameBuffer );
			m_pFrameBuffer->Unbind();
			l_bReturn = true;
		}

		return l_bReturn;
	}

	void RenderTarget::stFRAME_BUFFER::Cleanup()
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
	const Castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine * p_pRoot, eTARGET_TYPE p_eTargetType )
		: m_pEngine( p_pRoot )
		, m_eTargetType( p_eTargetType )
		, m_ePixelFormat( ePIXEL_FORMAT_A8R8G8B8 )
		, m_eDepthFormat( ePIXEL_FORMAT_DEPTH24S8 )
		, m_bInitialised( false )
		, m_size( Size( 100, 100 ) )
		, m_pRenderTechnique( )
		, m_bMultisampling( false )
		, m_iSamplesCount( 0 )
		, m_bStereo( false )
		, m_rIntraOcularDistance( 0 )
		, m_bDeferredRendering( false )
		, m_uiIndex( ++sm_uiCount )
		, m_strTechniqueName( cuT( "direct" ) )
	{
		m_wpDepthStencilState = p_pRoot->CreateDepthStencilState( cuT( "RenderTargetState_" ) + str_utils::to_string( m_uiIndex ) );
		m_wpRasteriserState = p_pRoot->CreateRasteriserState( cuT( "RenderTargetState_" ) + str_utils::to_string( m_uiIndex ) );
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::Initialise( uint32_t p_index )
	{
		m_fbLeftEye.Create( this );
		m_fbRightEye.Create( this );

		if ( !m_pRenderTechnique )
		{
			Parameters l_params;

			if ( m_strTechniqueName == cuT( "msaa" ) )
			{
				m_bMultisampling = true;
				l_params.Add( cuT( "samples_count" ), m_iSamplesCount );
			}
			else if ( m_strTechniqueName == cuT( "ssaa" ) )
			{
				l_params.Add( cuT( "samples_count" ), m_iSamplesCount );
			}
			else if ( m_strTechniqueName == cuT( "deferred" ) )
			{
				m_bDeferredRendering = true;
			}

			m_pRenderTechnique = m_pEngine->CreateTechnique( m_strTechniqueName, *this, l_params );
		}

		m_fbLeftEye.Initialise( p_index, m_size );
		m_size = m_fbLeftEye.m_pColorTexture->GetDimensions();
		m_fbRightEye.Initialise( p_index, m_size );
		m_pRenderTechnique->Create();
		uint32_t l_index = p_index;
		m_pRenderTechnique->Initialise( l_index );
		m_bInitialised = true;
	}

	void RenderTarget::Cleanup()
	{
		m_bInitialised = false;
		m_pRenderTechnique->Cleanup();
		m_fbLeftEye.Cleanup();
		m_fbRightEye.Cleanup();
		m_pRenderTechnique->Destroy();
		m_fbLeftEye.Destroy();
		m_fbRightEye.Destroy();
		m_pRenderTechnique.reset();
	}

	void RenderTarget::Render( double p_dFrameTime )
	{
		SceneSPtr l_pScene = GetScene();

		if ( l_pScene && l_pScene->HasChanged() )
		{
			l_pScene->InitialiseGeometries();
		}

		if ( m_bInitialised && l_pScene )
		{
			if ( m_bStereo && m_rIntraOcularDistance > 0 && GetEngine()->GetRenderSystem()->IsStereoAvailable() )
			{
				if ( GetCameraLEye() && GetCameraREye() )
				{
					DoRender( m_fbLeftEye, GetCameraLEye(), p_dFrameTime );
					DoRender( m_fbRightEye, GetCameraREye(), p_dFrameTime );
				}
				else
				{
					CameraSPtr l_pCamera = GetCamera();

					if ( l_pCamera )
					{
						DoRender( m_fbLeftEye, GetCamera(), p_dFrameTime );
					}
				}
			}
			else
			{
				CameraSPtr l_pCamera = GetCamera();

				if ( l_pCamera )
				{
					DoRender( m_fbLeftEye, GetCamera(), p_dFrameTime );
				}
			}
		}
	}

	DynamicTextureSPtr RenderTarget::CreateDynamicTexture()const
	{
		return m_pEngine->GetRenderSystem()->CreateDynamicTexture();
	}

	eTOPOLOGY RenderTarget::GetPrimitiveType()const
	{
		return ( GetCamera() ? GetCamera()->GetPrimitiveType() : eTOPOLOGY_COUNT );
	}

	eVIEWPORT_TYPE RenderTarget::GetViewportType()const
	{
		return ( GetCamera() ? GetCamera()->GetViewportType() : eVIEWPORT_TYPE_COUNT );
	}

	void RenderTarget::SetPrimitiveType( eTOPOLOGY val )
	{
		if ( GetCamera() )
		{
			GetCamera()->SetPrimitiveType( val );
		}
	}

	void RenderTarget::SetViewportType( eVIEWPORT_TYPE val )
	{
		if ( GetCamera() )
		{
			GetCamera()->SetViewportType( val );
		}
	}

	void RenderTarget::SetCamera( CameraSPtr p_pCamera )
	{
		SceneNodeSPtr l_pCamNode;
		SceneNodeSPtr l_pLECamNode;
		SceneNodeSPtr l_pRECamNode;
		String l_strLENodeName;
		String l_strRENodeName;
		String l_strIndex = cuT( "_RT" ) + str_utils::to_string( m_uiIndex );
		SceneSPtr l_pScene = GetScene();

		if ( l_pScene )
		{
			if ( GetCameraLEye() )
			{
				l_pScene->RemoveCamera( GetCameraLEye() );
			}

			if ( GetCameraREye() )
			{
				l_pScene->RemoveCamera( GetCameraREye() );
			}
		}

		if ( GetCamera() )
		{
			l_pCamNode = GetCamera()->GetParent();
			l_strLENodeName = l_pCamNode->GetName() + l_strIndex + cuT( "_LEye" );
			l_strRENodeName = l_pCamNode->GetName() + l_strIndex + cuT( "_REye" );
			l_pCamNode->DetachChild( l_pCamNode->GetChild( l_strLENodeName ) );
			l_pCamNode->DetachChild( l_pCamNode->GetChild( l_strRENodeName ) );

			if ( l_pScene )
			{
				l_pScene->RemoveNode( l_pScene->GetNode( l_strLENodeName ) );
				l_pScene->RemoveNode( l_pScene->GetNode( l_strRENodeName ) );
			}
		}

		m_pCamera = p_pCamera;

		if ( p_pCamera )
		{
			l_pCamNode = p_pCamera->GetParent();

			if ( l_pScene )
			{
				l_strLENodeName	= l_pCamNode->GetName() + l_strIndex + cuT( "_LEye" );
				l_strRENodeName	= l_pCamNode->GetName() + l_strIndex + cuT( "_REye" );
				l_pLECamNode = l_pScene->CreateSceneNode( l_strLENodeName, l_pScene->GetNode( l_pCamNode->GetName() ) );
				l_pRECamNode = l_pScene->CreateSceneNode( l_strRENodeName, l_pScene->GetNode( l_pCamNode->GetName() ) );
				l_pLECamNode->Translate( Point3r( -m_rIntraOcularDistance / 2, 0, 0 ) );
				l_pRECamNode->Translate( Point3r( m_rIntraOcularDistance / 2, 0, 0 ) );
				m_pCameraLEye = l_pScene->CreateCamera( p_pCamera->GetName() + l_strIndex + cuT( "_LEye" ), l_pLECamNode, p_pCamera->GetViewport() );
				m_pCameraREye = l_pScene->CreateCamera( p_pCamera->GetName() + l_strIndex + cuT( "_REye" ), l_pRECamNode, p_pCamera->GetViewport() );
			}
		}
	}

	void RenderTarget::SetIntraOcularDistance( real p_rIod )
	{
		if ( GetCameraLEye() && GetCameraREye() )
		{
			SceneNodeSPtr l_pLECamNode = GetCameraLEye()->GetParent();
			SceneNodeSPtr l_pRECamNode = GetCameraREye()->GetParent();
			l_pLECamNode->Translate( Point3r( m_rIntraOcularDistance / 2, 0, 0 ) );
			l_pRECamNode->Translate( Point3r( -m_rIntraOcularDistance / 2, 0, 0 ) );
			l_pLECamNode->Translate( Point3r( -p_rIod / 2, 0, 0 ) );
			l_pRECamNode->Translate( Point3r( p_rIod / 2, 0, 0 ) );
		}

		m_rIntraOcularDistance = p_rIod;
	}

	void RenderTarget::SetSize( Size const & p_size )
	{
		m_size = p_size;
	}

	void RenderTarget::Resize()
	{
		m_fbLeftEye.m_pFrameBuffer->Resize( m_size );
		m_fbRightEye.m_pFrameBuffer->Resize( m_size );
	}

	void RenderTarget::SetTechnique( Castor::String const & p_strName )
	{
		m_strTechniqueName = p_strName;
		m_bDeferredRendering = p_strName == cuT( "deferred" );
		m_bMultisampling = p_strName == cuT( "msaa" );
	}

	void RenderTarget::DoRender( RenderTarget::stFRAME_BUFFER & p_fb, CameraSPtr p_pCamera, double p_dFrameTime )
	{
		m_pCurrentFrameBuffer = p_fb.m_pFrameBuffer;
		m_pCurrentCamera = p_pCamera;
		Clear();

		if ( m_pRenderTechnique->BeginRender() )
		{
			SceneSPtr l_pScene = GetScene();
			GetEngine()->GetRenderSystem()->GetCurrentContext()->Clear( eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
			l_pScene->RenderBackground( *p_pCamera );
			m_pRenderTechnique->Render( *l_pScene, *p_pCamera, GetPrimitiveType(), p_dFrameTime );
			m_pRenderTechnique->EndRender();
		}

		m_pCurrentFrameBuffer.reset();
		m_pCurrentCamera.reset();
#if DEBUG_BUFFERS
		p_fb.m_pColorAttach->DownloadBuffer( p_fb.m_pColorTexture->GetBuffer() );
		const Image l_tmp( cuT( "tmp" ), *p_fb.m_pColorTexture->GetBuffer() );
		Image::BinaryLoader()( l_tmp, File::GetUserDirectory() / cuT( "RenderTargetTexture_" ) + str_utils::to_string( ptrdiff_t( p_fb.m_pColorTexture.get() ), 16 ) + cuT( ".bmp" ) );
#endif
	}
}
