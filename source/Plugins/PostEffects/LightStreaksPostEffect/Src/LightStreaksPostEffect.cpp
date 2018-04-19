#include "LightStreaksPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Mesh/Vertex.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Buffer/VertexBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/SubpassDependency.hpp>
#include <RenderPass/SubpassDescription.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <Graphics/Image.hpp>

#include <numeric>

#include <GlslSource.hpp>

namespace light_streaks
{
	namespace
	{
		glsl::Shader getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = writer.paren( position + 1.0 ) / 2.0;
				gl_Position = vec4( position.xy(), 0.0, 1.0 );
			} );
			return writer.finalise();
		}

		glsl::Shader getHiPassProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, writer.adjustTexCoords( vtx_texture ) ).xyz(), 1.0 );
				auto maxComponent = writer.declLocale( cuT( "maxComponent" ), glsl::max( pxl_fragColor.r(), pxl_fragColor.g() ) );
				maxComponent = glsl::max( maxComponent, pxl_fragColor.b() );

				IF( writer, maxComponent > 1.0_f )
				{
					pxl_fragColor.xyz() /= maxComponent;
				}
				ELSE
				{
					pxl_fragColor.xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );
			return writer.finalise();
		}

		glsl::Shader getKawaseProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			UBO_KAWASE( writer, 0u, 0u );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto colour = writer.declLocale( cuT( "colour" )
					, vec3( 0.0_f ) );
				auto b = writer.declLocale( cuT( "b" )
					, pow( writer.cast< Float >( c3d_samples ), writer.cast< Float >( c3d_pass ) ) );
				auto texcoords = writer.declLocale( cuT( "texcoords" )
					, vtx_texture );

				if ( renderSystem->getCurrentDevice()->getClipDirection() != renderer::ClipDirection::eTopDown )
				{
					texcoords = vec2( texcoords.x(), 1.0 - texcoords.y() );
				}

				FOR( writer, Int, s, 0, "s < c3d_samples", "s++" )
				{
					// Weight = a^(b*s)
					auto weight = writer.declLocale( cuT( "weight" )
						, pow( c3d_attenuation, b * writer.cast< Float >( s ) ) );
					// Streak direction is a 2D vector in image space
					auto sampleCoord = writer.declLocale( cuT( "sampleCoord" )
						, texcoords + writer.paren( c3d_direction * b * vec2( s, s ) * c3d_pixelSize ) );
					// Scale and accumulate
					colour += texture( c3d_mapDiffuse, sampleCoord ).rgb() * clamp( weight, 0.0_f, 1.0_f );
				}
				ROF;

				pxl_fragColor = vec4( clamp( colour, vec3( 0.0_f ), vec3( 1.0_f ) ), 1.0_f );
			} );
			return writer.finalise();
		}

		glsl::Shader getCombineProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto index = castor3d::MinBufferIndex;
			auto c3d_mapScene = writer.declSampler< Sampler2D >( PostEffect::CombineMapScene, 0u, 0u );
			auto c3d_mapKawase1 = writer.declSampler< Sampler2D >( PostEffect::CombineMapKawase1, 1u, 0u );
			auto c3d_mapKawase2 = writer.declSampler< Sampler2D >( PostEffect::CombineMapKawase2, 2u, 0u );
			auto c3d_mapKawase3 = writer.declSampler< Sampler2D >( PostEffect::CombineMapKawase3, 3u, 0u );
			auto c3d_mapKawase4 = writer.declSampler< Sampler2D >( PostEffect::CombineMapKawase4, 4u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto texcoordsKawase = writer.declLocale( cuT( "texcoordsKawase" )
					, writer.adjustTexCoords( vtx_texture ) );
				pxl_fragColor = texture( c3d_mapScene, vec2( texcoordsKawase.x(), 1.0 - texcoordsKawase.y() ) );
				pxl_fragColor += texture( c3d_mapKawase1, texcoordsKawase );
				pxl_fragColor += texture( c3d_mapKawase2, texcoordsKawase );
				pxl_fragColor += texture( c3d_mapKawase3, texcoordsKawase );
				pxl_fragColor += texture( c3d_mapKawase4, texcoordsKawase );
			} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	PostEffect::Surface::Surface( castor3d::RenderSystem & renderSystem
		, renderer::Format format
		, renderer::Extent2D const & size
		, renderer::RenderPass const & renderPass )
	{
		renderer::ImageCreateInfo imageCreateInfo{};
		imageCreateInfo.flags = 0u;
		imageCreateInfo.arrayLayers = 1u;
		imageCreateInfo.extent.width = size.width;
		imageCreateInfo.extent.height = size.height;
		imageCreateInfo.extent.depth = 1u;
		imageCreateInfo.format = format;
		imageCreateInfo.imageType = renderer::TextureType::e2D;
		imageCreateInfo.initialLayout = renderer::ImageLayout::eUndefined;
		imageCreateInfo.mipLevels = 1u;
		imageCreateInfo.samples = renderer::SampleCountFlag::e1;
		imageCreateInfo.sharingMode = renderer::SharingMode::eExclusive;
		imageCreateInfo.tiling = renderer::ImageTiling::eOptimal;
		imageCreateInfo.usage = renderer::ImageUsageFlag::eColourAttachment
			| renderer::ImageUsageFlag::eSampled
			| renderer::ImageUsageFlag::eTransferDst
			| renderer::ImageUsageFlag::eTransferSrc;
		image = std::make_shared< castor3d::TextureLayout >( renderSystem
			, imageCreateInfo
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		image->initialise();

		renderer::FrameBufferAttachmentArray attachments{ { *renderPass.getAttachments().begin(), image->getDefaultView() } };
		frameBuffer = renderPass.createFrameBuffer( size, std::move( attachments ) );
	}

	//*********************************************************************************************

	castor::String const PostEffect::Type = cuT( "light_streaks" );
	castor::String const PostEffect::Name = cuT( "LightStreaks PostEffect" );
	castor::String const PostEffect::CombineMapScene = cuT( "c3d_mapScene" );
	castor::String const PostEffect::CombineMapKawase1 = cuT( "c3d_mapKawase1" );
	castor::String const PostEffect::CombineMapKawase2 = cuT( "c3d_mapKawase2" );
	castor::String const PostEffect::CombineMapKawase3 = cuT( "c3d_mapKawase3" );
	castor::String const PostEffect::CombineMapKawase4 = cuT( "c3d_mapKawase4" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
		: castor3d::PostEffect( PostEffect::Type
			, renderTarget
			, renderSystem
			, params )
		, m_kawaseUbo{ *renderSystem.getEngine() }
	{
		m_linearSampler = doCreateSampler( true );
		m_nearestSampler = doCreateSampler( false );
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_kawaseUbo.initialise();
		m_linearSampler->initialise();
		m_nearestSampler->initialise();
		auto & engine = *getRenderSystem()->getEngine();
		auto & device = *getRenderSystem()->getCurrentDevice();
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Create vertex buffer
		m_vertexBuffer = renderer::makeVertexBuffer< castor3d::NonTexturedQuad >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto data = m_vertexBuffer->lock( 0u, 1u, renderer::MemoryMapFlag::eWrite ) )
		{
			castor3d::NonTexturedQuad buffer
			{
				{
					castor::Point2f{ -1.0, -1.0 },
					castor::Point2f{ -1.0, +1.0 },
					castor::Point2f{ +1.0, -1.0 },
					castor::Point2f{ +1.0, -1.0 },
					castor::Point2f{ -1.0, +1.0 },
					castor::Point2f{ +1.0, +1.0 }
				},
			};
			*data = buffer;
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

		// Create descriptor and pipeline layouts
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};

		for ( uint32_t i = 0u; i < Count; ++i )
		{
			setLayoutBindings.emplace_back( i + 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment );
		}

		m_pipelines.combine.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.combine.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.combine.layout.descriptorLayout );
		m_pipelines.combine.layout.descriptorPool = m_pipelines.combine.layout.descriptorLayout->createPool( 1u );

		setLayoutBindings =
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_pipelines.hiPass.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.hiPass.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.hiPass.layout.descriptorLayout );
		m_pipelines.hiPass.layout.descriptorPool = m_pipelines.hiPass.layout.descriptorLayout->createPool( Count );

		setLayoutBindings =
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_pipelines.kawase.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.kawase.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.kawase.layout.descriptorLayout );
		m_pipelines.kawase.layout.descriptorPool = m_pipelines.kawase.layout.descriptorLayout->createPool( Count * 3u );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target->getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );
		size.width >>= 2;
		size.height >>= 2;
		uint32_t index = 0u;
		static float constexpr factor = 0.2f;
		static std::array< castor::Point2f, Count > directions
		{
			{
				castor::Point2f{ factor, factor },
				castor::Point2f{ -factor, -factor },
				castor::Point2f{ -factor, factor },
				castor::Point2f{ factor, -factor }
			}
		};

		for ( auto i = 0u; i < Count; ++i )
		{
			m_pipelines.hiPass.surfaces.emplace_back( *getRenderSystem()
				, m_target->getPixelFormat()
				, size
				, *m_renderPass );
			m_pipelines.kawase.surfaces.emplace_back( *getRenderSystem()
				, m_target->getPixelFormat()
				, size
				, *m_renderPass );

			for ( uint32_t j = 0u; j < 3u; ++j )
			{
				m_kawaseUbo.update( index
					, size
					, directions[i]
					, j );
				++index;
			}
		}

		m_kawaseUbo.upload();
		bool result = doInitialiseHiPassProgram();

		if ( result )
		{
			result = doInitialiseKawaseProgram();
		}

		if ( result )
		{
			result = doInitialiseCombineProgram();
		}

		if ( result )
		{
			result = doBuildCommandBuffer( timer );
		}

		m_result = m_pipelines.combine.surfaces[0].image.get();
		return result;
	}

	void PostEffect::doCleanup()
	{
		m_kawaseUbo.cleanup();
		m_vertexBuffer.reset();

		Pipeline dummy1, dummy2, dummy3;
		std::swap( m_pipelines.combine, dummy1 );
		std::swap( m_pipelines.kawase, dummy2 );
		std::swap( m_pipelines.hiPass, dummy3 );

		m_linearSampler->cleanup();
		m_nearestSampler->cleanup();

		m_renderPass.reset();
	}

	bool PostEffect::doWriteInto( castor::TextFile & file )
	{
		return true;
	}

	castor3d::SamplerSPtr PostEffect::doCreateSampler( bool p_linear )
	{
		castor::String name = cuT( "KawaseSampler_" );
		renderer::Filter mode;

		if ( p_linear )
		{
			name += cuT( "Linear" );
			mode = renderer::Filter::eLinear;
		}
		else
		{
			name += cuT( "Nearest" );
			mode = renderer::Filter::eNearest;
		}

		castor3d::SamplerSPtr sampler;

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( mode );
			sampler->setMagFilter( mode );
			sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			sampler->setWrapR( renderer::WrapMode::eClampToEdge );
		}
		else
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		return sampler;
	}

	bool PostEffect::doInitialiseHiPassProgram()
	{
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & device = *getRenderSystem()->getCurrentDevice();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const hipass = getHiPassProgram( getRenderSystem() );

		// Create pipeline
		renderer::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), renderer::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( vertex.getSource() );
		stages[1].module->loadShader( hipass.getSource() );

		renderer::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, false, false }
		};
		m_pipelines.hiPass.pipeline = m_pipelines.hiPass.layout.pipelineLayout->createPipeline( pipeline );

		// Create descriptor sets
		for ( auto & surface : m_pipelines.hiPass.surfaces )
		{
			surface.descriptorSets.emplace_back( m_pipelines.hiPass.layout.descriptorPool->createDescriptorSet( 0u ) );
			auto & descriptorSet = *surface.descriptorSets.back();
			descriptorSet.createBinding( m_pipelines.hiPass.layout.descriptorLayout->getBinding( 0u )
				, m_target->getDefaultView()
				, m_linearSampler->getSampler()
				, renderer::ImageLayout::eShaderReadOnlyOptimal );
			descriptorSet.update();
		}

		return true;
	}

	bool PostEffect::doInitialiseKawaseProgram()
	{
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & device = *getRenderSystem()->getCurrentDevice();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const kawase = getKawaseProgram( getRenderSystem() );

		// Create pipeline
		renderer::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), renderer::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( vertex.getSource() );
		stages[1].module->loadShader( kawase.getSource() );

		renderer::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, false, false }
		};
		m_pipelines.kawase.pipeline = m_pipelines.kawase.layout.pipelineLayout->createPipeline( pipeline );
		uint32_t index = 0u;

		for ( uint32_t j = 0u; j < Count; ++j )
		{
			auto * source = &m_pipelines.hiPass.surfaces[j];
			auto * destination = &m_pipelines.kawase.surfaces[j];
			auto & surface = m_pipelines.kawase.surfaces[j];

			for ( auto i = 0u; i < 3u; ++i )
			{
				surface.descriptorSets.emplace_back( m_pipelines.kawase.layout.descriptorPool->createDescriptorSet( 0u ) );
				auto & descriptorSet = *surface.descriptorSets.back();
				descriptorSet.createBinding( m_pipelines.kawase.layout.descriptorLayout->getBinding( 0u )
					, m_kawaseUbo.getUbo()
					, index );
				descriptorSet.createBinding( m_pipelines.kawase.layout.descriptorLayout->getBinding( 1u )
					, source->image->getDefaultView()
					, m_linearSampler->getSampler()
					, renderer::ImageLayout::eShaderReadOnlyOptimal );
				descriptorSet.update();
				std::swap( source, destination );
				++index;
			}
		}

		return true;
	}

	bool PostEffect::doInitialiseCombineProgram()
	{
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & device = *getRenderSystem()->getCurrentDevice();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const combine = getCombineProgram( getRenderSystem() );

		// Create pipeline
		renderer::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), renderer::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( vertex.getSource() );
		stages[1].module->loadShader( combine.getSource() );

		renderer::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, false, false }
		};
		m_pipelines.combine.pipeline = m_pipelines.combine.layout.pipelineLayout->createPipeline( pipeline );

		// Create view and associated frame buffer.
		m_pipelines.combine.surfaces.emplace_back( *getRenderSystem()
			, m_target->getPixelFormat()
			, size
			, *m_renderPass );

		// Create descriptor
		auto & surface = m_pipelines.combine.surfaces.back();
		surface.descriptorSets.emplace_back( m_pipelines.combine.layout.descriptorPool->createDescriptorSet( 0u ) );
		auto & descriptorSet = *surface.descriptorSets.back();
		descriptorSet.createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( 0u )
			, m_target->getDefaultView()
			, m_linearSampler->getSampler()
			, renderer::ImageLayout::eShaderReadOnlyOptimal );

		for ( uint32_t i = 0u; i < Count; ++i )
		{
			descriptorSet.createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( i + 1u )
				, m_pipelines.kawase.surfaces[i].image->getDefaultView()
				, m_linearSampler->getSampler()
				, renderer::ImageLayout::eShaderReadOnlyOptimal );
		}

		descriptorSet.update();

		return true;
	}

	bool PostEffect::doBuildCommandBuffer( castor3d::RenderPassTimer const & timer )
	{
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Fill command buffer.
		static renderer::ClearValueArray const clearValues[Count + 1]
		{
			{ renderer::ClearColorValue{ 1.0, 0.0, 0.0, 1.0 } },
			{ renderer::ClearColorValue{ 0.0, 1.0, 0.0, 1.0 } },
			{ renderer::ClearColorValue{ 0.0, 0.0, 1.0, 1.0 } },
			{ renderer::ClearColorValue{ 1.0, 1.0, 0.0, 1.0 } },
			{ renderer::ClearColorValue{ 0.0, 1.0, 1.0, 1.0 } },
		};
		bool result = m_commandBuffer->begin();

		if ( result )
		{
			m_commandBuffer->resetQueryPool( timer.getQuery()
				, 0u
				, 2u );
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, timer.getQuery()
				, 0u );
			// Put target image in fragment shader input layout.
			m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_target->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eColourAttachmentOptimal
					, renderer::AccessFlag::eColourAttachmentWrite ) );

			// Hi-pass.
			auto & hiPassSurface = m_pipelines.hiPass.surfaces[0];
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *hiPassSurface.frameBuffer
				, clearValues[0]
				, renderer::SubpassContents::eInline );
			m_commandBuffer->setViewport( { hiPassSurface.image->getWidth(), hiPassSurface.image->getHeight(), 0, 0, } );
			m_commandBuffer->setScissor( { 0, 0, hiPassSurface.image->getWidth(), hiPassSurface.image->getHeight() } );
			m_commandBuffer->bindPipeline( *m_pipelines.hiPass.pipeline );
			m_commandBuffer->bindDescriptorSet( *hiPassSurface.descriptorSets.back(),
				*m_pipelines.hiPass.layout.pipelineLayout );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 6u );
			m_commandBuffer->endRenderPass();

			// Put source image in transfer source layout
			m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eFragmentShader
				, renderer::PipelineStageFlag::eTransfer
				, hiPassSurface.image->getDefaultView().makeTransferSource( renderer::ImageLayout::eUndefined, 0u ) );

			// Copy Hi pass result to other Hi pass surfaces
			for ( uint32_t i = 1u; i < Count; ++i )
			{
				// Put destination image in transfer destination layout
				m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eFragmentShader
					, renderer::PipelineStageFlag::eTransfer
					, m_pipelines.hiPass.surfaces[i].image->getDefaultView().makeTransferDestination( renderer::ImageLayout::eUndefined, 0u ) );
				m_commandBuffer->copyImage( hiPassSurface.image->getDefaultView()
					, m_pipelines.hiPass.surfaces[i].image->getDefaultView() );
				// Put back destination image in shader input layout
				m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTransfer
					, renderer::PipelineStageFlag::eFragmentShader
					, m_pipelines.hiPass.surfaces[i].image->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eTransferDstOptimal
						, renderer::AccessFlag::eTransferWrite ) );
			}

			// Kawase blur passes.
			for ( uint32_t i = 0u; i < Count; ++i )
			{
				auto * source = &m_pipelines.hiPass.surfaces[i];
				auto * destination = &m_pipelines.kawase.surfaces[i];
				auto & kawaseSurface = m_pipelines.kawase.surfaces[i];

				for ( uint32_t j = 0u; j < 3u; ++j )
				{
					// Put source image in shader input layout
					m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTransfer
						, renderer::PipelineStageFlag::eFragmentShader
						, source->image->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );

					m_commandBuffer->beginRenderPass( *m_renderPass
						, *destination->frameBuffer
						, clearValues[i]
						, renderer::SubpassContents::eInline );
					m_commandBuffer->setViewport( { destination->image->getWidth(), destination->image->getHeight(), 0, 0, } );
					m_commandBuffer->setScissor( { 0, 0, destination->image->getWidth(), destination->image->getHeight() } );
					m_commandBuffer->bindPipeline( *m_pipelines.kawase.pipeline );
					m_commandBuffer->bindDescriptorSet( *kawaseSurface.descriptorSets[j],
						*m_pipelines.kawase.layout.pipelineLayout );
					m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
					m_commandBuffer->draw( 6u );
					m_commandBuffer->endRenderPass();
					std::swap( destination, source );
				}

				// Put Kawase surface's general view in fragment shader input layout.
				m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eTransfer
					, renderer::PipelineStageFlag::eFragmentShader
					, kawaseSurface.image->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			}

			// Combine pass.
			auto & combineSurface = m_pipelines.combine.surfaces.back();
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *combineSurface.frameBuffer
				, clearValues[Count]
				, renderer::SubpassContents::eInline );
			m_commandBuffer->setViewport( { size.width, size.height, 0, 0, } );
			m_commandBuffer->setScissor( { 0, 0, size.width, size.height } );
			m_commandBuffer->bindPipeline( *m_pipelines.combine.pipeline );
			m_commandBuffer->bindDescriptorSet( *combineSurface.descriptorSets.back()
				, *m_pipelines.combine.layout.pipelineLayout );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 6u );
			m_commandBuffer->endRenderPass();

			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, timer.getQuery()
				, 1u );
			result = m_commandBuffer->end();
		}

		return result;
	}
}
