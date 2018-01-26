#include "RenderColourLayerToTexture.hpp"

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
	RenderColourLayerToTexture::RenderColourLayerToTexture( Context & p_context
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
				ParticleElementDeclaration{ cuT( "position" ), uint32_t( ElementUsage::ePosition ), renderer::AttributeFormat::eVec2 },
				ParticleElementDeclaration{ cuT( "texcoord" ), uint32_t( ElementUsage::eTexCoords ), renderer::AttributeFormat::eVec2 }
			}
		}
	{
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	RenderColourLayerToTexture::~RenderColourLayerToTexture()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RenderColourLayerToTexture::initialise()
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
		m_vertexBuffer->initialise( renderer::MemoryPropertyFlag::eHostVisible );
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

		m_sampler = getOwner()->getRenderSystem()->getEngine()->getSamplerCache().add( cuT( "RenderColourLayerToTexture" ) );
		m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	void RenderColourLayerToTexture::cleanup()
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

	void RenderColourLayerToTexture::render( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, MatrixUbo & p_matrixUbo
		, RenderPipeline & p_pipeline
		, uint32_t p_layer )
	{
		doRender( p_position
			, p_size
			, p_texture
			, p_pipeline
			, p_matrixUbo
			, *m_geometryBuffers
			, p_layer );
	}

	void RenderColourLayerToTexture::render( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, uint32_t p_layer )
	{
		doRender( p_position
			, p_size
			, p_texture
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers
			, p_layer );
	}

	void RenderColourLayerToTexture::doRender( Position const & p_position
		, Size const & p_size
		, TextureLayout const & p_texture
		, RenderPipeline & p_pipeline
		, MatrixUbo & p_matrixUbo
		, GeometryBuffers const & p_geometryBuffers
		, uint32_t p_layer )
	{
		REQUIRE( p_texture.getLayersCount() > p_layer );
		m_viewport.setPosition( p_position );
		m_viewport.resize( p_size );
		m_viewport.update();
		m_viewport.apply();

		REQUIRE( m_layerIndexUniform );
		m_layerIndexUniform->setValue( p_layer );

		p_matrixUbo.update( m_viewport.getProjection() );
		p_pipeline.apply();
		m_layerIndexUniform->update();

		p_texture.bind( MinTextureIndex );
		m_sampler->bind( MinTextureIndex );
		p_geometryBuffers.draw( uint32_t( m_arrayVertex.size() ), 0u );
		m_sampler->unbind( MinTextureIndex );
		p_texture.unbind( MinTextureIndex );
	}

	ShaderProgramSPtr RenderColourLayerToTexture::doCreateProgram()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer, 0 );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ) );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ) );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_projection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2DArray >( cuT( "c3d_mapDiffuse" ), MinTextureIndex, 0u );
			auto c3d_iIndex = writer.declUniform< Int >( cuT( "c3d_iIndex" ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vec3( vtx_texture, c3d_iIndex ) ).xyz(), 1.0 );
			} );
			pxl = writer.finalise();
		}

		auto & cache = renderSystem.getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->createUniform< UniformType::eSampler >( cuT( "c3d_mapDiffuse" ), ShaderType::ePixel )->setValue( MinTextureIndex );
		m_layerIndexUniform = program->createUniform< UniformType::eInt >( cuT( "c3d_iIndex" ), ShaderType::ePixel );
		program->initialise();
		return program;
	}
}
