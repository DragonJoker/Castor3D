#include "Castor3D/Render/Ssao/SsaoRawAOPass.hpp"

#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

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

#include <random>

using namespace castor;
using namespace castor3d;

#define C3D_BentNormalsAggregateAll 0

namespace castor3d
{
	namespace
	{
		ShaderPtr doGetVertexProgram( Engine & engine )
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( "position", 0u );

			// Shader outputs
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		enum Idx
		{
			SsaoCfgUboIdx,
			GpInfoUboIdx,
			DepthMapIdx,
			NormalMapIdx,
		};

		ShaderPtr doGetPixelProgram( Engine & engine
			, bool useNormalsBuffer )
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

			auto in = writer.getIn();

			auto pxl_fragColor = writer.declOutput< Vec3 >( "pxl_fragColor", 0u );
			auto pxl_bentNormal = writer.declOutput< Vec4 >( "pxl_bentNormal", 1u );

#define visibility pxl_fragColor.r()
#define bilateralKey pxl_fragColor.g()
#define readNormal( normal ) -( transpose( inverse( c3d_mtxGView ) ) * vec4( normal.xyz(), 1.0 ) ).xyz()
#define writeNormal( normal ) ( transpose( inverse( c3d_mtxInvView ) ) * vec4( -normal.xyz(), 1.0 ) ).xyz()

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
						, alpha * ( 6.28_f * writer.cast< Float >( c3d_numSpiralTurns ) ) + spinAngle );

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
					writer.returnStmt( clamp( csZ * ( 1.0_f / c3d_farPlaneZ ), 0.0_f, 1.0_f ) );
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
						, c3d_projInfo );
					writer.returnStmt( position );
				}
				, InIVec2{ writer, "ssPosition" } );

			auto getMipLevel = writer.implementFunction< Int >( "getMipLevel"
				, [&]( Float const & ssRadius )
				{
					// Derivation:
					//  mipLevel = floor(log(ssR / MAX_OFFSET));
					writer.returnStmt( clamp( writer.cast< Int >( floor( log2( ssRadius ) ) ) - c3d_logMaxOffset
						, 0_i
						, c3d_maxMipLevel ) );
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
						, c3d_projInfo );
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
					IF( writer, c3d_highQuality )
					{
						// Epsilon inside the sqrt for rsqrt operation
						auto f = writer.declLocale( "f"
							, max( 1.0_f - vv * c3d_invRadius2, 0.0_f ) );
						writer.returnStmt( f * max( ( vn - c3d_bias ) * inverseSqrt( epsilon + vv ), 0.0_f ) );
					}
					ELSE
					{
						// Avoid the square root from above.
						//  Assumes the desired result is intensity/radius^6 in main()
						auto f = writer.declLocale( "f"
							, max( c3d_radius2 - vv, 0.0_f ) );
						writer.returnStmt( f * f * f * max( ( vn - c3d_bias ) / ( epsilon + vv ), 0.0_f ) );
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
						, csRay * c3d_bendStepSize );
					auto current = writer.declLocale( "currentRadius"
						, csCenter + step );
					auto currentOcclusion = writer.declLocale( "currentOcclusion"
						, 0.0_f );
					auto stepIdx = writer.declLocale( "stepIdx"
						, 0_i );

					WHILE( writer, stepIdx < c3d_bendStepCount && currentOcclusion == 0.0_f )
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

			writer.implementMain( [&]()
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
						normal = normalize( readNormal( normal ) );
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
							pxl_bentNormal.rgb() = normalize( writeNormal( normal ) );
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
						, c3d_projScale * c3d_radius / csCenter.z() );

					IF( writer, ssDiskRadius <= c3d_minRadius )
					{
						// There is no way to compute AO at this radius
						visibility = 1.0_f;
						pxl_bentNormal.rgb() = writeNormal( normal );
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

					FOR( writer, Int, i, 0, i < c3d_numSamples, ++i )
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
								, c3d_numSamples
								, randomPatternRotationAngle
								, 1.0_f
								, occlusion ) );
						sum += occlusion;
						bentNormal += sampleRay( csCenter, csRay );
					}
					ROF;

					bentNormal = normalize( bentNormal )/* * 0.5_f + 0.5_f*/;
					pxl_bentNormal.xyz() = writeNormal( bentNormal );

					auto A = writer.declLocale< Float >( "A" );

					IF( writer, c3d_highQuality )
					{
						A = pow( max( 0.0_f
								, 1.0_f - sqrt( sum * ( 3.0_f / writer.cast< Float >( c3d_numSamples ) ) ) )
							, c3d_intensity );
					}
					ELSE
					{
						A = max( 0.0_f
							, 1.0_f - sum * c3d_intensityDivR6 * ( 5.0_f / writer.cast< Float >( c3d_numSamples ) ) );
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
						, clamp( ssDiskRadius - c3d_minRadius
							, 0.0_f
							, 1.0_f ) );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::PipelineShaderStageCreateInfoArray doGetProgram( Engine & engine
			, RenderDevice const & device
			, bool useNormalsBuffer
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetPixelProgram( engine, useNormalsBuffer );
			return
			{
				makeShaderState( device, vertexShader ),
				makeShaderState( device, pixelShader ),
			};
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, VkSamplerAddressMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_NEAREST );
				sampler->setMagFilter( VK_FILTER_NEAREST );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, RenderDevice const & device
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, name, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );

			ashes::ImageCreateInfo image
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
					| VK_IMAGE_USAGE_SAMPLED_BIT ),
			};
			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise( device );
			return result;
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, std::vector< std::reference_wrapper< TextureUnit const > > const & textures )
		{
			ashes::VkAttachmentDescriptionArray attaches;
			ashes::VkAttachmentReferenceArray references;
			uint32_t index = 0u;

			for ( auto & texture : textures )
			{
				attaches.push_back(
					{
						0u,
						texture.get().getTexture()->getPixelFormat(),
						VK_SAMPLE_COUNT_1_BIT,
						VK_ATTACHMENT_LOAD_OP_CLEAR,
						VK_ATTACHMENT_STORE_OP_STORE,
						VK_ATTACHMENT_LOAD_OP_DONT_CARE,
						VK_ATTACHMENT_STORE_OP_DONT_CARE,
						VK_IMAGE_LAYOUT_UNDEFINED,
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					} );
				references.push_back(
					{
						index++,
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					} );
			}

			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					references,
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
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( "SsaoRawAO"
				, std::move( createInfo ) );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, std::vector< std::reference_wrapper< TextureUnit const > > const & textures )
		{
			ashes::ImageViewCRefArray attaches;

			for ( auto & texture : textures )
			{
				attaches.emplace_back( texture.get().getTexture()->getDefaultView().getTargetView() );
			}

			auto size = textures.front().get().getTexture()->getDimensions();
			auto result = renderPass.createFrameBuffer( "SsaoRawAO"
				, VkExtent2D{ size.width, size.height }
				, std::move( attaches ) );
			return result;
		}

		rq::BindingDescriptionArray createBindings( bool normals )
		{
			rq::BindingDescriptionArray result
			{
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ashes::nullopt },	// SsaoCfg UBO
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ashes::nullopt },	// GpInfo UBO
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D },	// Depth Map
			};

			if ( normals )
			{
				result.push_back( { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_IMAGE_VIEW_TYPE_2D } );	// Normal Map
			}

			return result;
		}
	}

	//*********************************************************************************************

	SsaoRawAOPass::RenderQuad::RenderQuad( Engine & engine
		, RenderDevice const & device
		, ashes::RenderPass const & renderPass
		, VkExtent2D const & size
		, SsaoConfigUbo & ssaoConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, TextureUnit const & depth
		, ashes::ImageView const * normals )
		: castor3d::RenderQuad{ device
			, cuT( "SsaoRawAO" )
			, VK_FILTER_NEAREST
			, { createBindings( normals )
				, ( normals
					? ( *normals )->subresourceRange
					: depth.getTexture()->getDefaultView().getSampledView()->subresourceRange )
				, ashes::nullopt } }
		, vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName() }
		, pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName() }
		, m_depthView{ normals ? &depth.getTexture()->getDefaultView().getSampledView() : nullptr }
		, m_depthSampler{ normals ? depth.getSampler() : nullptr }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_gpInfoUbo{ gpInfoUbo }
	{
		m_sampler->initialise( m_device );
		ashes::WriteDescriptorSetArray bindings
		{
			makeDescriptorWrite( m_ssaoConfigUbo.getUbo(), SsaoCfgUboIdx ),
			makeDescriptorWrite( m_gpInfoUbo.getUbo(), GpInfoUboIdx ),
			makeDescriptorWrite( depth.getTexture()->getDefaultView().getTargetView(), m_sampler->getSampler(), DepthMapIdx ),
		};

		if ( normals )
		{
			bindings.emplace_back( makeDescriptorWrite( *normals, m_sampler->getSampler(), NormalMapIdx ) );
		}

		createPipelineAndPass( size
			, {}
			, doGetProgram( engine, device, normals != nullptr, vertexShader, pixelShader )
			, renderPass
			, bindings );
	}
	
	//*********************************************************************************************

	SsaoRawAOPass::SsaoRawAOPass( Engine & engine
		, RenderDevice const & device
		, VkExtent2D const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, GpInfoUbo const & gpInfoUbo
		, TextureUnit const & linearisedDepthBuffer
		, ashes::ImageView const & normals )
		: m_engine{ engine }
		, m_device{ device }
		, m_ssaoConfig{ config }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_linearisedDepthBuffer{ linearisedDepthBuffer }
		, m_normals{ normals }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine
			, m_device
			, "SsaoRawAOResult"
			, SsaoRawAOPass::ResultFormat
			, m_size ) }
		, m_bentNormals{ doCreateTexture( m_engine
			, m_device
			, "BentNormals"
			, VK_FORMAT_R32G32B32A32_SFLOAT
			, m_size ) }
		, m_renderPass{ doCreateRenderPass( m_device, { m_result, m_bentNormals } ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, { m_result, m_bentNormals } ) }
		, m_quads{ RenderQuad{ m_engine
				, m_device
				, *m_renderPass
				, m_size
				, m_ssaoConfigUbo
				, m_gpInfoUbo
				, m_linearisedDepthBuffer
				, nullptr }
			, RenderQuad{ m_engine
				, m_device
				, *m_renderPass
				, m_size
				, m_ssaoConfigUbo
				, m_gpInfoUbo
				, m_linearisedDepthBuffer
				, &m_normals } }
		, m_commandBuffers{ m_device.graphicsCommandPool->createCommandBuffer( "SsaoRawAO" )
			, m_device.graphicsCommandPool->createCommandBuffer( "SsaoRawAONormals" ) }
		, m_finished{ m_device->createSemaphore( "SsaoRawAO" ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_device, cuT( "Scalable Ambient Obscurance" ), cuT( "Raw AO" ) ) }
	{
		for ( auto i = 0u; i < m_commandBuffers.size(); ++i )
		{
			auto & cmd = *m_commandBuffers[i];
			auto & quad = m_quads[i];

			cmd.begin();
			cmd.beginDebugBlock(
				{
					"SSAO - Raw AO",
					makeFloatArray( m_engine.getNextRainbowColour() ),
				} );
			m_timer->beginPass( cmd );
			cmd.beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, { opaqueWhiteClearColor, opaqueWhiteClearColor }
				, VK_SUBPASS_CONTENTS_INLINE );
			quad.registerPass( cmd );
			cmd.endRenderPass();
			m_timer->endPass( cmd );
			cmd.endDebugBlock();
			cmd.end();
		}
	}

	SsaoRawAOPass::~SsaoRawAOPass()
	{
		for ( auto & quad : m_quads )
		{
			quad.cleanup();
		}

		m_bentNormals.cleanup();
		m_result.cleanup();
	}

	ashes::Semaphore const & SsaoRawAOPass::compute( ashes::Semaphore const & toWait )const
	{
		RenderPassTimerBlock timerBlock{ m_timer->start() };
		timerBlock->notifyPassRender();
		auto * result = &toWait;
		auto index = m_ssaoConfig.useNormalsBuffer
			? 1u
			: 0u;
		m_device.graphicsQueue->submit( *m_commandBuffers[index]
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
		result = m_finished.get();
		return *result;
	}

	void SsaoRawAOPass::accept( SsaoConfig & config
		, PipelineVisitorBase & visitor )
	{
		if ( getResult().isTextured() )
		{
			visitor.visit( "SSAO Raw AO"
				, getResult().getTexture()->getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		if ( getBentResult().isTextured() )
		{
			visitor.visit( "SSAO Bent Normals"
				, getBentResult().getTexture()->getDefaultView().getSampledView()
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		}

		auto index = m_ssaoConfig.useNormalsBuffer
			? 1u
			: 0u;
		visitor.visit( m_quads[index].vertexShader );
		visitor.visit( m_quads[index].pixelShader );
		config.accept( m_quads[index].pixelShader.name, visitor );
	}
}
