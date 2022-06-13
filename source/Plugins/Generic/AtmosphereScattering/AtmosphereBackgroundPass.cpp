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

		enum Bindings : uint32_t
		{
			eAtmosphereScattering = castor3d::SceneBackground::SkyBoxImgIdx,
			eTransmittance,
			eInscatter,
			eCount,
		};

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;

			C3D_Matrix( writer
				, castor3d::SceneBackground::MtxUboIdx
				, 0u );
			C3D_Scene( writer
				, castor3d::SceneBackground::SceneUboIdx
				, 0u );
			C3D_AtmosphereScattering( writer
				, Bindings::eAtmosphereScattering
				, 0u );

			auto inPosition = writer.declInput< sdw::Vec4 >( "inPosition", 0u );
			auto outViewDir = writer.declOutput< sdw::Vec3 >( "outViewDir", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOutT< sdw::VoidT >{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOutT< sdw::VoidT > out )
				{
					outViewDir = c3d_matrixData.curViewToWorld( vec4( c3d_matrixData.projToView( inPosition ).xyz(), 0.0_f ) ).xyz();
					out.vtx.position = vec4( inPosition.xy(), 0.9999999, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getPixelProgram( castor3d::Engine const & engine )
		{
			sdw::FragmentWriter writer;

			castor3d::shader::Utils utils{ writer, engine };

			C3D_Scene( writer
				, castor3d::SceneBackground::SceneUboIdx
				, 0u );
			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphereScattering )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( Bindings::eTransmittance )
				, 0u );
			auto inscatterMap = writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "inscatterMap"
				, uint32_t( Bindings::eInscatter )
				, 0u );

			auto inViewDir = writer.declInput< sdw::Vec3 >( "inViewDir", 0u );

			// Fragment Outputs
			auto pxl_colour( writer.declOutput< sdw::Vec4 >( "pxl_colour", 0 ) );
			auto const M_PI{ sdw::Float{ castor::Pi< float > } };

			AtmosphereConfig atmosphereConfig{ writer, c3d_atmosphereData };

			writer.implementMainT< SurfaceT, sdw::VoidT >( sdw::FragmentInT< SurfaceT >{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentInT< SurfaceT > in
					, sdw::FragmentOut out )
				{
					auto pi = writer.declLocale( "pi"
						, sdw::Float{ castor::Pi< float > } );

					auto worldSunDir = writer.declLocale( "worldSunDir"
						, c3d_atmosphereData.getSunDir() );

					auto v = writer.declLocale( "v"
						, normalize( inViewDir ) );
					auto sunColor = writer.declLocale( "sunColor"
						, vec3( step( cos( pi / 180.0_f ), dot( v, worldSunDir ) ) ) * vec3( c3d_atmosphereData.sunIntensity ) );
					auto extinction = writer.declLocale< sdw::Vec3 >( "extinction" );
					auto inscatter = writer.declLocale( "inscatter"
						, atmosphereConfig.getSkyRadiance( c3d_sceneData.cameraPosition + c3d_atmosphereData.earthPos
							, v
							, worldSunDir
							, transmittanceMap
							, inscatterMap
							, extinction ) );
					auto finalColor = writer.declLocale( "finalColor"
						, sunColor * extinction + inscatter );
					pxl_colour = vec4( finalColor, 1.0_f );
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
			, sky::getPixelProgram( *m_device.renderSystem.getEngine() ) };
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

		m_descriptorBindings.push_back( { uint32_t( sky::Bindings::eAtmosphereScattering )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, 1u
			, shaderStages
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( sky::Bindings::eAtmosphereScattering )
			, 0u
			, 1u
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER } );
		m_descriptorWrites.back().bufferInfo.push_back( VkDescriptorBufferInfo{ atmosphere.getAtmosphereUbo().getUbo().getBuffer()
				, atmosphere.getAtmosphereUbo().getUbo().getByteOffset()
				, atmosphere.getAtmosphereUbo().getUbo().getByteRange() } );

		m_descriptorBindings.push_back( { uint32_t( sky::Bindings::eTransmittance )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, 1u
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( sky::Bindings::eTransmittance )
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VkDescriptorImageInfo{ m_background->getSampler().getSampler()
				, atmosphere.getTransmittance().texture->getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );

		m_descriptorBindings.push_back( { uint32_t( sky::Bindings::eInscatter )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, 1u
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( sky::Bindings::eInscatter )
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VkDescriptorImageInfo{ m_background->getSampler().getSampler()
				, atmosphere.getInscattering().texture->getDefaultView().getSampledView()
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
