#include "RenderVarianceToTexture.hpp"

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
	RenderVarianceToTexture::RenderVarianceToTexture( Context & context
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

	RenderVarianceToTexture::~RenderVarianceToTexture()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RenderVarianceToTexture::initialise()
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

		doInitialiseDepth();
		doInitialiseVariance();

		m_sampler = renderSystem.getEngine()->getSamplerCache().add( cuT( "RenderVarianceToTexture" ) );
		m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	void RenderVarianceToTexture::cleanup()
	{
		m_sampler.reset();
		m_pipelineDepth->cleanup();
		m_pipelineDepth.reset();
		m_pipelineVariance->cleanup();
		m_pipelineVariance.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffersDepth->cleanup();
		m_geometryBuffersDepth.reset();
		m_geometryBuffersVariance->cleanup();
		m_geometryBuffersVariance.reset();
		m_viewport.cleanup();
	}

	void RenderVarianceToTexture::render( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		doRender( position
			, size
			, texture
			, *m_pipelineDepth
			, m_matrixUbo
			, *m_geometryBuffersDepth );
		doRender( Position{ int32_t( position.x() + size.getWidth() ), position.y() }
			, size
			, texture
			, *m_pipelineVariance
			, m_matrixUbo
			, *m_geometryBuffersVariance );
	}

	void RenderVarianceToTexture::doRender( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, RenderPipeline & pipeline
		, MatrixUbo & matrixUbo
		, GeometryBuffers const & geometryBuffers )
	{
		m_viewport.setPosition( position );
		m_viewport.resize( size );
		m_viewport.update();
		m_viewport.apply();
		
		matrixUbo.update( m_viewport.getProjection() );
		pipeline.apply();

		texture.bind( 0u );
		m_sampler->bind( 0u );
		geometryBuffers.draw( uint32_t( m_arrayVertex.size() ), 0u );
		m_sampler->unbind( 0u );
		texture.unbind( 0u );
	}

	ShaderProgramSPtr RenderVarianceToTexture::doCreateProgramDepth()
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
			auto c3d_mapDiffuse = writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto depth = writer.declLocale( cuT( "depth" ), texture( c3d_mapDiffuse, vtx_texture.xy() ).x() );
				depth = 1.0_f - writer.paren( 1.0_f - depth ) * 25.0f;
				plx_v4FragColor = vec4( vec3( depth ), 1.0 );
			} );
			pxl = writer.finalise();
		}

		auto & cache = renderSystem.getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->createUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		program->initialise();
		return program;
	}

	ShaderProgramSPtr RenderVarianceToTexture::doCreateProgramVariance()
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
			auto c3d_mapDiffuse = writer.declUniform< Sampler2D >( ShaderProgram::MapDiffuse );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto depth = writer.declLocale( cuT( "depth" ), texture( c3d_mapDiffuse, vtx_texture.xy() ).y() );
				//depth = 1.0_f - writer.paren( 1.0_f - depth ) * 25.0f;
				plx_v4FragColor = vec4( vec3( glsl::sqrt( depth ) ), 1.0 );
			} );
			pxl = writer.finalise();
		}

		auto & cache = renderSystem.getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->createUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		program->initialise();
		return program;
	}

	void RenderVarianceToTexture::doInitialiseDepth()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		auto & program = *doCreateProgramDepth();
		program.initialise();
		m_geometryBuffersDepth = renderSystem.createGeometryBuffers( Topology::eTriangles
			, program );
		m_geometryBuffersDepth->initialise( { *m_vertexBuffer }
			, nullptr );
		DepthStencilState dsState;
		dsState.setDepthTest( false );
		dsState.setDepthMask( WritingMask::eZero );
		m_pipelineDepth = renderSystem.createRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipelineDepth->addUniformBuffer( m_matrixUbo.getUbo() );
	}

	void RenderVarianceToTexture::doInitialiseVariance()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		auto & program = *doCreateProgramVariance();
		program.initialise();
		m_geometryBuffersVariance = renderSystem.createGeometryBuffers( Topology::eTriangles
			, program );
		m_geometryBuffersVariance->initialise( { *m_vertexBuffer }
			, nullptr );
		DepthStencilState dsState;
		dsState.setDepthTest( false );
		dsState.setDepthMask( WritingMask::eZero );
		m_pipelineVariance = renderSystem.createRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipelineVariance->addUniformBuffer( m_matrixUbo.getUbo() );
	}
}
