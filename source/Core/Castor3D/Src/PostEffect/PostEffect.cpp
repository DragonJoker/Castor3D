#include "PostEffect.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderTarget.hpp"

#include "Texture/TextureLayout.hpp"

using namespace Castor;
namespace Castor3D
{
	//*********************************************************************************************

	PostEffect::PostEffectSurface::PostEffectSurface( Engine & engine )
		: m_colourTexture( engine )
	{
	}

	bool PostEffect::PostEffectSurface::Initialise( RenderTarget & p_renderTarget, Size const & p_size, uint32_t p_index, SamplerSPtr p_sampler )
	{
		m_size = p_size;
		m_colourTexture.SetIndex( p_index );

		m_fbo = p_renderTarget.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
		auto colourTexture = p_renderTarget.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGBA32F
			, p_size );

		m_colourTexture.SetSampler( p_sampler );
		colourTexture->GetImage().InitialiseSource();
		m_colourAttach = m_fbo->CreateAttachment( colourTexture );

		m_fbo->Create();
		m_colourTexture.SetTexture( colourTexture );
		m_colourTexture.Initialise();
		m_fbo->Initialise( p_size );
		m_fbo->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );

		m_fbo->Bind();
		m_fbo->Attach( AttachmentPoint::eColour, 0, m_colourAttach, colourTexture->GetType() );
		m_fbo->SetDrawBuffer( m_colourAttach );
		bool result = m_fbo->IsComplete();
		m_fbo->Unbind();

		return result;
	}

	void PostEffect::PostEffectSurface::Cleanup()
	{
		m_fbo->Bind();
		m_fbo->DetachAll();
		m_fbo->Unbind();
		m_fbo->Cleanup();

		m_colourTexture.Cleanup();
		m_fbo->Destroy();

		m_fbo.reset();
		m_colourAttach.reset();
	}

	//*********************************************************************************************

	PostEffect::PostEffect( String const & p_name, RenderTarget & p_renderTarget, RenderSystem & renderSystem, Parameters const & CU_PARAM_UNUSED( p_param ) )
		: OwnedBy< RenderSystem >{ renderSystem }
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
