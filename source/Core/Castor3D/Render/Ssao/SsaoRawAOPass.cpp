#include "Castor3D/Render/Ssao/SsaoRawAOPass.hpp"

#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
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

#include <RenderGraph/FrameGraph.hpp>

#include <random>

CU_ImplementCUSmartPtr( castor3d, SsaoRawAOPass )

namespace castor3d
{
	namespace
	{
		enum Idx
		{
			SsaoCfgUboIdx,
			GpInfoUboIdx,
			DepthMapIdx,
			NormalMapIdx,
		};

		ShaderPtr getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );

			writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
				, VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram( bool useNormalsBuffer )
		{
			using namespace sdw;
			FragmentWriter writer;

			//////////////////////////////////////////////////

			UBO_SSAO_CONFIG( writer, SsaoCfgUboIdx, 0u );
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			// Negative, "linear" values in world-space units
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapDepth", DepthMapIdx, 0u );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_mapNormal = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapNormal", NormalMapIdx, 0u, useNormalsBuffer );
			auto c3d_readMultiplyFirst = writer.declConstant( "c3d_readMultiplyFirst", vec3( 2.0_f ) );
			auto c3d_readAddSecond = writer.declConstant( "c3d_readAddSecond", vec3( 1.0_f ) );

			auto pxl_fragColor = writer.declOutput< Vec3 >( "pxl_fragColor", 0u );
			auto pxl_bentNormal = writer.declOutput< Vec4 >( "pxl_bentNormal", 1u );

#define visibility pxl_fragColor.r()
#define bilateralKey pxl_fragColor.g()

			//////////////////////////////////////////////////

			// Reconstruct camera-space P.xyz from screen-space S = (x, y) in
			// pixels and camera-space z < 0.  Assumes that the upper-left pixel center
			// is at (0.5, 0.5) [but that need not be the location at which the sample tap
			// was placed!]
			// Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
			auto reconstructCSPosition = writer.implementFunction< Vec3 >( "reconstructCSPosition"
				, [&]( Vec2 const & ssPosition
					, Float const & csZ
					, Vec4 const & projInfo )
				{
					writer.returnStmt( vec3( sdw::fma( ssPosition.xy(), projInfo.xy(), projInfo.zw() ) * csZ, csZ ) );
				}
				, InVec2{ writer, "ssPosition" }
				, InFloat{ writer, "csZ" }
				, InVec4{ writer, "projInfo" } );

			// Reconstructs camera-space normal from camera-space position
			auto reconstructNonUnitCSFaceNormal = writer.implementFunction< Vec3 >( "reconstructNonUnitCSFaceNormal"
				, [&]( Vec3 const & csPosition )
				{
					writer.returnStmt( cross( dFdy( csPosition ), dFdx( csPosition ) ) );
				}
				, InVec3{ writer, "csPosition" } );

			// Returns a unit vector and a screen-space radius for the tap on a unit disk
			//	(the caller should scale by the actual disk radius)
			auto tapLocation = writer.implementFunction< Vec2 >( "tapLocation"
				, [&]( Int const & sampleNumber
					, Int const & sampleCount
					, Float const & spinAngle
					, Float ssRadius )
				{
					// Radius relative to ssR
					auto alpha = writer.declLocale( "alpha"
						, ( writer.cast< Float >( sampleNumber ) + 0.5_f ) * ( 1.0_f / writer.cast< Float >( sampleCount ) ) );
					auto angle = writer.declLocale( "angle"
						, alpha * ( 6.28_f * writer.cast< Float >( c3d_ssaoConfigData.numSpiralTurns ) ) + spinAngle );

					ssRadius = alpha;
					writer.returnStmt( vec2( cos( angle ), sin( angle ) ) );
				}
				, InInt{ writer, "sampleNumber" }
				, InInt{ writer, "sampleCount" }
				, InFloat{ writer, "spinAngle" }
				, OutFloat{ writer, "ssRadius" } );


			// Used for packing Z into the GB channels
			auto csZToKey = writer.implementFunction< Float >( "csZToKey"
				, [&]( Float const & csZ )
				{
					writer.returnStmt( clamp( csZ * ( 1.0_f / c3d_ssaoConfigData.farPlaneZ ), 0.0_f, 1.0_f ) );
				}
				, InFloat{ writer, "csZ" } );

			// Read the camera-space position of the point at screen-space pixel ssPosition
			auto getPosition = writer.implementFunction< Vec3 >( "getPosition"
				, [&]( IVec2 const & ssPosition )
				{
					auto position = writer.declLocale< Vec3 >( "position" );
					position.z() = c3d_mapDepth.fetch( ssPosition, 0_i ).r();

					// Offset to pixel center
					position = reconstructCSPosition( vec2( ssPosition ) + vec2( 0.5_f )
						, position.z()
						, c3d_ssaoConfigData.projInfo );
					writer.returnStmt( position );
				}
				, InIVec2{ writer, "ssPosition" } );

			auto getMipLevel = writer.implementFunction< Int >( "getMipLevel"
				, [&]( Float const & ssRadius )
				{
					// Derivation:
					//  mipLevel = floor(log(ssR / MAX_OFFSET));
					writer.returnStmt( clamp( writer.cast< Int >( floor( log2( ssRadius ) ) ) - c3d_ssaoConfigData.logMaxOffset
						, 0_i
						, c3d_ssaoConfigData.maxMipLevel ) );
				}
				, InFloat{ writer, "ssRadius" } );

			// Read the camera-space position of the point at screen-space pixel ssP + unitOffset * ssR.
			// Assumes length(unitOffset) == 1.
			auto getOffsetPosition = writer.implementFunction< Vec3 >( "getOffsetPosition"
				, [&]( IVec2 const & ssCenter
					, Vec2 const & unitOffset
					, Float const & ssRadius
					, Float const & invCszBufferScale )
				{
					// Derivation:
					auto mipLevel = writer.declLocale( "mipLevel"
						, getMipLevel( ssRadius ) );
					auto ssPosition = writer.declLocale( "ssPosition"
						, ivec2( ssRadius * unitOffset ) + ssCenter );
					auto position = writer.declLocale< Vec3 >( "position" );

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
				, InIVec2{ writer, "ssCenter" }
				, InVec2{ writer, "unitOffset" }
				, InFloat{ writer, "ssRadius" }
				, InFloat{ writer, "invCszBufferScale" } );

			// Smaller return value = less occlusion
			auto fallOffFunction = writer.implementFunction< Float >( "fallOffFunction"
				, [&]( Float const & vv
					, Float const & vn
					, Float const & epsilon )
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
				, InFloat{ writer, "vv" }
				, InFloat{ writer, "vn" }
				, InFloat{ writer, "epsilon" } );

			// Compute the occlusion due to sample point \a occluder about camera-space point \a csCenter with unit normal \a normal
			auto aoValueFromPositionsAndNormal = writer.implementFunction< Float >( "aoValueFromPositionsAndNormal"
				, [&]( Vec3 const & csCenter
					, Vec3 const & normal
					, Vec3 const & occluder )
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
				, InVec3{ writer, "csCenter" }
				, InVec3{ writer, "normal" }
				, InVec3{ writer, "occluder" } );

			// Compute the occlusion due to sample with index \a tapIndex about the pixel at \a ssCenter that corresponds
			// to camera-space point \a csCenter with unit normal \a normal, using maximum screen-space sampling radius \a ssDiskRadius

			// Note that units of H() in the HPG12 paper are meters, not unitless.
			// The whole falloff/sampling function is therefore unitless.
			// In this implementation, we factor out (9 / radius).

			// Four versions of the falloff function are implemented below."
			auto sampleAO = writer.implementFunction< Vec3 >( "sampleAO"
				, [&]( IVec2 const & ssCenter
					, Vec3 const & csCenter
					, Vec3 const & normal
					, Float const & ssDiskRadius
					, Int const & tapIndex
					, Int const & tapCount
					, Float const & randomPatternRotationAngle
					, Float const & invCszBufferScale
					, Float occlusion )
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
				, InIVec2{ writer, "ssCenter" }
				, InVec3{ writer, "csCenter" }
				, InVec3{ writer, "normal" }
				, InFloat{ writer, "ssDiskRadius" }
				, InInt{ writer, "tapIndex" }
				, InInt{ writer, "tapCount" }
				, InFloat{ writer, "randomPatternRotationAngle" }
				, InFloat{ writer, "invCszBufferScale" }
				, OutFloat{ writer, "occlusion" } );

			auto isOccluded = writer.implementFunction< Float >( "isOccluded"
				, [&]( Vec3 const & csPosition
					, Vec3 const & csCenter )
				{
					writer.returnStmt( 0.0_f );
				}
				, InVec3{ writer, "csPosition" }
				, InVec3{ writer, "csCenter" } );

			auto sampleRay = writer.implementFunction< Vec3 >( "sampleRay"
				, [&]( Vec3 const & csCenter
					, Vec3 const & csRay )
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

					writer.returnStmt( current * ( 1.0_f - ( currentOcclusion / writer.cast< Float >( stepIdx ) ) ) );
				}
				, InVec3{ writer, "csCenter" }
				, InVec3{ writer, "csRay" } );

			auto square = writer.implementFunction< Float >( "square"
				, [&]( Float const & x )
				{
					writer.returnStmt( x * x );
				}
				, InFloat{ writer, "x" } );

			writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
				, FragmentOut out )
				{
					// Pixel being shaded
					auto ssCenter = writer.declLocale( "ssCenter"
						, ivec2( in.fragCoord.xy() ) );

					// World space point being shaded
					auto csCenter = writer.declLocale( "csCenter"
						, getPosition( ssCenter ) );

					bilateralKey = csZToKey( csCenter.z() );

					auto normal = writer.declLocale< Vec3 >( "normal" );

					if ( useNormalsBuffer )
					{
						normal = c3d_mapNormal.fetch( ivec2( in.fragCoord.xy() ), 0_i ).xyz();
						normal = normalize( c3d_gpInfoData.readNormal( normal ) );
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
							pxl_bentNormal.rgb() = normalize( c3d_gpInfoData.writeNormal( normal ) );
							pxl_bentNormal.a() = 0.0f;
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
						pxl_bentNormal.rgb() = c3d_gpInfoData.writeNormal( normal );
						pxl_bentNormal.a() = 0.0f;
						writer.returnStmt();
					}
					FI;

					// Hash function used in the HPG12 AlchemyAO paper
					auto randomPatternRotationAngle = writer.declLocale( "randomPatternRotationAngle"
						, 10.0_f * writer.cast< Float >( ( 3 * ssCenter.x() ) ^ ( ssCenter.y() + ssCenter.x() * ssCenter.y() ) ) );

					auto sum = writer.declLocale( "sum"
						, 0.0_f );
					auto bentNormal = writer.declLocale( "bentNormal"
						, vec3( 0.0_f ) );

					FOR( writer, Int, i, 0, i < c3d_ssaoConfigData.numSamples, ++i )
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
					pxl_bentNormal.xyz() = c3d_gpInfoData.writeNormal( bentNormal );

					auto A = writer.declLocale< Float >( "A" );

					IF( writer, c3d_ssaoConfigData.highQuality )
					{
						A = pow( max( 0.0_f
								, 1.0_f - sqrt( sum * ( 3.0_f / writer.cast< Float >( c3d_ssaoConfigData.numSamples ) ) ) )
							, c3d_ssaoConfigData.intensity );
					}
					ELSE
					{
						A = max( 0.0_f
							, 1.0_f - sum * c3d_ssaoConfigData.intensityDivR6 * ( 5.0_f / writer.cast< Float >( c3d_ssaoConfigData.numSamples ) ) );
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
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		Texture doCreateTexture( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size )
		{
			return Texture{ device
				, handler
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

		crg::rq::Config getConfig( VkExtent2D const & renderSize
			, SsaoConfig const & ssaoConfig
			, ashes::PipelineShaderStageCreateInfoArray const & stages0
			, ashes::PipelineShaderStageCreateInfoArray const & stages1 )
		{
			crg::rq::Config result;
			result.enabled( &ssaoConfig.enabled );
			result.renderSize( renderSize );
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
			, 2u
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
		: vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, prefix + "SsaoRawAO" + ( useNormalsBuffer ? std::string{ "Normals" } : std::string{} )
			, getVertexProgram() }
		, pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, prefix + "SsaoRawAO" + ( useNormalsBuffer ? std::string{ "Normals" } : std::string{} )
			, getPixelProgram( useNormalsBuffer ) }
		, stages{ makeShaderState( device, vertexShader )
			, makeShaderState( device, pixelShader ) }
	{
	}
	
	//*********************************************************************************************

	SsaoRawAOPass::SsaoRawAOPass( crg::FrameGraph & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, crg::FramePass const & previousPass
		, VkExtent2D const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, Texture const & linearisedDepthBuffer
		, Texture const & normals )
		: m_device{ device }
		, m_graph{ graph }
		, m_ssaoConfig{ config }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_size{ size }
		, m_result{ doCreateTexture( graph.getHandler()
			, m_device
			, graph.getName() + "SsaoRawAOResult"
			, SsaoRawAOPass::ResultFormat
			, m_size ) }
		, m_bentNormals{ doCreateTexture( graph.getHandler()
			, m_device
			, graph.getName() + "BentNormals"
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, m_size ) }
		, m_programs{ Program{ device, false, graph.getName() }
			, Program{ device, true, graph.getName() } }
	{
		stepProgressBar( progress, "Creating SSAO raw AO pass" );
		auto & pass = graph.createPass( "SsaoRawAO"
			, [this, progress]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				stepProgressBar( progress, "Initialising SSAO raw AO pass" );
				auto result = std::make_unique< RenderQuad >( pass
					, context
					, graph
					, getConfig( m_size
						, m_ssaoConfig
						, m_programs[0].stages
						, m_programs[1].stages )
					, m_ssaoConfig );
				m_device.renderSystem.getEngine()->registerTimer( graph.getName() + "/SSAO"
					, result->getTimer() );
				return result;
			} );
		m_lastPass = &pass;
		pass.addDependency( previousPass );
		m_ssaoConfigUbo.createPassBinding( pass, SsaoCfgUboIdx );
		m_gpInfoUbo.createPassBinding( pass, GpInfoUboIdx );
		pass.addSampledView( linearisedDepthBuffer.sampledViewId
			, DepthMapIdx
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( normals.sampledViewId
			, NormalMapIdx
			, VK_IMAGE_LAYOUT_UNDEFINED );
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
		, PipelineVisitorBase & visitor )
	{
		visitor.visit( "SSAO Raw AO"
			, getResult()
			, m_graph.getFinalLayout( getResult().sampledViewId ).layout
			, TextureFactors{}.invert( true ) );
		visitor.visit( "SSAO Bent Normals"
			, getBentResult()
			, m_graph.getFinalLayout( getBentResult().sampledViewId ).layout
			, TextureFactors{}.invert( true ) );

		auto index = m_ssaoConfig.useNormalsBuffer
			? 1u
			: 0u;
		visitor.visit( m_programs[index].vertexShader );
		visitor.visit( m_programs[index].pixelShader );
		config.accept( m_programs[index].pixelShader.name, visitor );
	}
}
