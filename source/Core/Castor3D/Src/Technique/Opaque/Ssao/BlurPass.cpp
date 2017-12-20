#include "BlurPass.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "State/BlendState.hpp"
#include "State/DepthStencilState.hpp"
#include "State/MultisampleState.hpp"
#include "State/RasteriserState.hpp"
#include "Technique/Opaque/Ssao/SsaoConfig.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;

#define COMPUTE_PLANE_WEIGHT 1

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

			//////////////////////////////////////////////////////////////////////////////////////////////

			/** Type of data to read from source.  This macro allows
			the same blur shader to be used on different kinds of input data. */
#define ValueType            Float
#define InValueType          InFloat
#define OutValueType         OutFloat

			/** Swizzle to use to extract the channels of source. This macro allows
			the same blur shader to be used on different kinds of input data. */
#define ValueComponents      r()

#define ValueIsKey           0

			/** Channel encoding the bilateral key value (which must not be the same as ValueComponents) */
#if NUM_KEY_COMPONENTS == 2
#   define KeyComponents     gb()
#else
#   define KeyComponents     g()
#endif

			auto index = MinTextureIndex;
			auto c3d_mapInput = writer.declSampler< Sampler2D >( cuT( "c3d_mapInput" ), index++ );

			UBO_SSAO_CONFIG( writer );

			/** (1, 0) or (0, 1)*/
			auto c3d_axis = writer.declUniform< IVec2 >( cuT( "c3d_axis" ) );

			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec3 >( cuT( "pxl_fragColor" ) );
#define  result         pxl_fragColor.ValueComponents
#define  keyPassThrough pxl_fragColor.KeyComponents

#if NUM_KEY_COMPONENTS == 2

			/** Returns a number on (0, 1) */
			auto unpackKey = writer.implementFunction< Float >( cuT( "unpackKey" )
				, [&]( Vec2 const & p )
				{
					writer.returnStmt( p.x() * ( 256.0 / 257.0 ) + p.y() * ( 1.0 / 257.0 ) );
				}
				, InVec2{ &writer, cuT( "p" ) } );

#else

			/** Returns a number on (0, 1) */
			auto unpackKey = writer.implementFunction< Float >( cuT( "unpackKey" )
				, [&]( Float const & p )
				{
					writer.returnStmt( p );
				}
				, InFloat{ &writer, cuT( "p" ) } );

#endif

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

			auto positionFromKey = writer.implementFunction< Vec3 >( cuT( "positionFromKey" )
				, [&]( Float const & key
					, IVec2 const & ssCenter
					, Vec4 const & projInfo )
				{
					auto z = writer.declLocale( cuT( "z" )
						, key * c3d_farPlaneZ );
					auto position = writer.declLocale( cuT( "position" )
						, reconstructCSPosition( vec2( ssCenter ) + vec2( 0.5_f )
							, z
							, projInfo ) );
					writer.returnStmt( position );
				}
				, InFloat{ &writer, cuT( "key" ) }
				, InIVec2{ &writer, cuT( "ssCenter" ) }
				, InVec4{ &writer, cuT( "projInfo" ) } );


			auto getTapInformation = writer.implementFunction< Void >( cuT( "getTapInformation" )
				, [&]( IVec2 const & tapLoc
					, Float & tapKey
					, ValueType & value
					, Vec3 & tapNormal )
				{
					auto temp = writer.declLocale( cuT( "temp" )
						, texelFetch( c3d_mapInput, tapLoc, 0 ) );
					tapKey = unpackKey( temp.KeyComponents );
					value = temp.ValueComponents;
#ifdef normal_notNull
					tapNormal = texelFetch( normal_buffer, tapLoc, 0 ).xyz;
					tapNormal = normalize( tapNormal * normal_readMultiplyFirst.xyz() + normal_readAddSecond.xyz() );
#else
					tapNormal = vec3( 0.0_f );
#endif
				}
				, InIVec2{ &writer, cuT( "tapLoc" ) }
				, OutFloat{ &writer, cuT( "tapKey" ) }
				, OutValueType{ &writer, cuT( "value" ) }
				, OutVec3{ &writer, cuT( "tapNormal" ) } );

			auto calculateBilateralWeight = writer.implementFunction< Float >( cuT( "calculateBilateralWeight" )
				, [&]( Float const & key
					, Float const & tapKey
					, IVec2 const & tapLoc
					, Vec3 const & normal
					, Vec3 const & tapNormal
					, Vec3 const & position )
				{
					auto scale = writer.declLocale( cuT( "scale" )
						, 1.5_f * c3d_invRadius );

					// The "bilateral" weight. As depth difference increases, decrease weight.
					// The key values are in scene-specific scale. To make them scale-invariant, factor in
					// the AO radius, which should be based on the scene scale.
					auto depthWeight = writer.declLocale( cuT( "depthWeight" )
						, max( 0.0_f, 1.0_f - writer.paren( c3d_edgeSharpness * 2000.0 ) * abs( tapKey - key ) * scale ) );
					auto k_normal = writer.declLocale( cuT( "k_normal" )
						, 1.0_f );
					auto k_plane = writer.declLocale( cuT( "k_plane" )
						, 1.0_f );

					// Prevents blending over creases. 
					auto normalWeight = writer.declLocale( cuT( "normalWeight" )
						, 1.0_f );
					auto planeWeight = writer.declLocale( cuT( "planeWeight" )
						, 1.0_f );

#   if defined(normal_notNull)

					auto normalCloseness = writer.declLocale( cuT( "normalCloseness" )
						, dot( tapNormal, normal ) );
#       if (COMPUTE_PLANE_WEIGHT == 0)
					normalCloseness = normalCloseness * normalCloseness;
					normalCloseness = normalCloseness * normalCloseness;
					k_normal = 4.0_f;
#       endif
					auto normalError = writer.declLocale( cuT( "normalError" )
						, ( 1.0_f - normalCloseness ) * k_normal );
					normalWeight = max( writer.paren( 1.0_f - c3d_edgeSharpness * normalError ), 0.0_f );

#       if (COMPUTE_PLANE_WEIGHT == 1)
					auto lowDistanceThreshold2 = writer.declLocale( cuT( "lowDistanceThreshold2" )
						, 0.001_f );

					auto tapPosition = writer.declLocale( cuT( "tapPosition" )
						, positionFromKey( tapKey, tapLoc, projInfo ) );

					// Change in position in camera space
					auto dq = writer.declLocale( cuT( "dq" )
						, position - tapPosition );

					// How far away is this point from the original sample
					// in camera space? (Max value is unbounded)
					auto distance2 = writer.declLocale( cuT( "distance2" )
						, dot( dq, dq ) );

					// How far off the expected plane (on the perpendicular) is this point?  Max value is unbounded.
					auto planeError = writer.declLocale( cuT( "planeError" )
						, max( abs( dot( dq, tapNormal ) ), abs( dot( dq, normal ) ) ) );

					// Minimum distance threshold must be scale-invariant, so factor in the radius
					planeWeight = ( distance2 * square( scale ) < lowDistanceThreshold2 )
						? 1.0_f
						: pow( max( 0.0_f, 1.0_f - c3d_edgeSharpness * 2.0 * k_plane * planeError / sqrt( distance2 ) ), 2.0 );
#       endif

#   endif

					writer.returnStmt( depthWeight * normalWeight * planeWeight );
				}
				, InFloat{ &writer, cuT( "key" ) }
				, InFloat{ &writer, cuT( "tapKey" ) }
				, InIVec2{ &writer, cuT( "tapLoc" ) }
				, InVec3{ &writer, cuT( "normal" ) }
				, InVec3{ &writer, cuT( "tapNormal" ) }
				, InVec3{ &writer, cuT( "position" ) } );

			REQUIRE( config.m_blurRadius > 0 && config.m_blurRadius < 7 );
			auto gaussian = writer.declUniform( cuT( "gaussian" ), config.m_blurRadius + 1u
				, ( config.m_blurRadius == 1u
					? std::vector< Float >{ { 0.5_f, 0.25_f } }
					: ( config.m_blurRadius == 2u
						? std::vector< Float >{ { 0.153170_f, 0.144893_f, 0.122649_f } }
						: ( config.m_blurRadius == 3u
							? std::vector< Float >{ { 0.153170_f, 0.144893_f, 0.122649_f, 0.092902_f } }
							: ( config.m_blurRadius == 4u
								? std::vector< Float >{ { 0.153170_f, 0.144893_f, 0.122649_f, 0.092902_f, 0.062970_f } }
								: ( config.m_blurRadius == 5u
									? std::vector< Float >{ { 0.111220_f, 0.107798_f, 0.098151_f, 0.083953_f, 0.067458_f, 0.050920_f } }
									: std::vector< Float >{ { 0.111220_f, 0.107798_f, 0.098151_f, 0.083953_f, 0.067458_f, 0.050920_f, 0.036108_f } } ) ) ) ) ) );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ssCenter = writer.declLocale( cuT( "ssCenter" )
						, ivec2( gl_FragCoord.xy() ) );

					auto temp = writer.declLocale( cuT( "temp" )
						, texelFetch( c3d_mapInput, ssCenter, 0 ) );
					auto sum = writer.declLocale( cuT( "sum" )
						, temp.ValueComponents );

					keyPassThrough = temp.KeyComponents;
					auto key = writer.declLocale( cuT( "key" )
						, unpackKey( keyPassThrough ) );
					auto normal = writer.declLocale< Vec3 >( cuT( "normal" ) );

#ifdef normal_notNull
					normal = normalize( texelFetch( normal_buffer, ssCenter, 0 ).xyz() * normal_readMultiplyFirst.xyz + normal_readAddSecond.xyz );
#endif

					IF( writer, key == 1.0_f )
					{
						// Sky pixel (if you aren't using depth keying, disable this test)
						result = sum;
						writer.returnStmt();
					}
					FI;

					// Base weight for depth falloff.  Increase this for more blurriness,
					// decrease it for better edge discrimination
					auto BASE = writer.declLocale( cuT( "BASE" )
						, gaussian[0] );
					auto totalWeight = writer.declLocale( cuT( "totalWeight" )
						, BASE );
					sum *= totalWeight;


					auto position = writer.declLocale( cuT( "position" )
						, positionFromKey( key, ssCenter, c3d_projInfo ) );

# if MDB_WEIGHTS==0

					FOR( writer, Int, r, -c3d_blurRadius, "r <= c3d_blurRadius", "++r" )
					{
						// We already handled the zero case above.  This loop should be unrolled and the static branch optimized out,
						// so the IF statement has no runtime cost
						IF( writer, r != 0_i )
						{
							auto tapLoc = writer.declLocale( cuT( "tapLoc" )
								, ssCenter + c3d_axis * writer.paren( r * c3d_blurStepSize ) );

							// spatial domain: offset gaussian tap
							auto weight = writer.declLocale( cuT( "weight" )
								, 0.3_f + gaussian[abs( r )] );

							auto tapKey = writer.declLocale< Float >( cuT( "tapKey" ) );
							auto value = writer.declLocale< ValueType >( cuT( "value" ) );
							auto tapNormal = writer.declLocale< Vec3 >( cuT( "tapNormal" ) );
							writer << getTapInformation( tapLoc, tapKey, value, tapNormal ) << endi;

							auto bilateralWeight = writer.declLocale( cuT( "bilateralWeight" )
								, calculateBilateralWeight( key
								, tapKey
								, tapLoc
								, normal
								, tapNormal
								, position ) );

							weight *= bilateralWeight;
							sum += value * weight;
							totalWeight += weight;
						}
						FI;
					}
					ROF;

#else 

					float lastBilateralWeight = 9999.0;
					for ( int r = -1; r >= -R; --r ) {
						ivec2 tapLoc = ssCenter + axis * writer.paren( r * SCALE );

						float tapKey;
						VALUE_TYPE value;
						vec3 tapNormal;
						getTapInformation( tapLoc, tapKey, value, tapNormal );

						float bilateralWeight = calculateBilateralWeight( key, tapKey, tapLoc, normal, tapNormal, position );

						// spatial domain: offset gaussian tap
						float weight = 0.3 + gaussian[abs( r )];

						bilateralWeight = min( lastBilateralWeight, bilateralWeight );
						lastBilateralWeight = bilateralWeight;
						weight *= bilateralWeight;
						sum += value * weight;
						totalWeight += weight;
					}

					lastBilateralWeight = 9999.0;
					for ( int r = 1; r <= R; ++r ) {
						ivec2 tapLoc = ssCenter + axis * ( r * SCALE );

						float tapKey;
						VALUE_TYPE value;
						vec3 tapNormal;
						getTapInformation( tapLoc, tapKey, value, tapNormal );

						float bilateralWeight = calculateBilateralWeight( key, tapKey, tapLoc, normal, tapNormal, position );

						// spatial domain: offset gaussian tap
						float weight = 0.3 + gaussian[abs( r )];

						bilateralWeight = min( lastBilateralWeight, bilateralWeight );
						lastBilateralWeight = bilateralWeight;
						weight *= bilateralWeight;
						sum += value * weight;
						totalWeight += weight;
					}

#endif

					auto const epsilon = writer.declLocale( cuT( "epsilon" )
						, 0.0001_f );
					result = sum / writer.paren( totalWeight + epsilon );

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

	SsaoBlurPass::SsaoBlurPass( Engine & engine
		, Size const & size
		, SsaoConfig const & config
		, MatrixUbo & matrixUbo
		, SsaoConfigUbo & ssaoConfigUbo )
		: m_engine{ engine }
		, m_size{ size }
		, m_matrixUbo{ matrixUbo }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_result{ engine }
		, m_program{ doGetProgram( engine, config ) }
		, m_axis{ *m_program->findUniform< UniformType::eVec2i >( cuT( "c3d_axis" ), ShaderType::ePixel ) }
		, m_pipeline{ doCreatePipeline( engine, *m_program ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "SSAO" ), cuT( "Blur" ) ) }
	{
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_ssaoConfigUbo.getUbo() );

		auto & renderSystem = *m_engine.getRenderSystem();
		auto sampler = doCreateSampler( m_engine, cuT( "SSAOBlur_Result" ), WrapMode::eClampToEdge );
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
		m_fbo->setClearColour( RgbaColour::fromPredefined( PredefinedRgbaColour::eOpaqueWhite ) );

		m_resultAttach = m_fbo->createAttachment( ssaoResult );
		m_fbo->bind();
		m_fbo->attach( AttachmentPoint::eColour, 0u, m_resultAttach, ssaoResult->getType() );
		m_fbo->setDrawBuffer( m_resultAttach );
		ENSURE( m_fbo->isComplete() );
		m_fbo->unbind();
	}

	SsaoBlurPass::~SsaoBlurPass()
	{
		m_resultAttach.reset();
		m_fbo->cleanup();
		m_fbo.reset();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_program.reset();
		m_result.cleanup();
	}

	void SsaoBlurPass::blur( TextureUnit const & input
		, Point2i const & axis )
	{
		m_timer->start();
		m_axis.setValue( axis );
		m_fbo->bind( FrameBufferTarget::eDraw );
		m_fbo->clear( BufferComponent::eColour );
		m_pipeline->apply();
		m_engine.getRenderSystem()->getCurrentContext()->renderTexture( m_size
			, *input.getTexture()
			, *m_pipeline
			, m_matrixUbo );
		m_fbo->unbind();
		m_timer->stop();
	}
}
