#include "PostEffect.hpp"

#include "Engine.hpp"

#include "Render/RenderTarget.hpp"
#include "Texture/TextureLayout.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>

using namespace castor;
namespace castor3d
{
	//*********************************************************************************************

	PostEffect::PostEffectSurface::PostEffectSurface( Engine & engine )
		: m_colourTexture( engine )
	{
	}

	bool PostEffect::PostEffectSurface::initialise( RenderTarget & renderTarget
		, renderer::RenderPass const & renderPass
		, Size const & size
		, uint32_t index
		, SamplerSPtr sampler
		, PixelFormat format )
	{
		m_size = size;
		m_colourTexture.setIndex( index );

		auto colourTexture = std::make_shared< TextureLayout >( *renderTarget.getEngine()->getRenderSystem()
			, renderer::TextureType::e2D
			, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled
			, renderer::MemoryPropertyFlag::eDeviceLocal
			, format
			, size );

		colourTexture->getImage().initialiseSource();
		m_colourTexture.setSampler( sampler );
		m_colourTexture.setTexture( colourTexture );
		m_colourTexture.initialise();

		m_fbo = renderPass.createFrameBuffer( renderer::UIVec2{ size }
			, { colourTexture->getView() } );
		return true;
	}

	void PostEffect::PostEffectSurface::cleanup()
	{
		m_fbo.reset();
		m_colourTexture.cleanup();
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
