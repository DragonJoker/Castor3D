#include "BlurPass.hpp"

#include "Engine.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
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

using namespace castor;

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
		
		glsl::Shader doGetPixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			/** Increase to make depth edges crisper. Decrease to reduce flicker. */
			auto EDGE_SHARPNESS = writer.declConstant( cuT( "EDGE_SHARPNESS" ), 1.0_f );

			/** Step in 2-pixel intervals since we already blurred against neighbors in the
			first AO pass.  This constant can be increased while R decreases to improve
			performance at the expense of some dithering artifacts.

			Morgan found that a scale of 3 left a 1-pixel checkerboard grid that was
			unobjectionable after shading was applied but eliminated most temporal incoherence
			from using small numbers of sample taps.
			*/
			auto SCALE = writer.declConstant( cuT( "SCALE" ), 2.0_f );

			/** Filter radius in pixels. This will be multiplied by SCALE. */
			static uint32_t constexpr filterRadius = 4;
			auto R = writer.declConstant( cuT( "R" ), Int{ int( filterRadius ) } );

			//////////////////////////////////////////////////////////////////////////////////////////////

			/** Type of data to read from source.  This macro allows
			the same blur shader to be used on different kinds of input data. */
#define VALUE_TYPE        float
#define VALUE_IS_KEY       0


			// Gaussian coefficients
			auto gaussian = writer.declUniform( cuT( "gaussian" )
				, filterRadius + 1u
				, std::vector< Float >
				{
					{
						// 0.356642, 0.239400, 0.072410, 0.009869
						// 0.398943, 0.241971, 0.053991, 0.004432, 0.000134  // stddev = 1.0
						0.153170_f, 0.144893_f, 0.122649_f, 0.092902_f, 0.062970_f  // stddev = 2.0
						// 0.111220, 0.107798, 0.098151, 0.083953, 0.067458, 0.050920, 0.036108 // stddev = 3.0
					}
				} );

			/** (1, 0) or (0, 1)*/
			auto c3d_axis = writer.declUniform< IVec2 >( cuT( "c3d_axis" ) );

			auto index = MinTextureIndex;
			auto c3d_mapInput = writer.declSampler< Sampler2D >( cuT( "c3d_mapInput" ), index++ );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );


			/** Returns a number on (0, 1) */
			auto unpackKey = writer.implementFunction< Float >( cuT( "unpackKey" )
				, [&]( Vec2 const & p )
				{
					writer.returnStmt( p.x() * writer.paren( 256.0_f / 257.0 ) + p.y() * writer.paren( 1.0_f / 257.0 ) );
				}
				, InVec2{ &writer, cuT( "p" ) } );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec3 >( cuT( "pxl_fragColor" ) );
#define  result         pxl_fragColor.r()
#define  keyPassThrough pxl_fragColor.gb()

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ssC = writer.declLocale( cuT( "ssC" )
						, ivec2( gl_FragCoord.xy() ) );

					auto temp = writer.declLocale( cuT( "temp" )
						, texelFetch( c3d_mapInput, ssC, 0 ) );

					keyPassThrough = temp.gb();
					auto key = writer.declLocale( cuT( "key" )
						, unpackKey( keyPassThrough ) );

					auto sum = writer.declLocale( cuT( "sum" )
						, temp.r() );

					IF( writer, key == 1.0_f )
					{
						// Sky pixel (if you aren't using depth keying, disable this test)
						result = sum;
					}
					ELSE
					{
						// Base weight for depth falloff.  Increase this for more blurriness,
						// decrease it for better edge discrimination
						auto BASE = writer.declLocale( cuT( "BASE" )
							, Float( gaussian[0] ) );
						auto totalWeight = writer.declLocale( cuT( "totalWeight" )
							, BASE );
						sum *= totalWeight;

						FOR( writer, Int, r, -R, "r <= R", "++r" )
						{
							// We already handled the zero case above.  This loop should be unrolled and the static branch optimized out,
							// so the IF statement has no runtime cost
							IF( writer, r != 0_i )
							{
								temp = texelFetch( c3d_mapInput, ivec2( ssC + c3d_axis * writer.paren( SCALE * r ) ), 0_i );
								auto tapKey = writer.declLocale( cuT( "tapKey" )
									, unpackKey( temp.gb() ) );
								auto value = writer.declLocale( cuT( "value" )
									, temp.r() );

								// spatial domain: offset gaussian tap
								auto weight = writer.declLocale( cuT( "weight" )
									, 0.3_f + gaussian[abs( r )] );

								// range domain (the "bilateral" weight). As depth difference increases, decrease weight.
								weight *= max( 0.0_f
									, 1.0_f - writer.paren( 2000.0_f * EDGE_SHARPNESS ) * abs( tapKey - key ) );

								sum += value * weight;
								totalWeight += weight;
							}
							FI;
						}
						ROF;

						const float epsilon = 0.0001f;
						result = sum / writer.paren( totalWeight + epsilon );
					}
					FI;

				} );
			return writer.finalise();
		}

		ShaderProgramSPtr doGetProgram( Engine & engine )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetPixelProgram( engine );
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
		, Size const & size )
		: m_engine{ engine }
		, m_size{ size }
		, m_matrixUbo{ engine }
		, m_viewport{ engine }
		, m_result{ engine }
		, m_program{ doGetProgram( engine ) }
		, m_axis{ *m_program->findUniform< UniformType::eVec2i >( cuT( "c3d_axis" ), ShaderType::ePixel ) }
		, m_pipeline{ doCreatePipeline( engine, *m_program ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "SSAO" ), cuT( "Blur" ) ) }
	{
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.initialise();
		m_viewport.resize( m_size );
		m_viewport.update();

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
		m_matrixUbo.getUbo().cleanup();
		m_viewport.cleanup();
		m_result.cleanup();
	}

	void SsaoBlurPass::blur( TextureUnit const & input
		, Point2i const & axis )
	{
		m_timer->start();
		m_axis.setValue( axis );
		m_fbo->bind( FrameBufferTarget::eDraw );
		m_fbo->clear( BufferComponent::eColour );
		input.getTexture()->bind( MinTextureIndex );
		input.getSampler()->bind( MinTextureIndex );
		m_pipeline->apply();
		m_viewport.apply();
		m_engine.getRenderSystem()->getCurrentContext()->renderTexture( m_size
			, *input.getTexture()
			, *m_pipeline
			, m_matrixUbo );
		m_fbo->unbind();
		m_timer->stop();
	}
}
