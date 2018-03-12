#include "PostEffect.hpp"

#include "Engine.hpp"

#include "Render/RenderTarget.hpp"
#include "Texture/TextureLayout.hpp"

#include <Command/CommandBuffer.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
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
		, renderer::Format format
		, uint32_t mipLevels )
	{
		m_size = size;
		m_colourTexture.setIndex( index );

		renderer::ImageCreateInfo image{};
		image.flags = renderer::ImageCreateFlag::eCubeCompatible;
		image.arrayLayers = 1u;
		image.extent.width = size[0];
		image.extent.height = size[1];
		image.extent.depth = 1u;
		image.format = format;
		image.imageType = renderer::TextureType::e2D;
		image.initialLayout = renderer::ImageLayout::eUndefined;
		image.mipLevels = mipLevels;
		image.samples = renderer::SampleCountFlag::e1;
		image.sharingMode = renderer::SharingMode::eExclusive;
		image.tiling = renderer::ImageTiling::eOptimal;
		image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
		auto colourTexture = std::make_shared< TextureLayout >( *renderTarget.getEngine()->getRenderSystem()
			, image
			, renderer::MemoryPropertyFlag::eDeviceLocal );

		colourTexture->getImage().initialiseSource();
		m_colourTexture.setSampler( sampler );
		m_colourTexture.setTexture( colourTexture );
		m_colourTexture.initialise();

		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *renderPass.getAttachments().begin(), colourTexture->getDefaultView() );
		m_fbo = renderPass.createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
			, std::move( attaches ) );
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
