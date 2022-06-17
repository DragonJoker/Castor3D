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

			C3D_Camera( writer
				, AtmosphereBackgroundPass::eCamera
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
				, { false, &c3d_cameraData, true, true }
				, { transmittanceExtent.width, transmittanceExtent.height }
				, &transmittanceMap };

			auto getValFromTLUT = writer.implementFunction< sdw::Vec3 >( "getValFromTLUT"
				, [&]( sdw::Vec3 const & pos
					, sdw::Vec3 const & sunDir )
				{
					auto height = writer.declLocale( "height"
						, length( pos ) );
					auto up = writer.declLocale( "up"
						, pos / height );
					auto sunCosZenithAngle = writer.declLocale( "sunCosZenithAngle"
						, dot( sunDir, up ) );
					auto uv = writer.declLocale( "uv"
						, vec2( clamp( 0.5_f + 0.5_f * sunCosZenithAngle, 0.0_f, 1.0_f )
							, max( 0.0_f, min( 1.0_f, ( height - c3d_atmosphereData.bottomRadius ) / ( c3d_atmosphereData.topRadius - c3d_atmosphereData.bottomRadius ) ) ) ) );
					writer.returnStmt( transmittanceMap.lod( uv, 0.0_f ).rgb() );
				}
				, sdw::InVec3{ writer, "pos" }
				, sdw::InVec3{ writer, "sunDir" } );

			auto getSunLuminance = writer.implementFunction< sdw::Vec3 >( "getSunLuminance"
				, [&]( sdw::Vec3 const & worldPos
					, sdw::Vec3 const & worldDir )
				{
					if ( renderSunDisk )
					{
						auto sunSolidAngle = writer.declLocale( "sunSolidAngle"
							, 0.53_f * sdw::Float{ castor::Pi< float > } / 180.0_f );
						auto minSunCosTheta = writer.declLocale( "minSunCosTheta"
							, cos( sunSolidAngle ) );
						auto cosTheta = writer.declLocale( "cosTheta"
							, dot( worldDir, c3d_atmosphereData.sunDirection ) );
						auto sunLuminance = writer.declLocale( "sunLuminance"
							, vec3( 1000000.0_f ) ); // arbitrary. But fine, not use when comparing the models

						IF( writer, cosTheta < minSunCosTheta )
						{
							auto offset = writer.declLocale( "offset"
								, minSunCosTheta - cosTheta );
							auto gaussianBloom = writer.declLocale( "gaussianBloom"
								, exp( -offset * 50000.0_f ) * 0.5_f );
							auto invBloom = writer.declLocale( "invBloom"
								, 1.0_f / ( 0.02_f + offset * 300.0_f ) * 0.01_f );
							sunLuminance = vec3( gaussianBloom + invBloom );
						}
						FI;

						// Use smoothstep to limit the effect, so it drops off to actual zero.
						sunLuminance = smoothStep( vec3( 0.002_f ), vec3( 1.0_f ), sunLuminance );

						IF( writer, length( sunLuminance ) > 0.0_f )
						{
							IF( writer, atmosphereConfig.raySphereIntersectNearest( worldPos, worldDir, vec3( 0.0_f ), c3d_atmosphereData.bottomRadius ) >= 0.0_f )
							{
								sunLuminance *= vec3( 0.0_f );
							}
							ELSE
							{
								 // If the sun value is applied to this pixel, we need to calculate the transmittance to obscure it.
								sunLuminance *= getValFromTLUT( worldPos, c3d_atmosphereData.sunDirection );
							}
							FI;
						}
						FI;

						writer.returnStmt( sunLuminance );
					}

					writer.returnStmt( vec3( 0.0_f ) );
				}
				, sdw::InVec3{ writer, "worldPos" }
				, sdw::InVec3{ writer, "worldDir" } );

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
						, vec2( sdw::Float{ float( renderSize.width + 1u ) }, float( renderSize.height + 1u ) ) );

					if ( colorTransmittance )
					{
						pxl_transmittance = vec4( 0.0_f, 0.0_f, 0.0_f, 1 );
					}

					auto clipSpace = writer.declLocale( "clipSpace"
						, vec3( ( pixPos / targetSize ) * vec2( 2.0_f, 2.0_f ) - vec2( 1.0_f, 1.0_f ), 1.0_f ) );
					auto hPos = writer.declLocale( "hPos"
						, c3d_cameraData.camProjToWorld( vec4( clipSpace, 1.0_f ) ) );

					auto worldDir = writer.declLocale( "worldDir"
						, normalize( hPos.xyz() / hPos.w() - c3d_cameraData.position ) );
					auto worldPos = writer.declLocale( "worldPos"
						, c3d_cameraData.position + vec3( 0.0_f, c3d_atmosphereData.bottomRadius, 0.0_f ) );

					auto depthBufferValue = writer.declLocale( "depthBufferValue"
						, -1.0_f );

					auto viewHeight = writer.declLocale( "viewHeight"
						, length( worldPos ) );
					auto L = writer.declLocale( "L"
						, vec3( 0.0_f ) );
					depthBufferValue = depthMap.fetch( ivec2( pixPos ), 0_i );

					if ( fastSky )
					{
						IF( writer, viewHeight < c3d_atmosphereData.topRadius && depthBufferValue >= 1.0_f )
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

							pxl_luminance = vec4( skyViewMap.lod( uv, 0.0_f ).rgb() + getSunLuminance( worldPos, worldDir ), 1.0_f );
							writer.returnStmt();
						}
						FI;
					}
					else
					{
						IF( writer, depthBufferValue >= 1.0_f )
						{
							L += getSunLuminance( worldPos, worldDir );
						}
						FI;
					}

					if ( fastAerialPerspective )
					{
						CU_Require( !colorTransmittance
							&& "The FASTAERIALPERSPECTIVE_ENABLED path does not support COLORED_TRANSMITTANCE_ENABLED." );
						auto depthBufferWorldPos = writer.declLocale( "depthBufferWorldPos"
							, atmosphereConfig.getWorldPos( depthBufferValue
								, pixPos
								, targetSize ) );
						auto tDepth = writer.declLocale( "tDepth"
							, length( depthBufferWorldPos - ( worldPos + vec3( 0.0_f, -c3d_atmosphereData.bottomRadius, 0.0_f ) ) ) );
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
							pxl_luminance = vec4( getSunLuminance( worldPos, worldDir ), 1.0_f );
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
								, dot( throughput, vec3( 1.0_f / 3.0_f ) ) );
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
					pxl_luminance.xyz() /= pxl_luminance.a();	// Normalise according to sample count when path tracing

					// Similar setup to the Bruneton demo
					auto whitePoint = writer.declLocale( "whitePoint"
						, vec3( 1.08241_f, 0.96756_f, 0.95003_f ) );
					auto exposure = writer.declLocale( "exposure"
						, 10.0_f );
					auto hdr = vec3( 1.0_f ) - exp( -pxl_luminance.rgb() / whitePoint * exposure );
					pxl_luminance = vec4( hdr, pxl_luminance.a() );
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
			, background }
		, crg::RenderQuad{ pass
			, context
			, graph
			, crg::ru::Config{ 1u, true }
			, crg::rq::Config{}
				.isEnabled( IsEnabledCallback( [this](){ return castor3d::BackgroundPassBase::doIsEnabled(); } ) )
				.renderSize( size )
				.program( doInitialiseShader( device, background, size ) ) }
		, m_atmosBackground{ background }
	{
	}

	void AtmosphereBackgroundPass::doResetPipeline()
	{
		resetCommandBuffer();
		resetPipeline( {} );
		reRecordCurrent();
	}

	crg::VkPipelineShaderStageCreateInfoArray AtmosphereBackgroundPass::doInitialiseShader( castor3d::RenderDevice const & device
		, AtmosphereBackground & background
		, VkExtent2D const & size )
	{
		castor::DataHolderT< Shaders >::getData().vertexShader = { VK_SHADER_STAGE_VERTEX_BIT
			, atmos::Name
			, atmos::getVertexProgram() };
		castor::DataHolderT< Shaders >::getData().pixelShader = { VK_SHADER_STAGE_FRAGMENT_BIT
			, atmos::Name
			, atmos::getPixelProgram( size, background.getTransmittance().getExtent() ) };
		castor::DataHolderT< Shaders >::getData().stages =
		{
			makeShaderState( device, castor::DataHolderT< Shaders >::getData().vertexShader ),
			makeShaderState( device, castor::DataHolderT< Shaders >::getData().pixelShader ),
		};
		return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( castor::DataHolderT< Shaders >::getData().stages );
	}

	//************************************************************************************************
}
