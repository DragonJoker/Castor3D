#include "EnvironmentPrefilter.hpp"

#include "Engine.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Miscellaneous/PushConstantRange.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Pipeline/Scissor.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <Pipeline/Viewport.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <Shader/ShaderProgram.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureUnit doCreatePrefilteredTexture( Engine & engine
			, Size const & size )
		{
			renderer::ImageCreateInfo image{};
			image.flags = renderer::ImageCreateFlag::eCubeCompatible;
			image.arrayLayers = 6u;
			image.extent.width = size[0];
			image.extent.height = size[1];
			image.extent.depth = 1u;
			image.format = renderer::Format::eR32G32B32_SFLOAT;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			auto texture = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			SamplerSPtr sampler;
			auto name = cuT( "IblTexturesPrefiltered" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().create( name );
				sampler->setMinFilter( renderer::Filter::eLinear );
				sampler->setMagFilter( renderer::Filter::eLinear );
				sampler->setMipFilter( renderer::MipmapMode::eLinear );
				sampler->setWrapS( renderer::WrapMode::eClampToEdge );
				sampler->setWrapT( renderer::WrapMode::eClampToEdge );
				sampler->setWrapR( renderer::WrapMode::eClampToEdge );
				engine.getSamplerCache().add( name, sampler );
			}

			TextureUnit result{ engine };
			result.setTexture( texture );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}
	}

	EnvironmentPrefilter::EnvironmentPrefilter( Engine & engine
		, castor::Size const & size
		, renderer::Texture const & srcTexture )
		: m_renderSystem{ *engine.getRenderSystem() }
		, m_result{ doCreatePrefilteredTexture( engine, size ) }
		, m_vertexData
		{
			{
				Point3f{ -1, +1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, -1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, +1, -1 }, Point3f{ +1, +1, -1 },
				Point3f{ -1, -1, +1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, -1, -1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, -1, +1 }, Point3f{ -1, +1, +1 },
				Point3f{ +1, -1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, -1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, -1, -1 }, Point3f{ +1, +1, -1 },
				Point3f{ -1, -1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, +1, +1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, -1, +1 }, Point3f{ +1, -1, +1 },
				Point3f{ -1, +1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ +1, +1, -1 }, Point3f{ +1, +1, +1 }, Point3f{ -1, +1, -1 }, Point3f{ -1, +1, +1 },
				Point3f{ -1, -1, -1 }, Point3f{ +1, -1, -1 }, Point3f{ -1, -1, +1 }, Point3f{ +1, -1, -1 }, Point3f{ +1, -1, +1 }, Point3f{ -1, -1, +1 },
			}
		}
	{
		auto & dstTexture = m_result.getTexture()->getTexture();

		if ( !engine.getSamplerCache().has( cuT( "EnvironmentPrefilter" ) ) )
		{
			m_sampler = engine.getSamplerCache().add( cuT( "EnvironmentPrefilter" ) );
			m_sampler->setMinFilter( renderer::Filter::eLinear );
			m_sampler->setMagFilter( renderer::Filter::eLinear );
			m_sampler->setMipFilter( renderer::MipmapMode::eLinear );
			m_sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			m_sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			m_sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			m_sampler->initialise();
		}
		else
		{
			m_sampler = engine.getSamplerCache().find( cuT( "EnvironmentPrefilter" ) );
		}
		
		auto & device = *m_renderSystem.getCurrentDevice();
		// Initialise the vertex buffer.
		m_vertexBuffer = renderer::makeVertexBuffer< NonTexturedCube >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		// Initialise the vertex layout.
		m_vertexLayout = renderer::makeLayout< NonTexturedCube >( 0u );
		m_vertexLayout->createAttribute( 0u, renderer::Format::eR32G32B32_SFLOAT, offsetof( NonTexturedCube::Quad::Vertex, position ) );

		// Initialise the UBO.
		m_configUbo = renderer::makeUniformBuffer< castor::Matrix4x4f >( device
			, 6u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
		m_configUbo->getData( 0u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 1u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 2u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } );
		m_configUbo->getData( 3u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } );
		m_configUbo->getData( 4u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->getData( 5u ) = matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } );
		m_configUbo->upload();

		// Prepare descriptor and pipeline layouts.
		renderer::DescriptorSetLayoutBindingArray bindings;
		bindings.emplace_back( 0u
			, renderer::DescriptorType::eCombinedImageSampler
			, renderer::ShaderStageFlag::eFragment );
		bindings.emplace_back( 1u
			, renderer::DescriptorType::eUniformBuffer
			, renderer::ShaderStageFlag::eVertex );
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_descriptorPool = m_descriptorLayout->createPool( 6u * ( glsl::Utils::MaxIblReflectionLod + 1 ) );
		renderer::PushConstantRange range{ renderer::ShaderStageFlag::eFragment, 0u, uint32_t( sizeof( float ) ) };
		m_pipelineLayout = device.createPipelineLayout( { *m_descriptorLayout }, { range } );

		// Create the render passes.
		renderer::RenderPassCreateInfo createInfo{};
		createInfo.flags = 0u;

		createInfo.attachments.resize( 1u );
		createInfo.attachments[0].index = 0u;
		createInfo.attachments[0].format = dstTexture.getFormat();
		createInfo.attachments[0].samples = renderer::SampleCountFlag::e1;
		createInfo.attachments[0].loadOp = renderer::AttachmentLoadOp::eDontCare;
		createInfo.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		createInfo.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		createInfo.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		createInfo.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		createInfo.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderer::AttachmentReference colourReference;
		colourReference.attachment = 0u;
		colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

		createInfo.subpasses.resize( 1u );
		createInfo.subpasses[0].flags = 0u;
		createInfo.subpasses[0].colorAttachments = { colourReference };

		createInfo.dependencies.resize( 2u );
		createInfo.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		createInfo.dependencies[0].dstSubpass = 0u;
		createInfo.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		createInfo.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
		createInfo.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		createInfo.dependencies[1].srcSubpass = 0u;
		createInfo.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		createInfo.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		createInfo.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
		createInfo.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		createInfo.dependencies[1].dstAccessMask = renderer::AccessFlag::eMemoryRead;
		createInfo.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		auto program = doCreateProgram( size );

		for ( auto mipLevel = 0u; mipLevel < glsl::Utils::MaxIblReflectionLod + 1u; ++mipLevel )
		{
			m_renderPasses.push_back( CubePasses{} );
			auto & mipPasses = m_renderPasses[mipLevel];

			for ( auto face = 0u; face < 6u; ++face )
			{
				auto & facePass = mipPasses[face];
				facePass.pushConstants = std::make_unique< renderer::PushConstantsBuffer< float > >( renderer::ShaderStageFlag::eFragment
					, renderer::PushConstantArray{ { 0u, 0u, renderer::ConstantFormat::eFloat } } );

				// Create the views.
				if ( mipLevel == 0u )
				{
					facePass.srcView = srcTexture.createView( renderer::TextureViewType::e2D
						, srcTexture.getFormat()
						, mipLevel
						, 1u
						, face
						, 1u );
				}
				else
				{
					// After first mip level, the source view is the previous mip level of destination.
					facePass.srcView = dstTexture.createView( renderer::TextureViewType::e2D
						, dstTexture.getFormat()
						, mipLevel - 1
						, 1u
						, face
						, 1u );
				}

				facePass.dstView = dstTexture.createView( renderer::TextureViewType::e2D
					, dstTexture.getFormat()
					, mipLevel
					, 1u
					, face
					, 1u );

				// Initialise the descriptor set.
				facePass.descriptorSet = m_descriptorPool->createDescriptorSet();
				facePass.descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
					, *facePass.srcView
					, m_sampler->getSampler() );
				facePass.descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
					, *m_configUbo
					, face
					, 1u );
				facePass.descriptorSet->update();

				// Create the render pass.
				facePass.renderPass = device.createRenderPass( createInfo );

				// Initialise the frame buffer.
				renderer::FrameBufferAttachmentArray attaches;
				attaches.emplace_back( *( facePass.renderPass->getAttachments().begin() ), *facePass.dstView );
				facePass.frameBuffer = facePass.renderPass->createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
					, std::move( attaches ) );

				// Initialise the pipeline.
				facePass.pipeline = m_pipelineLayout->createPipeline( {
					program,
					*facePass.renderPass,
					renderer::VertexInputState::create( *m_vertexLayout ),
					renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
					renderer::RasterisationState{},
					renderer::MultisampleState{},
					renderer::ColourBlendState::createDefault(),
					{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
					renderer::DepthStencilState{ 0u, false, false },
				} );
			}
		}

		m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			for ( unsigned int mipLevel = 0; mipLevel < glsl::Utils::MaxIblReflectionLod + 1; ++mipLevel )
			{
				auto & mipPasses = m_renderPasses[mipLevel];
				Size mipSize{ uint32_t( size.getWidth() * std::pow( 0.5, mipLevel ) )
					, uint32_t( size.getHeight() * std::pow( 0.5, mipLevel ) ) };
				auto roughness = mipLevel / float( glsl::Utils::MaxIblReflectionLod );

				for ( auto face = 0u; face < 6u; ++face )
				{
					auto & facePass = mipPasses[face];
					*facePass.pushConstants->getData() = roughness;
					m_commandBuffer->beginRenderPass( *facePass.renderPass
						, *facePass.frameBuffer
						, { renderer::ClearColorValue{ 0, 0, 0, 0 } }
						, renderer::SubpassContents::eInline );
					m_commandBuffer->bindPipeline( *facePass.pipeline );
					m_commandBuffer->setViewport( { mipSize[0], mipSize[0], 0, 0 } );
					m_commandBuffer->setScissor( { 0, 0, mipSize[0], mipSize[0] } );
					m_commandBuffer->pushConstants( *m_pipelineLayout , *facePass.pushConstants );
					m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
					m_commandBuffer->draw( 36u, 1u, 0u, 0u );
					m_commandBuffer->endRenderPass();
				}
			}

			m_commandBuffer->end();
		}
	}

	void EnvironmentPrefilter::render()
	{
		auto & device = *m_renderSystem.getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer, nullptr );
	}

	renderer::ShaderStageStateArray EnvironmentPrefilter::doCreateProgram( Size const & size )
	{
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, MatrixUbo::BindingPoint, 0 );

			// Outputs
			auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				vtx_worldPosition = position;
				auto view = writer.declLocale( cuT( "normal" )
					, mat4( mat3( c3d_curView ) ) );
				gl_Position = writer.paren( c3d_projection * view * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ m_renderSystem.createGlslWriter() };

			// Inputs
			auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" ), 0u );
			auto c3d_mapDiffuse = writer.declSampler< SamplerCube >( cuT( "c3d_mapDiffuse" ), MinBufferIndex, 0u );
			Ubo config{ writer, cuT( "Config" ), 0u };
			auto c3d_roughness = config.declMember< Float >( cuT( "c3d_roughness" ) );
			config.end();

			// Outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

			auto distributionGGX = writer.implementFunction< Float >( cuT( "DistributionGGX" )
				, [&]( Vec3 const & N
					, Vec3 const & H
					, Float const & roughness )
				{
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.declLocale( cuT( "a" )
						, roughness * roughness );
					auto a2 = writer.declLocale( cuT( "a2" )
						, a * a );
					auto NdotH = writer.declLocale( cuT( "NdotH" )
						, max( dot( N, H ), 0.0 ) );
					auto NdotH2 = writer.declLocale( cuT( "NdotH2" )
						, NdotH * NdotH );

					auto nom = writer.declLocale( cuT( "nom" )
						, a2 );
					auto denom = writer.declLocale( cuT( "denom" )
						, ( NdotH2 * writer.paren( a2 - 1.0 ) + 1.0 ) );
					denom = denom * denom * PI;

					writer.returnStmt( nom / denom );
				}
				, InVec3{ &writer, cuT( "N" ) }
				, InVec3{ &writer, cuT( "H" ) }
				, InFloat{ &writer, cuT( "roughness" ) } );

			auto radicalInverse = writer.implementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & inBits )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto bits = writer.declLocale( cuT( "bits" )
						, inBits );
					bits = writer.paren( bits << 16u ) | writer.paren( bits >> 16u );
					bits = writer.paren( writer.paren( bits & 0x55555555_ui ) << 1u ) | writer.paren( writer.paren( bits & 0xAAAAAAAA_ui ) >> 1u );
					bits = writer.paren( writer.paren( bits & 0x33333333_ui ) << 2u ) | writer.paren( writer.paren( bits & 0xCCCCCCCC_ui ) >> 2u );
					bits = writer.paren( writer.paren( bits & 0x0F0F0F0F_ui ) << 4u ) | writer.paren( writer.paren( bits & 0xF0F0F0F0_ui ) >> 4u );
					bits = writer.paren( writer.paren( bits & 0x00FF00FF_ui ) << 8u ) | writer.paren( writer.paren( bits & 0xFF00FF00_ui ) >> 8u );
					writer.returnStmt( writer.cast< Float >( bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ &writer, cuT( "inBits" ) } );

			auto hammersley = writer.implementFunction< Vec2 >( cuT( "Hammersley" )
				, [&]( UInt const & i
					, UInt const & n )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					writer.returnStmt( vec2( writer.cast< Float >( i ) / writer.cast< Float >( n ), radicalInverse( i ) ) );
				}
				, InUInt{ &writer, cuT( "i" ) }
				, InUInt{ &writer, cuT( "n" ) } );

			auto importanceSample = writer.implementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
				, [&]( Vec2 const & xi
					, Vec3 const & n
					, Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.declLocale( cuT( "a" )
						, roughness * roughness );

					auto phi = writer.declLocale( cuT( "phi" )
						, 2.0_f * PI * xi.x() );
					auto cosTheta = writer.declLocale( cuT( "cosTheta" )
						, sqrt( writer.paren( 1.0 - xi.y() ) / writer.paren( 1.0 + writer.paren( a * a - 1.0 ) * xi.y() ) ) );
					auto sinTheta = writer.declLocale( cuT( "sinTheta" )
						, sqrt( 1.0 - cosTheta * cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto H = writer.declLocale< Vec3 >( cuT( "H" ) );
					H.x() = cos( phi ) * sinTheta;
					H.y() = sin( phi ) * sinTheta;
					H.z() = cosTheta;

					// from tangent-space vector to world-space sample vector
					auto up = writer.declLocale( cuT( "up" )
						, writer.ternary( glsl::abs( n.z() ) < 0.999, vec3( 0.0_f, 0.0, 1.0 ), vec3( 1.0_f, 0.0, 0.0 ) ) );
					auto tangent = writer.declLocale( cuT( "tangent" )
						, normalize( cross( up, n ) ) );
					auto bitangent = writer.declLocale( cuT( "bitangent" )
						, cross( n, tangent ) );

					auto sampleVec = writer.declLocale( cuT( "sampleVec" )
						, tangent * H.x() + bitangent * H.y() + n * H.z() );
					writer.returnStmt( normalize( sampleVec ) );
				}
				, InVec2{ &writer, cuT( "xi" ) }
				, InVec3{ &writer, cuT( "n" ) }
				, InFloat{ &writer, cuT( "roughness" ) } );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto constexpr PI = 3.1415926535897932384626433832795028841968;
				// From https://learnopengl.com/#!PBR/Lighting
				auto N = writer.declLocale( cuT( "N" )
					, normalize( vtx_worldPosition ) );
				auto R = writer.declLocale( cuT( "R" )
					, N );
				auto V = writer.declLocale( cuT( "V" )
					, R );

				auto sampleCount = writer.declLocale( cuT( "sampleCount" )
					, 1024_ui );
				auto totalWeight = writer.declLocale( cuT( "totalWeight" )
					, 0.0_f );
				auto prefilteredColor = writer.declLocale( cuT( "prefilteredColor" )
					, vec3( 0.0_f ) );

				FOR( writer, UInt, i, 0u, "i < sampleCount", "++i" )
				{
					auto xi = writer.declLocale( cuT( "xi" )
						, hammersley( i, sampleCount ) );
					auto H = writer.declLocale( cuT( "H" )
						, importanceSample( xi, N, c3d_roughness ) );
					auto L = writer.declLocale( cuT( "L" )
						, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

					auto NdotL = writer.declLocale( cuT( "NdotL" )
						, max( dot( N, L ), 0.0 ) );

					IF( writer, "NdotL > 0.0" )
					{
						auto D = writer.declLocale( cuT( "D" )
							, distributionGGX( N, H, c3d_roughness ) );
						auto NdotH = writer.declLocale( cuT( "NdotH" )
							, max( dot( N, H ), 0.0 ) );
						auto HdotV = writer.declLocale( cuT( "HdotV" )
							, max( dot( H, V ), 0.0 ) );
						auto pdf = writer.declLocale( cuT( "pdf" )
							, D * NdotH / writer.paren( 4.0_f * HdotV ) + 0.0001 );

						auto resolution = writer.declLocale( cuT( "resolution" )
							, Float( int( size.getWidth() ) ) ); // resolution of source cubemap (per face)
						auto saTexel = writer.declLocale( cuT( "saTexel" )
							, 4.0_f * PI / writer.paren( 6.0 * resolution * resolution ) );
						auto saSample = writer.declLocale( cuT( "saSample" )
							, 1.0_f / writer.paren( writer.cast< Float >( sampleCount ) * pdf + 0.0001 ) );
						auto mipLevel = writer.declLocale( cuT( "mipLevel" )
							, writer.ternary( c3d_roughness == 0.0_f, 0.0_f, 0.5_f * log2( saSample / saTexel ) ) );

						prefilteredColor += texture( c3d_mapDiffuse, L, mipLevel ).rgb() * NdotL;
						totalWeight += NdotL;
					}
					FI;
				}
				ROF;

				prefilteredColor = prefilteredColor / totalWeight;
				pxl_fragColor = vec4( prefilteredColor, 1.0 );
			} );

			pxl = writer.finalise();
		}

		renderer::ShaderStageStateArray program
		{
			{ m_renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eVertex ) },
			{ m_renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( vtx.getSource() );
		program[1].module->loadShader( pxl.getSource() );
		return program;
	}
}
