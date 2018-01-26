#include "TextureProjection.hpp"

#include "Engine.hpp"

#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	TextureProjection::TextureProjection( Context & p_context )
		: OwnedBy< Context >{ p_context }
		, m_matrixUbo{ *p_context.getRenderSystem()->getEngine() }
		, m_modelMatrixUbo{ *p_context.getRenderSystem()->getEngine() }
		, m_bufferVertex
		{
			{
				-1, +1, -1,/**/+1, -1, -1,/**/-1, -1, -1,/**/+1, -1, -1,/**/-1, +1, -1,/**/+1, +1, -1,/**/
				-1, -1, +1,/**/-1, +1, -1,/**/-1, -1, -1,/**/-1, +1, -1,/**/-1, -1, +1,/**/-1, +1, +1,/**/
				+1, -1, -1,/**/+1, +1, +1,/**/+1, -1, +1,/**/+1, +1, +1,/**/+1, -1, -1,/**/+1, +1, -1,/**/
				-1, -1, +1,/**/+1, +1, +1,/**/-1, +1, +1,/**/+1, +1, +1,/**/-1, -1, +1,/**/+1, -1, +1,/**/
				-1, +1, -1,/**/+1, +1, +1,/**/+1, +1, -1,/**/+1, +1, +1,/**/-1, +1, -1,/**/-1, +1, +1,/**/
				-1, -1, -1,/**/+1, -1, -1,/**/-1, -1, +1,/**/+1, -1, -1,/**/+1, -1, +1,/**/-1, -1, +1,/**/
			}
		}
		, m_declaration
		{
			{
				ParticleElementDeclaration{ cuT( "position" ), uint32_t( ElementUsage::ePosition ), renderer::AttributeFormat::eVec3 }
			}
		}
	{
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_sampler = p_context.getRenderSystem()->getEngine()->getSamplerCache().add( cuT( "TextureProjection" ) );
		m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	TextureProjection::~TextureProjection()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void TextureProjection::initialise()
	{
		m_sampler->initialise();
		auto & program = doInitialiseShader();
		doInitialiseVertexBuffer();
		doInitialisePipeline( program );
	}

	void TextureProjection::cleanup()
	{
		m_matrixUbo.getUbo().cleanup();
		m_modelMatrixUbo.getUbo().cleanup();
		m_sampler.reset();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
	}

	void TextureProjection::render( TextureLayout const & p_texture
		, Camera const & p_camera )
	{
		static Matrix3x3r const Identity{ 1.0f };
		auto node = p_camera.getParent();
		matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
		m_matrixUbo.update( p_camera.getView()
			, p_camera.getViewport().getProjection() );
		m_modelMatrixUbo.update( m_mtxModel, Identity );
		m_sizeUniform->setValue( Point2f{ p_camera.getViewport().getWidth()
			, p_camera.getViewport().getHeight() } );
		p_camera.apply();
		m_pipeline->apply();
		p_texture.bind( MinTextureIndex );
		m_sampler->bind( MinTextureIndex );
		m_geometryBuffers->draw( uint32_t( m_arrayVertex.size() ), 0u );
		m_sampler->unbind( MinTextureIndex );
		p_texture.unbind( MinTextureIndex );
	}

	ShaderProgram & TextureProjection::doInitialiseShader()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ) );
			UBO_MATRIX( writer, 0 );
			UBO_MODEL_MATRIX( writer, 0 );

			// Outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = writer.paren( c3d_projection * c3d_curView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			} );

			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), MinTextureIndex, 0u );
			auto c3d_size = writer.declUniform< Vec2 >( cuT( "c3d_size" ) );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Outputs
			auto pxl_FragColor = writer.declOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_FragColor = texture( c3d_mapDiffuse, gl_FragCoord.xy() / c3d_size );
			} );

			pxl = writer.finalise();
		}

		auto & cache = renderSystem.getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		m_sizeUniform = program->createUniform< UniformType::eVec2f >( cuT( "c3d_size" ), ShaderType::ePixel );
		program->initialise();
		return *program;
	}

	bool TextureProjection::doInitialiseVertexBuffer()
	{
		m_vertexBuffer = std::make_shared< VertexBuffer >( *getOwner()->getRenderSystem()->getEngine(), m_declaration );
		m_vertexBuffer->resize( uint32_t( m_arrayVertex.size() * m_declaration.stride() ) );
		m_vertexBuffer->linkCoords( m_arrayVertex.begin(), m_arrayVertex.end() );
		return m_vertexBuffer->initialise( renderer::MemoryPropertyFlag::eHostVisible );
	}

	bool TextureProjection::doInitialisePipeline( ShaderProgram & p_program )
	{
		DepthStencilState dsState;
		dsState.setDepthTest( true );
		dsState.setDepthMask( WritingMask::eZero );
		dsState.setDepthFunc( DepthFunc::eLEqual );

		RasteriserState rsState;
		rsState.setCulledFaces( Culling::eFront );

		m_pipeline = getOwner()->getRenderSystem()->createRenderPipeline( std::move( dsState )
			, std::move( rsState )
			, BlendState{}
			, MultisampleState{}
			, p_program
			, PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_modelMatrixUbo.getUbo() );
		m_geometryBuffers = getOwner()->getRenderSystem()->createGeometryBuffers( Topology::eTriangles
			, m_pipeline->getProgram() );
		return m_geometryBuffers->initialise( { *m_vertexBuffer }, nullptr );
	}
}
