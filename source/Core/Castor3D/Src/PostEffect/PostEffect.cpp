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

	bool PostEffect::PostEffectSurface::initialise( RenderTarget & renderTarget
		, Size const & size
		, uint32_t index
		, SamplerSPtr sampler
		, PixelFormat format )
	{
		m_size = size;
		m_colourTexture.setIndex( index );

		m_fbo = renderTarget.getEngine()->getRenderSystem()->createFrameBuffer();
		auto colourTexture = renderTarget.getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
			, AccessType::eRead
			, AccessType::eRead | AccessType::eWrite
			, format
			, size );

		m_colourTexture.setSampler( sampler );
		colourTexture->getImage().initialiseSource();
		m_colourAttach = m_fbo->createAttachment( colourTexture );

		m_colourTexture.setTexture( colourTexture );
		m_colourTexture.initialise();
		m_fbo->initialise();
		m_fbo->setClearColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueBlack ) );

		m_fbo->bind();
		m_fbo->attach( AttachmentPoint::eColour, 0, m_colourAttach, colourTexture->getType() );
		m_fbo->setDrawBuffer( m_colourAttach );
		bool result = m_fbo->isComplete();
		REQUIRE( result );
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

	PostEffect::PostEffect( String const & name
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & CU_PARAM_UNUSED( parameters )
		, bool postToneMapping )
		: OwnedBy< RenderSystem >{ renderSystem }
		, Named{ name }
		, m_renderTarget{ renderTarget }
		, m_postToneMapping{ postToneMapping }
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
