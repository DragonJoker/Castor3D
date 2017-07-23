#include "RenderTarget.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "HDR/ToneMapping.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Shader/PassBuffer.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

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
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "render_target\n" ) + m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget name" );

		if ( result && p_target.GetScene() )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tscene \"" ) + p_target.GetScene()->GetName() + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget scene" );
		}

		if ( result && p_target.GetCamera() )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tcamera \"" ) + p_target.GetCamera()->GetName() + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget camera" );
		}

		if ( result )
		{
			result = p_file.Print( 256, ( m_tabs + cuT( "\tsize %d %d\n" ) ).c_str(), p_target.GetSize().width(), p_target.GetSize().height() ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget size" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tformat " ) + Castor::PF::GetFormatName( p_target.GetPixelFormat() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget format" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\ttone_mapping \"" ) + p_target.m_toneMapping->GetName() + cuT( "\"" ) )
					   && p_target.m_toneMapping->WriteInto( p_file )
					   && p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget tone mapping" );
		}

		if ( result
			&& p_target.m_renderTechnique
			&& p_target.m_renderTechnique->IsMultisampling() )
		{
			result = p_file.WriteText( m_tabs + cuT( "\t" ) )
				&& p_target.m_renderTechnique->WriteInto( p_file )
				&& p_file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget technique" );
		}

		if ( result )
		{
			for ( auto const & effect : p_target.m_postEffects )
			{
				result = p_file.WriteText( m_tabs + cuT( "\tpostfx \"" ) + effect->GetName() + cuT( "\"" ) )
						   && effect->WriteInto( p_file )
						   && p_file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget post effect" );
			}
		}

		if ( result )
		{
			result = SsaoConfig::TextWriter{ m_tabs + cuT( "\t" ) }( p_target.m_ssaoConfig, p_file );
		}

		p_file.WriteText( m_tabs + cuT( "}\n" ) );
		return result;
	}

	//*************************************************************************************************

	RenderTarget::stFRAME_BUFFER::stFRAME_BUFFER( RenderTarget & p_renderTarget )
		: m_renderTarget{ p_renderTarget }
		, m_colourTexture{ *p_renderTarget.GetEngine() }
	{
	}

	bool RenderTarget::stFRAME_BUFFER::Initialise( uint32_t p_index, Size const & p_size )
	{
		m_frameBuffer = m_renderTarget.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		SamplerSPtr sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( RenderTarget::DefaultSamplerName + string::to_string( m_renderTarget.m_index ) );
		auto colourTexture = m_renderTarget.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, m_renderTarget.GetPixelFormat(), p_size );
		m_colourAttach = m_frameBuffer->CreateAttachment( colourTexture );
		m_colourTexture.SetTexture( colourTexture );
		m_colourTexture.SetSampler( sampler );
		m_colourTexture.SetIndex( p_index );
		m_colourTexture.GetTexture()->GetImage().InitialiseSource();
		Size size = m_colourTexture.GetTexture()->GetDimensions();
		m_frameBuffer->Create();
		m_colourTexture.GetTexture()->Initialise();
		m_frameBuffer->Initialise( size );

		m_frameBuffer->Bind();
		m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture.GetTexture()->GetType() );
		m_frameBuffer->SetDrawBuffer( m_colourAttach );
		bool result = m_frameBuffer->IsComplete();
		m_frameBuffer->Unbind();

		return result;
	}

	void RenderTarget::stFRAME_BUFFER::Cleanup()
	{
		m_frameBuffer->Bind();
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_colourTexture.Cleanup();
		m_frameBuffer->Destroy();
		m_colourAttach.reset();
		m_colourTexture.SetTexture( nullptr );
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
		, m_frameBuffer{ *this }
	{
		m_toneMapping = GetEngine()->GetRenderTargetCache().GetToneMappingFactory().Create( cuT( "linear" ), *GetEngine(), Parameters{} );
		SamplerSPtr sampler = GetEngine()->GetSamplerCache().Add( RenderTarget::DefaultSamplerName + string::to_string( m_index ) );
		sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
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
				try
				{
					auto name = cuT( "RenderTargetTechnique_" ) + string::to_string( m_index );
					m_renderTechnique = GetEngine()->GetRenderTechniqueCache().Add( name
						, std::make_shared< RenderTechnique >( name
							, *this
							, *GetEngine()->GetRenderSystem()
							, m_techniqueParameters
							, m_ssaoConfig ) );
					m_bMultisampling = m_renderTechnique->IsMultisampling();
				}
				catch ( Exception & p_exc )
				{
					Logger::LogError( cuT( "Couldn't load render technique: " ) + string::string_cast< xchar >( p_exc.GetFullDescription() ) );
					throw;
				}
			}

			m_size = m_frameBuffer.m_colourTexture.GetTexture()->GetDimensions();
			m_renderTechnique->Initialise( p_index );

			for ( auto effect : m_postEffects )
			{
				effect->Initialise();
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

			for ( auto effect : m_postEffects )
			{
				effect->Cleanup();
			}

			m_postEffects.clear();
			m_initialised = false;
			GetEngine()->GetRenderTechniqueCache().Remove( cuT( "RenderTargetTechnique_" ) + string::to_string( m_index ) );
			m_renderTechnique->Cleanup();
			m_frameBuffer.Cleanup();
			m_renderTechnique.reset();
		}
	}

	void RenderTarget::Render( RenderInfo & p_info )
	{
		SceneSPtr scene = GetScene();

		if ( scene )
		{
			if ( m_initialised && scene->IsInitialised() )
			{
				CameraSPtr pCamera = GetCamera();

				if ( pCamera )
				{
					scene->GetGeometryCache().FillInfo( p_info );
					DoRender( p_info, m_frameBuffer, GetCamera() );
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
			ToneMappingSPtr toneMapping;
			std::swap( m_toneMapping, toneMapping );
			// Give ownership of the tone mapping to the event (capture by value in the lambda).
			GetEngine()->PostEvent( MakeFunctorEvent( EventType::ePreRender, [toneMapping]()
			{
				toneMapping->Cleanup();
			} ) );
		}

		m_toneMapping = GetEngine()->GetRenderTargetCache().GetToneMappingFactory().Create( p_name, *GetEngine(), p_parameters );
	}

	void RenderTarget::SetSize( Size const & p_size )
	{
		m_size = p_size;
	}

	void RenderTarget::AddTechniqueParameters( Parameters const & p_parameters )
	{
		m_techniqueParameters.Add( p_parameters );
	}

	void RenderTarget::AddShadowProducer( Light & p_light )
	{
		if ( m_renderTechnique )
		{
			m_renderTechnique->AddShadowProducer( p_light );
		}
	}

	void RenderTarget::DoRender( RenderInfo & p_info, RenderTarget::stFRAME_BUFFER & p_fb, CameraSPtr p_pCamera )
	{
		SceneSPtr scene = GetScene();
		p_fb.m_frameBuffer->SetClearColour( scene->GetBackgroundColour() );

		if ( scene )
		{
			// Render the scene through the RenderTechnique.
			m_renderTechnique->Render( p_info );

			// Then draw the render's result to the RenderTarget's frame buffer.
			p_fb.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			p_fb.m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
			m_toneMapping->SetConfig( scene->GetHdrConfig() );
			m_toneMapping->Apply( GetSize(), m_renderTechnique->GetResult() );
			// We also render overlays.
			GetEngine()->GetMaterialCache().GetPassBuffer().Bind();
			GetEngine()->GetOverlayCache().Render( *scene, m_size );
			p_fb.m_frameBuffer->Unbind();
		}

#if DEBUG_BUFFERS

		p_fb.m_colourAttach->DownloadBuffer();
		const Image tmp( cuT( "tmp" ), *p_fb.m_colourTexture.GetTexture()->GetImage().GetBuffer() );
		Image::BinaryWriter()( tmp, Engine::GetEngineDirectory() / String( cuT( "RenderTargetTexture_" ) + string::to_string( ptrdiff_t( p_fb.m_colourTexture.GetTexture().get() ), 16 ) + cuT( ".png" ) ) );

#endif

	}
}
