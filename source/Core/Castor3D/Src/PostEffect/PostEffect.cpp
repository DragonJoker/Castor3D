#include "PostEffect.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderTarget.hpp"

#include "Texture/TextureLayout.hpp"

using namespace castor;
namespace castor3d
{
	//*********************************************************************************************

	PostEffect::PostEffectSurface::PostEffectSurface( Engine & engine )
		: m_colourTexture( engine )
	{
	}

	bool PostEffect::PostEffectSurface::initialise( RenderTarget & p_renderTarget, Size const & p_size, uint32_t p_index, SamplerSPtr p_sampler )
	{
		m_size = p_size;
		m_colourTexture.setIndex( p_index );

		m_fbo = p_renderTarget.getEngine()->getRenderSystem()->createFrameBuffer();
		auto colourTexture = p_renderTarget.getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGBA32F
			, p_size );

		m_colourTexture.setSampler( p_sampler );
		colourTexture->getImage().initialiseSource();
		m_colourAttach = m_fbo->createAttachment( colourTexture );

		m_colourTexture.setTexture( colourTexture );
		m_colourTexture.initialise();
		m_fbo->initialise();
		m_fbo->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );

		m_fbo->bind();
		m_fbo->attach( AttachmentPoint::eColour, 0, m_colourAttach, colourTexture->getType() );
		m_fbo->setDrawBuffer( m_colourAttach );
		bool result = m_fbo->isComplete();
		m_fbo->unbind();

		return result;
	}

	void PostEffect::PostEffectSurface::cleanup()
	{
		m_fbo->bind();
		m_fbo->detachAll();
		m_fbo->unbind();
		m_fbo->cleanup();

		m_colourTexture.cleanup();
		m_fbo->cleanup();

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

	bool PostEffect::writeInto( castor::TextFile & p_file )
	{
		return doWriteInto( p_file );
	}

	//*********************************************************************************************
}
