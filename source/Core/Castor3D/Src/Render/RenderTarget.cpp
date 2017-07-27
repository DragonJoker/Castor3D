#include "RenderTarget.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "HDR/ToneMapping.hpp"
#include "Render/RenderPassTimer.hpp"
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
	RenderTarget::TextWriter::TextWriter( String const & tabs )
		: Castor::TextWriter< RenderTarget >{ tabs }
	{
	}

	bool RenderTarget::TextWriter::operator()( RenderTarget const & target, TextFile & file )
	{
		Logger::LogInfo( m_tabs + cuT( "Writing RenderTarget" ) );
		bool result = file.WriteText( cuT( "\n" ) + m_tabs + cuT( "render_target\n" ) + m_tabs + cuT( "{\n" ) ) > 0;
		Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget name" );

		if ( result && target.GetScene() )
		{
			result = file.WriteText( m_tabs + cuT( "\tscene \"" ) + target.GetScene()->GetName() + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget scene" );
		}

		if ( result && target.GetCamera() )
		{
			result = file.WriteText( m_tabs + cuT( "\tcamera \"" ) + target.GetCamera()->GetName() + cuT( "\"\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget camera" );
		}

		if ( result )
		{
			result = file.Print( 256, ( m_tabs + cuT( "\tsize %d %d\n" ) ).c_str(), target.GetSize().width(), target.GetSize().height() ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget size" );
		}

		if ( result )
		{
			result = file.WriteText( m_tabs + cuT( "\tformat " ) + Castor::PF::GetFormatName( target.GetPixelFormat() ) + cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget format" );
		}

		if ( result )
		{
			result = file.WriteText( m_tabs + cuT( "\ttone_mapping \"" ) + target.m_toneMapping->GetName() + cuT( "\"" ) )
					   && target.m_toneMapping->WriteInto( file )
					   && file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget tone mapping" );
		}

		if ( result
			&& target.m_renderTechnique
			&& target.m_renderTechnique->IsMultisampling() )
		{
			result = file.WriteText( m_tabs + cuT( "\t" ) )
				&& target.m_renderTechnique->WriteInto( file )
				&& file.WriteText( cuT( "\n" ) ) > 0;
			Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget technique" );
		}

		if ( result )
		{
			for ( auto const & effect : target.m_postEffects )
			{
				result = file.WriteText( m_tabs + cuT( "\tpostfx \"" ) + effect->GetName() + cuT( "\"" ) )
						   && effect->WriteInto( file )
						   && file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< RenderTarget >::CheckError( result, "RenderTarget post effect" );
			}
		}

		if ( result )
		{
			result = SsaoConfig::TextWriter{ m_tabs + cuT( "\t" ) }( target.m_ssaoConfig, file );
		}

		file.WriteText( m_tabs + cuT( "}\n" ) );
		return result;
	}

	//*************************************************************************************************

	RenderTarget::TargetFbo::TargetFbo( RenderTarget & renderTarget )
		: m_renderTarget{ renderTarget }
		, m_colourTexture{ *renderTarget.GetEngine() }
	{
	}

	bool RenderTarget::TargetFbo::Initialise( uint32_t index, Size const & size )
	{
		m_frameBuffer = m_renderTarget.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		SamplerSPtr sampler = m_renderTarget.GetEngine()->GetSamplerCache().Find( RenderTarget::DefaultSamplerName + string::to_string( m_renderTarget.m_index ) );
		auto colourTexture = m_renderTarget.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, m_renderTarget.GetPixelFormat(), size );
		m_colourAttach = m_frameBuffer->CreateAttachment( colourTexture );
		m_colourTexture.SetTexture( colourTexture );
		m_colourTexture.SetSampler( sampler );
		m_colourTexture.SetIndex( index );
		m_colourTexture.GetTexture()->GetImage().InitialiseSource();
		Size dimensions = m_colourTexture.GetTexture()->GetDimensions();
		m_frameBuffer->Create();
		m_colourTexture.GetTexture()->Initialise();
		m_frameBuffer->Initialise( dimensions );

		m_frameBuffer->Bind();
		m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture.GetTexture()->GetType() );
		m_frameBuffer->SetDrawBuffer( m_colourAttach );
		bool result = m_frameBuffer->IsComplete();
		m_frameBuffer->Unbind();

		return result;
	}

	void RenderTarget::TargetFbo::Cleanup()
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

	RenderTarget::RenderTarget( Engine & engine, TargetType type )
		: OwnedBy< Engine >{ engine }
		, m_type{ type }
		, m_pixelFormat{ PixelFormat::eA8R8G8B8 }
		, m_initialised{ false }
		, m_size{ Size{ 100u, 100u } }
		, m_renderTechnique{}
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

	void RenderTarget::Initialise( uint32_t index )
	{
		if ( !m_initialised )
		{
			m_frameBuffer.Initialise( index, m_size );

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
				}
				catch ( Exception & p_exc )
				{
					Logger::LogError( cuT( "Couldn't load render technique: " ) + string::string_cast< xchar >( p_exc.GetFullDescription() ) );
					throw;
				}
			}

			m_size = m_frameBuffer.m_colourTexture.GetTexture()->GetDimensions();
			m_renderTechnique->Initialise( index );

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

	void RenderTarget::Render( RenderInfo & info )
	{
		SceneSPtr scene = GetScene();

		if ( scene )
		{
			if ( m_initialised && scene->IsInitialised() )
			{
				CameraSPtr pCamera = GetCamera();

				if ( pCamera )
				{
					scene->GetGeometryCache().FillInfo( info );
					DoRender( info, m_frameBuffer, GetCamera() );
				}
			}
		}
	}

	ViewportType RenderTarget::GetViewportType()const
	{
		return ( GetCamera() ? GetCamera()->GetViewportType() : ViewportType::eCount );
	}

	void RenderTarget::SetViewportType( ViewportType value )
	{
		if ( GetCamera() )
		{
			GetCamera()->SetViewportType( value );
		}
	}

	void RenderTarget::SetCamera( CameraSPtr camera )
	{
		m_camera = camera;
	}

	void RenderTarget::SetToneMappingType( String const & name
		, Parameters const & parameters )
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

		m_toneMapping = GetEngine()->GetRenderTargetCache().GetToneMappingFactory().Create( name, *GetEngine(), parameters );
	}

	void RenderTarget::SetSize( Size const & size )
	{
		m_size = size;
	}

	void RenderTarget::AddTechniqueParameters( Parameters const & parameters )
	{
		m_techniqueParameters.Add( parameters );
	}

	void RenderTarget::AddShadowProducer( Light & light )
	{
		if ( m_renderTechnique )
		{
			m_renderTechnique->AddShadowProducer( light );
		}
	}

	void RenderTarget::DoRender( RenderInfo & info
		, RenderTarget::TargetFbo & fbo
		, CameraSPtr camera )
	{
		SceneSPtr scene = GetScene();
		fbo.m_frameBuffer->SetClearColour( scene->GetBackgroundColour() );

		// Render the scene through the RenderTechnique.
		m_renderTechnique->Render( info );

		// Then draw the render's result to the RenderTarget's frame buffer.
		fbo.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		fbo.m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_toneMapping->SetConfig( scene->GetHdrConfig() );
		m_toneMapping->Apply( GetSize(), m_renderTechnique->GetResult(), info );
		// We also render overlays.
		GetEngine()->GetMaterialCache().GetPassBuffer().Bind();
		GetEngine()->GetOverlayCache().Render( *scene, m_size );
		fbo.m_frameBuffer->Unbind();

#if DEBUG_BUFFERS

		fbo.m_colourAttach->DownloadBuffer();
		const Image tmp( cuT( "tmp" ), *fbo.m_colourTexture.GetTexture()->GetImage().GetBuffer() );
		Image::BinaryWriter()( tmp, Engine::GetEngineDirectory() / String( cuT( "RenderTargetTexture_" ) + string::to_string( ptrdiff_t( fbo.m_colourTexture.GetTexture().get() ), 16 ) + cuT( ".png" ) ) );

#endif
	}
}
