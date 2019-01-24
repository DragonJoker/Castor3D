#include "LightStreaksPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Mesh/Vertex.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Shader/Shaders/GlslUtils.hpp>
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
#include <Sync/ImageMemoryBarrier.hpp>

#include <Graphics/Image.hpp>

#include <numeric>

#include <ShaderWriter/Source.hpp>

namespace light_streaks
{
	namespace
	{
		std::unique_ptr< sdw::Shader > getVertexProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer, renderSystem->isTopDown() };
			utils.declareNegateVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = writer.paren( position + 1.0_f ) / 2.0_f;
					out.gl_out.gl_Position = vec4( position.xy(), 0.0, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getHiPassProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDiffuse", 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = vec4( texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).xyz(), 1.0 );
					auto maxComponent = writer.declLocale( "maxComponent"
						, max( pxl_fragColor.r(), pxl_fragColor.g() ) );
					maxComponent = max( maxComponent, pxl_fragColor.b() );

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
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getKawaseProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_KAWASE( writer, 0u, 0u );
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDiffuse", 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto colour = writer.declLocale( "colour"
						, vec3( 0.0_f ) );
					auto b = writer.declLocale( "b"
						, pow( writer.cast< Float >( c3d_samples ), writer.cast< Float >( c3d_pass ) ) );
					auto texcoords = writer.declLocale( "texcoords"
						, vtx_texture );

					if ( getCurrentDevice( *renderSystem ).getClipDirection() != ashes::ClipDirection::eTopDown )
					{
						texcoords = vec2( texcoords.x(), 1.0 - texcoords.y() );
					}

					FOR( writer, Int, s, 0, s < c3d_samples, ++s )
					{
						// Weight = a^(b*s)
						auto weight = writer.declLocale( "weight"
							, pow( c3d_attenuation, b * writer.cast< Float >( s ) ) );
						// Streak direction is a 2D vector in image space
						auto sampleCoord = writer.declLocale( "sampleCoord"
							, texcoords + writer.paren( c3d_direction * b * vec2( s, s ) * c3d_pixelSize ) );
						// Scale and accumulate
						colour += texture( c3d_mapDiffuse, sampleCoord ).rgb() * clamp( weight, 0.0_f, 1.0_f );
					}
					ROF;

					pxl_fragColor = vec4( clamp( colour, vec3( 0.0_f ), vec3( 1.0_f ) ), 1.0_f );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getCombineProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto index = castor3d::MinBufferIndex;
			auto c3d_mapScene = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapScene, 0u, 0u );
			auto c3d_mapKawase1 = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapKawase1, 1u, 0u );
			auto c3d_mapKawase2 = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapKawase2, 2u, 0u );
			auto c3d_mapKawase3 = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapKawase3, 3u, 0u );
			auto c3d_mapKawase4 = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapKawase4, 4u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			castor3d::shader::Utils utils{ writer, renderSystem->isTopDown() };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
			{
				auto texcoordsKawase = writer.declLocale( "texcoordsKawase"
					, vtx_texture );
				pxl_fragColor = texture( c3d_mapScene, utils.bottomUpToTopDown( texcoordsKawase ) );
				pxl_fragColor += texture( c3d_mapKawase1, texcoordsKawase );
				pxl_fragColor += texture( c3d_mapKawase2, texcoordsKawase );
				pxl_fragColor += texture( c3d_mapKawase3, texcoordsKawase );
				pxl_fragColor += texture( c3d_mapKawase4, texcoordsKawase );
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	PostEffect::Surface::Surface( castor3d::RenderSystem & renderSystem
		, ashes::Format format
		, ashes::Extent2D const & size
		, ashes::RenderPass const & renderPass )
	{
		ashes::ImageCreateInfo imageCreateInfo{};
		imageCreateInfo.flags = 0u;
		imageCreateInfo.arrayLayers = 1u;
		imageCreateInfo.extent.width = size.width;
		imageCreateInfo.extent.height = size.height;
		imageCreateInfo.extent.depth = 1u;
		imageCreateInfo.format = format;
		imageCreateInfo.imageType = ashes::TextureType::e2D;
		imageCreateInfo.initialLayout = ashes::ImageLayout::eUndefined;
		imageCreateInfo.mipLevels = 1u;
		imageCreateInfo.samples = ashes::SampleCountFlag::e1;
		imageCreateInfo.sharingMode = ashes::SharingMode::eExclusive;
		imageCreateInfo.tiling = ashes::ImageTiling::eOptimal;
		imageCreateInfo.usage = ashes::ImageUsageFlag::eColourAttachment
			| ashes::ImageUsageFlag::eSampled
			| ashes::ImageUsageFlag::eTransferDst
			| ashes::ImageUsageFlag::eTransferSrc;
		image = std::make_shared< castor3d::TextureLayout >( renderSystem
			, imageCreateInfo
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		image->initialise();

		ashes::FrameBufferAttachmentArray attachments{ { *renderPass.getAttachments().begin(), image->getDefaultView() } };
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
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, params )
		, m_kawaseUbo{ *renderSystem.getEngine() }
		, m_pipelines
		{
			{
				{ ashes::ShaderStageFlag::eVertex, "LightStreaksHiPass" },
				{ ashes::ShaderStageFlag::eFragment, "LightStreaksHiPass" },
			},
			{
				{ ashes::ShaderStageFlag::eVertex, "LightStreaksKawase" },
				{ ashes::ShaderStageFlag::eFragment, "LightStreaksKawase" },
			},
			{
				{ ashes::ShaderStageFlag::eVertex, "LightStreaksCombine" },
				{ ashes::ShaderStageFlag::eFragment, "LightStreaksCombine" },
			},
		}
	{
		m_linearSampler = doCreateSampler( true );
		m_nearestSampler = doCreateSampler( false );

		m_passesCount = 4u;
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

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "HiPass" )
			, ashes::ShaderStageFlag::eVertex
			, *m_pipelines.hiPass.vertexShader.shader );
		visitor.visit( cuT( "HiPass" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pipelines.hiPass.pixelShader.shader );

		visitor.visit( cuT( "Kawase" )
			, ashes::ShaderStageFlag::eVertex
			, *m_pipelines.kawase.vertexShader.shader );
		visitor.visit( cuT( "Kawase" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pipelines.kawase.pixelShader.shader );
		visitor.visit( cuT( "Kawase" )
			, ashes::ShaderStageFlag::eFragment
			, cuT( "Kawase" )
			, cuT( "Attenuation" )
			, m_kawaseUbo.getUbo().getData().attenuation );
		visitor.visit( cuT( "Kawase" )
			, ashes::ShaderStageFlag::eFragment
			, cuT( "Kawase" )
			, cuT( "Samples" )
			, m_kawaseUbo.getUbo().getData().samples );

		visitor.visit( cuT( "Combine" )
			, ashes::ShaderStageFlag::eVertex
			, *m_pipelines.combine.vertexShader.shader );
		visitor.visit( cuT( "Combine" )
			, ashes::ShaderStageFlag::eFragment
			, *m_pipelines.combine.pixelShader.shader );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_kawaseUbo.initialise();
		m_linearSampler->initialise();
		m_nearestSampler->initialise();
		auto & device = getCurrentDevice( *this );
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Create vertex buffer
		m_vertexBuffer = ashes::makeVertexBuffer< castor3d::NonTexturedQuad >( device
			, 1u
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );

		if ( auto data = m_vertexBuffer->lock( 0u, 1u, ashes::MemoryMapFlag::eWrite ) )
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
		ashes::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
		};

		for ( uint32_t i = 0u; i < Count; ++i )
		{
			setLayoutBindings.emplace_back( i + 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment );
		}

		m_pipelines.combine.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.combine.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.combine.layout.descriptorLayout );
		m_pipelines.combine.layout.descriptorPool = m_pipelines.combine.layout.descriptorLayout->createPool( 1u );

		setLayoutBindings =
		{
			{ 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment }
		};
		m_pipelines.hiPass.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.hiPass.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.hiPass.layout.descriptorLayout );
		m_pipelines.hiPass.layout.descriptorPool = m_pipelines.hiPass.layout.descriptorLayout->createPool( Count );

		setLayoutBindings =
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
			{ 1u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment }
		};
		m_pipelines.kawase.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.kawase.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.kawase.layout.descriptorLayout );
		m_pipelines.kawase.layout.descriptorPool = m_pipelines.kawase.layout.descriptorLayout->createPool( Count * 3u );

		// Create the render pass.
		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target->getPixelFormat();
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

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

		Pipeline dummy1
		{
			{ ashes::ShaderStageFlag::eVertex, "LightStreaksCombine" },
			{ ashes::ShaderStageFlag::eVertex, "LightStreaksCombine" },
		};
		Pipeline dummy2
		{
			{ ashes::ShaderStageFlag::eVertex, "LightStreaksKawase" },
			{ ashes::ShaderStageFlag::eVertex, "LightStreaksKawase" },
		};
		Pipeline dummy3
		{
			{ ashes::ShaderStageFlag::eVertex, "LightStreaksHiPass" },
			{ ashes::ShaderStageFlag::eVertex, "LightStreaksHiPass" },
		};
		std::swap( m_pipelines.combine, dummy1 );
		std::swap( m_pipelines.kawase, dummy2 );
		std::swap( m_pipelines.hiPass, dummy3 );

		m_linearSampler->cleanup();
		m_nearestSampler->cleanup();

		m_renderPass.reset();
	}

	bool PostEffect::doWriteInto( castor::TextFile & file, castor::String const & tabs )
	{
		return file.writeText( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) ) > 0;
	}

	castor3d::SamplerSPtr PostEffect::doCreateSampler( bool p_linear )
	{
		castor::String name = cuT( "KawaseSampler_" );
		ashes::Filter mode;

		if ( p_linear )
		{
			name += cuT( "Linear" );
			mode = ashes::Filter::eLinear;
		}
		else
		{
			name += cuT( "Nearest" );
			mode = ashes::Filter::eNearest;
		}

		castor3d::SamplerSPtr sampler;

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( mode );
			sampler->setMagFilter( mode );
			sampler->setWrapS( ashes::WrapMode::eClampToEdge );
			sampler->setWrapT( ashes::WrapMode::eClampToEdge );
			sampler->setWrapR( ashes::WrapMode::eClampToEdge );
		}
		else
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		return sampler;
	}

	bool PostEffect::doInitialiseHiPassProgram()
	{
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentDevice( *this );
		m_pipelines.hiPass.vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pipelines.hiPass.pixelShader.shader = getHiPassProgram( getRenderSystem() );

		// Create pipeline
		ashes::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), ashes::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, ashes::Format::eR32G32_SFLOAT, 0u } );

		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( renderSystem.compileShader( m_pipelines.hiPass.vertexShader ) );
		stages[1].module->loadShader( renderSystem.compileShader( m_pipelines.hiPass.pixelShader ) );

		ashes::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{},
			ashes::MultisampleState{},
			ashes::ColourBlendState::createDefault(),
			{ ashes::DynamicState::eViewport, ashes::DynamicState::eScissor },
			ashes::DepthStencilState{ 0u, false, false }
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
				, ashes::ImageLayout::eShaderReadOnlyOptimal );
			descriptorSet.update();
		}

		return true;
	}

	bool PostEffect::doInitialiseKawaseProgram()
	{
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentDevice( *this );
		m_pipelines.kawase.vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pipelines.kawase.pixelShader.shader = getKawaseProgram( getRenderSystem() );

		// Create pipeline
		ashes::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), ashes::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, ashes::Format::eR32G32_SFLOAT, 0u } );

		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( renderSystem.compileShader( m_pipelines.kawase.vertexShader ) );
		stages[1].module->loadShader( renderSystem.compileShader( m_pipelines.kawase.pixelShader ) );

		ashes::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{},
			ashes::MultisampleState{},
			ashes::ColourBlendState::createDefault(),
			{ ashes::DynamicState::eViewport, ashes::DynamicState::eScissor },
			ashes::DepthStencilState{ 0u, false, false }
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
					, ashes::ImageLayout::eShaderReadOnlyOptimal );
				descriptorSet.update();
				std::swap( source, destination );
				++index;
			}
		}

		return true;
	}

	bool PostEffect::doInitialiseCombineProgram()
	{
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentDevice( *this );
		m_pipelines.combine.vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pipelines.combine.pixelShader.shader = getCombineProgram( getRenderSystem() );

		// Create pipeline
		ashes::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), ashes::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, ashes::Format::eR32G32_SFLOAT, 0u } );

		ashes::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( renderSystem.compileShader( m_pipelines.combine.vertexShader ) );
		stages[1].module->loadShader( renderSystem.compileShader( m_pipelines.combine.pixelShader ) );

		ashes::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{},
			ashes::MultisampleState{},
			ashes::ColourBlendState::createDefault(),
			{ ashes::DynamicState::eViewport, ashes::DynamicState::eScissor },
			ashes::DepthStencilState{ 0u, false, false }
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
			, ashes::ImageLayout::eShaderReadOnlyOptimal );

		for ( uint32_t i = 0u; i < Count; ++i )
		{
			descriptorSet.createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( i + 1u )
				, m_pipelines.kawase.surfaces[i].image->getDefaultView()
				, m_linearSampler->getSampler()
				, ashes::ImageLayout::eShaderReadOnlyOptimal );
		}

		descriptorSet.update();

		return true;
	}

	bool PostEffect::doBuildCommandBuffer( castor3d::RenderPassTimer const & timer )
	{
		auto & device = getCurrentDevice( *this );
		ashes::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Fill command buffer.
		static ashes::ClearValueArray const clearValues[Count + 1]
		{
			{ ashes::ClearColorValue{ 1.0, 0.0, 0.0, 1.0 } },
			{ ashes::ClearColorValue{ 0.0, 1.0, 0.0, 1.0 } },
			{ ashes::ClearColorValue{ 0.0, 0.0, 1.0, 1.0 } },
			{ ashes::ClearColorValue{ 1.0, 1.0, 0.0, 1.0 } },
			{ ashes::ClearColorValue{ 0.0, 1.0, 1.0, 1.0 } },
		};
		auto & hiPassSurface = m_pipelines.hiPass.surfaces[0];
		castor3d::CommandsSemaphore hiPassCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & hiPassCmd = *hiPassCommands.commandBuffer;
		hiPassCmd.begin();
		timer.beginPass( hiPassCmd, 0u );
		// Put target image in fragment shader input layout.
		hiPassCmd.memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_target->getDefaultView().makeShaderInputResource( ashes::ImageLayout::eColourAttachmentOptimal
				, ashes::AccessFlag::eColourAttachmentWrite ) );

		// Hi-pass.
		hiPassCmd.beginRenderPass( *m_renderPass
			, *hiPassSurface.frameBuffer
			, clearValues[0]
			, ashes::SubpassContents::eInline );
		hiPassCmd.setViewport( { hiPassSurface.image->getWidth(), hiPassSurface.image->getHeight(), 0, 0, } );
		hiPassCmd.setScissor( { 0, 0, hiPassSurface.image->getWidth(), hiPassSurface.image->getHeight() } );
		hiPassCmd.bindPipeline( *m_pipelines.hiPass.pipeline );
		hiPassCmd.bindDescriptorSet( *hiPassSurface.descriptorSets.back(),
			*m_pipelines.hiPass.layout.pipelineLayout );
		hiPassCmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		hiPassCmd.draw( 6u );
		hiPassCmd.endRenderPass();
		// Put source image in transfer source layout
		hiPassCmd.memoryBarrier( ashes::PipelineStageFlag::eFragmentShader
			, ashes::PipelineStageFlag::eTransfer
			, hiPassSurface.image->getDefaultView().makeTransferSource( ashes::ImageLayout::eUndefined, 0u ) );
		timer.endPass( hiPassCmd, 0u );
		hiPassCmd.end();
		m_commands.emplace_back( std::move( hiPassCommands ) );

		// Copy Hi pass result to other Hi pass surfaces
		castor3d::CommandsSemaphore copyCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & copyCmd = *copyCommands.commandBuffer;
		copyCmd.begin();
		timer.beginPass( copyCmd, 1u );
		for ( uint32_t i = 1u; i < Count; ++i )
		{
			// Put destination image in transfer destination layout
			copyCmd.memoryBarrier( ashes::PipelineStageFlag::eFragmentShader
				, ashes::PipelineStageFlag::eTransfer
				, m_pipelines.hiPass.surfaces[i].image->getDefaultView().makeTransferDestination( ashes::ImageLayout::eUndefined, 0u ) );
			copyCmd.copyImage( hiPassSurface.image->getDefaultView()
				, m_pipelines.hiPass.surfaces[i].image->getDefaultView() );
			// Put back destination image in shader input layout
			copyCmd.memoryBarrier( ashes::PipelineStageFlag::eTransfer
				, ashes::PipelineStageFlag::eFragmentShader
				, m_pipelines.hiPass.surfaces[i].image->getDefaultView().makeShaderInputResource( ashes::ImageLayout::eTransferDstOptimal
					, ashes::AccessFlag::eTransferWrite ) );
		}

		timer.endPass( copyCmd, 1u );
		copyCmd.end();
		m_commands.emplace_back( std::move( copyCommands ) );

		// Kawase blur passes.
		castor3d::CommandsSemaphore kawaseCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & kawaseCmd = *kawaseCommands.commandBuffer;
		kawaseCmd.begin();
		timer.beginPass( kawaseCmd, 2u );
		for ( uint32_t i = 0u; i < Count; ++i )
		{
			auto * source = &m_pipelines.hiPass.surfaces[i];
			auto * destination = &m_pipelines.kawase.surfaces[i];
			auto & kawaseSurface = m_pipelines.kawase.surfaces[i];

			for ( uint32_t j = 0u; j < 3u; ++j )
			{
				// Put source image in shader input layout
				kawaseCmd.memoryBarrier( ashes::PipelineStageFlag::eTransfer
					, ashes::PipelineStageFlag::eFragmentShader
					, source->image->getDefaultView().makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );

				kawaseCmd.beginRenderPass( *m_renderPass
					, *destination->frameBuffer
					, clearValues[i]
					, ashes::SubpassContents::eInline );
				kawaseCmd.setViewport( { destination->image->getWidth(), destination->image->getHeight(), 0, 0, } );
				kawaseCmd.setScissor( { 0, 0, destination->image->getWidth(), destination->image->getHeight() } );
				kawaseCmd.bindPipeline( *m_pipelines.kawase.pipeline );
				kawaseCmd.bindDescriptorSet( *kawaseSurface.descriptorSets[j],
					*m_pipelines.kawase.layout.pipelineLayout );
				kawaseCmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
				kawaseCmd.draw( 6u );
				kawaseCmd.endRenderPass();
				std::swap( destination, source );
			}

			// Put Kawase surface's general view in fragment shader input layout.
			kawaseCmd.memoryBarrier( ashes::PipelineStageFlag::eTransfer
				, ashes::PipelineStageFlag::eFragmentShader
				, kawaseSurface.image->getDefaultView().makeShaderInputResource( ashes::ImageLayout::eUndefined, 0u ) );
		}

		timer.endPass( kawaseCmd, 2u );
		kawaseCmd.end();
		m_commands.emplace_back( std::move( kawaseCommands ) );

		// Combine pass.
		castor3d::CommandsSemaphore combineCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & combineCmd = *combineCommands.commandBuffer;
		combineCmd.begin();
		timer.beginPass( combineCmd, 3u );
		auto & combineSurface = m_pipelines.combine.surfaces.back();
		combineCmd.beginRenderPass( *m_renderPass
			, *combineSurface.frameBuffer
			, clearValues[Count]
			, ashes::SubpassContents::eInline );
		combineCmd.setViewport( { size.width, size.height, 0, 0, } );
		combineCmd.setScissor( { 0, 0, size.width, size.height } );
		combineCmd.bindPipeline( *m_pipelines.combine.pipeline );
		combineCmd.bindDescriptorSet( *combineSurface.descriptorSets.back()
			, *m_pipelines.combine.layout.pipelineLayout );
		combineCmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		combineCmd.draw( 6u );
		combineCmd.endRenderPass();

		timer.endPass( combineCmd, 3u );
		combineCmd.end();
		m_commands.emplace_back( std::move( combineCommands ) );

		return true;
	}
}
