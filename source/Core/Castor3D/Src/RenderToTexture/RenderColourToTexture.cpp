#include "RenderColourToTexture.hpp"

#include "Engine.hpp"

#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderColourToTexture::RenderColourToTexture( Context & p_context
		, MatrixUbo & matrixUbo
		, bool p_invertU )
		: OwnedBy< Context >{ p_context }
		, m_matrixUbo{ matrixUbo }
		, m_viewport{ *p_context.getRenderSystem()->getEngine() }
		, m_bufferVertex
		{
			{
				0, 0, ( p_invertU ? 1.0f : 0.0f ), 0,
				1, 1, ( p_invertU ? 0.0f : 1.0f ), 1,
				0, 1, ( p_invertU ? 1.0f : 0.0f ), 1,
				0, 0, ( p_invertU ? 1.0f : 0.0f ), 0,
				1, 0, ( p_invertU ? 0.0f : 1.0f ), 0,
				1, 1, ( p_invertU ? 0.0f : 1.0f ), 1
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

	RenderColourToTexture::~RenderColourToTexture()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RenderColourToTexture::initialise()
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
		dsState.setDepthMask( WritingMask::eZero );
		m_pipeline = renderSystem.createRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );

		String const name = cuT( "RenderColourToTexture" );
		SamplerSPtr sampler;

		if ( getOwner()->getRenderSystem()->getEngine()->getSamplerCache().has( name + cuT( "_Linear" ) ) )
		{
			m_samplerLinear = getOwner()->getRenderSystem()->getEngine()->getSamplerCache().find( name + cuT( "_Linear" ) );
		}
		else
		{
			m_samplerLinear = getOwner()->getRenderSystem()->getEngine()->getSamplerCache().add( name + cuT( "_Linear" ) );
			m_samplerLinear->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_samplerLinear->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_samplerLinear->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			m_samplerLinear->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			m_samplerLinear->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		}

		if ( getOwner()->getRenderSystem()->getEngine()->getSamplerCache().has( name + cuT( "_Nearest" ) ) )
		{
			m_samplerNearest = getOwner()->getRenderSystem()->getEngine()->getSamplerCache().find( name + cuT( "_Nearest" ) );
		}
		else
		{
			m_samplerNearest = getOwner()->getRenderSystem()->getEngine()->getSamplerCache().add( name + cuT( "_Nearest" ) );
			m_samplerNearest->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
			m_samplerNearest->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
			m_samplerNearest->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			m_samplerNearest->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			m_samplerNearest->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
		}

		m_viewport.update();
	}

	void RenderColourToTexture::cleanup()
	{
		m_samplerNearest.reset();
		m_samplerLinear.reset();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_viewport.cleanup();
	}

	void RenderColourToTexture::render( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, MatrixUbo & matrixUbo
		, RenderPipeline & pipeline )
	{
		doRender( position
			, size
			, texture
			, pipeline
			, matrixUbo
			, *m_geometryBuffers
			, *m_samplerLinear );
	}

	void RenderColourToTexture::renderNearest( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, MatrixUbo & matrixUbo
		, RenderPipeline & pipeline )
	{
		doRender( position
			, size
			, texture
			, pipeline
			, matrixUbo
			, *m_geometryBuffers
			, *m_samplerNearest );
	}

	void RenderColourToTexture::render( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		doRender( position
			, size
			, texture
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers
			, *m_samplerLinear );
	}

	void RenderColourToTexture::renderNearest( Position const & position
		, Size const & size
		, TextureLayout const & texture )
	{
		doRender( position
			, size
			, texture
			, *m_pipeline
			, m_matrixUbo
			, *m_geometryBuffers
			, *m_samplerNearest );
	}

	void RenderColourToTexture::doRender( Position const & position
		, Size const & size
		, TextureLayout const & texture
		, RenderPipeline & pipeline
		, MatrixUbo & matrixUbo
		, GeometryBuffers const & geometryBuffers
		, Sampler const & sampler )
	{
		m_viewport.setPosition( position );
		m_viewport.resize( size );
		m_viewport.apply();

		matrixUbo.update( m_viewport.getProjection() );
		pipeline.apply();

		texture.bind( MinTextureIndex );
		sampler.bind( MinTextureIndex );
		geometryBuffers.draw( uint32_t( m_arrayVertex.size() ), 0u );
		sampler.unbind( MinTextureIndex );
		texture.unbind( MinTextureIndex );
	}

	ShaderProgramSPtr RenderColourToTexture::doCreateProgram()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			UBO_MATRIX( writer, 0 );

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.declAttribute< Vec2 >( ShaderProgram::Texture );

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
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse, MinTextureIndex, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
			} );
			pxl = writer.finalise();
		}

		auto & cache = renderSystem.getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->createUniform< UniformType::eSampler >( ShaderProgram::MapDiffuse, ShaderType::ePixel )->setValue( MinTextureIndex );
		program->initialise();
		return program;
	}
}
