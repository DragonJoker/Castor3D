#include "PostEffect.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"

#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"

#include "Texture/TextureLayout.hpp"

using namespace Castor;
namespace Castor3D
{
	//*********************************************************************************************

	PostEffect::PostEffectSurface::PostEffectSurface( Engine & p_engine )
		: m_colourTexture( p_engine )
	{
	}

	bool PostEffect::PostEffectSurface::Initialise( RenderTarget & p_renderTarget, Size const & p_size, uint32_t p_index, SamplerSPtr p_sampler )
	{
		bool l_return = false;
		m_size = p_size;
		m_colourTexture.SetIndex( p_index );

		m_fbo = p_renderTarget.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		auto l_colourTexture = p_renderTarget.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );

		m_colourTexture.SetSampler( p_sampler );
		l_colourTexture->GetImage().SetSource( p_size, ePIXEL_FORMAT_A8R8G8B8 );
		m_colourAttach = m_fbo->CreateAttachment( l_colourTexture );

		m_fbo->Create();
		m_colourTexture.SetTexture( l_colourTexture );
		m_colourTexture.Initialise();
		m_fbo->Initialise( p_size );
		m_fbo->SetClearColour( Colour::from_predef( Colour::ePREDEFINED_FULLALPHA_BLACK ) );

		if ( m_fbo->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
		{
			m_fbo->Attach( eATTACHMENT_POINT_COLOUR, 0, m_colourAttach, l_colourTexture->GetType() );
			l_return = m_fbo->IsComplete();
			m_fbo->Unbind();
		}

		return l_return;
	}

	void PostEffect::PostEffectSurface::Cleanup()
	{
		m_fbo->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_fbo->DetachAll();
		m_fbo->Unbind();
		m_fbo->Cleanup();

		m_colourTexture.Cleanup();
		m_fbo->Destroy();

		m_fbo.reset();
		m_colourAttach.reset();
	}

	//*********************************************************************************************

	PostEffect::PostEffect( String const & p_name, RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & CU_PARAM_UNUSED( p_param ) )
		: OwnedBy< RenderSystem >{ p_renderSystem }
		, Named{ p_name }
		, m_renderTarget{ p_renderTarget }
	{
	}

	PostEffect::~PostEffect()
	{
	}

	bool PostEffect::WriteInto( Castor::TextFile & p_file )
	{
		return DoWriteInto( p_file );
	}

	//*********************************************************************************************
}
