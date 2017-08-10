#include "FinalCombinePass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/SsaoConfig.hpp>
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
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

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
			auto declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
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

			auto & renderSystem = *engine.getRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->getData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		GeometryBuffersSPtr doCreateVao( Engine & engine
			, ShaderProgram & p_program
			, VertexBuffer & p_vbo )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto result = renderSystem.createGeometryBuffers( Topology::eTriangles
				, p_program );
			result->initialise( { p_vbo }, nullptr );
			return result;
		}

		GLSL::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		GLSL::Shader doGetPixelProgram( Engine & engine
			, GLSL::FogType p_fogType )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto c3d_mapDepth = writer.declUniform< Sampler2D >( getTextureName( WbTexture::eDepth ) );
			auto c3d_mapAccumulation = writer.declUniform< Sampler2D >( getTextureName( WbTexture::eAccumulation ) );
			auto c3d_mapRevealage = writer.declUniform< Sampler2D >( getTextureName( WbTexture::eRevealage ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			GLSL::Utils utils{ writer };
			utils.declareCalcVSPosition();

			GLSL::Fog fog{ p_fogType, writer };

			auto maxComponent = writer.implementFunction< Float >( cuT( "maxComponent" )
				, [&]( Vec3 const & v )
			{
				writer.returnStmt( max( max( v.x(), v.y() ), v.z() ) );
			}, InVec3{ &writer, cuT( "v" ) } );

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
					IF( writer, GLSL::isinf( maxComponent( GLSL::abs( accum.rgb() ) ) ) )
					{
						accum.rgb() = vec3( accum.a() );
					}
					FI;

					auto averageColor = writer.declLocale( cuT( "averageColor" )
						, accum.rgb() / max( accum.a(), 0.00001_f ) );

					pxl_fragColor = vec4( averageColor.rgb(), 1.0_f - revealage );

					if ( p_fogType != FogType::eDisabled )
					{
						auto position = writer.declLocale( cuT( "position" ), utils.calcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						fog.applyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.finalise();
		}
		
		ShaderProgramSPtr doCreateProgram( Engine & engine
			, GLSL::FogType p_fogType )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetPixelProgram( engine, p_fogType );
			ShaderProgramSPtr program = engine.getShaderProgramCache().getNewProgram( false );
			program->createObject( ShaderType::eVertex );
			program->createObject( ShaderType::ePixel );
			program->createUniform< UniformType::eSampler >( getTextureName( WbTexture::eDepth ), ShaderType::ePixel )->setValue( 0u );
			program->createUniform< UniformType::eSampler >( getTextureName( WbTexture::eAccumulation ), ShaderType::ePixel )->setValue( 1u );
			program->createUniform< UniformType::eSampler >( getTextureName( WbTexture::eRevealage ), ShaderType::ePixel )->setValue( 2u );

			program->setSource( ShaderType::eVertex, vtx );
			program->setSource( ShaderType::ePixel, pxl );
			program->initialise();
			return program;
		}

		RenderPipelineUPtr doCreateRenderPipeline( Engine & engine
			, ShaderProgram & p_program
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfoUbo )
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
				, p_program
				, PipelineFlags{} );
			pipeline->addUniformBuffer( p_matrixUbo.getUbo() );
			pipeline->addUniformBuffer( p_sceneUbo.getUbo() );
			pipeline->addUniformBuffer( p_gpInfoUbo.getUbo() );
			return pipeline;
		}
	}

	//*********************************************************************************************

	FinalCombineProgram::FinalCombineProgram( Engine & engine
		, VertexBuffer & p_vbo
		, MatrixUbo & p_matrixUbo
		, SceneUbo & p_sceneUbo
		, GpInfoUbo & p_gpInfoUbo
		, GLSL::FogType p_fogType )
		: m_program{ doCreateProgram( engine, p_fogType ) }
		, m_geometryBuffers{ doCreateVao( engine, *m_program, p_vbo ) }
		, m_pipeline{ doCreateRenderPipeline( engine, *m_program, p_matrixUbo, p_sceneUbo, p_gpInfoUbo ) }
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

	void FinalCombineProgram::render()const
	{
		m_pipeline->apply();
		m_geometryBuffers->draw( 6u, 0 );
	}

	//*********************************************************************************************

	FinalCombinePass::FinalCombinePass( Engine & engine
		, Size const & p_size )
		: m_size{ p_size }
		, m_viewport{ engine }
		, m_vertexBuffer{ doCreateVbo( engine ) }
		, m_matrixUbo{ engine }
		, m_sceneUbo{ engine }
		, m_gpInfo{ engine }
		, m_programs
		{
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eDisabled },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eLinear },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eExponential },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eSquaredExponential }
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
		m_sceneUbo.getUbo().cleanup();
		m_gpInfo.getUbo().cleanup();
		m_vertexBuffer->cleanup();
	}

	void FinalCombinePass::render( WeightedBlendPassResult const & p_r
		, FrameBuffer const & p_frameBuffer
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj
		, Fog const & p_fog )
	{
		m_gpInfo.update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		m_sceneUbo.update( p_camera, p_fog );
		p_frameBuffer.bind( FrameBufferTarget::eDraw );
		p_frameBuffer.setDrawBuffers();

		auto & program = m_programs[size_t( p_fog.getType() )];

		m_viewport.apply();
		p_r[size_t( WbTexture::eDepth )]->getTexture()->bind( 0u );
		p_r[size_t( WbTexture::eDepth )]->getSampler()->bind( 0u );
		p_r[size_t( WbTexture::eAccumulation )]->getTexture()->bind( 1u );
		p_r[size_t( WbTexture::eAccumulation )]->getSampler()->bind( 1u );
		p_r[size_t( WbTexture::eRevealage )]->getTexture()->bind( 2u );
		p_r[size_t( WbTexture::eRevealage )]->getSampler()->bind( 2u );
		program.render();
		p_r[size_t( WbTexture::eRevealage )]->getTexture()->unbind( 2u );
		p_r[size_t( WbTexture::eRevealage )]->getSampler()->unbind( 2u );
		p_r[size_t( WbTexture::eAccumulation )]->getTexture()->unbind( 1u );
		p_r[size_t( WbTexture::eAccumulation )]->getSampler()->unbind( 1u );
		p_r[size_t( WbTexture::eDepth )]->getTexture()->unbind( 0u );
		p_r[size_t( WbTexture::eDepth )]->getSampler()->unbind( 0u );
	}

	//*********************************************************************************************
}
