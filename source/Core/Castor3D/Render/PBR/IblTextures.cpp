#include "Castor3D/Render/PBR/IblTextures.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/Cache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/Buffer/UniformBuffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/StagingTexture.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/ResourceHandler.hpp>

using namespace castor;
using namespace sdw;

#define C3D_GenerateBRDFIntegration 0

namespace castor3d
{
	namespace
	{
#if !C3D_GenerateBRDFIntegration
		void doLoadPrefilteredBrdfView( Engine & engine
			, RenderDevice const & device
			, Texture const & texture )
		{
			auto image = std::make_unique< ashes::Image >( *device, texture.image, texture.imageId.data->info );
			PxBufferBaseSPtr buffer;

			if ( engine.getImageCache().has( cuT( "BRDF" ) ) )
			{
				auto image = engine.getImageCache().find( cuT( "BRDF" ) );
				buffer = image->getPixels();
			}
			else
			{
				auto imagePath = Engine::getEngineDirectory() / cuT( "Core" ) / cuT( "brdf.png" );
				auto image = engine.getImageCache().add( cuT( "BRDF" )
					, imagePath
					, false );
				buffer = image->getPixels();
			}

			buffer = PxBufferBase::create( buffer->getDimensions()
				, PixelFormat::eR8G8B8A8_UNORM
				, buffer->getConstPtr()
				, buffer->getFormat() );
			auto result = image->createView( VK_IMAGE_VIEW_TYPE_2D, texture.getFormat() );
			auto & renderSystem = *engine.getRenderSystem();
			auto staging = device->createStagingTexture( VK_FORMAT_R8G8B8A8_UNORM
				, makeExtent2D( buffer->getDimensions() ) );
			auto data = device.graphicsData();;
			staging->uploadTextureData( *data->queue
				, *data->commandPool
				, VK_FORMAT_R8G8B8A8_UNORM
				, buffer->getConstPtr()
				, result );
		}
#endif

		Texture doCreatePrefilteredBrdf( RenderDevice const & device
			, crg::ResourceHandler & handler
			, Size const & size )
		{
			Texture result{ device
				, handler
				, "IblTexturesResult"
				, 0u
				, { size[0], size[1], 1u }
				, 1u
				, 1u
#if !C3D_GenerateBRDFIntegration
				, VK_FORMAT_R8G8B8A8_UNORM
#else
				, VK_FORMAT_R32G32B32A32_SFLOAT
#endif
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
#if !C3D_GenerateBRDFIntegration
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
#endif
					| VK_IMAGE_USAGE_SAMPLED_BIT ) };
			result.create();

#if !C3D_GenerateBRDFIntegration
			doLoadPrefilteredBrdfView( *device.renderSystem.getEngine(), device, result );
#else
			BrdfPrefilter filter{ *scene.getEngine()
				, { m_prefilteredBrdf->getDimensions().width, m_prefilteredBrdf->getDimensions().height }
			, *m_prefilteredBrdfView };
			filter.render();
#endif
			return result;
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, RenderDevice const & device )
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
				result->setMinFilter( VK_FILTER_LINEAR );
				result->setMagFilter( VK_FILTER_LINEAR );
				result->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				result->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
				engine.getSamplerCache().add( name, result );
			}

			result->initialise( device );
			return result;
		}
	}

	//************************************************************************************************

	IblTextures::IblTextures( Scene & scene
		, RenderDevice const & device
		, Texture const & source
		, SamplerSPtr sampler )
		: OwnedBy< Scene >{ scene }
		, m_prefilteredBrdf{ doCreatePrefilteredBrdf( device, scene.getEngine()->getGraphResourceHandler(), Size{ 512u, 512u } ) }
		, m_sampler{ doCreateSampler( *scene.getEngine(), device ) }
		, m_radianceComputer{ *scene.getEngine(), device, Size{ 32u, 32u }, source }
		, m_environmentPrefilter{ *scene.getEngine(), device, Size{ 128u, 128u }, source, std::move( sampler ) }
	{
	}

	void IblTextures::update( QueueData const & queueData )
	{
		m_radianceComputer.render( queueData );
		m_environmentPrefilter.render( queueData );
	}

	ashes::Semaphore const & IblTextures::update( QueueData const & queueData
		, ashes::Semaphore const & toWait )
	{
		auto result = &toWait;
		result = &m_radianceComputer.render( queueData, *result );
		result = &m_environmentPrefilter.render( queueData, *result );
		return *result;
	}
}
