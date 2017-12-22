#include "RawSsaoPass.hpp"

#include "LineariseDepthPass.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

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
			UBO_MATRIX( writer );
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = c3d_projection * vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetPixelProgram( Engine & engine
			, SsaoConfig const & config )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();
			auto index = MinTextureIndex;

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

			// Negative, "linear" values in world-space units
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), index++ );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_mapNormal = writer.declSampler< Sampler2D >( cuT( "c3d_mapNormal" ), index++, config.m_useNormalsBuffer );
			auto c3d_readMultiplyFirst = writer.declUniform( cuT( "c3d_readMultiplyFirst" ), config.m_useNormalsBuffer, vec4( 2.0_f ) );
			auto c3d_readAddSecond = writer.declUniform( cuT( "c3d_readAddSecond" ), config.m_useNormalsBuffer, vec4( 1.0_f ) );

			UBO_SSAO_CONFIG( writer );

			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			auto pxl_fragColor = writer.declOutput< Vec3 >( cuT( "pxl_fragColor" ) );

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

		ShaderProgramSPtr doGetProgram( Engine & engine
			, SsaoConfig const & config )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetPixelProgram( engine, config );
			ShaderProgramSPtr program = engine.getShaderProgramCache().getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		RenderPipelineUPtr doCreatePipeline( Engine & engine
			, ShaderProgram & program )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.setCulledFaces( Culling::eNone );
			return engine.getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, BlendState{}
				, MultisampleState{}
				, program
				, PipelineFlags{} );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				sampler->setWrappingMode( TextureUVW::eU, mode );
				sampler->setWrappingMode( TextureUVW::eV, mode );
			}

			return sampler;
		}
	}

	//*********************************************************************************************

	RawSsaoPass::RawSsaoPass( Engine & engine
		, Size const & size
		, SsaoConfig const & config
		, MatrixUbo & matrixUbo
		, SsaoConfigUbo & ssaoConfigUbo )
		: m_engine{ engine }
		, m_matrixUbo{ matrixUbo }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_size{ size }
		, m_result{ engine }
		, m_program{ doGetProgram( engine, config ) }
		, m_pipeline{ doCreatePipeline( engine, *m_program ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "SSAO" ), cuT( "Raw AO" ) ) }
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto sampler = doCreateSampler( m_engine, cuT( "SSAORaw_Result" ), WrapMode::eClampToEdge );
		auto ssaoResult = renderSystem.createTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite );
		ssaoResult->setSource( PxBufferBase::create( m_size
			, PixelFormat::eRGB32F ) );
		m_result.setTexture( ssaoResult );
		m_result.setSampler( sampler );
		m_result.setIndex( MinTextureIndex );
		m_result.initialise();

		m_fbo = renderSystem.createFrameBuffer();
		m_fbo->initialise();
		m_fbo->setClearColour( RgbaColour::fromPredefined(PredefinedRgbaColour::eOpaqueWhite ) );

		m_resultAttach = m_fbo->createAttachment( ssaoResult );
		m_fbo->bind();
		m_fbo->attach( AttachmentPoint::eColour, 0u, m_resultAttach, ssaoResult->getType() );
		m_fbo->setDrawBuffer( m_resultAttach );
		ENSURE( m_fbo->isComplete() );
		m_fbo->unbind();

		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_ssaoConfigUbo.getUbo() );
	}

	RawSsaoPass::~RawSsaoPass()
	{
		m_resultAttach.reset();
		m_fbo->cleanup();
		m_fbo.reset();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_program.reset();
		m_ssaoConfigUbo.getUbo().cleanup();
		m_matrixUbo.getUbo().cleanup();
		m_result.cleanup();
	}

	void RawSsaoPass::compute( TextureUnit const & linearisedDepthBuffer
		, TextureUnit const & normals )
	{
		m_timer->start();
		m_fbo->bind( FrameBufferTarget::eDraw );
		m_fbo->clear( BufferComponent::eColour );
		normals.getTexture()->bind( MinTextureIndex + 1 );
		normals.getSampler()->bind( MinTextureIndex + 1 );
		m_pipeline->apply();
		m_engine.getRenderSystem()->getCurrentContext()->renderTexture( m_size
			, *linearisedDepthBuffer.getTexture()
			, *m_pipeline
			, m_matrixUbo );
		m_fbo->unbind();
		m_timer->stop();
	}
}
