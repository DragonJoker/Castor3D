#include "RawSsaoPass.hpp"

#include "LineariseDepthPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Command/Queue.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Pipeline/ShaderStageState.hpp>
#include <Pipeline/VertexInputState.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_Position() = writer.rendererScalePosition( vec4( position, 0.0, 1.0 ) );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetPixelProgram( Engine & engine
			, SsaoConfig const & config )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();
			auto index = MinBufferIndex;

			// If using depth mip levels, the log of the maximum pixel offset before we need to switch to a lower
			// miplevel to maintain reasonable spatial locality in the cache
			// If this number is too small (< 3), too many taps will land in the same pixel, and we'll get bad variance that manifests as flashing.
			// If it is too high (> 5), we'll get bad performance because we're not using the MIP levels effectively
			auto const LOG_MAX_OFFSET = writer.declConstant( cuT( "LOG_MAX_OFFSET" ), 3_i );

			// This must be less than or equal to the MAX_MIP_LEVEL defined in SSAO.cpp
			auto const MAX_MIP_LEVEL = writer.declConstant( cuT( "MAX_MIP_LEVEL" ), Int{ int( LineariseDepthPass::MaxMipLevel ) } );

			// pixels
			auto const MIN_RADIUS = writer.declConstant( cuT( "MIN_RADIUS" ), 3.0_f );

			auto const VARIATION = writer.declConstant( cuT( "VARIATION" ), 0_i );

			//////////////////////////////////////////////////

			UBO_SSAO_CONFIG( writer, 0u, 0u );
			// Negative, "linear" values in world-space units
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), 1u, 0u );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_mapNormal = writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" ), 2u, 0u, config.m_useNormalsBuffer );
			auto c3d_readMultiplyFirst = writer.declConstant( cuT( "c3d_readMultiplyFirst" ), vec4( 2.0_f ), config.m_useNormalsBuffer );
			auto c3d_readAddSecond = writer.declConstant( cuT( "c3d_readAddSecond" ), vec4( 1.0_f ), config.m_useNormalsBuffer );

			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			auto pxl_fragColor = writer.declOutput< Vec3 >( cuT( "pxl_fragColor" ), 0u );

#define visibility      pxl_fragColor.r()
#define bilateralKey    pxl_fragColor.g()

			//////////////////////////////////////////////////

			// Reconstruct camera-space P.xyz from screen-space S = (x, y) in
			// pixels and camera-space z < 0.  Assumes that the upper-left pixel center
			// is at (0.5, 0.5) [but that need not be the location at which the sample tap
			// was placed!]
			// Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
			auto reconstructCSPosition = writer.implementFunction< Vec3 >( cuT( "reconstructCSPosition" )
				, [&]( Vec2 const & S
					, Float const & z
					, Vec4 const & projInfo )
				{
					writer.returnStmt( vec3( glsl::fma( S.xy(), projInfo.xy(), projInfo.zw() ) * z, z ) );
				}
				, InVec2{ &writer, cuT( "S" ) }
				, InFloat{ &writer, cuT( "z" ) }
				, InVec4{ &writer, cuT( "projInfo" ) } );

			// Reconstructs camera-space unit normal from camera-space position
			auto reconstructCSFaceNormal = writer.implementFunction< Vec3 >( cuT( "reconstructCSFaceNormal" )
				, [&]( Vec3 const & vsPosition )
				{
					writer.returnStmt( normalize( cross( dFdy( vsPosition ), dFdx( vsPosition ) ) ) );
				}
				, InVec3{ &writer, cuT( "vsPosition" ) } );

			// Reconstructs camera-space normal from camera-space position
			auto reconstructNonUnitCSFaceNormal = writer.implementFunction< Vec3 >( cuT( "reconstructNonUnitCSFaceNormal" )
				, [&]( Vec3 const & vsPosition )
				{
					writer.returnStmt( cross( dFdy( vsPosition ), dFdx( vsPosition ) ) );
				}
				, InVec3{ &writer, cuT( "vsPosition" ) } );

			// Returns a unit vector and a screen-space radius for the tap on a unit disk (the caller should scale by the actual disk radius)
			auto tapLocation = writer.implementFunction< Vec2 >( cuT( "tapLocation" )
				, [&]( Int const & sampleNumber
					, Float const & spinAngle
					, Float ssRadius )
				{
					// Radius relative to ssR
					auto alpha = writer.declLocale( cuT( "alpha" )
						, writer.cast< Float >( sampleNumber + 0.5_f ) * writer.paren( 1.0_f / c3d_numSamples ) );
					auto angle = writer.declLocale( cuT( "angle" )
						, alpha * writer.paren( 6.28_f * c3d_numSpiralTurns ) + spinAngle );

					ssRadius = alpha;
					writer.returnStmt( vec2( cos( angle ), sin( angle ) ) );
				}
				, InInt{ &writer, cuT( "sampleNumber" ) }
				, InFloat{ &writer, cuT( "spinAngle" ) }
				, OutFloat{ &writer, cuT( "ssRadius" ) } );


			// Used for packing Z into the GB channels
			auto csZToKey = writer.implementFunction< Float >( cuT( "csZToKey" )
				, [&]( Float const & z )
				{
					writer.returnStmt( clamp( z * writer.paren( 1.0_f / c3d_farPlaneZ ), 0.0_f, 1.0_f ) );
				}
				, InFloat{ &writer, cuT( "z" ) } );

			// Read the camera-space position of the point at screen-space pixel ssPosition
			auto getPosition = writer.implementFunction< Vec3 >( cuT( "getPosition" )
				, [&]( IVec2 const & ssPosition )
				{
					auto position = writer.declLocale< Vec3 >( cuT( "position" ) );
					position.z() = texelFetch( c3d_mapDepth, ssPosition, 0 ).r();

					// Offset to pixel center
					position = reconstructCSPosition( vec2( ssPosition ) + vec2( 0.5_f )
						, position.z()
						, c3d_projInfo );
					writer.returnStmt( position );
				}
				, InIVec2{ &writer, cuT( "ssPosition" ) } );

			auto getMipLevel = writer.implementFunction< Int >( cuT( "getMipLevel" )
				, [&]( Float const & ssRadius )
				{
					// Derivation:
					writer.returnStmt( clamp( writer.cast< Int >( floor( log2( ssRadius ) ) ) - LOG_MAX_OFFSET
						, 0_i
						, MAX_MIP_LEVEL ) );
				}
				, InFloat{ &writer, cuT( "ssRadius" ) } );

			// Read the camera-space position of the point at screen-space pixel ssP + unitOffset * ssR.
			// Assumes length(unitOffset) == 1
			auto getOffsetPosition = writer.implementFunction< Vec3 >( cuT( "getOffsetPosition" )
				, [&]( IVec2 const & ssCenter
					, Vec2 const & unitOffset
					, Float const & ssRadius
					, Float const & invCszBufferScale )
				{
					// Derivation:
					auto mipLevel = writer.declLocale( cuT( "mipLevel" )
						, getMipLevel( ssRadius ) );
					auto ssPosition = writer.declLocale( cuT( "ssPosition" )
						, ivec2( ssRadius * unitOffset ) + ssCenter );
					auto position = writer.declLocale< Vec3 >( cuT( "position" ) );

					// We need to divide by 2^mipLevel to read the appropriately scaled coordinate from a MIP-map.
					// Manually clamp to the texture size because texelFetch bypasses the texture unit
					auto mipPosition = writer.declLocale( cuT( "mipPosition" )
						, clamp( ivec2( ssPosition.x() >> mipLevel, ssPosition.y() >> mipLevel )
							, ivec2( 0_i )
							, textureSize( c3d_mapDepth, mipLevel ) - ivec2( 1_i ) ) );
					position.z() = texelFetch( c3d_mapDepth, mipPosition, mipLevel ).r();

					// Offset to pixel center
					position = reconstructCSPosition( writer.paren( vec2( ssPosition ) + vec2( 0.5_f ) ) * invCszBufferScale
						, position.z()
						, c3d_projInfo );
					writer.returnStmt( position );
				}
				, InIVec2{ &writer, cuT( "ssCenter" ) }
				, InVec2{ &writer, cuT( "unitOffset" ) }
				, InFloat{ &writer, cuT( "ssRadius" ) }
				, InFloat{ &writer, cuT( "invCszBufferScale" ) } );

			// Read the camera-space position of the points at screen-space pixel ssP + unitOffset * ssR in both channels of the packed csz buffer.
			// Assumes length(unitOffset) == 1
			auto getOffsetPositions = writer.implementFunction< Void >( cuT( "getOffsetPositions" )
				, [&]( IVec2 const & ssCenter
					, Vec2 const & unitOffset
					, Float const & ssRadius
					, Vec3 pos0
					, Vec3 pos1 )
				{
					// Derivation:
					auto mipLevel = writer.declLocale( cuT( "mipLevel" )
						, getMipLevel( ssRadius ) );
					auto ssPosition = writer.declLocale( cuT( "ssPosition" )
						, ivec2( ssRadius * unitOffset ) + ssCenter );

					// We need to divide by 2^mipLevel to read the appropriately scaled coordinate from a MIP-map.
					// Manually clamp to the texture size because texelFetch bypasses the texture unit
					auto mipPosition = writer.declLocale( cuT( "mipPosition" )
						, clamp( ivec2( ssPosition.x() >> mipLevel, ssPosition.y() >> mipLevel )
							, ivec2( 0_i )
							, textureSize( c3d_mapDepth, mipLevel ) - ivec2( 1_i ) ) );
					auto depths = writer.declLocale( cuT( "depths" )
						, texelFetch( c3d_mapDepth, mipPosition, mipLevel ).rg() );

					// Offset to pixel center
					pos0 = reconstructCSPosition( vec2( ssPosition ) + vec2( 0.5_f )
						, depths.x()
						, c3d_projInfo );
					pos1 = reconstructCSPosition( vec2( ssPosition ) + vec2( 0.5_f )
						, depths.y()
						, c3d_projInfo );
				}
				, InIVec2{ &writer, cuT( "ssCenter" ) }
				, InVec2{ &writer, cuT( "unitOffset" ) }
				, InFloat{ &writer, cuT( "ssRadius" ) }
				, OutVec3{ &writer, cuT( "pos0" ) }
				, OutVec3{ &writer, cuT( "pos1" ) } );

			auto fallOffFunction = writer.implementFunction< Float >( cuT( "fallOffFunction" )
				, [&]( Float const & vv
					, Float const & vn
					, Float const & epsilon )
				{
					// A: From the HPG12 paper
					// Note large epsilon to avoid overdarkening within cracks
					//  Assumes the desired result is intensity/radius^6 in main()
					// return float(vv < radius2) * max((vn - bias) / (epsilon + vv), 0.0) * radius2 * 0.6;

					// B: Smoother transition to zero (lowers contrast, smoothing out corners). [Recommended]
					if ( config.m_highQuality )
					{
						// Epsilon inside the sqrt for rsqrt operation
						auto f = writer.declLocale( cuT( "f" )
							, max( 1.0_f - vv * c3d_invRadius2, 0.0_f ) );
						writer.returnStmt( f * max( writer.paren( vn - c3d_bias ) * inversesqrt( epsilon + vv ), 0.0_f ) );
					}
					else
					{
						// Avoid the square root from above.
						//  Assumes the desired result is intensity/radius^6 in main()
						auto f = writer.declLocale( cuT( "f" )
							, max( c3d_radius2 - vv, 0.0_f ) );
						writer.returnStmt( f * f * f * max( writer.paren( vn - c3d_bias ) / writer.paren( epsilon + vv ), 0.0_f ) );
					}

					// C: Medium contrast (which looks better at high radii), no division.  Note that the 
					// contribution still falls off with radius^2, but we've adjusted the rate in a way that is
					// more computationally efficient and happens to be aesthetically pleasing.  Assumes 
					// division by radius^6 in main()
					// return 4.0 * max(1.0 - vv * invRadius2, 0.0) * max(vn - bias, 0.0);

					// D: Low contrast, no division operation
					//return 2.0 * float(vv < radius * radius) * max(vn - bias, 0.0);
				}
				, InFloat{ &writer, cuT( "vv" ) }
				, InFloat{ &writer, cuT( "vn" ) }
				, InFloat{ &writer, cuT( "epsilon" ) } );

			// Compute the occlusion due to sample point \a Q about camera-space point \a vsPosition with unit normal \a normal
			auto aoValueFromPositionsAndNormal = writer.implementFunction< Float >( cuT( "aoValueFromPositionsAndNormal" )
				, [&]( Vec3 const & vsPosition
					, Vec3 const & normal
					, Vec3 const & occluder )
				{
					auto v = writer.declLocale( cuT( "v" )
						, occluder - vsPosition );
					auto vv = writer.declLocale( cuT( "vv" )
						, dot( v, v ) );
					auto vn = writer.declLocale( cuT( "vn" )
						, dot( v, normal ) );
					auto const epsilon = writer.declLocale( cuT( "epsilon" )
						, 0.001_f );

					// Without the angular adjustment term, surfaces seen head on have less AO
					writer.returnStmt( fallOffFunction( vv, vn, epsilon ) * mix( 1.0_f, max( 0.0_f, 1.5_f * normal.z() ), 0.35_f ) );
				}
				, InVec3{ &writer, cuT( "vsPosition" ) }
				, InVec3{ &writer, cuT( "normal" ) }
				, InVec3{ &writer, cuT( "occluder" ) } );

			// Compute the occlusion due to sample with index \a tapIndex about the pixel at \a ssCenter that corresponds
			// to camera-space point \a vsPosition with unit normal \a normal, using maximum screen-space sampling radius \a ssDiskRadius

			// Note that units of H() in the HPG12 paper are meters, not unitless.
			// The whole falloff/sampling function is therefore unitless.
			// In this implementation, we factor out (9 / radius).

			// Four versions of the falloff function are implemented below."
			auto sampleAO = writer.implementFunction< Float >( cuT( "sampleAO" )
				, [&]( IVec2 const & ssCenter
					, Vec3 const & vsPosition
					, Vec3 const & normal
					, Float const & ssDiskRadius
					, Int const & tapIndex
					, Float const & randomPatternRotationAngle
					, Float const & invCszBufferScale )
				{
					// Offset on the unit disk, spun for this pixel
					auto ssRadius = writer.declLocale< Float >( cuT( "ssRadius" ) );
					auto unitOffset = writer.declLocale( cuT( "unitOffset" )
						, tapLocation( tapIndex, randomPatternRotationAngle, ssRadius ) );

					// Ensure that the taps are at least 1 pixel away
					ssRadius = max( 0.75_f, ssRadius * ssDiskRadius );

					// The occluding point in camera space
					auto occluder = writer.declLocale( cuT( "occluder" )
						, getOffsetPosition( ssCenter
							, unitOffset
							, ssRadius
							, invCszBufferScale ) );

					writer.returnStmt( aoValueFromPositionsAndNormal( vsPosition, normal, occluder ) );
				}
				, InIVec2{ &writer, cuT( "ssCenter" ) }
				, InVec3{ &writer, cuT( "vsPosition" ) }
				, InVec3{ &writer, cuT( "normal" ) }
				, InFloat{ &writer, cuT( "ssDiskRadius" ) }
				, InInt{ &writer, cuT( "tapIndex" ) }
				, InFloat{ &writer, cuT( "randomPatternRotationAngle" ) }
				, InFloat{ &writer, cuT( "invCszBufferScale" ) } );

			auto square = writer.implementFunction< Float >( cuT( "square" )
				, [&]( Float const & x )
				{
					writer.returnStmt( x * x );
				}
				, InFloat{ &writer, cuT( "x") } );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					// Pixel being shaded
					auto ssCenter = writer.declLocale( cuT( "ssCenter" )
						, ivec2( gl_FragCoord.xy() ) );

					// Camera space point being shaded
					auto vsPosition = writer.declLocale( cuT( "vsPosition" )
						, getPosition( ssCenter ) );

					bilateralKey = csZToKey( vsPosition.z() );

					if ( config.m_useNormalsBuffer )
					{
						auto normal = writer.declLocale( cuT( "normal" )
							, texelFetch( c3d_mapNormal, ivec2( gl_FragCoord.xy() ), 0 ).xyz() );
						normal = writer.paren( c3d_worldToCamera * vec4( normal, 1.0_f ) ).xyz();
						normal = normalize( glsl::fma( normal, c3d_readMultiplyFirst.xyz(), c3d_readAddSecond.xyz() ) );
					}
					else
					{
						// Reconstruct normals from positions.
						auto normal = writer.declLocale( cuT( "normal" )
							, reconstructNonUnitCSFaceNormal( vsPosition ) );

						// Since normal is computed from the cross product of camera-space edge vectors from points at adjacent pixels,
						// its magnitude will be proportional to the square of distance from the camera
						IF( writer, dot( normal, normal ) > writer.paren( square( vsPosition.z() * vsPosition.z() * 0.00006 ) ) )
						{
							// if the threshold is too big you will see black dots where we used a bad normal at edges, too small -> white
							// The normals from depth should be very small values before normalization,
							// except at depth discontinuities, where they will be large and lead
							// to 1-pixel false occlusions because they are not reliable
							visibility = 1.0_f;
							writer.returnStmt();
						}
						ELSE
						{
							normal = normalize( normal );
						}
						FI;
					}

					auto normal = writer.declBuiltin< Vec3 >( cuT( "normal" ) );

					// Choose the screen-space sample radius
					// proportional to the projected area of the sphere
					auto ssDiskRadius = writer.declLocale( cuT( "ssDiskRadius" )
						, c3d_projScale * c3d_radius / vsPosition.z() );

					IF( writer, ssDiskRadius <= MIN_RADIUS )
					{
						// There is no way to compute AO at this radius
						visibility = 1.0_f;
						writer.returnStmt();
					}
					FI;

					// Hash function used in the HPG12 AlchemyAO paper
					auto randomPatternRotationAngle = writer.declLocale( cuT( "randomPatternRotationAngle" )
						, 10.0_f * writer.paren( writer.paren( 3 * ssCenter.x() ) ^ writer.paren( ssCenter.y() + ssCenter.x() * ssCenter.y() ) ) );

					auto sum = writer.declLocale( cuT( "sum" )
						, 0.0_f );

					FOR( writer, Int, i, 0, "i < c3d_numSamples", "++i" )
					{
						sum += sampleAO( ssCenter
							, vsPosition
							, normal
							, ssDiskRadius
							, i
							, randomPatternRotationAngle
							, 1.0_f );
					}
					ROF;

					if ( config.m_highQuality )
					{
						auto A = writer.declLocale( cuT( "A" )
							, pow( max( 0.0_f
									, 1.0_f - sqrt( sum * writer.paren( 3.0_f / c3d_numSamples ) ) )
								, c3d_intensity ) );
					}
					else
					{
						auto A = writer.declLocale( cuT( "A" )
							, max( 0.0_f
								, 1.0_f - sum * c3d_intensityDivR6 * writer.paren( 5.0_f / c3d_numSamples ) ) );
						// Anti-tone map to reduce contrast and drag dark region farther
						// (x^0.2 + 1.2 * x^4)/2.2
						A = writer.paren( pow( A, 0.2_f ) + 1.2_f * A * A * A * A ) / 2.2_f;
					}

					auto A = writer.declBuiltin< Float >( cuT( "A" ) );
					// Visualize random spin distribution
					//A = mod(randomPatternRotationAngle / (2 * 3.141592653589), 1.0);

					// Fade in as the radius reaches 2 pixels
					visibility = mix( 1.0_f
						, A
						, clamp( ssDiskRadius - MIN_RADIUS
							, 0.0_f
							, 1.0_f ) );
				} );
			return writer.finalise();
		}

		renderer::ShaderStageStateArray doGetProgram( Engine & engine
			, SsaoConfig const & config
			, glsl::Shader & vertexShader
			, glsl::Shader & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader = doGetVertexProgram( engine );
			pixelShader = doGetPixelProgram( engine, config );
			renderer::ShaderStageStateArray program
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( vertexShader.getSource() );
			program[1].module->loadShader( pixelShader.getSource() );
			return program;
		}

		renderer::PipelinePtr doCreatePipeline( renderer::PipelineLayout const & layout
			, renderer::ShaderStageStateArray program
			, renderer::RenderPass const & renderPass
			, renderer::VertexLayout const & vertexLayout
			, renderer::Extent2D const & size )
		{
			return layout.createPipeline(
			{
				program,
				renderPass,
				renderer::VertexInputState::create( vertexLayout ),
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
				renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eNone },
				renderer::MultisampleState{},
				renderer::ColourBlendState::createDefault(),
				{},
				renderer::DepthStencilState{ 0u, false, false, },
				std::nullopt,
				renderer::Viewport{ size.width, size.height, 0, 0 },
				renderer::Scissor{ 0, 0, size.width, size.height }
			} );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, renderer::WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eNearest );
				sampler->setMagFilter( renderer::Filter::eNearest );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine, renderer::Extent2D const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, cuT( "SSAORaw_Result" ), renderer::WrapMode::eClampToEdge );

			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = renderer::Format::eR32G32B32A32_SFLOAT;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;

			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}
		
		renderer::RenderPassPtr doCreateRenderPass( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );

			// Create the render pass.
			renderer::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = renderer::Format::eR32G32B32A32_SFLOAT;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() ), texture.getTexture()->getDefaultView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( renderer::Extent2D{ size.width, size.height }
				, std::move( attaches ) );
		}

		renderer::VertexBufferPtr< NonTexturedQuad > doCreateVertexBuffer( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			auto result = renderer::makeVertexBuffer< NonTexturedQuad >( device
				, 1u
				, 0u
				, renderer::MemoryPropertyFlag::eHostVisible );

			if ( auto buffer = result->lock( 0u
				, 1u
				, renderer::MemoryMapFlag::eInvalidateRange | renderer::MemoryMapFlag::eWrite ) )
			{
				*buffer = NonTexturedQuad
				{
					{
						{ Point2f{ -1.0, -1.0 } },
						{ Point2f{ -1.0, +1.0 } },
						{ Point2f{ +1.0, -1.0 } },
						{ Point2f{ +1.0, -1.0 } },
						{ Point2f{ -1.0, +1.0 } },
						{ Point2f{ +1.0, +1.0 } },
					}
				};
				result->flush( 0u, 1u );
				result->unlock();
			}

			return result;
		}

		renderer::VertexLayoutPtr doCreateVertexLayout( Engine & engine )
		{
			auto result = renderer::makeLayout< NonTexturedQuad::Vertex >( 0u );
			result->createAttribute( 0u, renderer::Format::eR32G32_SFLOAT, offsetof( NonTexturedQuad::Vertex, position ) );
			return result;
		}

		renderer::DescriptorSetLayoutPtr doCreateDescriptorLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			renderer::DescriptorSetLayoutBindingArray bindings
			{
				{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eFragment },
				{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
				{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			};
			return device.createDescriptorSetLayout( std::move( bindings ) );
		}

		renderer::DescriptorSetPtr doCreateDescriptor( renderer::DescriptorSetPool & pool
			, renderer::DescriptorSetLayout const & layout
			, SsaoConfigUbo & ssaoConfigUbo
			, TextureUnit const & linearisedDepthBuffer
			, renderer::TextureView const & normals
			, renderer::Sampler const & sampler )
		{
			auto result = pool.createDescriptorSet();
			result->createBinding( layout.getBinding( 0u )
				, ssaoConfigUbo.getUbo()
				, 0u
				, 1u );
			result->createBinding( layout.getBinding( 1u )
				, linearisedDepthBuffer.getTexture()->getDefaultView()
				, linearisedDepthBuffer.getSampler()->getSampler() );
			result->createBinding( layout.getBinding( 2u )
				, normals
				, sampler );
			result->update();
			return result;
		}
	}

	//*********************************************************************************************

	RawSsaoPass::RawSsaoPass( Engine & engine
		, renderer::Extent2D const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, TextureUnit const & linearisedDepthBuffer
		, renderer::TextureView const & normals )
		: m_engine{ engine }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_linearisedDepthBuffer{ linearisedDepthBuffer }
		, m_normals{ normals }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_program{ doGetProgram( m_engine, config, m_vertexShader, m_pixelShader ) }
		, m_sampler{ getCurrentDevice( m_engine ).createSampler( renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::Filter::eNearest
			, renderer::Filter::eNearest ) }
		, m_descriptorLayout{ doCreateDescriptorLayout( m_engine ) }
		, m_descriptorPool{ m_descriptorLayout->createPool( 1u ) }
		, m_descriptor{ doCreateDescriptor( *m_descriptorPool, *m_descriptorLayout, ssaoConfigUbo, linearisedDepthBuffer, normals, *m_sampler ) }
		, m_pipelineLayout{ getCurrentDevice( m_engine ).createPipelineLayout( *m_descriptorLayout ) }
		, m_renderPass{ doCreateRenderPass( m_engine ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_result ) }
		, m_vertexBuffer{ doCreateVertexBuffer( m_engine ) }
		, m_vertexLayout{ doCreateVertexLayout( m_engine ) }
		, m_pipeline{ doCreatePipeline( *m_pipelineLayout, m_program, *m_renderPass, *m_vertexLayout, size ) }
		, m_commandBuffer{ getCurrentDevice( m_engine ).getGraphicsCommandPool().createCommandBuffer() }
		, m_finished{ getCurrentDevice( m_engine ).createSemaphore() }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, cuT( "SSAO" ), cuT( "Raw AO" ) ) }
	{
		static renderer::ClearColorValue const colour{ 1.0, 1.0, 1.0, 1.0 };

		m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse );
		m_timer->beginPass( *m_commandBuffer );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { colour }
			, renderer::SubpassContents::eInline );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->bindDescriptorSet( *m_descriptor, *m_pipelineLayout );
		m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		m_commandBuffer->draw( 6u );
		m_commandBuffer->endRenderPass();
		m_timer->endPass( *m_commandBuffer );
		m_commandBuffer->end();
	}

	RawSsaoPass::~RawSsaoPass()
	{
		m_timer.reset();
		m_commandBuffer.reset();
		m_pipeline.reset();
		m_pipelineLayout.reset();
		m_sampler.reset();
		m_vertexLayout.reset();
		m_vertexBuffer.reset();
		m_frameBuffer.reset();
		m_renderPass.reset();
		m_result.cleanup();
	}

	renderer::Semaphore const & RawSsaoPass::compute( renderer::Semaphore const & toWait )const
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto * result = &toWait;

		device.getGraphicsQueue().submit( *m_commandBuffer
			, toWait
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	void RawSsaoPass::accept( SsaoConfig & config
		, RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "SSAO - Raw" )
			, renderer::ShaderStageFlag::eVertex
			, m_vertexShader );
		visitor.visit( cuT( "SSAO - Raw" )
			, renderer::ShaderStageFlag::eFragment
			, m_pixelShader );
		config.accept( cuT( "SSAO - Raw" ), visitor );
	}
}
