#include "Castor3D/Render/Ssao/SsaoRawAOPass.hpp"

#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Math/Angle.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Queue.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FrameGraph.hpp>

#include <random>

CU_ImplementSmartPtr( castor3d, SsaoRawAOPass )

namespace castor3d
{
	namespace ssaoraw
	{
		enum Idx
		{
			SsaoCfgUboIdx,
			CameraUboIdx,
			DepthMapIdx,
			NormalMapIdx,
		};

		static ShaderPtr getProgram( RenderDevice const & device
			, bool useNormalsBuffer )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_SsaoConfig( writer, SsaoCfgUboIdx, 0u );
			C3D_Camera( writer, CameraUboIdx, 0u );
			// Negative, "linear" values in world-space units
			auto c3d_mapDepth = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapDepth", DepthMapIdx, 0u );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_mapNormal = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapNormal", NormalMapIdx, 0u, useNormalsBuffer );
			auto c3d_readMultiplyFirst = writer.declConstant( "c3d_readMultiplyFirst", vec3( 2.0_f ) );
			auto c3d_readAddSecond = writer.declConstant( "c3d_readAddSecond", vec3( 1.0_f ) );

			// Inputs
			auto inPosition = writer.declInput< sdw::Vec2 >( "inPosition", sdw::EntryPoint::eVertex, 0u );

			// Outputs
			auto outColour = writer.declOutput< sdw::Vec3 >( "outColour", sdw::EntryPoint::eFragment, 0u );
			auto outBentNormal = writer.declOutput< sdw::Vec4 >( "outBentNormal", sdw::EntryPoint::eFragment, 1u );

#define visibility outColour.r()
#define bilateralKey outColour.g()

			//////////////////////////////////////////////////

			// Reconstruct camera-space P.xyz from screen-space S = (x, y) in
			// pixels and camera-space z < 0.  Assumes that the upper-left pixel center
			// is at (0.5, 0.5) [but that need not be the location at which the sample tap
			// was placed!]
			// Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
			auto reconstructCSPosition = writer.implementFunction< sdw::Vec3 >( "reconstructCSPosition"
				, [&]( sdw::Vec2 const & ssPosition
					, sdw::Float const & csZ
					, sdw::Vec4 const & projInfo )
				{
					writer.returnStmt( vec3( sdw::fma( ssPosition.xy(), projInfo.xy(), projInfo.zw() ) * csZ, csZ ) );
				}
				, sdw::InVec2{ writer, "ssPosition" }
				, sdw::InFloat{ writer, "csZ" }
				, sdw::InVec4{ writer, "projInfo" } );

			// Reconstructs camera-space normal from camera-space position
			auto reconstructNonUnitCSFaceNormal = writer.implementFunction< sdw::Vec3 >( "reconstructNonUnitCSFaceNormal"
				, [&]( sdw::Vec3 const & csPosition )
				{
					writer.returnStmt( cross( dFdy( csPosition ), dFdx( csPosition ) ) );
				}
				, sdw::InVec3{ writer, "csPosition" } );

			// Returns a unit vector and a screen-space radius for the tap on a unit disk
			//	(the caller should scale by the actual disk radius)
			auto tapLocation = writer.implementFunction< sdw::Vec2 >( "tapLocation"
				, [&]( sdw::Int const & sampleNumber
					, sdw::Int const & sampleCount
					, sdw::Float const & spinAngle
					, sdw::Float ssRadius )
				{
					// Radius relative to ssR
					auto alpha = writer.declLocale( "alpha"
						, ( writer.cast< sdw::Float >( sampleNumber ) + 0.5_f ) * ( 1.0_f / writer.cast< sdw::Float >( sampleCount ) ) );
					auto angle = writer.declLocale( "angle"
						, alpha * ( 6.28_f * writer.cast< sdw::Float >( c3d_ssaoConfigData.numSpiralTurns ) ) + spinAngle );

					ssRadius = alpha;
					writer.returnStmt( vec2( cos( angle ), sin( angle ) ) );
				}
				, sdw::InInt{ writer, "sampleNumber" }
				, sdw::InInt{ writer, "sampleCount" }
				, sdw::InFloat{ writer, "spinAngle" }
				, sdw::OutFloat{ writer, "ssRadius" } );


			// Used for packing Z into the GB channels
			auto csZToKey = writer.implementFunction< sdw::Float >( "csZToKey"
				, [&]( sdw::Float const & csZ )
				{
					writer.returnStmt( clamp( csZ * ( 1.0_f / c3d_ssaoConfigData.farPlaneZ ), 0.0_f, 1.0_f ) );
				}
				, sdw::InFloat{ writer, "csZ" } );

			// Read the camera-space position of the point at screen-space pixel ssPosition
			auto getPosition = writer.implementFunction< sdw::Vec3 >( "getPosition"
				, [&]( sdw::IVec2 const & ssPosition )
				{
					auto position = writer.declLocale< sdw::Vec3 >( "position" );
					position.z() = c3d_mapDepth.fetch( ssPosition, 0_i ).r();

					// Offset to pixel center
					position = reconstructCSPosition( vec2( ssPosition ) + vec2( 0.5_f )
						, position.z()
						, c3d_ssaoConfigData.projInfo );
					writer.returnStmt( position );
				}
				, sdw::InIVec2{ writer, "ssPosition" } );

			auto getMipLevel = writer.implementFunction< sdw::Int >( "getMipLevel"
				, [&]( sdw::Float const & ssRadius )
				{
					// Derivation:
					//  mipLevel = floor(log(ssR / MAX_OFFSET));
					writer.returnStmt( clamp( writer.cast< sdw::Int >( floor( log2( ssRadius ) ) ) - c3d_ssaoConfigData.logMaxOffset
						, 0_i
						, c3d_ssaoConfigData.maxMipLevel ) );
				}
				, sdw::InFloat{ writer, "ssRadius" } );

			// Read the camera-space position of the point at screen-space pixel ssP + unitOffset * ssR.
			// Assumes length(unitOffset) == 1.
			auto getOffsetPosition = writer.implementFunction< sdw::Vec3 >( "getOffsetPosition"
				, [&]( sdw::IVec2 const & ssCenter
					, sdw::Vec2 const & unitOffset
					, sdw::Float const & ssRadius
					, sdw::Float const & invCszBufferScale )
				{
					// Derivation:
					auto mipLevel = writer.declLocale( "mipLevel"
						, getMipLevel( ssRadius ) );
					auto ssPosition = writer.declLocale( "ssPosition"
						, ivec2( ssRadius * unitOffset ) + ssCenter );
					auto position = writer.declLocale< sdw::Vec3 >( "position" );

					// We need to divide by 2^mipLevel to read the appropriately scaled coordinate from a MIP-map.
					// Manually clamp to the texture size because texelFetch bypasses the texture unit
					auto mipPosition = writer.declLocale( "mipPosition"
						, clamp( ivec2( ssPosition.x() >> mipLevel, ssPosition.y() >> mipLevel )
							, ivec2( 0_i )
							, c3d_mapDepth.getSize( mipLevel ) - ivec2( 1_i ) ) );
					position.z() = c3d_mapDepth.fetch( mipPosition, mipLevel ).r();

					// Offset to pixel center
					position = reconstructCSPosition( ( vec2( ssPosition ) + vec2( 0.5_f ) ) * invCszBufferScale
						, position.z()
						, c3d_ssaoConfigData.projInfo );
					writer.returnStmt( position );
				}
				, sdw::InIVec2{ writer, "ssCenter" }
				, sdw::InVec2{ writer, "unitOffset" }
				, sdw::InFloat{ writer, "ssRadius" }
				, sdw::InFloat{ writer, "invCszBufferScale" } );

			// Smaller return value = less occlusion
			auto fallOffFunction = writer.implementFunction< sdw::Float >( "fallOffFunction"
				, [&]( sdw::Float const & vv
					, sdw::Float const & vn
					, sdw::Float const & epsilon )
				{
					// A: From the HPG12 paper
					// Note large epsilon to avoid overdarkening within cracks
					//  Assumes the desired result is intensity/radius^6 in main()
					// return float(vv < radius2) * max((vn - bias) / (epsilon + vv), 0.0) * radius2 * 0.6;

					// B: Smoother transition to zero (lowers contrast, smoothing out corners). [Recommended]
					IF( writer, c3d_ssaoConfigData.highQuality )
					{
						// Epsilon inside the sqrt for rsqrt operation
						auto f = writer.declLocale( "f"
							, max( 1.0_f - vv * c3d_ssaoConfigData.invRadius2, 0.0_f ) );
						writer.returnStmt( f * max( ( vn - c3d_ssaoConfigData.bias ) * inverseSqrt( epsilon + vv ), 0.0_f ) );
					}
					ELSE
					{
						// Avoid the square root from above.
						//  Assumes the desired result is intensity/radius^6 in main()
						auto f = writer.declLocale( "f"
							, max( c3d_ssaoConfigData.radius2 - vv, 0.0_f ) );
						writer.returnStmt( f * f * f * max( ( vn - c3d_ssaoConfigData.bias ) / ( epsilon + vv ), 0.0_f ) );
					}
					FI;

					// C: Medium contrast (which looks better at high radii), no division.  Note that the 
					// contribution still falls off with radius^2, but we've adjusted the rate in a way that is
					// more computationally efficient and happens to be aesthetically pleasing.  Assumes 
					// division by radius^6 in main()
					// return 4.0 * max(1.0 - vv * invRadius2, 0.0) * max(vn - bias, 0.0);

					// D: Low contrast, no division operation
					//return 2.0 * float(vv < radius * radius) * max(vn - bias, 0.0);
				}
				, sdw::InFloat{ writer, "vv" }
				, sdw::InFloat{ writer, "vn" }
				, sdw::InFloat{ writer, "epsilon" } );

			// Compute the occlusion due to sample point \a occluder about camera-space point \a csCenter with unit normal \a normal
			auto aoValueFromPositionsAndNormal = writer.implementFunction< sdw::Float >( "aoValueFromPositionsAndNormal"
				, [&]( sdw::Vec3 const & csCenter
					, sdw::Vec3 const & normal
					, sdw::Vec3 const & occluder )
				{
					auto v = writer.declLocale( "v"
						, occluder - csCenter );
					auto vv = writer.declLocale( "vv"
						, dot( v, v ) );
					auto vn = writer.declLocale( "vn"
						, dot( v, normal ) );

					// Without the angular adjustment term, surfaces seen head on have less AO
					writer.returnStmt( fallOffFunction( vv, vn, 0.001_f ) * mix( 1.0_f, max( 0.0_f, 1.5_f * normal.z() ), 0.35_f ) );
				}
				, sdw::InVec3{ writer, "csCenter" }
				, sdw::InVec3{ writer, "normal" }
				, sdw::InVec3{ writer, "occluder" } );

			// Compute the occlusion due to sample with index \a tapIndex about the pixel at \a ssCenter that corresponds
			// to camera-space point \a csCenter with unit normal \a normal, using maximum screen-space sampling radius \a ssDiskRadius

			// Note that units of H() in the HPG12 paper are meters, not unitless.
			// The whole falloff/sampling function is therefore unitless.
			// In this implementation, we factor out (9 / radius).

			// Four versions of the falloff function are implemented below."
			auto sampleAO = writer.implementFunction< sdw::Vec3 >( "sampleAO"
				, [&]( sdw::IVec2 const & ssCenter
					, sdw::Vec3 const & csCenter
					, sdw::Vec3 const & normal
					, sdw::Float const & ssDiskRadius
					, sdw::Int const & tapIndex
					, sdw::Int const & tapCount
					, sdw::Float const & randomPatternRotationAngle
					, sdw::Float const & invCszBufferScale
					, sdw::Float occlusion )
				{
					// Offset on the unit disk, spun for this pixel
					auto ssRadius = writer.declLocale( "ssRadius"
						, 0.0_f );
					auto unitOffset = writer.declLocale( "unitOffset"
						, tapLocation( tapIndex, tapCount, randomPatternRotationAngle, ssRadius ) );

					// Ensure that the taps are at least 1 pixel away
					ssRadius = max( 0.75_f, ssRadius * ssDiskRadius );

					// The occluding point in camera space
					auto occluder = writer.declLocale( "occluder"
						, getOffsetPosition( ssCenter
							, unitOffset
							, ssRadius
							, invCszBufferScale ) );
					occlusion = aoValueFromPositionsAndNormal( csCenter, normal, occluder );
					writer.returnStmt( occluder - csCenter );
				}
				, sdw::InIVec2{ writer, "ssCenter" }
				, sdw::InVec3{ writer, "csCenter" }
				, sdw::InVec3{ writer, "normal" }
				, sdw::InFloat{ writer, "ssDiskRadius" }
				, sdw::InInt{ writer, "tapIndex" }
				, sdw::InInt{ writer, "tapCount" }
				, sdw::InFloat{ writer, "randomPatternRotationAngle" }
				, sdw::InFloat{ writer, "invCszBufferScale" }
				, sdw::OutFloat{ writer, "occlusion" } );

			auto isOccluded = writer.implementFunction< sdw::Float >( "isOccluded"
				, [&]( sdw::Vec3 const & csPosition
					, sdw::Vec3 const & csCenter )
				{
					writer.returnStmt( 0.0_f );
				}
				, sdw::InVec3{ writer, "csPosition" }
				, sdw::InVec3{ writer, "csCenter" } );

			auto sampleRay = writer.implementFunction< sdw::Vec3 >( "sampleRay"
				, [&]( sdw::Vec3 const & csCenter
					, sdw::Vec3 const & csRay )
				{
					// Offset on the unit disk, spun for this pixel
					auto step = writer.declLocale( "step"
						, csRay * c3d_ssaoConfigData.bendStepSize );
					auto current = writer.declLocale( "currentRadius"
						, csCenter + step );
					auto currentOcclusion = writer.declLocale( "currentOcclusion"
						, 0.0_f );
					auto stepIdx = writer.declLocale( "stepIdx"
						, 0_i );

					WHILE( writer, stepIdx < c3d_ssaoConfigData.bendStepCount && currentOcclusion == 0.0_f )
					{
						currentOcclusion = isOccluded( csCenter, current );
						stepIdx += 1u;
						current += step;
					}
					ELIHW;

					writer.returnStmt( current * ( 1.0_f - ( currentOcclusion / writer.cast< sdw::Float >( stepIdx ) ) ) );
				}
				, sdw::InVec3{ writer, "csCenter" }
				, sdw::InVec3{ writer, "csRay" } );

			auto square = writer.implementFunction< sdw::Float >( "square"
				, [&]( sdw::Float const & x )
				{
					writer.returnStmt( x * x );
				}
				, sdw::InFloat{ writer, "x" } );

			writer.implementEntryPoint( [&]( sdw::VertexIn in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( inPosition, 0.0_f, 1.0_f );
				} );

			writer.implementEntryPoint( [&]( sdw::FragmentIn in
				, sdw::FragmentOut out )
				{
					// Pixel being shaded
					auto ssCenter = writer.declLocale( "ssCenter"
						, ivec2( in.fragCoord.xy() ) );

					// World space point being shaded
					auto csCenter = writer.declLocale( "csCenter"
						, getPosition( ssCenter ) );

					bilateralKey = csZToKey( csCenter.z() );

					auto normal = writer.declLocale< sdw::Vec3 >( "normal" );

					if ( useNormalsBuffer )
					{
						normal = c3d_mapNormal.fetch( ivec2( in.fragCoord.xy() ), 0_i ).xyz();
						normal = normalize( c3d_cameraData.readNormal( normal ) );
					}
					else
					{
						// Reconstruct normals from positions.
						normal = reconstructNonUnitCSFaceNormal( csCenter );

						// Since normal is computed from the cross product of camera-space edge vectors from points
						// at adjacent pixels, its magnitude will be proportional to the square of distance from the camera
						//
						// if the threshold # is too big you will see black dots where we used a bad normal at edges, too small -> white
						IF( writer, dot( normal, normal ) > ( square( csCenter.z() * csCenter.z() * 0.00006_f ) ) )
						{
							// The normals from depth should be very small values before normalization,
							// except at depth discontinuities, where they will be large and lead
							// to 1-pixel false occlusions because they are not reliable
							visibility = 1.0_f;
							outBentNormal.rgb() = normalize( c3d_cameraData.writeNormal( normal ) );
							outBentNormal.a() = 0.0f;
							writer.returnStmt();
						}
						ELSE
						{
							// Precision is pretty bad on 16-bit depth
							normal = normalize( normal );
						}
						FI;
					}

					// Choose the screen-space sample radius
					// proportional to the projected area of the sphere
					auto ssDiskRadius = writer.declLocale( "ssDiskRadius"
						, c3d_ssaoConfigData.projScale * c3d_ssaoConfigData.radius / csCenter.z() );

					IF( writer, ssDiskRadius <= c3d_ssaoConfigData.minRadius )
					{
						// There is no way to compute AO at this radius
						visibility = 1.0_f;
						outBentNormal.rgb() = c3d_cameraData.writeNormal( normal );
						outBentNormal.a() = 0.0f;
						writer.returnStmt();
					}
					FI;

					// Hash function used in the HPG12 AlchemyAO paper
					auto randomPatternRotationAngle = writer.declLocale( "randomPatternRotationAngle"
						, 10.0_f * writer.cast< sdw::Float >( ( 3 * ssCenter.x() ) ^ ( ssCenter.y() + ssCenter.x() * ssCenter.y() ) ) );

					auto sum = writer.declLocale( "sum"
						, 0.0_f );
					auto bentNormal = writer.declLocale( "bentNormal"
						, vec3( 0.0_f ) );

					FOR( writer, sdw::Int, i, 0, i < c3d_ssaoConfigData.numSamples, ++i )
					{
						auto occluder = writer.declLocale( "occluder"
							, vec3( 0.0_f ) );
						auto occlusion = writer.declLocale( "occlusion"
							, 0.0_f );
						auto ssRadius = writer.declLocale( "ssRadius"
							, 0.0_f );
						auto csRay = writer.declLocale( "ray"
							, sampleAO( ssCenter
								, csCenter
								, normal
								, ssDiskRadius
								, i
								, c3d_ssaoConfigData.numSamples
								, randomPatternRotationAngle
								, 1.0_f
								, occlusion ) );
						sum += occlusion;
						bentNormal += sampleRay( csCenter, csRay );
					}
					ROF;

					bentNormal = normalize( bentNormal )/* * 0.5_f + 0.5_f*/;
					outBentNormal.xyz() = c3d_cameraData.writeNormal( bentNormal );

					auto A = writer.declLocale< sdw::Float >( "A" );

					IF( writer, c3d_ssaoConfigData.highQuality )
					{
						A = pow( max( 0.0_f
								, 1.0_f - sqrt( sum * ( 3.0_f / writer.cast< sdw::Float >( c3d_ssaoConfigData.numSamples ) ) ) )
							, c3d_ssaoConfigData.intensity );
					}
					ELSE
					{
						A = max( 0.0_f
							, 1.0_f - sum * c3d_ssaoConfigData.intensityDivR6 * ( 5.0_f / writer.cast< sdw::Float >( c3d_ssaoConfigData.numSamples ) ) );
						// Anti-tone map to reduce contrast and drag dark region farther
						// (x^0.2 + 1.2 * x^4)/2.2
						A = ( pow( A, 0.2_f ) + 1.2_f * A * A * A * A ) / 2.2_f;
					}
					FI;

					// Visualize random spin distribution
					//A = mod(randomPatternRotationAngle / (2 * 3.141592653589), 1.0);

					// Fade in as the radius reaches 2 pixels
					visibility = mix( 1.0_f
						, A
						, clamp( ssDiskRadius - c3d_ssaoConfigData.minRadius
							, 0.0_f
							, 1.0_f ) );
				} );
			return writer.getBuilder().releaseShader();
		}

		static Texture doCreateTexture( crg::ResourcesCache & resources
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size )
		{
			return Texture{ device
				, resources
				, name
				, 0u
				, VkExtent3D{ size.width, size.height, 1u }
				, 1u
				, 1u
				, format
				, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT
					| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) };
		}

		static crg::rq::Config getConfig( VkExtent2D const & renderSize
			, SsaoConfig const & ssaoConfig
			, uint32_t const & passIndex
			, ashes::PipelineShaderStageCreateInfoArray const & stages0
			, ashes::PipelineShaderStageCreateInfoArray const & stages1 )
		{
			crg::rq::Config result;
			result.enabled( &ssaoConfig.enabled );
			result.renderSize( renderSize );
			result.passIndex( &passIndex );
			result.programs( { crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages0 )
				, crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages1 ) } );
			return result;
		}
	}

	//*********************************************************************************************

	SsaoRawAOPass::RenderQuad::RenderQuad( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, crg::rq::Config config
		, SsaoConfig const & ssaoConfig )
		: crg::RenderQuad{ pass
			, context
			, graph
			, { 2u, false }
			, std::move( config ) }
		, ssaoConfig{ ssaoConfig }
	{
	}

	uint32_t SsaoRawAOPass::RenderQuad::doGetPassIndex()const
	{
		return ssaoConfig.useNormalsBuffer ? 1u : 0u;
	}
	
	//*********************************************************************************************

	SsaoRawAOPass::Program::Program( RenderDevice const & device
		, bool useNormalsBuffer
		, castor::String const & prefix )
		: shader{ prefix + "SsaoRawAO" + ( useNormalsBuffer ? std::string{ "Normals" } : std::string{} )
			, ssaoraw::getProgram( device, useNormalsBuffer ) }
		, stages{ makeProgramStates( device, shader ) }
	{
	}
	
	//*********************************************************************************************

	SsaoRawAOPass::SsaoRawAOPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, crg::FramePass const & previousPass
		, VkExtent2D const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, CameraUbo const & cameraUbo
		, crg::ImageViewIdArray const & linearisedDepthBufferViews
		, Texture const & normals
		, uint32_t const & passIndex )
		: m_device{ device }
		, m_graph{ graph }
		, m_ssaoConfig{ config }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_cameraUbo{ cameraUbo }
		, m_size{ size }
		, m_result{ ssaoraw::doCreateTexture( *normals.resources
			, m_device
			, m_graph.getName() + "SsaoRawAOResult"
			, device.selectSmallestFormatRSFloatFormat( VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
				| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
				| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT )
			, m_size ) }
		, m_bentNormals{ ssaoraw::doCreateTexture( *normals.resources
			, m_device
			, m_graph.getName() + "BentNormals"
			, device.selectSmallestFormatRGBSFloatFormat( VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
				| VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
				| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT )
			, m_size ) }
		, m_programs{ Program{ device, false, m_graph.getName() }
			, Program{ device, true, m_graph.getName() } }
	{
		stepProgressBar( progress, "Creating SSAO raw AO pass" );
		auto & pass = m_graph.createPass( "RawAO"
			, [this, &passIndex, progress]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising SSAO raw AO pass" );
				auto result = std::make_unique< RenderQuad >( pass
					, context
					, graph
					, ssaoraw::getConfig( m_size
						, m_ssaoConfig
						, passIndex
						, m_programs[0].stages
						, m_programs[1].stages )
					, m_ssaoConfig );
				m_device.renderSystem.getEngine()->registerTimer( pass.getFullName()
					, result->getTimer() );
				return result;
			} );
		m_lastPass = &pass;
		pass.addDependency( previousPass );
		m_ssaoConfigUbo.createPassBinding( pass, ssaoraw::SsaoCfgUboIdx );
		m_cameraUbo.createPassBinding( pass, ssaoraw::CameraUboIdx );
		pass.addSampledView( pass.mergeViews( linearisedDepthBufferViews ), ssaoraw::DepthMapIdx );
		pass.addSampledView( normals.sampledViewId, ssaoraw::NormalMapIdx );
		pass.addOutputColourView( m_result.targetViewId, opaqueWhiteClearColor );
		pass.addOutputColourView( m_bentNormals.targetViewId, transparentBlackClearColor );
		m_result.create();
		m_bentNormals.create();
	}

	SsaoRawAOPass::~SsaoRawAOPass()
	{
		m_result.destroy();
		m_bentNormals.destroy();
	}

	void SsaoRawAOPass::accept( SsaoConfig & config
		, ConfigurationVisitorBase & visitor )
	{
		visitor.visit( "SSAO Raw AO"
			, getResult()
			, m_graph.getFinalLayoutState( getResult().sampledViewId ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( "SSAO Bent Normals"
			, getBentResult()
			, m_graph.getFinalLayoutState( getBentResult().sampledViewId ).layout
			, TextureFactors{}.invert( true ) );

		auto index = m_ssaoConfig.useNormalsBuffer
			? 1u
			: 0u;
		visitor.visit( m_programs[index].shader );
		config.accept( visitor );
	}
}
