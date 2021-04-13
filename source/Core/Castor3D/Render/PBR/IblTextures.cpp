#include "Castor3D/Render/PBR/IblTextures.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
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

using namespace castor;
using namespace sdw;

#define C3D_GenerateBRDFIntegration 0

namespace castor3d
{
	namespace
	{
		ashes::ImagePtr doCreatePrefilteredBrdf( RenderDevice const & device
			, Size const & size )
		{
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
#if !C3D_GenerateBRDFIntegration
				VK_FORMAT_R8G8B8A8_UNORM,
#else
				VK_FORMAT_R32G32B32A32_SFLOAT,
#endif
				{ size[0], size[1], 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
#if !C3D_GenerateBRDFIntegration
					| VK_IMAGE_USAGE_TRANSFER_DST_BIT
#endif
					| VK_IMAGE_USAGE_SAMPLED_BIT),
			};
			return makeImage( device
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "IblTexturesResult" );
		}

#if !C3D_GenerateBRDFIntegration
		ashes::ImageView doCreatePrefilteredBrdfView( Engine & engine
			, RenderDevice const & device
			, ashes::Image const & texture )
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
				auto image = engine.getImageCache().add( cuT( "BRDF" )
					, imagePath
					, false );
				buffer = image->getPixels();
			}

			buffer = PxBufferBase::create( buffer->getDimensions()
				, PixelFormat::eR8G8B8A8_UNORM
				, buffer->getConstPtr()
				, buffer->getFormat() );
			auto result = texture.createView( VK_IMAGE_VIEW_TYPE_2D, texture.getFormat() );
			auto & renderSystem = *engine.getRenderSystem();
			auto staging = device->createStagingTexture( VK_FORMAT_R8G8B8A8_UNORM
				, makeExtent2D( buffer->getDimensions() ) );
			staging->uploadTextureData( *device.graphicsQueue
				, *device.graphicsCommandPool
				, VK_FORMAT_R8G8B8A8_UNORM
				, buffer->getConstPtr()
				, result );
			return result;
		}
#endif

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
		, ashes::Image const & source
		, SamplerSPtr sampler )
		: OwnedBy< Scene >{ scene }
		, m_prefilteredBrdf{ doCreatePrefilteredBrdf( device, Size{ 512u, 512u } ) }
#if !C3D_GenerateBRDFIntegration
		, m_prefilteredBrdfView{ doCreatePrefilteredBrdfView( *scene.getEngine(), device, *m_prefilteredBrdf ) }
#else
		, m_prefilteredBrdfView{ m_prefilteredBrdf->createView( VK_IMAGE_VIEW_TYPE_2D, m_prefilteredBrdf->getFormat() ) }
#endif
		, m_sampler{ doCreateSampler( *scene.getEngine(), device ) }
		, m_radianceComputer{ *scene.getEngine(), device, Size{ 32u, 32u }, source }
		, m_environmentPrefilter{ *scene.getEngine(), device, Size{ 128u, 128u }, source, std::move( sampler ) }
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
		m_prefilteredBrdf.reset();
	}

	void IblTextures::update()
	{
		m_radianceComputer.render();
		m_environmentPrefilter.render();
	}

	ashes::Semaphore const & IblTextures::update( ashes::Semaphore const & toWait )
	{
		auto result = &toWait;
		result = &m_radianceComputer.render( *result );
		result = &m_environmentPrefilter.render( *result );
		return *result;
	}

	void IblTextures::debugDisplay( Size const & /*renderSize*/ )const
	{
		//int width = int( m_prefilteredBrdf->getDimensions().width );
		//int height = int( m_prefilteredBrdf->getDimensions().height );
		//int left = 0u;
		//int top = renderSize.getHeight() - height;
		//auto size = Size( width, height );
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
