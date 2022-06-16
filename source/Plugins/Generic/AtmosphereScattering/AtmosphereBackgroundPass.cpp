#include "AtmosphereScattering/AtmosphereBackgroundPass.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace atmos
	{
		castor::String const Name{ "Atmosphere" };

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
			, VkExtent3D const & transmittanceExtent
			, bool colorTransmittance = false
			, bool fastSky = true
			, bool fastAerialPerspective = true
			, bool renderSunDisk = true )
		{
			sdw::FragmentWriter writer;

			C3D_Matrix( writer
				, castor3d::SceneBackground::MtxUboIdx
				, 0u );
			C3D_Scene( writer
				, castor3d::SceneBackground::SceneUboIdx
				, 0u );
			C3D_AtmosphereScattering( writer
				, AtmosphereBackgroundPass::eAtmosphere
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( AtmosphereBackgroundPass::eTransmittance )
				, 0u );
			auto multiScatterMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "multiScatterMap"
				, uint32_t( AtmosphereBackgroundPass::eMultiScatter )
				, 0u );
			auto skyViewMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "skyViewMap"
				, uint32_t( AtmosphereBackgroundPass::eSkyView )
				, 0u );
			auto volumeMap = writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "volumeMap"
				, uint32_t( AtmosphereBackgroundPass::eVolume )
				, 0u );
			auto depthMap = writer.declCombinedImg< sdw::CombinedImage2DR32 >( "depthMap"
				, uint32_t( AtmosphereBackgroundPass::eDepth )
				, 0u );

			auto sampleCountIni = writer.declConstant( "sampleCountIni"
				, 0.0_f );	// Can go a low as 10 sample but energy lost starts to be visible.
			auto planetRadiusOffset = writer.declConstant( "planetRadiusOffset"
				, 0.01_f );
			auto apSliceCount = writer.declConstant( "apSliceCount"
				, 32.0_f );

			// Fragment Outputs
			auto pxl_luminance( writer.declOutput< sdw::Vec4 >( "pxl_luminance", 0u ) );
			auto pxl_transmittance( writer.declOutput< sdw::Vec4 >( "pxl_transmittance", 1u, colorTransmittance ) );
			auto const M_PI{ sdw::Float{ castor::Pi< float > } };

			AtmosphereConfig atmosphereConfig{ writer
				, c3d_atmosphereData
				, { false, &c3d_matrixData, true, true }
				, { transmittanceExtent.width, transmittanceExtent.height }
				, &transmittanceMap };

			auto getSunLuminance = writer.implementFunction< sdw::Vec3 >( "getSunLuminance"
				, [&]( sdw::Vec3 const & worldPos
					, sdw::Vec3 const & worldDir
					, sdw::Float const & planetRadius )
				{
					if ( renderSunDisk )
					{
						IF( writer, dot( worldDir, c3d_atmosphereData.sunDirection ) > cos( 0.5_f * 0.505_f * 3.14159_f / 180.0_f ) )
						{
							auto t = writer.declLocale( "t"
								, atmosphereConfig.raySphereIntersectNearest( worldPos, worldDir, vec3( 0.0_f, 0.0_f, 0.0_f ), planetRadius ) );
							IF( writer, t < 0.0f ) // no intersection
							{
								auto sunLuminance = writer.declLocale( "sunLuminance"
									, vec3( 1000000.0_f ) ); // arbitrary. But fine, not use when comparing the models
								writer.returnStmt( sunLuminance );
							}
							FI;
						}
						FI;
					}

					writer.returnStmt( vec3( 0.0_f ) );
				}
				, sdw::InVec3{ writer, "worldPos" }
				, sdw::InVec3{ writer, "worldDir" }
				, sdw::InFloat{ writer, "planetRadius" } );

			auto aerialPerspectiveDepthToSlice = writer.implementFunction< sdw::Float >( "aerialPerspectiveDepthToSlice"
				, [&]( sdw::Float const & depth )
				{
					auto apKmPerSlice = writer.declConstant( "apKmPerSlice"
						, 4.0_f );
					writer.returnStmt( depth * ( 1.0_f / apKmPerSlice ) );
				}
				, sdw::InFloat{ writer, "slice" } );

			auto process = writer.implementFunction< sdw::Void >( "process"
				, [&]( sdw::Vec2 const & pixPos )
				{
					auto targetSize = writer.declLocale( "targetSize"
						, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );

					if ( colorTransmittance )
					{
						pxl_transmittance = vec4( 0.0_f, 0.0_f, 0.0_f, 1 );
					}

					auto clipSpace = writer.declLocale( "clipSpace"
						, vec3( ( pixPos / targetSize ) * vec2( 2.0_f, -2.0_f ) - vec2( 1.0_f, -1.0_f ), 1.0_f ) );
					auto hPos = writer.declLocale( "hPos"
						, c3d_matrixData.curProjToWorld( vec4( clipSpace, 1.0_f ) ) );

					auto worldDir = writer.declLocale( "worldDir"
						, normalize( hPos.xyz() / hPos.w() - c3d_sceneData.cameraPosition ) );
					auto worldPos = writer.declLocale( "worldPos"
						, c3d_sceneData.cameraPosition + vec3( 0.0_f, 0.0_f, c3d_atmosphereData.bottomRadius ) );

					auto depthBufferValue = writer.declLocale( "depthBufferValue"
						, -1.0_f );

					auto viewHeight = writer.declLocale( "viewHeight"
						, length( worldPos ) );
					auto L = writer.declLocale( "L"
						, vec3( 0.0_f ) );
					depthBufferValue = depthMap.fetch( ivec2( pixPos ), 0_i );

					if ( fastSky )
					{
						IF( writer, viewHeight < c3d_atmosphereData.topRadius && depthBufferValue == 1.0_f )
						{
							auto uv = writer.declLocale< sdw::Vec2 >( "uv" );
							auto upVector = writer.declLocale( "upVector"
								, normalize( worldPos ) );
							auto viewZenithCosAngle = writer.declLocale( "viewZenithCosAngle"
								, dot( worldDir, upVector ) );

							auto sideVector = writer.declLocale( "sideVector"
								, normalize( cross( upVector, worldDir ) ) );		// assumes non parallel vectors
							auto forwardVector = writer.declLocale( "forwardVector"
								, normalize( cross( sideVector, upVector ) ) );	// aligns toward the sun light but perpendicular to up vector
							auto lightOnPlane = writer.declLocale( "lightOnPlane"
								, vec2( dot( c3d_atmosphereData.sunDirection, forwardVector )
									, dot( c3d_atmosphereData.sunDirection, sideVector ) ) );
							lightOnPlane = normalize( lightOnPlane );
							auto lightViewCosAngle = writer.declLocale( "lightViewCosAngle"
								, lightOnPlane.x() );

							auto intersectGround = writer.declLocale( "intersectGround"
								, atmosphereConfig.raySphereIntersectNearest( worldPos, worldDir, vec3( 0.0_f ), c3d_atmosphereData.bottomRadius ) >= 0.0_f );

							atmosphereConfig.skyViewLutParamsToUv( intersectGround, viewZenithCosAngle, lightViewCosAngle, viewHeight, uv, targetSize );

							pxl_luminance = vec4( skyViewMap.lod( uv, 0.0_f ).rgb() + getSunLuminance( worldPos, worldDir, c3d_atmosphereData.bottomRadius ), 1.0_f );
							writer.returnStmt();
						}
						FI;
					}
					else
					{
						IF( writer, depthBufferValue == 1.0_f )
						{
							L += getSunLuminance( worldPos, worldDir, c3d_atmosphereData.bottomRadius );
						}
						FI;
					}

					if ( fastAerialPerspective )
					{
						CU_Require( !colorTransmittance
							&& "The FASTAERIALPERSPECTIVE_ENABLED path does not support COLORED_TRANSMITTANCE_ENABLED." );
						clipSpace = vec3( ( pixPos / targetSize ) * vec2( 2.0_f, -2.0_f ) - vec2( 1.0_f, -1.0_f ), depthBufferValue );
						auto depthBufferWorldPos = writer.declLocale( "depthBufferWorldPos"
							, c3d_matrixData.curProjToWorld( vec4( clipSpace, 1.0_f ) ) );
						depthBufferWorldPos /= depthBufferWorldPos.w();
						auto tDepth = writer.declLocale( "tDepth"
							, length( depthBufferWorldPos.xyz() - ( worldPos + vec3( 0.0_f, 0.0_f, -c3d_atmosphereData.bottomRadius ) ) ) );
						auto slice = writer.declLocale( "slice"
							, aerialPerspectiveDepthToSlice( tDepth ) );
						auto weight = writer.declLocale( "weight"
							, 1.0_f );

						IF( writer, slice < 0.5_f )
						{
							// We multiply by weight to fade to 0 at depth 0. That works for luminance and opacity.
							weight = clamp( slice * 2.0_f, 0.0_f, 1.0_f );
							slice = 0.5_f;
						}
						FI;

						auto w = writer.declLocale( "w"
							, sqrt( slice / apSliceCount ) );	// squared distribution

						auto AP = writer.declLocale( "AP"
							, weight * volumeMap.lod( vec3( pixPos / targetSize, w ), 0.0_f ) );
						L.rgb() += AP.rgb();
						auto opacity = writer.declLocale( "opacity"
							, AP.a() );

						pxl_luminance = vec4( L, opacity );
					}
					else
					{
						// Move to top atmosphere as the starting point for ray marching.
						// This is critical to be after the above to not disrupt above atmosphere tests and voxel selection.
						IF( writer, !atmosphereConfig.moveToTopAtmosphere( worldPos, worldDir ) )
						{
							// Ray is not intersecting the atmosphere		
							pxl_luminance = vec4( getSunLuminance( worldPos, worldDir, c3d_atmosphereData.bottomRadius ), 1.0_f );
							writer.returnStmt();
						}
						FI;

						SingleScatteringResult ss = writer.declLocale( "ss"
							, atmosphereConfig.integrateScatteredLuminance( pixPos
								, worldPos
								, worldDir
								, c3d_atmosphereData.sunDirection
								, sampleCountIni
								, depthBufferValue ) );

						L += ss.luminance;
						auto throughput = writer.declLocale( "throughput"
							, ss.transmittance );

						if ( colorTransmittance )
						{
							pxl_luminance = vec4( L, 1.0_f );
							pxl_transmittance = vec4( throughput, 1.0_f );
						}
						else
						{
							auto transmittance = writer.declLocale( "transmittance"
								, dot( throughput, vec3( 1.0_f / 3.0_f, 1.0_f / 3.0_f, 1.0_f / 3.0_f ) ) );
							pxl_luminance = vec4( L, 1.0_f - transmittance );
						}
					}
				}
				, sdw::InVec2{ writer, "pixPos" } );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::FragmentIn{ writer }
				, sdw::FragmentOut{ writer }
				, [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
				{
					process( in.fragCoord.xy() );
					pxl_luminance /= pxl_luminance.a();	// Normalise according to sample count when path tracing

					// Similar setup to the Bruneton demo
					auto whitePoint = writer.declLocale( "whitePoint"
						, vec3( 1.08241_f, 0.96756_f, 0.95003_f ) );
					auto exposure = writer.declLocale( "exposure"
						, 10.0_f );
					auto hdr = vec3( 1.0_f ) - exp( -pxl_luminance.rgb() / whitePoint * exposure );
					pxl_luminance = vec4( hdr, 1.0_f );
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
		, crg::ImageViewId const * depth )
		: castor3d::BackgroundPassBase{ pass
			, context
			, graph
			, device
			, background
			, size
			, depth }
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
			, atmos::Name
			, atmos::getVertexProgram() };
		m_pixelModule = { VK_SHADER_STAGE_FRAGMENT_BIT
			, atmos::Name
			, atmos::getPixelProgram( m_size, m_atmosBackground.getTransmittance().getExtent() ) };
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

		m_descriptorBindings.push_back( { uint32_t( AtmosphereBackgroundPass::eDepth )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, 1u
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, nullptr } );
		auto depth = m_graph.createImageView( *m_depth );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( AtmosphereBackgroundPass::eDepth )
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VkDescriptorImageInfo{ m_background->getSampler().getSampler()
				, depth
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );

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

		m_descriptorBindings.push_back( { uint32_t( AtmosphereBackgroundPass::eSkyView )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, 1u
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( AtmosphereBackgroundPass::eSkyView )
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VkDescriptorImageInfo{ m_background->getSampler().getSampler()
				, atmosphere.getSkyView().sampledView
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );

		m_descriptorBindings.push_back( { uint32_t( AtmosphereBackgroundPass::eVolume )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, 1u
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, nullptr } );
		m_descriptorWrites.push_back( crg::WriteDescriptorSet{ uint32_t( AtmosphereBackgroundPass::eVolume )
			, 0u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VkDescriptorImageInfo{ m_background->getSampler().getSampler()
				, atmosphere.getVolume().sampledView
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );

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
