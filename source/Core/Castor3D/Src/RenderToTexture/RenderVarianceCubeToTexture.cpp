#include "RenderVarianceCubeToTexture.hpp"

#include "Engine.hpp"

#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderVarianceCubeToTexture::RenderVarianceCubeToTexture( Context & context
		, MatrixUbo & matrixUbo )
		: OwnedBy< Context >{ context }
		, m_matrixUbo{ matrixUbo }
		, m_viewport{ *context.getRenderSystem()->getEngine() }
		, m_bufferVertex
		{
			{
				0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1
			}
		}
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 },
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 }
			}
		}
	{
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	RenderVarianceCubeToTexture::~RenderVarianceCubeToTexture()
	{
	}

	void RenderVarianceCubeToTexture::initialise()
	{
		m_viewport.initialise();
		auto & renderSystem = *getOwner()->getRenderSystem();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *renderSystem.getEngine()
			, m_declaration );
		m_vertexBuffer->resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->linkCoords( m_arrayVertex.begin()
			, m_arrayVertex.end() );
		m_vertexBuffer->initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );

		m_sampler = getOwner()->getRenderSystem()->getEngine()->getSamplerCache().add( cuT( "RenderVarianceCubeToTexture" ) );
		m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );

		doInitialiseDepth();
		doInitialiseVariance();
	}

	void RenderVarianceCubeToTexture::cleanup()
	{
		m_sampler.reset();
		m_pipelineDepth.m_pipeline->cleanup();
		m_pipelineDepth.m_pipeline.reset();
		m_pipelineVariance.m_pipeline->cleanup();
		m_pipelineVariance.m_pipeline.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_pipelineDepth.m_geometryBuffers->cleanup();
		m_pipelineDepth.m_geometryBuffers.reset();
		m_pipelineVariance.m_geometryBuffers->cleanup();
		m_pipelineVariance.m_geometryBuffers.reset();
		m_viewport.cleanup();
	}

	void RenderVarianceCubeToTexture::render( castor::Position const & position
		, castor::Size const & size
		, TextureLayout const & texture )
	{
		Position pos = position;
		int w = size.getWidth();
		int h = size.getHeight();
		doRender( pos + Position{ w * 2, h * 1 }
			, size
			, texture
			, Point3f{ 1, 0, 0 }
			, m_pipelineDepth );
		doRender( pos + Position{ w * 1, h * 1 }
			, size
			, texture
			, Point3f{ 0, -1, 0 }
			, m_pipelineDepth );
		doRender( pos + Position{ w * 0, h * 1 }
			, size
			, texture
			, Point3f{ -1, 0, 0 }
			, m_pipelineDepth );
		doRender( pos + Position{ w * 3, h * 1 }
			, size
			, texture
			, Point3f{ 0, 1, 0 }
			, m_pipelineDepth );
		doRender( pos + Position{ w * 1, h * 0 }
			, size
			, texture
			, Point3f{ 0, 0, -1 }
			, m_pipelineDepth );
		doRender( pos + Position{ w * 1, h * 2 }
			, size
			, texture
			, Point3f{ 0, 0, 1 }
			, m_pipelineDepth );
		/*
		pos = Position{ pos.x() + int32_t( w * 4 )
			, pos.y() };
		doRender( pos + Position{ w * 2, h * 1 }
			, size
			, texture
			, Point3f{ 1, 0, 0 }
			, m_pipelineVariance );
		doRender( pos + Position{ w * 1, h * 1 }
			, size
			, texture
			, Point3f{ 0, -1, 0 }
			, m_pipelineVariance );
		doRender( pos + Position{ w * 0, h * 1 }
			, size
			, texture
			, Point3f{ -1, 0, 0 }
			, m_pipelineVariance );
		doRender( pos + Position{ w * 3, h * 1 }
			, size
			, texture
			, Point3f{ 0, 1, 0 }
			, m_pipelineVariance );
		doRender( pos + Position{ w * 1, h * 0 }
			, size
			, texture
			, Point3f{ 0, 0, -1 }
			, m_pipelineVariance );
		doRender( pos + Position{ w * 1, h * 2 }
			, size
			, texture
			, Point3f{ 0, 0, 1 }
			, m_pipelineVariance );*/
	}

	void RenderVarianceCubeToTexture::doRender( Point2i const & position
		, Size const & size
		, TextureLayout const & texture
		, Point3f const & face
		, Pipeline & pipeline )
	{
		REQUIRE( texture.getType() == TextureType::eCube );
		m_viewport.setPosition( Position{ position[0], position[1] } );
		m_viewport.resize( size );
		m_viewport.update();
		m_viewport.apply();

		REQUIRE( pipeline.m_faceUniform );
		pipeline.m_faceUniform->setValue( face );

		m_matrixUbo.update( m_viewport.getProjection() );
		pipeline.m_pipeline->apply();

		texture.bind( 0u );
		m_sampler->bind( 0u );
		pipeline.m_geometryBuffers->draw( uint32_t( m_arrayVertex.size() ), 0u );
		m_sampler->unbind( 0u );
		texture.unbind( 0u );
	}

	ShaderProgramSPtr RenderVarianceCubeToTexture::doCreateDepthProgram()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declUniform< SamplerCube >( ShaderProgram::MapDiffuse );
			auto c3d_face = writer.declUniform< Vec3 >( cuT( "c3d_face" ) );
			auto c3d_v2UvMult = writer.declUniform< Vec2 >( cuT( "c3d_v2UvMult" ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto mapCoord = writer.declLocale( cuT( "mapCoord" ), vtx_texture * 2.0_f - 1.0_f );
				auto uv = writer.declLocale< Vec3 >( cuT( "uv" )
					, writer.ternary( c3d_face.x() != 0.0_f
						, vec3( c3d_face.x(), mapCoord )
						, writer.ternary( c3d_face.y() != 0.0_f
							, vec3( mapCoord.x(), c3d_face.y(), mapCoord.y() )
							, vec3( mapCoord, c3d_face.z() ) ) ) );
				auto depth = writer.declLocale( cuT( "depth" ), texture( c3d_mapDiffuse, uv ).x() );
				plx_v4FragColor = vec4( depth, depth, depth, 1.0 );
			} );
			pxl = writer.finalise();
		}

		auto & cache = renderSystem.getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->createUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->setValue( 0u );
		return program;
	}

	ShaderProgramSPtr RenderVarianceCubeToTexture::doCreateVarianceProgram()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declUniform< SamplerCube >( ShaderProgram::MapDiffuse );
			auto c3d_face = writer.declUniform< Vec3 >( cuT( "c3d_face" ) );
			auto c3d_v2UvMult = writer.declUniform< Vec2 >( cuT( "c3d_v2UvMult" ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto mapCoord = writer.declLocale( cuT( "mapCoord" ), vtx_texture * 2.0_f - 1.0_f );
				auto uv = writer.declLocale< Vec3 >( cuT( "uv" )
					, writer.ternary( c3d_face.x() != 0.0_f
						, vec3( c3d_face.x(), mapCoord )
						, writer.ternary( c3d_face.y() != 0.0_f
							, vec3( mapCoord.x(), c3d_face.y(), mapCoord.y() )
							, vec3( mapCoord, c3d_face.z() ) ) ) );
				auto depth = writer.declLocale( cuT( "depth" ), texture( c3d_mapDiffuse, uv ).y() );
				plx_v4FragColor = vec4( depth, depth, depth, 1.0 );
			} );
			pxl = writer.finalise();
		}

		auto & cache = renderSystem.getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->createUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->setValue( 0u );
		return program;
	}

	void RenderVarianceCubeToTexture::doInitialiseDepth()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		auto & program = *doCreateDepthProgram();
		m_pipelineDepth.m_faceUniform = program.createUniform< UniformType::eVec3f >( cuT( "c3d_face" ), ShaderType::ePixel );
		program.initialise();

		m_pipelineDepth.m_geometryBuffers = renderSystem.createGeometryBuffers( Topology::eTriangles
			, program );
		m_pipelineDepth.m_geometryBuffers->initialise( { *m_vertexBuffer }
			, nullptr );

		DepthStencilState dsState;
		dsState.setDepthTest( false );
		dsState.setDepthMask( WritingMask::eZero );
		m_pipelineDepth.m_pipeline = renderSystem.createRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipelineDepth.m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
	}

	void RenderVarianceCubeToTexture::doInitialiseVariance()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		auto & program = *doCreateVarianceProgram();
		m_pipelineVariance.m_faceUniform = program.createUniform< UniformType::eVec3f >( cuT( "c3d_face" ), ShaderType::ePixel );
		program.initialise();

		m_pipelineVariance.m_geometryBuffers = renderSystem.createGeometryBuffers( Topology::eTriangles
			, program );
		m_pipelineVariance.m_geometryBuffers->initialise( { *m_vertexBuffer }
		, nullptr );

		DepthStencilState dsState;
		dsState.setDepthTest( false );
		dsState.setDepthMask( WritingMask::eZero );
		m_pipelineVariance.m_pipeline = renderSystem.createRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipelineVariance.m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
	}
}
