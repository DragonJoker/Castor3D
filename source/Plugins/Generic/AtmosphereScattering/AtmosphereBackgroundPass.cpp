#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderMesh.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace sky
	{
		castor::String const Name{ "RenderSky" };

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;
			sdw::Vec2 position = writer.declInput< sdw::Vec2 >( "position", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOut{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( VkExtent2D const & renderSize
			, VkExtent3D const & transmittanceExtent )
		{
			sdw::FragmentWriter writer;

			C3D_Matrix( writer
				, castor3d::SceneBackground::MtxUboIdx
				, 0u );
			C3D_Scene( writer
				, castor3d::SceneBackground::SceneUboIdx
				, 0u );
			C3D_AtmosphereScattering( writer
				, uint32_t( AtmosphereBackgroundPass::eAtmosphere )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( AtmosphereBackgroundPass::eTransmittance )
				, 0u );
			auto multiScatterMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "multiScatterMap"
				, uint32_t( AtmosphereBackgroundPass::eMultiScatter )
				, 0u );

			auto  sampleCountIni = writer.declConstant( "sampleCountIni"
				, 30.0_f );	// Can go a low as 10 sample but energy lost starts to be visible.
			auto  depthBufferValue = writer.declConstant( "depthBufferValue"
				, -1.0_f );
			auto planetRadiusOffset = writer.declConstant( "planetRadiusOffset"
				, 0.01_f );

			// Fragment Outputs
			auto pxl_colour( writer.declOutput< sdw::Vec4 >( "pxl_colour", 0u ) );
			auto const M_PI{ sdw::Float{ castor::Pi< float > } };

			AtmosphereConfig atmosphereConfig{ writer
				, c3d_atmosphereData
				, c3d_matrixData
				, { false, false, true, true }
				, { transmittanceExtent.width, transmittanceExtent.height }
				, &transmittanceMap };

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					auto targetSize = writer.declLocale( "targetSize"
						, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );
					auto pixPos = writer.declLocale( "pixPos"
						, in.fragCoord.xy() );
					auto uv = writer.declLocale( "uv"
						, pixPos / targetSize );

					auto clipSpace = writer.declLocale( "clipSpace"
						, vec3( uv * vec2( 2.0_f, -2.0_f ) - vec2( 1.0_f, -1.0_f ), 1.0_f ) );
					auto hPos = writer.declLocale( "hPos"
						, c3d_matrixData.curProjToWorld( vec4( clipSpace, 1.0_f ) ) );

					auto worldDir = writer.declLocale( "worldDir"
						, normalize( hPos.xyz() / hPos.w() - c3d_sceneData.cameraPosition ) );
					auto worldPos = writer.declLocale( "worldPos"
						, c3d_sceneData.cameraPosition + vec3( 0.0_f, 0.0_f, c3d_atmosphereData.bottomRadius ) );

					auto viewHeight = writer.declLocale( "viewHeight"
						, length( worldPos ) );

					auto viewZenithCosAngle = writer.declLocale< sdw::Float >( "viewZenithCosAngle" );
					auto lightViewCosAngle = writer.declLocale< sdw::Float >( "lightViewCosAngle" );
					atmosphereConfig.uvToSkyViewLutParams( viewZenithCosAngle, lightViewCosAngle, viewHeight, uv, targetSize );

					auto sunDir = writer.declLocale< sdw::Vec3 >( "sunDir" );
					{
						auto upVector = writer.declLocale( "upVector"
							, worldPos / viewHeight );
						auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
							, dot( upVector, c3d_atmosphereData.sunDirection ) );
						sunDir = normalize( vec3( sqrt( 1.0_f - sunZenithCosAngle * sunZenithCosAngle ), 0.0_f, sunZenithCosAngle ) );
					}

					worldPos = vec3( 0.0_f, 0.0_f, viewHeight );

					auto viewZenithSinAngle = writer.declLocale( "viewZenithSinAngle"
						, sqrt( 1.0_f - viewZenithCosAngle * viewZenithCosAngle ) );
					worldDir = vec3( viewZenithSinAngle * lightViewCosAngle
						, viewZenithSinAngle * sqrt( 1.0_f - lightViewCosAngle * lightViewCosAngle )
						, viewZenithCosAngle );


					// Move to top atmospehre
					IF( writer, !atmosphereConfig.moveToTopAtmosphere( worldPos, worldDir ) )
					{
						// Ray is not intersecting the atmosphere
						pxl_colour = vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f );
					}
					ELSE
					{
						auto ss = writer.declLocale( "ss"
							, atmosphereConfig.integrateScatteredLuminance( pixPos
							, worldPos
							, worldDir
							, sunDir
							, sampleCountIni
							, depthBufferValue ) );
						pxl_colour = vec4( ss.luminance, 1.0_f );
					}
					FI;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereBackgroundPass::AtmosphereBackgroundPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, AtmosphereBackground & background
		, VkExtent2D const & size
		, bool usesDepth )
		: castor3d::BackgroundPassBase{ pass
			, context
			, graph
			, device
			, background
			, size
			, usesDepth }
		, m_atmosBackground{ background }
	{
	}

	void AtmosphereBackgroundPass::doInitialiseVertexBuffer()
	{
		if ( m_vertexBuffer )
		{
			return;
		}

		using castor::Point3f;
		auto data = m_device.graphicsData();
		ashes::StagingBuffer stagingBuffer{ *m_device.device, 0u, sizeof( Vertex ) * 3u };

		// Vertex Buffer
		std::vector< Vertex > vertexData
		{
			// Front
			{ Point3f{ -1.0, -3.0, 0.0 } },
			{ Point3f{ -1.0, +1.0, 0.0 } },
			{ Point3f{ +3.0, +1.0, 0.0 } },
		};
		m_vertexBuffer = makeVertexBuffer< Vertex >( m_device
			, 3u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, "Background" );
		stagingBuffer.uploadVertexData( *data->queue
			, *data->commandPool
			, vertexData
			, *m_vertexBuffer );
	}

	ashes::PipelineShaderStageCreateInfoArray AtmosphereBackgroundPass::doInitialiseShader()
	{
		m_vertexModule = { VK_SHADER_STAGE_VERTEX_BIT
			, sky::Name
			, sky::getVertexProgram() };
		m_pixelModule = { VK_SHADER_STAGE_FRAGMENT_BIT
			, sky::Name
			, sky::getPixelProgram( m_size, m_atmosBackground.getTransmittance().getExtent() ) };
		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( m_device, m_vertexModule ),
			makeShaderState( m_device, m_pixelModule ),
		};
	}

	void AtmosphereBackgroundPass::doFillDescriptorBindings()
	{
		VkShaderStageFlags shaderStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		m_descriptorBindings.clear();
		m_descriptorWrites.clear();

		for ( auto & uniform : m_pass.buffers )
		{
			m_descriptorBindings.push_back( { uniform.binding
				, uniform.getDescriptorType()
				, 1u
				, shaderStages
				, nullptr } );
			m_descriptorWrites.push_back( uniform.getBufferWrite() );
		}

		auto & atmosphere = static_cast< AtmosphereBackground const & >( *m_background );

		m_descriptorBindings.push_back( { uint32_t( AtmosphereBackgroundPass::eAtmosphere )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, 1u
			, shaderStages
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( AtmosphereBackgroundPass::eAtmosphere )
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER } );
		m_descriptorWrites.back().bufferInfo.push_back( VkDescriptorBufferInfo{ atmosphere.getAtmosphereUbo().getUbo().getBuffer()
				, atmosphere.getAtmosphereUbo().getUbo().getByteOffset()
				, atmosphere.getAtmosphereUbo().getUbo().getByteRange() } );

		m_descriptorBindings.push_back( { uint32_t( AtmosphereBackgroundPass::eTransmittance )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, 1u
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( AtmosphereBackgroundPass::eTransmittance )
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VkDescriptorImageInfo{ m_background->getSampler().getSampler()
				, atmosphere.getTransmittance().sampledView
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );

		m_descriptorBindings.push_back( { uint32_t( AtmosphereBackgroundPass::eMultiScatter )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, 1u
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( AtmosphereBackgroundPass::eMultiScatter )
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VkDescriptorImageInfo{ m_background->getSampler().getSampler()
				, atmosphere.getMultiScatter().sampledView
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
	}

	void AtmosphereBackgroundPass::doCreatePipeline()
	{
		m_pipeline = m_device->createPipeline( ashes::GraphicsPipelineCreateInfo{ 0u
			, doInitialiseShader()
			, ashes::PipelineVertexInputStateCreateInfo{ 0u
			, { 1u, { 0u, sizeof( castor::Point3f ), VK_VERTEX_INPUT_RATE_VERTEX } }
			, { 1u, { 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, 0u } } }
			, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, VK_FALSE }
			, std::nullopt
			, doCreateViewportState()
			, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
			, ashes::PipelineMultisampleStateCreateInfo{}
			, ( m_usesDepth
				? std::make_optional( ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_TRUE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL } )
				: std::nullopt )
			, ashes::PipelineColorBlendStateCreateInfo{ 0u, VK_FALSE, VK_LOGIC_OP_COPY, doGetBlendAttachs() }
			, std::nullopt
			, *m_pipelineLayout
			, getRenderPass() } );
	}

	//************************************************************************************************
}
