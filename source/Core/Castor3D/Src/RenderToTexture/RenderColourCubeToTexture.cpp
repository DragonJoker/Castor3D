#include "RenderColourCubeToTexture.hpp"

#include "Engine.hpp"

#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/MatrixUbo.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderColourCubeToTexture::RenderColourCubeToTexture( Context & p_context
		, MatrixUbo & p_matrixUbo )
		: OwnedBy< Context >{ p_context }
		, m_matrixUbo{ p_matrixUbo }
		, m_viewport{ *p_context.getRenderSystem()->getEngine() }
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

	RenderColourCubeToTexture::~RenderColourCubeToTexture()
	{
	}

	void RenderColourCubeToTexture::initialise()
	{
		m_viewport.initialise();
		auto & program = *doCreateProgram();
		auto & renderSystem = *getOwner()->getRenderSystem();
		program.initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *renderSystem.getEngine()
			, m_declaration );
		m_vertexBuffer->resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->linkCoords( m_arrayVertex.begin()
			, m_arrayVertex.end() );
		m_vertexBuffer->initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );
		m_geometryBuffers = renderSystem.createGeometryBuffers( Topology::eTriangles
			, program );
		m_geometryBuffers->initialise( { *m_vertexBuffer }
		, nullptr );

		DepthStencilState dsState;
		dsState.setDepthTest( false );
		m_pipeline = renderSystem.createRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		m_sampler = getOwner()->getRenderSystem()->getEngine()->getSamplerCache().add( cuT( "RenderColourCubeToTexture" ) );
		m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	void RenderColourCubeToTexture::cleanup()
	{
		m_sampler.reset();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_viewport.cleanup();
	}

	void RenderColourCubeToTexture::render( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture )
	{
		int w = p_size.getWidth();
		int h = p_size.getHeight();
		doRender( Position{ p_position.x() + w * 0, p_position.y() + h * 1 }
			, p_size
			, p_texture
			, Point3f{ -1, 0, 0 }
			, Point2f{ -1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		doRender( Position{ p_position.x() + w * 1, p_position.y() + h * 1 }
			, p_size
			, p_texture
			, Point3f{ 0, -1, 0 }
			, Point2f{ 1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		doRender( Position{ p_position.x() + w * 2, p_position.y() + h * 1 }
			, p_size
			, p_texture
			, Point3f{ 1, 0, 0 }
			, Point2f{ 1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		doRender( Position{ p_position.x() + w * 3, p_position.y() + h * 1 }
			, p_size
			, p_texture
			, Point3f{ 0, 1, 0 }
			, Point2f{ 1, -1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		doRender( Position{ p_position.x() + w * 1, p_position.y() + h * 0 }
			, p_size
			, p_texture
			, Point3f{ 0, 0, -1 }
			, Point2f{ 1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
		doRender( Position{ p_position.x() + w * 1, p_position.y() + h * 2 }
			, p_size
			, p_texture
			, Point3f{ 0, 0, 1 }
			, Point2f{ 1, 1 }
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers );
	}

	void RenderColourCubeToTexture::doRender( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, Point3f const & p_face
		, castor::Point2f const & p_uvMult
		, RenderPipeline & p_pipeline
		, MatrixUbo & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers )
	{
		REQUIRE( p_texture.getType() == TextureType::eCube );
		m_viewport.setPosition( p_position );
		m_viewport.resize( p_size );
		m_viewport.update();
		m_viewport.apply();

		REQUIRE( m_faceUniform );
		m_faceUniform->setValue( p_face );

		p_matrixUbo.update( m_viewport.getProjection() );
		p_pipeline.apply();

		p_texture.bind( 0u );
		m_sampler->bind( 0u );
		p_geometryBuffers.draw( uint32_t( m_arrayVertex.size() ), 0 );
		m_sampler->unbind( 0u );
		p_texture.unbind( 0u );
	}

	ShaderProgramSPtr RenderColourCubeToTexture::doCreateProgram()
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
			auto c3d_v3Face = writer.declUniform< Vec3 >( cuT( "c3d_v3Face" ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto plx_v4FragColor = writer.declFragData< Vec4 >( cuT( "plx_v4FragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto mapCoord = writer.declLocale( cuT( "mapCoord" ), vtx_texture * 2.0_f - 1.0_f );
				auto uv = writer.declLocale< Vec3 >( cuT( "uv" )
					, writer.ternary( c3d_v3Face.x() != 0.0_f
						, vec3( c3d_v3Face.x(), mapCoord )
						, writer.ternary( c3d_v3Face.y() != 0.0_f
							, vec3( mapCoord.x(), c3d_v3Face.y(), mapCoord.y() )
							, vec3( mapCoord, c3d_v3Face.z() ) ) ) );
				plx_v4FragColor = vec4( texture( c3d_mapDiffuse, uv ).xyz(), 1.0 );
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
		m_faceUniform = program->createUniform< UniformType::eVec3f >( cuT( "c3d_v3Face" ), ShaderType::ePixel );
		program->initialise();
		return program;
	}
}
