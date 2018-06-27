#include "ShadowMapPoint.hpp"

#include "Engine.hpp"
#include "Cache/SamplerCache.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/Culling/DummyCuller.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "ShadowMap/ShadowMapPassPoint.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureView.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseVariance( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Point_Shadow" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
				sampler->setBorderColour( renderer::BorderColour::eFloatOpaqueWhite );
			}

			renderer::ImageCreateInfo image{};
			image.flags = renderer::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			image.format = ShadowMapPoint::VarianceFormat;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		TextureUnit doInitialiseDepth( Engine & engine
			, Size const & size )
		{
			String const name = cuT( "ShadowMap_Point_Depth" );
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
				sampler->setBorderColour( renderer::BorderColour::eFloatOpaqueWhite );
			}

			renderer::ImageCreateInfo image{};
			image.flags = renderer::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			image.format = ShadowMapPoint::LinearDepthFormat;

			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		std::vector< ShadowMap::PassData > createPasses( Engine & engine
			, Scene & scene
			, ShadowMap & shadowMap )
		{
			std::vector< ShadowMap::PassData > result;

			for ( auto i = 0u; i < 6u; ++i )
			{
				ShadowMap::PassData passData
				{
					std::make_unique< MatrixUbo >( engine ),
					nullptr,
					std::make_unique< DummyCuller >( scene ),
					nullptr,
				};
				passData.pass = std::make_shared< ShadowMapPassPoint >( engine
					, *passData.matrixUbo
					, *passData.culler
					, shadowMap );
				result.emplace_back( std::move( passData ) );
			}

			return result;
		}
	}

	ShadowMapPoint::ShadowMapPoint( Engine & engine
		, Scene & scene )
		: ShadowMap{ engine
			, doInitialiseVariance( engine, Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } )
			, doInitialiseDepth( engine, Size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize } )
			, createPasses( engine, scene, *this ) }
	{
	}

	ShadowMapPoint::~ShadowMapPoint()
	{
	}

	void ShadowMapPoint::update( Camera const & camera
		, RenderQueueArray & queues
		, Light & light
		, uint32_t index )
	{
		m_shadowType = light.getShadowType();

		for ( auto & pass : m_passes )
		{
			pass.pass->update( camera, queues, light, index );
		}
	}

	renderer::Semaphore const & ShadowMapPoint::render( renderer::Semaphore const & toWait )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static renderer::ClearColorValue const white{ component, component, component, component };
		static renderer::DepthStencilClearValue const zero{ 1.0f, 0 };
		auto & myTimer = m_passes[0].pass->getTimer();
		auto timerBlock = myTimer.start();
		auto * result = &toWait;

		for ( size_t face = 0u; face < m_passes.size(); ++face )
		{
			m_passes[face].pass->updateDeviceDependent( uint32_t( face ) );
		}

		m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit );

		for ( size_t face = 0u; face < m_passes.size(); ++face )
		{
			auto & pass = m_passes[face];
			auto & timer = pass.pass->getTimer();
			auto & renderPass = pass.pass->getRenderPass();
			auto & frameBuffer = m_frameBuffers[face];

			timer.notifyPassRender();
			timer.beginPass( *m_commandBuffer );
			m_commandBuffer->beginRenderPass( renderPass
				, *frameBuffer.frameBuffer
				, { zero, white, white }
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { pass.pass->getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			timer.endPass( *m_commandBuffer );
		}

		m_commandBuffer->end();
		auto & device = getCurrentDevice( *this );

		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	void ShadowMapPoint::debugDisplay( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, castor::Size const & size, uint32_t index )
	{
		//Size displaySize{ 128u, 128u };
		//Position position{ int32_t( displaySize.getWidth() * 4 * index), int32_t( displaySize.getHeight() * 4 ) };
		//getEngine()->getRenderSystem()->getCurrentDevice()->renderVarianceCube( position
		//	, displaySize
		//	, *m_shadowMap.getTexture() );
		//position = Position{ int32_t( displaySize.getWidth() * 4 * ( index + 2 ) ), int32_t( displaySize.getHeight() * 4 ) };
		//getEngine()->getRenderSystem()->getCurrentDevice()->renderDepthCube( position
		//	, displaySize
		//	, *m_linearMap.getTexture() );
	}

	void ShadowMapPoint::doInitialise()
	{
		renderer::Extent2D size{ ShadowMapPassPoint::TextureSize, ShadowMapPassPoint::TextureSize };
		auto & device = getCurrentDevice( *this );

		renderer::ImageCreateInfo depth{};
		depth.arrayLayers = 1u;
		depth.extent.width = size.width;
		depth.extent.height = size.height;
		depth.extent.depth = 1u;
		depth.imageType = renderer::TextureType::e2D;
		depth.mipLevels = 1u;
		depth.samples = renderer::SampleCountFlag::e1;
		depth.usage = renderer::ImageUsageFlag::eDepthStencilAttachment;
		depth.format = ShadowMapPoint::RawDepthFormat;
		m_depthTexture = device.createTexture( depth, renderer::MemoryPropertyFlag::eDeviceLocal );

		renderer::ImageViewCreateInfo depthView;
		depthView.format = depth.format;
		depthView.viewType = renderer::TextureViewType::e2D;
		depthView.subresourceRange.aspectMask = renderer::ImageAspectFlag::eDepth;
		depthView.subresourceRange.baseArrayLayer = 0u;
		depthView.subresourceRange.layerCount = 1u;
		depthView.subresourceRange.baseMipLevel = 0u;
		depthView.subresourceRange.levelCount = 1u;
		m_depthView = m_depthTexture->createView( depthView );

		auto & variance = m_shadowMap.getTexture()->getTexture();
		auto & linear = m_linearMap.getTexture()->getTexture();
		uint32_t face = 0u;

		for ( auto & frameBuffer : m_frameBuffers )
		{
			auto & pass = m_passes[face];
			auto & renderPass = pass.pass->getRenderPass();
			frameBuffer.varianceView = variance.createView( renderer::TextureViewType::e2D
				, variance.getFormat()
				, 0u
				, 1u
				, face
				, 1u );
			frameBuffer.linearView = linear.createView( renderer::TextureViewType::e2D
				, linear.getFormat()
				, 0u
				, 1u
				, face
				, 1u );
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 0u ), *m_depthView );
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 1u ), *frameBuffer.linearView );
			attaches.emplace_back( *( renderPass.getAttachments().begin() + 2u ), *frameBuffer.varianceView );
			frameBuffer.frameBuffer = renderPass.createFrameBuffer( size, std::move( attaches ) );
			++face;
		}

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
	}

	void ShadowMapPoint::doCleanup()
	{
		for ( auto & frameBuffer : m_frameBuffers )
		{
			frameBuffer.frameBuffer.reset();
			frameBuffer.varianceView.reset();
			frameBuffer.linearView = nullptr;
		}

		m_depthView.reset();
		m_depthTexture.reset();
	}

	void ShadowMapPoint::doUpdateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapPoint );
	}

	glsl::Shader ShadowMapPoint::doGetPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, renderer::CompareOp alphaFunc )const
	{
		using namespace glsl;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		Ubo shadowMap{ writer, ShadowMapPassPoint::ShadowMapUbo, ShadowMapPassPoint::UboBindingPoint };
		auto c3d_wordLightPosition( shadowMap.declMember< Vec3 >( ShadowMapPassPoint::WorldLightPosition ) );
		auto c3d_farPlane( shadowMap.declMember< Float >( ShadowMapPassPoint::FarPlane ) );
		shadowMap.end();

		auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" )
			, RenderPass::VertexOutputs::WorldPositionLocation );
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" )
			, RenderPass::VertexOutputs::TextureLocation );
		auto vtx_material = writer.declInput< Int >( cuT( "vtx_material" )
			, RenderPass::VertexOutputs::MaterialLocation );
		auto c3d_mapOpacity( writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
			, MinBufferIndex
			, 1u
			, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );

		auto materials = shader::createMaterials( writer, passFlags );
		materials->declare();

		// Fragment Outputs
		auto pxl_linear = writer.declFragData< Float >( cuT( "pxl_linear" ), 0u );
		auto pxl_variance = writer.declFragData< Vec2 >( cuT( "pxl_variance" ), 1u );

		auto main = [&]()
		{
			doDiscardAlpha( writer
				, textureFlags
				, alphaFunc
				, vtx_material
				, *materials );

			auto depth = writer.declLocale( cuT( "depth" )
				, length( vtx_worldPosition - c3d_wordLightPosition ) );
			pxl_linear = depth / c3d_farPlane;
			pxl_variance.x() = pxl_linear;
			pxl_variance.y() = pxl_linear * pxl_linear;

			auto dx = writer.declLocale( cuT( "dx" )
				, dFdx( pxl_linear ) );
			auto dy = writer.declLocale( cuT( "dy" )
				, dFdy( pxl_linear ) );
			pxl_variance.y() += 0.25_f * writer.paren( dx * dx + dy * dy );
		};

		writer.implementFunction< void >( cuT( "main" ), main );
		return writer.finalise();
	}
}
