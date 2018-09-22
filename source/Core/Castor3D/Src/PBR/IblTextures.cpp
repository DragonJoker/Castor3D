#include "IblTextures.hpp"

#include "Engine.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Image/StagingTexture.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

#include <Graphics/Image.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;
using namespace glsl;

#define C3D_GenerateBRDFIntegration 0

namespace castor3d
{
	namespace
	{
		ashes::TexturePtr doCreatePrefilteredBrdf( RenderSystem const & renderSystem
			, Size const & size )
		{
			auto & device = getCurrentDevice( renderSystem );
			ashes::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
#if !C3D_GenerateBRDFIntegration
			image.format = ashes::Format::eR8G8B8A8_UNORM;
#else
			image.format = ashes::Format::eR32G32B32A32_SFLOAT;
#endif
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.usage = ashes::ImageUsageFlag::eColourAttachment
#if !C3D_GenerateBRDFIntegration
				| ashes::ImageUsageFlag::eTransferDst
#endif
				| ashes::ImageUsageFlag::eSampled;
			return device.createTexture( image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
		}

#if !C3D_GenerateBRDFIntegration
		ashes::TextureViewPtr doCreatePrefilteredBrdfView( Engine & engine
			, ashes::Texture const & texture )
		{
			PxBufferBaseSPtr buffer;

			if ( engine.getImageCache().has( cuT( "BRDF" ) ) )
			{
				auto image = engine.getImageCache().find( cuT( "BRDF" ) );
				buffer = image->getPixels();
			}
			else
			{
				auto imagePath = Engine::getEngineDirectory() / cuT( "Core" ) / cuT( "brdf.png" );
				auto image = engine.getImageCache().add( cuT( "BRDF" ), imagePath );
				buffer = image->getPixels();
			}

			buffer = PxBufferBase::create( buffer->dimensions()
				, PixelFormat::eA8R8G8B8
				, buffer->constPtr()
				, buffer->format() );
			auto result = texture.createView( ashes::TextureViewType::e2D, texture.getFormat() );
			auto & device = getCurrentDevice( engine );
			auto staging = device.createStagingTexture( ashes::Format::eR8G8B8A8_UNORM
				, { buffer->dimensions().getWidth(), buffer->dimensions().getHeight() } );
			auto commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
			staging->uploadTextureData( *commandBuffer
				, ashes::Format::eR8G8B8A8_UNORM
				, buffer->constPtr()
				, *result );
			return result;
		}
#endif

		SamplerSPtr doCreateSampler( Engine & engine )
		{
			SamplerSPtr result;
			auto name = cuT( "IblTexturesBRDF" );

			if ( engine.getSamplerCache().has( name ) )
			{
				result = engine.getSamplerCache().find( name );
			}
			else
			{
				result = engine.getSamplerCache().create( name );
				result->setMinFilter( ashes::Filter::eLinear );
				result->setMagFilter( ashes::Filter::eLinear );
				result->setWrapS( ashes::WrapMode::eClampToEdge );
				result->setWrapT( ashes::WrapMode::eClampToEdge );
				result->setWrapR( ashes::WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, result );
			}

			result->initialise();
			return result;
		}
	}

	//************************************************************************************************

	IblTextures::IblTextures( Scene & scene
		, ashes::Texture const & source
		, SamplerSPtr sampler )
		: OwnedBy< Scene >{ scene }
		, m_prefilteredBrdf{ doCreatePrefilteredBrdf( *scene.getEngine()->getRenderSystem(), Size{ 512u, 512u } ) }
#if !C3D_GenerateBRDFIntegration
		, m_prefilteredBrdfView{ doCreatePrefilteredBrdfView( *scene.getEngine(), *m_prefilteredBrdf ) }
#else
		, m_prefilteredBrdfView{ m_prefilteredBrdf->createView( ashes::TextureViewType::e2D, m_prefilteredBrdf->getFormat() ) }
#endif
		, m_sampler{ doCreateSampler( *scene.getEngine() ) }
		, m_radianceComputer{ *scene.getEngine(), Size{ 32u, 32u }, source }
		, m_environmentPrefilter{ *scene.getEngine(), Size{ 128u, 128u }, source, std::move( sampler ) }
	{
#if C3D_GenerateBRDFIntegration
		BrdfPrefilter filter{ *scene.getEngine()
			, { m_prefilteredBrdf->getDimensions().width, m_prefilteredBrdf->getDimensions().height }
			, *m_prefilteredBrdfView };
		filter.render();
#endif
	}

	IblTextures::~IblTextures()
	{
		m_prefilteredBrdfView.reset();
		m_prefilteredBrdf.reset();
	}

	void IblTextures::update()
	{
		m_radianceComputer.render();
		m_environmentPrefilter.render();
	}

	void IblTextures::debugDisplay( Size const & renderSize )const
	{
		int width = int( m_prefilteredBrdf->getDimensions().width );
		int height = int( m_prefilteredBrdf->getDimensions().height );
		int left = 0u;
		int top = renderSize.getHeight() - height;
		auto size = Size( width, height );
		//auto & context = *getScene()->getEngine()->getRenderSystem()->getCurrentContext();
		//context.renderTexture( Position{ left, top }
		//	, size
		//	, *m_prefilteredBrdf.getTexture() );
		//left += 512;
		//context.renderTextureCube( Position{ left, top }
		//	, Size( width / 4, height / 4u )
		//	, *m_prefilteredEnvironment.getTexture() );
		//left += 512;
		//context.renderTextureCube( Position{ left, top }
		//	, Size( width / 4, height / 4u )
		//	, *m_radianceTexture.getTexture() );
	}
}
