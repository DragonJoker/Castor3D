#include "RenderTarget.hpp"

#include "CameraCache.hpp"
#include "Engine.hpp"
#include "MaterialCache.hpp"
#include "OverlayCache.hpp"
#include "SamplerCache.hpp"
#include "SceneCache.hpp"
#include "SceneNodeCache.hpp"
#include "TargetCache.hpp"
#include "TechniqueCache.hpp"

#include "Context.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Miscellaneous/Parameter.hpp"
#include "PostEffect/PostEffectFactory.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Log/Logger.hpp>
#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	RenderTarget::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< RenderTarget >{ p_tabs }
	{
	}

	bool RenderTarget::TextWriter::operator()( RenderTarget const & p_target, TextFile & p_file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing RenderTarget" ) );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "render_target\n" ) + m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget name" );

		if ( l_return && p_target.GetScene() )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tscene \"" ) + p_target.GetScene()->GetName() + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget scene" );
		}

		if ( l_return && p_target.GetCamera() )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tcamera \"" ) + p_target.GetCamera()->GetName() + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget camera" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, ( m_tabs + cuT( "\tsize %d %d\n" ) ).c_str(), p_target.GetSize().width(), p_target.GetSize().height() ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget size" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tformat " ) + Castor::PF::GetFormatName( p_target.GetPixelFormat() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget format" );
		}

		if ( l_return && p_target.IsUsingStereo() )
		{
			l_return = p_file.Print( 256, ( m_tabs + cuT( "\tstereo %.2f\n" ) ).c_str(), p_target.GetIntraOcularDistance() ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget stereo configuration" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttone_mapping \"" ) + p_target.m_toneMapping->GetName() + cuT( "\"" ) )
					   && p_target.m_toneMapping->WriteInto( p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget tone mapping" );
		}

		if ( l_return && p_target.m_renderTechnique )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttechnique \"" ) + p_target.m_renderTechnique->GetName() + cuT( "\"" ) )
					   && p_target.m_renderTechnique->WriteInto( p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget technique" );
		}

		if ( l_return )
		{
			for ( auto const & l_effect : p_target.m_postEffects )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tpostfx \"" ) + l_effect->GetName() + cuT( "\"" ) )
						   && l_effect->WriteInto( p_file )
						   && p_file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< RenderTarget >::CheckError( l_return, "RenderTarget post effect" );
			}
		}

		p_file.WriteText( m_tabs + cuT( "}\n" ) );
		return l_return;
	}

	//*************************************************************************************************

	RenderTarget::stFRAME_BUFFER::stFRAME_BUFFER( RenderTarget & p_renderTarget )
		: m_renderTarget{ p_renderTarget }
		, m_colorTexture{ *p_renderTarget.GetEngine() }
	{
	}

	bool RenderTarget::stFRAME_BUFFER::Create()
	{
		m_frameBuffer = m_renderTarget.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		SamplerSPtr l_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( RenderTarget::DefaultSamplerName + string::to_string( m_renderTarget.m_index ) );
		auto l_colourTexture = m_renderTarget.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_pColorAttach = m_frameBuffer->CreateAttachment( l_colourTexture );
		m_colorTexture.SetTexture( l_colourTexture );
		m_colorTexture.SetSampler( l_sampler );
		return true;
	}

	void RenderTarget::stFRAME_BUFFER::Destroy()
	{
		m_frameBuffer->Destroy();
		m_pColorAttach.reset();
		m_colorTexture.SetTexture( nullptr );
		m_frameBuffer.reset();
	}

	bool RenderTarget::stFRAME_BUFFER::Initialise( uint32_t p_index, Size const & p_size )
	{
		bool l_return = false;
		m_colorTexture.SetIndex( p_index );
		m_colorTexture.GetTexture()->GetImage().SetSource( p_size, m_renderTarget.GetPixelFormat() );
		Size l_size = m_colorTexture.GetTexture()->GetImage().GetDimensions();
		m_frameBuffer->Create();
		m_colorTexture.GetTexture()->Create();
		m_colorTexture.GetTexture()->Initialise();
		m_frameBuffer->Initialise( l_size );

		if ( m_frameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
		{
			m_frameBuffer->Attach( eATTACHMENT_POINT_COLOUR, 0, m_pColorAttach, m_colorTexture.GetTexture()->GetType() );
			l_return = m_frameBuffer->IsComplete();
			m_frameBuffer->Unbind();
		}

		return l_return;
	}

	void RenderTarget::stFRAME_BUFFER::Cleanup()
	{
		m_frameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_colorTexture.Cleanup();
	}

	//*************************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;
	const Castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & p_engine, eTARGET_TYPE p_eTargetType )
		: OwnedBy< Engine >{ p_engine }
		, m_eTargetType{ p_eTargetType }
		, m_pixelFormat{ PixelFormat::A8R8G8B8 }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_renderTechnique{}
		, m_bMultisampling{ false }
		, m_samplesCount{ 0 }
		, m_bStereo{ false }
		, m_rIntraOcularDistance{ 0 }
		, m_index{ ++sm_uiCount }
		, m_techniqueName{ cuT( "direct" ) }
		, m_fbLeftEye{ *this }
		, m_fbRightEye{ *this }
	{
		m_toneMapping = GetEngine()->GetRenderTargetCache().GetToneMappingFactory().Create( cuT( "linear" ), *GetEngine(), Parameters{} );
		SamplerSPtr l_sampler = GetEngine()->GetSamplerCache().Add( RenderTarget::DefaultSamplerName + string::to_string( m_index ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Linear );
		l_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Linear );
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::Initialise( uint32_t p_index )
	{
		if ( !m_initialised )
		{
			m_fbLeftEye.Create();
			m_fbRightEye.Create();

			if ( !m_renderTechnique )
			{
				if ( m_techniqueName == cuT( "msaa" ) )
				{
					m_bMultisampling = true;
				}

				try
				{
					m_renderTechnique = GetEngine()->GetRenderTechniqueCache().Add( cuT( "RenderTargetTechnique_" ) + string::to_string( m_index ), m_techniqueName, *this, m_techniqueParameters );
				}
				catch ( Exception & p_exc )
				{
					Logger::LogError( cuT( "Couldn't load technique " ) + m_techniqueName + cuT( ": " ) + string::string_cast< xchar >( p_exc.GetFullDescription() ) );
					throw;
				}
			}

			m_fbLeftEye.Initialise( p_index, m_size );
			m_size = m_fbLeftEye.m_colorTexture.GetTexture()->GetImage().GetDimensions();
			m_fbRightEye.Initialise( p_index, m_size );
			m_renderTechnique->Create();
			uint32_t l_index = p_index;
			m_renderTechnique->Initialise( l_index );

			SceneSPtr l_scene = GetScene();
			auto l_camera = m_pCamera.lock();

			if ( l_scene && l_camera )
			{
				m_renderTechnique->AddScene( *l_scene, *l_camera );
				m_renderTechnique->AddScene( *l_scene, *m_pCameraLEye.lock() );
				m_renderTechnique->AddScene( *l_scene, *m_pCameraREye.lock() );
			}

			for ( auto l_effect : m_postEffects )
			{
				l_effect->Initialise();
			}

			m_initialised = m_toneMapping->Initialise();
		}
	}

	void RenderTarget::Cleanup()
	{
		if ( m_initialised )
		{
			m_toneMapping->Cleanup();
			m_toneMapping.reset();

			for ( auto l_effect : m_postEffects )
			{
				l_effect->Cleanup();
			}

			m_postEffects.clear();
			m_initialised = false;
			GetEngine()->GetRenderTechniqueCache().Remove( cuT( "RenderTargetTechnique_" ) + string::to_string( m_index ) );
			m_renderTechnique->Cleanup();
			m_fbLeftEye.Cleanup();
			m_fbRightEye.Cleanup();
			m_renderTechnique->Destroy();
			m_fbLeftEye.Destroy();
			m_fbRightEye.Destroy();
			m_renderTechnique.reset();
		}
	}

	void RenderTarget::Render( uint32_t p_frameTime )
	{
		SceneSPtr l_scene = GetScene();

		if ( l_scene )
		{
			if ( m_initialised )
			{
				if ( m_bStereo
					 && m_rIntraOcularDistance > 0
					 && GetEngine()->GetRenderSystem()->GetGpuInformations().IsStereoAvailable()
					 && GetCameraLEye()
					 && GetCameraREye() )
				{
					DoRender( m_fbLeftEye, GetCameraLEye(), p_frameTime );
					DoRender( m_fbRightEye, GetCameraREye(), p_frameTime );
				}
				else
				{
					CameraSPtr l_pCamera = GetCamera();

					if ( l_pCamera )
					{
						DoRender( m_fbLeftEye, GetCamera(), p_frameTime );
					}
				}
			}
		}
	}

	eVIEWPORT_TYPE RenderTarget::GetViewportType()const
	{
		return ( GetCamera() ? GetCamera()->GetViewportType() : eVIEWPORT_TYPE_COUNT );
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
		String l_strIndex = cuT( "_RT" ) + string::to_string( m_index );
		SceneSPtr l_scene = GetScene();

		if ( l_scene )
		{
			if ( GetCameraLEye() )
			{
				l_scene->GetCameraCache().Remove( GetCameraLEye()->GetName() );
			}

			if ( GetCameraREye() )
			{
				l_scene->GetCameraCache().Remove( GetCameraREye()->GetName() );
			}
		}

		if ( GetCamera() )
		{
			l_pCamNode = GetCamera()->GetParent();
			l_strLENodeName = l_pCamNode->GetName() + l_strIndex + cuT( "_LEye" );
			l_strRENodeName = l_pCamNode->GetName() + l_strIndex + cuT( "_REye" );
			l_pCamNode->DetachChild( l_pCamNode->GetChild( l_strLENodeName ) );
			l_pCamNode->DetachChild( l_pCamNode->GetChild( l_strRENodeName ) );

			if ( l_scene )
			{
				l_scene->GetSceneNodeCache().Remove( l_strLENodeName );
				l_scene->GetSceneNodeCache().Remove( l_strRENodeName );
			}
		}

		m_pCamera = p_pCamera;

		if ( p_pCamera )
		{
			l_pCamNode = p_pCamera->GetParent();

			if ( l_scene && l_pCamNode )
			{
				l_strLENodeName = l_pCamNode->GetName() + l_strIndex + cuT( "_LEye" );
				l_strRENodeName = l_pCamNode->GetName() + l_strIndex + cuT( "_REye" );
				l_pLECamNode = l_scene->GetSceneNodeCache().Add( l_strLENodeName, l_scene->GetSceneNodeCache().Find( l_pCamNode->GetName() ) );
				l_pRECamNode = l_scene->GetSceneNodeCache().Add( l_strRENodeName, l_scene->GetSceneNodeCache().Find( l_pCamNode->GetName() ) );
				l_pLECamNode->Translate( Point3r( -m_rIntraOcularDistance / 2, 0, 0 ) );
				l_pRECamNode->Translate( Point3r( m_rIntraOcularDistance / 2, 0, 0 ) );
				m_pCameraLEye = l_scene->GetCameraCache().Add( p_pCamera->GetName() + l_strIndex + cuT( "_LEye" ), l_pLECamNode, p_pCamera->GetViewport() );
				m_pCameraREye = l_scene->GetCameraCache().Add( p_pCamera->GetName() + l_strIndex + cuT( "_REye" ), l_pRECamNode, p_pCamera->GetViewport() );
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

	void RenderTarget::SetToneMappingType( String const & p_name, Parameters const & p_parameters )
	{
		if ( m_toneMapping )
		{
			ToneMappingSPtr l_toneMapping;
			std::swap( m_toneMapping, l_toneMapping );
			// Give ownership of the tone mapping to the event (capture by value in the lambda).
			GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [l_toneMapping]()
			{
				l_toneMapping->Cleanup();
			} ) );
		}

		m_toneMapping = GetEngine()->GetRenderTargetCache().GetToneMappingFactory().Create( p_name, *GetEngine(), p_parameters );
	}

	void RenderTarget::SetSize( Size const & p_size )
	{
		m_size = p_size;
	}

	void RenderTarget::SetTechnique( Castor::String const & p_name, Parameters const & p_parameters )
	{
		m_techniqueName = p_name;
		m_techniqueParameters = p_parameters;
		m_bMultisampling = p_name == cuT( "msaa" );
	}

	void RenderTarget::DoRender( RenderTarget::stFRAME_BUFFER & p_fb, CameraSPtr p_pCamera, uint32_t p_frameTime )
	{
		m_visibleObjectsCount = 0u;
		m_pCurrentFrameBuffer = p_fb.m_frameBuffer;
		m_pCurrentCamera = p_pCamera;
		SceneSPtr l_scene = GetScene();
		p_fb.m_frameBuffer->SetClearColour( l_scene->GetBackgroundColour() );

		if ( l_scene )
		{
			// Render the scene through the RenderTechnique.
			m_renderTechnique->Render( *l_scene, *p_pCamera, p_frameTime, m_visibleObjectsCount );

			// Then draw the render's result to the RenderTarget's frame buffer.
			if ( p_fb.m_frameBuffer->Bind() )
			{
				p_fb.m_frameBuffer->Clear();
				GetToneMapping()->Apply( GetSize(), m_renderTechnique->GetResult() );
				// We also render overlays.
				GetEngine()->GetOverlayCache().Render( *l_scene, m_size );
				p_fb.m_frameBuffer->Unbind();
			}
		}

		m_pCurrentFrameBuffer.reset();
		m_pCurrentCamera.reset();

#if DEBUG_BUFFERS

		p_fb.m_pColorAttach->DownloadBuffer();
		const Image l_tmp( cuT( "tmp" ), *p_fb.m_pColorTexture->GetBuffer() );
		Image::BinaryLoader()( l_tmp, Engine::GetEngineDirectory() / cuT( "RenderTargetTexture_" ) + string::to_string( ptrdiff_t( p_fb.m_pColorTexture.get() ), 16 ) + cuT( ".png" ) );

#endif

	}
}
