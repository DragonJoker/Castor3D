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
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
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
			UBO_GPINFO( writer );
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

			// Total number of direct samples to take at each pixel
			auto NUM_SAMPLES = writer.declConstant( cuT( "NUM_SAMPLES" ), 256_i );

			// If using depth mip levels, the log of the maximum pixel offset before we need to switch to a lower
			// miplevel to maintain reasonable spatial locality in the cache
			// If this number is too small (< 3), too many taps will land in the same pixel, and we'll get bad variance that manifests as flashing.
			// If it is too high (> 5), we'll get bad performance because we're not using the MIP levels effectively
			auto LOG_MAX_OFFSET = writer.declConstant( cuT( "LOG_MAX_OFFSET" ), 3_i );

			// This must be less than or equal to the MAX_MIP_LEVEL defined in SSAO.cpp
			auto MAX_MIP_LEVEL = writer.declConstant( cuT( "MAX_MIP_LEVEL" ), Int{ int( LineariseDepthPass::MaxMipLevel ) } );

			// Used for preventing AO computation on the sky (at infinite depth) and defining the CS Z to bilateral depth key scaling.
			// This need not match the real far plane.
			auto FAR_PLANE_Z = writer.declConstant( cuT( "FAR_PLANE_Z" ), -300.0_f );

			// This is the number of turns around the circle that the spiral pattern makes.
			// This should be prime to prevent taps from lining up.
			// This particular choice was tuned for numSamples == 9
			auto NUM_SPIRAL_TURNS = writer.declConstant( cuT( "NUM_SPIRAL_TURNS" ), 10_i );

			//////////////////////////////////////////////////

			// Negative, \"linear\" values in world-space units
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), index++ );

			UBO_SSAO_CONFIG( writer );

			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			auto pxl_fragColor = writer.declOutput< Vec3 >( cuT( "pxl_fragColor" ) );

#define visibility      pxl_fragColor.r()
#define bilateralKey    pxl_fragColor.gb()

			//////////////////////////////////////////////////

			// Reconstruct camera-space P.xyz from screen-space S = (x, y) in
			// pixels and camera-space z < 0.  Assumes that the upper-left pixel center
			// is at (0.5, 0.5) [but that need not be the location at which the sample tap
			// was placed!]
			// Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
			auto reconstructCSPosition = writer.implementFunction< Vec3 >( cuT( "reconstructCSPosition" )
				, [&]( Vec2 const & S
					, Float const & z )
				{
					writer.returnStmt( vec3( glsl::fma( S.xy(), c3d_projInfo.xy(), c3d_projInfo.zw() ) * z, z ) );
				}
				, InVec2{ &writer, cuT( "S" ) }
				, InFloat{ &writer, cuT( "z" ) } );

			// Reconstructs screen-space unit normal from screen-space position
			auto reconstructCSFaceNormal = writer.implementFunction< Vec3 >( cuT( "reconstructCSFaceNormal" )
				, [&]( Vec3 const & C )
				{
					writer.returnStmt( normalize( cross( dFdy( C ), dFdx( C ) ) ) );
				}
				, InVec3{ &writer, cuT( "C" ) } );

			// Returns a unit vector and a screen-space radius for the tap on a unit disk (the caller should scale by the actual disk radius)
			auto tapLocation = writer.implementFunction< Vec2 >( cuT( "tapLocation" )
				, [&]( Int const & sampleNumber
					, Float const & spinAngle
					, Float & ssR )
				{
					// Radius relative to ssR
					auto alpha = writer.declLocale( cuT( "alpha" )
						, writer.cast< Float >( sampleNumber + 0.5_f ) * writer.paren( 1.0_f / NUM_SAMPLES ) );
					auto angle = writer.declLocale( cuT( "angle" )
						, alpha * writer.paren( 6.28_f * NUM_SPIRAL_TURNS ) + spinAngle );

					ssR = alpha;
					writer.returnStmt( vec2( cos( angle ), sin( angle ) ) );
				}
				, InInt{ &writer, cuT( "sampleNumber" ) }
				, InFloat{ &writer, cuT( "spinAngle" ) }
				, OutFloat{ &writer, cuT( "ssR" ) } );


			// Used for packing Z into the GB channels
			auto csZToKey = writer.implementFunction< Float >( cuT( "csZToKey" )
				, [&]( Float const & z )
				{
					writer.returnStmt( clamp( z * writer.paren( 1.0_f / FAR_PLANE_Z ), 0.0_f, 1.0_f ) );
				}
				, InFloat{ &writer, cuT( "z" ) } );


			// Used for packing Z into the GB channels
			auto packKey = writer.implementFunction< Void >( cuT( "packKey" )
				, [&]( Float const & key
					, Vec2 & p )
				{
					// Round to the nearest 1/256.0
					auto temp = writer.declLocale( cuT( "temp" )
						, floor( key * 256.0_f ) );

					// Integer part
					p.x() = temp * writer.paren( 1.0_f / 256.0 );

					// Fractional part
					p.y() = key * 256.0 - temp;
				}
				, InFloat{ &writer, cuT( "key" ) }
				, OutVec2{ &writer, cuT( "p" ) } );


			// Read the camera-space position of the point at screen-space pixel ssP
			auto getPosition = writer.implementFunction< Vec3 >( cuT( "getPosition" )
				, [&]( IVec2 const & ssP )
				{
					auto P = writer.declLocale< Vec3 >( cuT( "P" ) );
					P.z() = texelFetch( c3d_mapDepth, ssP, 0 ).r();

					// Offset to pixel center
					P = reconstructCSPosition( vec2( ssP ) + vec2( 0.5_f ), P.z() );
					writer.returnStmt( P );
				}
				, InIVec2{ &writer, cuT( "ssP" ) } );

			// Read the camera-space position of the point at screen-space pixel ssP + unitOffset * ssR.  Assumes length(unitOffset) == 1
			auto getOffsetPosition = writer.implementFunction< Vec3 >( cuT( "getOffsetPosition" )
				, [&]( IVec2 const & ssC
					, Vec2 const & unitOffset
					, Float const & ssR )
				{
					// Derivation:
					auto mipLevel = writer.declLocale( cuT( "mipLevel" )
						, clamp( findMSB( writer.cast< Int >( ssR ) ) - LOG_MAX_OFFSET, 0, MAX_MIP_LEVEL ) );

					auto ssP = writer.declLocale( cuT( "ssP" )
						, ivec2( ssR * unitOffset ) + ssC );

					auto P = writer.declLocale< Vec3 >( cuT( "P" ) );

					// We need to divide by 2^mipLevel to read the appropriately scaled coordinate from a MIP-map.
					// Manually clamp to the texture size because texelFetch bypasses the texture unit
					auto mipP = writer.declLocale( cuT( "mipP" )
						, clamp( ivec2( ssP.x() >> mipLevel, ssP.y() >> mipLevel )
							, ivec2( 0_i )
							, textureSize( c3d_mapDepth, mipLevel ) - ivec2( 1_i ) ) );
					P.z() = texelFetch( c3d_mapDepth, mipP, mipLevel ).r();

					// Offset to pixel center
					P = reconstructCSPosition( vec2( ssP ) + vec2( 0.5_f ), P.z() );

					writer.returnStmt( P );
				}
				, InIVec2{ &writer, cuT( "ssC" ) }
				, InVec2{ &writer, cuT( "unitOffset" ) }
				, InFloat{ &writer, cuT( "ssR" ) } );

			auto radius2 = writer.declLocale( cuT( "radius2" )
				, c3d_radius * c3d_radius );

			// Compute the occlusion due to sample with index \a i about the pixel at \a ssC that corresponds
			// to camera-space point \\a C with unit normal \\a n_C, using maximum screen-space sampling radius \\a ssDiskRadius

			// Note that units of H() in the HPG12 paper are meters, not
			// unitless.  The whole falloff/sampling function is therefore
			// unitless.  In this implementation, we factor out (9 / radius).

			// Four versions of the falloff function are implemented below."
			auto sampleAO = writer.implementFunction< Float >( cuT( "sampleAO" )
				, [&]( IVec2 const & ssC
					, Vec3 const & C
					, Vec3 const & n_C
					, Float const & ssDiskRadius
					, Int const & tapIndex
					, Float const & randomPatternRotationAngle )
				{
					// Offset on the unit disk, spun for this pixel
					auto ssR = writer.declLocale< Float >( cuT( "ssR" ) );
					auto unitOffset = writer.declLocale( cuT( "unitOffset" )
						, tapLocation( tapIndex, randomPatternRotationAngle, ssR ) );
					ssR *= ssDiskRadius;

					// The occluding point in camera space
					auto Q = writer.declLocale( cuT( "Q" )
						, getOffsetPosition( ssC, unitOffset, ssR ) );

					auto v = writer.declLocale( cuT( "v" )
						, Q - C );

					auto vv = writer.declLocale( cuT( "vv" )
						, dot( v, v ) );
					auto vn = writer.declLocale( cuT( "vn" )
						, dot( v, n_C ) );

					float constexpr epsilon = 0.01f;

					// A: From the HPG12 paper
					// Note large epsilon to avoid overdarkening within cracks
					// writer.returnStmt( writer.cast< Float >( vv < radius2 ) * max( writer.paren( vn - bias ) / writer.paren( epsilon + vv ), 0.0 ) * radius2 * 0.6 );

					// B: Smoother transition to zero (lowers contrast, smoothing out corners). [Recommended]
					auto f = writer.declLocale( cuT( "f" )
						, max( radius2 - vv, 0.0_f ) );

					writer.returnStmt( f * f * f * max( writer.paren( vn - c3d_bias ) / writer.paren( epsilon + vv ), 0.0 ) );

					// C: Medium contrast (which looks better at high radii), no division.  Note that the
					// contribution still falls off with radius^2, but we've adjusted the rate in a way that is
					// more computationally efficient and happens to be aesthetically pleasing.
					// writer.returnStmt( 4.0 * max( 1.0_f - vv * invRadius2, 0.0 ) * max( vn - bias, 0.0 ) );

					// D: Low contrast, no division operation
					// return writer.returnStmt( 2.0 * float( vv < radius2 ) * max( vn - bias, 0.0 ) );
				}
				, InIVec2{ &writer, cuT( "ssC" ) }
				, InVec3{ &writer, cuT( "C" ) }
				, InVec3{ &writer, cuT( "n_C" ) }
				, InFloat{ &writer, cuT( "ssDiskRadius" ) }
				, InInt{ &writer, cuT( "tapIndex" ) }
				, InFloat{ &writer, cuT( "randomPatternRotationAngle" ) } );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					// Pixel being shaded
					auto ssC = writer.declLocale( cuT( "ssC" )
						, ivec2( gl_FragCoord.xy() ) );

					// World space point being shaded
					auto C = writer.declLocale( cuT( "C" )
						, getPosition( ssC ) );

					writer << packKey( csZToKey( C.z() ), bilateralKey ) << endi;

					writer.multilineComment( cuT( "Unneccessary with depth test.\n" )
						cuT( "if (C.z < FAR_PLANE_Z) {\n" )
						cuT( "// We're on the skybox\n" )
						cuT( "discard;\n" )
						cuT( "}\n" ) );

					// Hash function used in the HPG12 AlchemyAO paper
					auto randomPatternRotationAngle = writer.declLocale( cuT( "randomPatternRotationAngle" )
						, 10.0_f * writer.paren( 3 * ssC.x() ^ ssC.y() + ssC.x() * ssC.y() ) );

					// Reconstruct normals from positions. These will lead to 1-pixel black lines
					// at depth discontinuities, however the blur will wipe those out so they are not visible
					// in the final image.
					auto n_C = writer.declLocale( cuT( "n_C" )
						, reconstructCSFaceNormal( C ) );

					// Choose the screen-space sample radius
					// proportional to the projected area of the sphere
					auto ssDiskRadius = writer.declLocale( cuT( "ssDiskRadius" )
						, -c3d_projScale * c3d_radius / C.z() );

					auto sum = writer.declLocale( cuT( "sum" )
						, 0.0_f );

					FOR( writer, Int, i, 0, "i < NUM_SAMPLES", "++i" )
					{
						sum += sampleAO( ssC, C, n_C, ssDiskRadius, i, randomPatternRotationAngle );
					}
					ROF;

					auto A = writer.declLocale( cuT( "A" )
						, max( 0.0_f, 1.0_f - sum * c3d_intensityDivR6 * writer.paren( 5.0_f / NUM_SAMPLES ) ) );

					// Bilateral box-filter over a quad for free, respecting depth edges
					// (the difference that this makes is subtle)
					IF( writer, abs( dFdx( C.z() ) ) < 0.02_f )
					{
						A -= dFdx( A ) * writer.paren( writer.paren( ssC.x() & 1 ) - 0.5_f );
					}
					FI;
					IF( writer, abs( dFdy( C.z() ) ) < 0.02_f )
					{
						A -= dFdy( A ) * writer.paren( writer.paren( ssC.y() & 1 ) - 0.5_f );
					}
					FI;

					visibility = A;
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
		, SsaoConfig const & config )
		: m_engine{ engine }
		, m_config{ config }
		, m_size{ size }
		, m_matrixUbo{ engine }
		, m_ssaoConfigUbo{ engine }
		, m_viewport{ engine }
		, m_result{ engine }
		, m_program{ doGetProgram( engine, config ) }
		, m_pipeline{ doCreatePipeline( engine, *m_program ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "SSAO" ), cuT( "Raw AO" ) ) }
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.initialise();
		m_viewport.resize( m_size );
		m_viewport.update();

		auto & renderSystem = *m_engine.getRenderSystem();
		auto sampler = doCreateSampler( m_engine, cuT( "SSAORaw_Result" ), WrapMode::eClampToEdge );
		auto ssaoResult = renderSystem.createTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite );
		ssaoResult->setSource( PxBufferBase::create( m_size
			, PixelFormat::eR8G8B8 ) );
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
		m_viewport.cleanup();
		m_result.cleanup();
	}

	void RawSsaoPass::compute( TextureUnit const & linearisedDepthBuffer
		, Viewport const & viewport )
	{
		m_timer->start();
		m_ssaoConfigUbo.update( m_config, viewport );
		m_matrixUbo.update( m_viewport.getProjection() );
		m_fbo->bind( FrameBufferTarget::eDraw );
		m_fbo->clear( BufferComponent::eColour );
		linearisedDepthBuffer.getTexture()->bind( MinTextureIndex );
		linearisedDepthBuffer.getSampler()->bind( MinTextureIndex );
		m_pipeline->apply();
		m_viewport.apply();
		m_engine.getRenderSystem()->getCurrentContext()->renderTexture( m_size
			, *linearisedDepthBuffer.getTexture()
			, *m_pipeline
			, m_matrixUbo );
		m_fbo->unbind();
		m_timer->stop();
	}
}
