#include "RenderTarget.hpp"

#include "Engine.hpp"
#include "Cache/CameraCache.hpp"
#include "Cache/MaterialCache.hpp"
#include "Cache/OverlayCache.hpp"
#include "Cache/SamplerCache.hpp"
#include "Cache/SceneCache.hpp"
#include "Cache/SceneNodeCache.hpp"
#include "Cache/TargetCache.hpp"
#include "Cache/TechniqueCache.hpp"

#include "Context.hpp"
#include "RenderPipeline.hpp"
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

	bool RenderTarget::stFRAME_BUFFER::Initialise( uint32_t p_index, Size const & p_size )
	{
		m_frameBuffer = m_renderTarget.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		SamplerSPtr l_sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( RenderTarget::DefaultSamplerName + string::to_string( m_renderTarget.m_index ) );
		auto l_colourTexture = m_renderTarget.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead | AccessType::eWrite, m_renderTarget.GetPixelFormat(), p_size );
		m_pColorAttach = m_frameBuffer->CreateAttachment( l_colourTexture );
		m_colorTexture.SetTexture( l_colourTexture );
		m_colorTexture.SetSampler( l_sampler );
		m_colorTexture.SetIndex( p_index );
		m_colorTexture.GetTexture()->GetImage().InitialiseSource();
		Size l_size = m_colorTexture.GetTexture()->GetDimensions();
		m_frameBuffer->Create();
		m_colorTexture.GetTexture()->Initialise();
		m_frameBuffer->Initialise( l_size );

		m_frameBuffer->Bind( FrameBufferMode::eConfig );
		m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_pColorAttach, m_colorTexture.GetTexture()->GetType() );
		bool l_return = m_frameBuffer->IsComplete();
		m_frameBuffer->Unbind();

		return l_return;
	}

	void RenderTarget::stFRAME_BUFFER::Cleanup()
	{
		m_frameBuffer->Bind( FrameBufferMode::eConfig );
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_colorTexture.Cleanup();
		m_frameBuffer->Destroy();
		m_pColorAttach.reset();
		m_colorTexture.SetTexture( nullptr );
		m_frameBuffer.reset();
	}

	//*************************************************************************************************

	uint32_t RenderTarget::sm_uiCount = 0;
	const Castor::String RenderTarget::DefaultSamplerName = cuT( "DefaultRTSampler" );

	RenderTarget::RenderTarget( Engine & p_engine, TargetType p_eTargetType )
		: OwnedBy< Engine >{ p_engine }
		, m_eTargetType{ p_eTargetType }
		, m_pixelFormat{ PixelFormat::eA8R8G8B8 }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_renderTechnique{}
		, m_bMultisampling{ false }
		, m_samplesCount{ 0 }
		, m_index{ ++sm_uiCount }
		, m_techniqueName{ cuT( "forward" ) }
		, m_frameBuffer{ *this }
	{
		m_toneMapping = GetEngine()->GetRenderTargetCache().GetToneMappingFactory().Create( cuT( "linear" ), *GetEngine(), Parameters{} );
		SamplerSPtr l_sampler = GetEngine()->GetSamplerCache().Add( RenderTarget::DefaultSamplerName + string::to_string( m_index ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
	}

	RenderTarget::~RenderTarget()
	{
	}

	void RenderTarget::Initialise( uint32_t p_index )
	{
		if ( !m_initialised )
		{
			m_frameBuffer.Initialise( p_index, m_size );

			if ( !m_renderTechnique )
			{
				if ( m_techniqueName.find( cuT( "msaa" ) ) != String::npos )
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

			m_size = m_frameBuffer.m_colorTexture.GetTexture()->GetDimensions();
			m_renderTechnique->Create();
			m_renderTechnique->Initialise( p_index );

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
			m_renderTechnique->Destroy();
			m_frameBuffer.Cleanup();
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
				CameraSPtr l_pCamera = GetCamera();

				if ( l_pCamera )
				{
					DoRender( m_frameBuffer, GetCamera(), p_frameTime );
				}
			}
		}
	}

	ViewportType RenderTarget::GetViewportType()const
	{
		return ( GetCamera() ? GetCamera()->GetViewportType() : ViewportType::eCount );
	}

	void RenderTarget::SetViewportType( ViewportType val )
	{
		if ( GetCamera() )
		{
			GetCamera()->SetViewportType( val );
		}
	}

	void RenderTarget::SetCamera( CameraSPtr p_pCamera )
	{
		m_pCamera = p_pCamera;
	}

	void RenderTarget::SetToneMappingType( String const & p_name, Parameters const & p_parameters )
	{
		if ( m_toneMapping )
		{
			ToneMappingSPtr l_toneMapping;
			std::swap( m_toneMapping, l_toneMapping );
			// Give ownership of the tone mapping to the event (capture by value in the lambda).
			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender, [l_toneMapping]()
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
		m_particlesCount = 0u;
		SceneSPtr l_scene = GetScene();
		p_fb.m_frameBuffer->SetClearColour( l_scene->GetBackgroundColour() );

		if ( l_scene )
		{
			// Render the scene through the RenderTechnique.
			m_renderTechnique->Render( p_frameTime, m_visibleObjectsCount, m_particlesCount );

			// Then draw the render's result to the RenderTarget's frame buffer.
			p_fb.m_frameBuffer->Bind();
			p_fb.m_frameBuffer->Clear();
			GetToneMapping()->Apply( GetSize(), m_renderTechnique->GetResult() );
			// We also render overlays.
			GetEngine()->GetOverlayCache().Render( *l_scene, m_size );
			p_fb.m_frameBuffer->Unbind();
		}

#if DEBUG_BUFFERS

		p_fb.m_pColorAttach->DownloadBuffer();
		const Image l_tmp( cuT( "tmp" ), *p_fb.m_colorTexture.GetTexture()->GetImage().GetBuffer() );
		Image::BinaryWriter()( l_tmp, Engine::GetEngineDirectory() / String( cuT( "RenderTargetTexture_" ) + string::to_string( ptrdiff_t( p_fb.m_colorTexture.GetTexture().get() ), 16 ) + cuT( ".png" ) ) );

#endif

	}
}
