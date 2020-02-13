#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>

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

			castor3d::shader::Utils utils{ writer };
			utils.declareNegateVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = ( position + 1.0_f ) / 2.0_f;
					out.gl_out.gl_Position = vec4( position.xy(), 0.0_f, 1.0_f );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< sdw::Shader > getHiPassProgram( castor3d::RenderSystem * renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			auto c3d_mapColor = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapColor", 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					pxl_fragColor = vec4( texture( c3d_mapColor, vtx_texture, 0.0_f ).xyz(), 1.0_f );
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
			auto c3d_mapHiPass = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapHiPass", 1u, 0u );
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

					FOR( writer, Int, s, 0, s < c3d_samples, ++s )
					{
						// Weight = a^(b*s)
						auto weight = writer.declLocale( "weight"
							, pow( c3d_attenuation, b * writer.cast< Float >( s ) ) );
						// Streak direction is a 2D vector in image space
						auto sampleCoord = writer.declLocale( "sampleCoord"
							, texcoords + ( c3d_direction * b * vec2( s, s ) * c3d_pixelSize ) );
						// Scale and accumulate
						colour += texture( c3d_mapHiPass, sampleCoord ).rgb() * clamp( weight, 0.0_f, 1.0_f );
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
			auto index = castor3d::getMinBufferIndex();
			auto c3d_mapScene = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapScene, 0u, 0u );
			auto c3d_mapKawase1 = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapKawase1, 1u, 0u );
			auto c3d_mapKawase2 = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapKawase2, 2u, 0u );
			auto c3d_mapKawase3 = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapKawase3, 3u, 0u );
			auto c3d_mapKawase4 = writer.declSampledImage< FImg2DRgba32 >( PostEffect::CombineMapKawase4, 4u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
			{
				pxl_fragColor = texture( c3d_mapScene, vtx_texture );
				pxl_fragColor += texture( c3d_mapKawase1, vtx_texture );
				pxl_fragColor += texture( c3d_mapKawase2, vtx_texture );
				pxl_fragColor += texture( c3d_mapKawase3, vtx_texture );
				pxl_fragColor += texture( c3d_mapKawase4, vtx_texture );
			} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	PostEffect::Surface::Surface( castor3d::RenderSystem & renderSystem
		, VkFormat format
		, VkExtent2D const & size
		, ashes::RenderPass const & renderPass
		, castor::String debugName )
	{
		ashes::ImageCreateInfo imageCreateInfo
		{
			0u,
			VK_IMAGE_TYPE_2D,
			format,
			{ size.width, size.height, 1u },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ),
		};
		image = std::make_shared< castor3d::TextureLayout >( renderSystem
			, imageCreateInfo
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, std::move( debugName ) );
		image->initialise();

		ashes::ImageViewCRefArray attachments{ image->getDefaultView() };
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
				{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksHiPass" },
				{ VK_SHADER_STAGE_FRAGMENT_BIT, "LightStreaksHiPass" },
			},
			{
				{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksKawase" },
				{ VK_SHADER_STAGE_FRAGMENT_BIT, "LightStreaksKawase" },
			},
			{
				{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksCombine" },
				{ VK_SHADER_STAGE_FRAGMENT_BIT, "LightStreaksCombine" },
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
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_pipelines.hiPass.vertexShader.shader );
		visitor.visit( cuT( "HiPass" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pipelines.hiPass.pixelShader.shader );

		visitor.visit( cuT( "Kawase" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_pipelines.kawase.vertexShader.shader );
		visitor.visit( cuT( "Kawase" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pipelines.kawase.pixelShader.shader );
		visitor.visit( cuT( "Kawase" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Kawase" )
			, cuT( "Attenuation" )
			, m_kawaseUbo.getUbo().getData().attenuation );
		visitor.visit( cuT( "Kawase" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Kawase" )
			, cuT( "Samples" )
			, m_kawaseUbo.getUbo().getData().samples );

		visitor.visit( cuT( "Combine" )
			, VK_SHADER_STAGE_VERTEX_BIT
			, *m_pipelines.combine.vertexShader.shader );
		visitor.visit( cuT( "Combine" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, *m_pipelines.combine.pixelShader.shader );
	}

	bool PostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_kawaseUbo.initialise();
		m_linearSampler->initialise();
		m_nearestSampler->initialise();
		auto & device = getCurrentRenderDevice( *this );
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Create vertex buffer
		m_vertexBuffer = castor3d::makeVertexBuffer< castor3d::NonTexturedQuad >( device
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "LightStreaks" );

		if ( auto data = m_vertexBuffer->lock( 0u, 1u, 0u ) )
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
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings
		{
			castor3d::makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};

		for ( uint32_t i = 0u; i < Count; ++i )
		{
			setLayoutBindings.emplace_back( castor3d::makeDescriptorSetLayoutBinding( i + 1u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		m_pipelines.combine.layout.descriptorLayout = device->createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.combine.layout.pipelineLayout = device->createPipelineLayout( *m_pipelines.combine.layout.descriptorLayout );
		m_pipelines.combine.layout.descriptorPool = m_pipelines.combine.layout.descriptorLayout->createPool( 1u );

		setLayoutBindings =
		{
			castor3d::makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_pipelines.hiPass.layout.descriptorLayout = device->createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.hiPass.layout.pipelineLayout = device->createPipelineLayout( *m_pipelines.hiPass.layout.descriptorLayout );
		m_pipelines.hiPass.layout.descriptorPool = m_pipelines.hiPass.layout.descriptorLayout->createPool( Count );

		setLayoutBindings =
		{
			castor3d::makeDescriptorSetLayoutBinding( 0u
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			castor3d::makeDescriptorSetLayoutBinding( 1u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_pipelines.kawase.layout.descriptorLayout = device->createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.kawase.layout.pipelineLayout = device->createPipelineLayout( *m_pipelines.kawase.layout.descriptorLayout );
		m_pipelines.kawase.layout.descriptorPool = m_pipelines.kawase.layout.descriptorLayout->createPool( Count * 3u );

		// Create the render pass.
		ashes::VkAttachmentDescriptionArray attachments
		{
			{
				0u,
				m_target->getPixelFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			}
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				ashes::nullopt,
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attachments ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		m_renderPass = device->createRenderPass( std::move( createInfo ) );
		setDebugObjectName( device, *m_renderPass, "LightStreaks" );
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
				, *m_renderPass
				, cuT( "LightStreaksHiPassSurface" ) );
			m_pipelines.kawase.surfaces.emplace_back( *getRenderSystem()
				, m_target->getPixelFormat()
				, size
				, *m_renderPass
				, cuT( "LightStreaksKawaseSurface" ) );

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
			{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksCombine" },
			{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksCombine" },
		};
		Pipeline dummy2
		{
			{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksKawase" },
			{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksKawase" },
		};
		Pipeline dummy3
		{
			{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksHiPass" },
			{ VK_SHADER_STAGE_VERTEX_BIT, "LightStreaksHiPass" },
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
		VkFilter mode;

		if ( p_linear )
		{
			name += cuT( "Linear" );
			mode = VK_FILTER_LINEAR;
		}
		else
		{
			name += cuT( "Nearest" );
			mode = VK_FILTER_NEAREST;
		}

		castor3d::SamplerSPtr sampler;

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( mode );
			sampler->setMagFilter( mode );
			sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
		}
		else
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		return sampler;
	}

	bool PostEffect::doInitialiseHiPassProgram()
	{
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentRenderDevice( *this );
		m_pipelines.hiPass.vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pipelines.hiPass.pixelShader.shader = getHiPassProgram( getRenderSystem() );

		// Create pipeline
		ashes::PipelineVertexInputStateCreateInfo inputState
		{
			0u,
			ashes::VkVertexInputBindingDescriptionArray
			{
				{ 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
			},
			ashes::VkVertexInputAttributeDescriptionArray
			{
				{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( castor3d::NonTexturedQuad::Vertex, position ) },
			}
		};

		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_pipelines.hiPass.vertexShader ) );
		stages.push_back( makeShaderState( device, m_pipelines.hiPass.pixelShader ) );

		m_pipelines.hiPass.pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				0u,
				stages,
				inputState,
				ashes::PipelineInputAssemblyStateCreateInfo{},
				ashes::nullopt,
				ashes::PipelineViewportStateCreateInfo{},
				ashes::PipelineRasterizationStateCreateInfo{},
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE },
				ashes::PipelineColorBlendStateCreateInfo{},
				ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
				*m_pipelines.hiPass.layout.pipelineLayout,
				*m_renderPass,
			} );

		// Create descriptor sets
		for ( auto & surface : m_pipelines.hiPass.surfaces )
		{
			surface.descriptorSets.emplace_back( m_pipelines.hiPass.layout.descriptorPool->createDescriptorSet( 0u ) );
			auto & descriptorSet = *surface.descriptorSets.back();
			descriptorSet.createBinding( m_pipelines.hiPass.layout.descriptorLayout->getBinding( 0u )
				, m_target->getDefaultView()
				, m_linearSampler->getSampler()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			descriptorSet.update();
		}

		return true;
	}

	bool PostEffect::doInitialiseKawaseProgram()
	{
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentRenderDevice( *this );
		m_pipelines.kawase.vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pipelines.kawase.pixelShader.shader = getKawaseProgram( getRenderSystem() );

		// Create pipeline
		ashes::PipelineVertexInputStateCreateInfo inputState
		{
			0u,
			ashes::VkVertexInputBindingDescriptionArray
			{
				{ 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
			},
			ashes::VkVertexInputAttributeDescriptionArray
			{
				{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( castor3d::NonTexturedQuad::Vertex, position ) },
			}
		};

		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_pipelines.kawase.vertexShader ) );
		stages.push_back( makeShaderState( device, m_pipelines.kawase.pixelShader ) );

		m_pipelines.kawase.pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				0u,
				stages,
				inputState,
				ashes::PipelineInputAssemblyStateCreateInfo{},
				ashes::nullopt,
				ashes::PipelineViewportStateCreateInfo{},
				ashes::PipelineRasterizationStateCreateInfo{},
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE },
				ashes::PipelineColorBlendStateCreateInfo{},
				ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
				*m_pipelines.kawase.layout.pipelineLayout,
				*m_renderPass,
			} );
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
				descriptorSet.createSizedBinding( m_pipelines.kawase.layout.descriptorLayout->getBinding( 0u )
					, m_kawaseUbo.getUbo()
					, index );
				descriptorSet.createBinding( m_pipelines.kawase.layout.descriptorLayout->getBinding( 1u )
					, source->image->getDefaultView()
					, m_linearSampler->getSampler()
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
				descriptorSet.update();
				std::swap( source, destination );
				++index;
			}
		}

		return true;
	}

	bool PostEffect::doInitialiseCombineProgram()
	{
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & renderSystem = *getRenderSystem();
		auto & device = getCurrentRenderDevice( *this );
		m_pipelines.combine.vertexShader.shader = getVertexProgram( getRenderSystem() );
		m_pipelines.combine.pixelShader.shader = getCombineProgram( getRenderSystem() );

		// Create pipeline
		ashes::PipelineVertexInputStateCreateInfo inputState
		{
			0u,
			ashes::VkVertexInputBindingDescriptionArray
			{
				{ 0u, sizeof( castor3d::NonTexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
			},
			ashes::VkVertexInputAttributeDescriptionArray
			{
				{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( castor3d::NonTexturedQuad::Vertex, position ) },
			}
		};

		ashes::PipelineShaderStageCreateInfoArray stages;
		stages.push_back( makeShaderState( device, m_pipelines.combine.vertexShader ) );
		stages.push_back( makeShaderState( device, m_pipelines.combine.pixelShader ) );

		m_pipelines.combine.pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				0u,
				stages,
				inputState,
				ashes::PipelineInputAssemblyStateCreateInfo{},
				ashes::nullopt,
				ashes::PipelineViewportStateCreateInfo{},
				ashes::PipelineRasterizationStateCreateInfo{},
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE },
				ashes::PipelineColorBlendStateCreateInfo{},
				ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
				*m_pipelines.combine.layout.pipelineLayout,
				*m_renderPass,
			} );

		// Create view and associated frame buffer.
		m_pipelines.combine.surfaces.emplace_back( *getRenderSystem()
			, m_target->getPixelFormat()
			, size
			, *m_renderPass
			, cuT( "LightStreaksCombineSurface" ) );

		// Create descriptor
		auto & surface = m_pipelines.combine.surfaces.back();
		surface.descriptorSets.emplace_back( m_pipelines.combine.layout.descriptorPool->createDescriptorSet( 0u ) );
		auto & descriptorSet = *surface.descriptorSets.back();
		descriptorSet.createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( 0u )
			, m_target->getDefaultView()
			, m_linearSampler->getSampler()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

		for ( uint32_t i = 0u; i < Count; ++i )
		{
			descriptorSet.createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( i + 1u )
				, m_pipelines.kawase.surfaces[i].image->getDefaultView()
				, m_linearSampler->getSampler()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		descriptorSet.update();

		return true;
	}

	bool PostEffect::doBuildCommandBuffer( castor3d::RenderPassTimer const & timer )
	{
		auto & device = getCurrentRenderDevice( *this );
		VkExtent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Fill command buffer.
		static ashes::VkClearValueArray const clearValues[Count + 1]
		{
			{ 1u, castor3d::opaqueBlackClearColor },
			{ 1u, castor3d::opaqueBlackClearColor },
			{ 1u, castor3d::opaqueBlackClearColor },
			{ 1u, castor3d::opaqueBlackClearColor },
			{ 1u, castor3d::opaqueBlackClearColor },
		};
		auto & hiPassSurface = m_pipelines.hiPass.surfaces[0];
		castor3d::CommandsSemaphore hiPassCommands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & hiPassCmd = *hiPassCommands.commandBuffer;
		hiPassCmd.begin();
		timer.beginPass( hiPassCmd, 0u );
		// Put target image in fragment shader input layout.
		hiPassCmd.memoryBarrier( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_target->getDefaultView().makeShaderInputResource( VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) );

		// Hi-pass.
		hiPassCmd.beginRenderPass( *m_renderPass
			, *hiPassSurface.frameBuffer
			, clearValues[0]
			, VK_SUBPASS_CONTENTS_INLINE );
		hiPassCmd.setViewport( ashes::makeViewport( hiPassSurface.image->getDimensions() ) );
		hiPassCmd.setScissor( ashes::makeScissor( hiPassSurface.image->getDimensions() ) );
		hiPassCmd.bindPipeline( *m_pipelines.hiPass.pipeline );
		hiPassCmd.bindDescriptorSet( *hiPassSurface.descriptorSets.back(),
			*m_pipelines.hiPass.layout.pipelineLayout );
		hiPassCmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		hiPassCmd.draw( 6u );
		hiPassCmd.endRenderPass();
		// Put source image in transfer source layout
		hiPassCmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, hiPassSurface.image->getDefaultView().makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		timer.endPass( hiPassCmd, 0u );
		hiPassCmd.end();
		m_commands.emplace_back( std::move( hiPassCommands ) );

		// Copy Hi pass result to other Hi pass surfaces
		castor3d::CommandsSemaphore copyCommands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & copyCmd = *copyCommands.commandBuffer;
		copyCmd.begin();
		timer.beginPass( copyCmd, 1u );
		for ( uint32_t i = 1u; i < Count; ++i )
		{
			// Put destination image in transfer destination layout
			copyCmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, m_pipelines.hiPass.surfaces[i].image->getDefaultView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			copyCmd.copyImage( hiPassSurface.image->getDefaultView()
				, m_pipelines.hiPass.surfaces[i].image->getDefaultView() );
			// Put back destination image in shader input layout
			copyCmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_pipelines.hiPass.surfaces[i].image->getDefaultView().makeShaderInputResource( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		}

		timer.endPass( copyCmd, 1u );
		copyCmd.end();
		m_commands.emplace_back( std::move( copyCommands ) );

		// Kawase blur passes.
		castor3d::CommandsSemaphore kawaseCommands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
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
				kawaseCmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, source->image->getDefaultView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );

				kawaseCmd.beginRenderPass( *m_renderPass
					, *destination->frameBuffer
					, clearValues[i]
					, VK_SUBPASS_CONTENTS_INLINE );
				kawaseCmd.setViewport( ashes::makeViewport( destination->image->getDimensions() ) );
				kawaseCmd.setScissor( ashes::makeScissor( destination->image->getDimensions() ) );
				kawaseCmd.bindPipeline( *m_pipelines.kawase.pipeline );
				kawaseCmd.bindDescriptorSet( *kawaseSurface.descriptorSets[j],
					*m_pipelines.kawase.layout.pipelineLayout );
				kawaseCmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
				kawaseCmd.draw( 6u );
				kawaseCmd.endRenderPass();
				std::swap( destination, source );
			}

			// Put Kawase surface's general view in fragment shader input layout.
			kawaseCmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, kawaseSurface.image->getDefaultView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		}

		timer.endPass( kawaseCmd, 2u );
		kawaseCmd.end();
		m_commands.emplace_back( std::move( kawaseCommands ) );

		// Combine pass.
		castor3d::CommandsSemaphore combineCommands
		{
			device.graphicsCommandPool->createCommandBuffer(),
			device->createSemaphore()
		};
		auto & combineCmd = *combineCommands.commandBuffer;
		combineCmd.begin();
		timer.beginPass( combineCmd, 3u );
		auto & combineSurface = m_pipelines.combine.surfaces.back();
		combineCmd.beginRenderPass( *m_renderPass
			, *combineSurface.frameBuffer
			, clearValues[Count]
			, VK_SUBPASS_CONTENTS_INLINE );
		combineCmd.setViewport( ashes::makeViewport( size ) );
		combineCmd.setScissor( ashes::makeScissor( size ) );
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
