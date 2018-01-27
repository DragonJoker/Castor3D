#include "FinalCombinePass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Technique/Opaque/Ssao/SsaoConfig.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Fog.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslFog.hpp"
#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		VertexBufferSPtr doCreateVbo( Engine & engine )
		{
			auto declaration = ParticleDeclaration(
			{
				ParticleElementDeclaration( cuT( "position" ), uint32_t( ElementUsage::ePosition ), renderer::AttributeFormat::eVec2 ),
				ParticleElementDeclaration{ cuT( "texcoord" ), uint32_t( ElementUsage::eTexCoords ), renderer::AttributeFormat::eVec2 },
			} );

			float data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			vertexBuffer->resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->getData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->initialise( renderer::MemoryPropertyFlag::eHostVisible );
			return vertexBuffer;
		}

		GeometryBuffersSPtr doCreateVao( Engine & engine
			, ShaderProgram & program
			, VertexBuffer & vbo )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto result = renderSystem.createGeometryBuffers( Topology::eTriangles
				, program );
			result->initialise( { vbo }, nullptr );
			return result;
		}

		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer, 0u );
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_projection * vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetPixelProgram( Engine & engine
			, FogType fogType )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer, 0u );
			UBO_GPINFO( writer, 0u );
			auto index = MinTextureIndex;
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eDepth ), index++, 0u );
			auto c3d_mapAccumulation = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eAccumulation ), index++, 0u );
			auto c3d_mapRevealage = writer.declSampler< Sampler2D >( getTextureName( WbTexture::eRevealage ), index++, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			glsl::Utils utils{ writer };
			utils.declareCalcVSPosition();

			shader::Fog fog{ fogType, writer };

			auto maxComponent = writer.implementFunction< Float >( cuT( "maxComponent" )
				, [&]( Vec3 const & v )
				{
					writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
				}
				, InVec3{ &writer, cuT( "v" ) } );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto coord = writer.declLocale( cuT( "coord" )
						, ivec2( gl_FragCoord.xy() ) );
					auto revealage = writer.declLocale( cuT( "revealage" )
						, texelFetch( c3d_mapRevealage, coord, 0 ).r() );

					IF( writer, revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						writer.discard();
					}
					FI;

					auto accum = writer.declLocale( cuT( "accum" )
						, texelFetch( c3d_mapAccumulation, coord, 0 ) );

					// Suppress overflow
					IF( writer, glsl::isinf( maxComponent( glsl::abs( accum.rgb() ) ) ) )
					{
						accum.rgb() = vec3( accum.a() );
					}
					FI;

					auto averageColor = writer.declLocale( cuT( "averageColor" )
						, accum.rgb() / max( accum.a(), 0.00001_f ) );

					pxl_fragColor = vec4( averageColor.rgb(), 1.0_f - revealage );

					if ( fogType != FogType::eDisabled )
					{
						auto position = writer.declLocale( cuT( "position" )
							, utils.calcVSPosition( vtx_texture
								, texture( c3d_mapDepth, vtx_texture ).r()
								, c3d_mtxInvProj ) );
						fog.applyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.finalise();
		}
		
		ShaderProgramSPtr doCreateProgram( Engine & engine
			, FogType fogType )
		{
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetPixelProgram( engine, fogType );
			ShaderProgramSPtr program = engine.getShaderProgramCache().getNewProgram( false );
			program->createObject( renderer::ShaderStageFlag::eVertex );
			program->createObject( renderer::ShaderStageFlag::eFragment );
			program->createUniform< UniformType::eSampler >( getTextureName( WbTexture::eDepth ), renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex + 0u );
			program->createUniform< UniformType::eSampler >( getTextureName( WbTexture::eAccumulation ), renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex + 1u );
			program->createUniform< UniformType::eSampler >( getTextureName( WbTexture::eRevealage ), renderer::ShaderStageFlag::eFragment )->setValue( MinTextureIndex + 2u );

			program->setSource( renderer::ShaderStageFlag::eVertex, vtx );
			program->setSource( renderer::ShaderStageFlag::eFragment, pxl );
			program->initialise();
			return program;
		}

		RenderPipelineUPtr doCreateRenderPipeline( Engine & engine
			, ShaderProgram & program
			, MatrixUbo & matrixUbo
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo )
		{
			DepthStencilState dsstate;
			dsstate.setDepthTest( false );
			dsstate.setDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.setCulledFaces( Culling::eNone );
			BlendState bdState;
			bdState.enableBlend( true );
			bdState.setAlphaBlend( BlendOperation::eAdd, BlendOperand::eSrcAlpha, BlendOperand::eInvSrcAlpha );
			bdState.setRgbBlend( BlendOperation::eAdd, BlendOperand::eSrcAlpha, BlendOperand::eInvSrcAlpha );
			auto pipeline = engine.getRenderSystem()->createRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, std::move( bdState )
				, MultisampleState{}
				, program
				, PipelineFlags{} );
			pipeline->addUniformBuffer( matrixUbo.getUbo() );
			pipeline->addUniformBuffer( sceneUbo.getUbo() );
			pipeline->addUniformBuffer( gpInfoUbo.getUbo() );
			return pipeline;
		}
	}

	//*********************************************************************************************

	FinalCombineProgram::FinalCombineProgram( Engine & engine
		, VertexBuffer & vbo
		, MatrixUbo & matrixUbo
		, SceneUbo & sceneUbo
		, GpInfoUbo & gpInfoUbo
		, FogType fogType )
		: m_program{ doCreateProgram( engine, fogType ) }
		, m_geometryBuffers{ doCreateVao( engine, *m_program, vbo ) }
		, m_pipeline{ doCreateRenderPipeline( engine, *m_program, matrixUbo, sceneUbo, gpInfoUbo ) }
	{
	}

	FinalCombineProgram::~FinalCombineProgram()
	{
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void FinalCombineProgram::render( VertexBuffer const & vbo )const
	{
		m_pipeline->apply();
		m_geometryBuffers->draw( 6u, 0u );
	}

	//*********************************************************************************************

	FinalCombinePass::FinalCombinePass( Engine & engine
		, Size const & size
		, SceneUbo & sceneUbo )
		: m_size{ size }
		, m_viewport{ engine }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_matrixUbo{ engine }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfo{ engine }
		, m_programs
		{
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, FogType::eDisabled },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, FogType::eLinear },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, FogType::eExponential },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, FogType::eSquaredExponential }
		}
	{
		m_viewport.setOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.initialise();
		m_viewport.resize( m_size );
		m_viewport.update();
		m_matrixUbo.update( m_viewport.getProjection() );
	}

	FinalCombinePass::~FinalCombinePass()
	{
		m_matrixUbo.getUbo().cleanup();
		m_gpInfo.getUbo().cleanup();
		m_vertexBuffer->cleanup();
	}

	void FinalCombinePass::render( WeightedBlendPassResult const & p_r
		, FrameBuffer const & frameBuffer
		, Camera const & camera
		, Matrix4x4r const & invViewProj
		, Matrix4x4r const & invView
		, Matrix4x4r const & invProj
		, FogType fogType )
	{
		m_gpInfo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
		frameBuffer.bind( FrameBufferTarget::eDraw );
		frameBuffer.setDrawBuffers();

		auto & program = m_programs[size_t( fogType )];

		m_viewport.apply();
		auto index = MinTextureIndex;
		p_r[size_t( WbTexture::eDepth )]->getTexture()->bind( index );
		p_r[size_t( WbTexture::eDepth )]->getSampler()->bind( index++ );
		p_r[size_t( WbTexture::eAccumulation )]->getTexture()->bind( index );
		p_r[size_t( WbTexture::eAccumulation )]->getSampler()->bind( index++ );
		p_r[size_t( WbTexture::eRevealage )]->getTexture()->bind( index );
		p_r[size_t( WbTexture::eRevealage )]->getSampler()->bind( index );
		program.render( *m_vertexBuffer );
		p_r[size_t( WbTexture::eRevealage )]->getTexture()->unbind( index );
		p_r[size_t( WbTexture::eRevealage )]->getSampler()->unbind( index-- );
		p_r[size_t( WbTexture::eAccumulation )]->getTexture()->unbind( index );
		p_r[size_t( WbTexture::eAccumulation )]->getSampler()->unbind( index-- );
		p_r[size_t( WbTexture::eDepth )]->getTexture()->unbind( index );
		p_r[size_t( WbTexture::eDepth )]->getSampler()->unbind( index );
	}

	//*********************************************************************************************
}
