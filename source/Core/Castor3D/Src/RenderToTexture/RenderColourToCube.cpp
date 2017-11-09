#include "RenderColourToCube.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

using namespace castor;

namespace castor3d
{
	RenderColourToCube::RenderColourToCube( Context & p_context
		, MatrixUbo & p_matrixUbo )
		: OwnedBy< Context >{ p_context }
		, m_matrixUbo{ p_matrixUbo }
		, m_viewport{ *p_context.getRenderSystem()->getEngine() }
		, m_bufferVertex
		{
			{
				-1, +1, -1, /**/+1, -1, -1, /**/-1, -1, -1, /**/+1, -1, -1, /**/-1, +1, -1, /**/+1, +1, -1,
				-1, -1, +1, /**/-1, +1, -1, /**/-1, -1, -1, /**/-1, +1, -1, /**/-1, -1, +1, /**/-1, +1, +1,
				+1, -1, -1, /**/+1, +1, +1, /**/+1, -1, +1, /**/+1, +1, +1, /**/+1, -1, -1, /**/+1, +1, -1,
				-1, -1, +1, /**/+1, +1, +1, /**/-1, +1, +1, /**/+1, +1, +1, /**/-1, -1, +1, /**/+1, -1, +1,
				-1, +1, -1, /**/+1, +1, +1, /**/+1, +1, -1, /**/+1, +1, +1, /**/-1, +1, -1, /**/-1, +1, +1,
				-1, -1, -1, /**/+1, -1, -1, /**/-1, -1, +1, /**/+1, -1, -1, /**/+1, -1, +1, /**/-1, -1, +1,
			}
		}
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 }
			}
		}
		, m_configUbo{ *p_context.getRenderSystem()->getEngine() }
	{
		m_dummy.setExposure( 1.0f );
		m_dummy.setGamma( 1.0f );
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}
	}

	RenderColourToCube::~RenderColourToCube()
	{
		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RenderColourToCube::initialise()
	{
		m_viewport.initialise();
		auto & program = *doCreateProgram();
		auto & renderSystem = *getOwner()->getRenderSystem();
		program.initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *renderSystem.getEngine()
			, m_declaration );
		m_vertexBuffer->resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->linkCoords( m_arrayVertex.begin(),
			m_arrayVertex.end() );
		m_vertexBuffer->initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );
		m_geometryBuffers = renderSystem.createGeometryBuffers( Topology::eTriangles
			, program );
		m_geometryBuffers->initialise( { *m_vertexBuffer }
			, nullptr );

		DepthStencilState dsState;
		dsState.setDepthFunc( DepthFunc::eLEqual );
		dsState.setDepthTest( false );
		dsState.setDepthMask( WritingMask::eAll );

		RasteriserState rsState;
		rsState.setCulledFaces( Culling::eFront );

		m_pipeline = renderSystem.createRenderPipeline( std::move( dsState )
			, std::move( rsState )
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->addUniformBuffer( m_matrixUbo.getUbo() );
		m_pipeline->addUniformBuffer( m_configUbo.getUbo() );

		m_sampler = renderSystem.getEngine()->getSamplerCache().add( cuT( "RenderColourToCube" ) );
		m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
	}

	void RenderColourToCube::cleanup()
	{
		m_configUbo.getUbo().cleanup();
		m_sampler.reset();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_viewport.cleanup();
	}

	void RenderColourToCube::render( Size const & p_size
		, TextureLayout const & p_2dTexture
		, TextureLayoutSPtr p_cubeTexture
		, FrameBufferSPtr p_fbo
		, std::array< FrameBufferAttachmentSPtr, 6 > const & p_attachs
		, HdrConfig const & hdrConfig )
	{
		bool hdr = p_2dTexture.getPixelFormat() == PixelFormat::eRGB16F
			|| p_2dTexture.getPixelFormat() == PixelFormat::eRGBA16F
			|| p_2dTexture.getPixelFormat() == PixelFormat::eRGB16F32F
			|| p_2dTexture.getPixelFormat() == PixelFormat::eRGBA16F32F
			|| p_2dTexture.getPixelFormat() == PixelFormat::eRGB32F
			|| p_2dTexture.getPixelFormat() == PixelFormat::eRGBA32F;

		if ( hdr )
		{
			m_configUbo.update( m_dummy );
		}
		else
		{
			m_configUbo.update( hdrConfig );
		}

		m_sampler->initialise();
		static Matrix4x4r const projection = matrix::perspective( Angle::fromDegrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
		static Matrix4x4r const views[] =
		{
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } )
		};

		m_viewport.resize( p_size );
		m_viewport.apply();
		p_fbo->bind( FrameBufferTarget::eDraw );
		p_fbo->clear( BufferComponent::eColour | BufferComponent::eDepth );
		p_2dTexture.bind( MinTextureIndex );

		for ( uint32_t i = 0; i < 6; ++i )
		{
			p_attachs[i]->attach( AttachmentPoint::eColour, 0u );
			p_fbo->setDrawBuffer( p_attachs[i] );
			m_matrixUbo.update( views[i], projection );
			m_pipeline->apply();

			m_sampler->bind( MinTextureIndex );
			m_geometryBuffers->draw( uint32_t( m_arrayVertex.size() ), 0u );
			m_sampler->unbind( MinTextureIndex );
		}

		p_2dTexture.unbind( MinTextureIndex );
		p_fbo->unbind();
	}

	ShaderProgramSPtr RenderColourToCube::doCreateProgram()
	{
		auto & renderSystem = *getOwner()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( writer );

			// Outputs
			auto vtx_worldPosition = writer.declOutput< Vec3 >( cuT( "vtx_worldPosition" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_worldPosition = position;
				gl_Position = writer.paren( c3d_projection * c3d_curView * vec4( vtx_worldPosition, 1.0 ) );
			} );

			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			UBO_HDR_CONFIG( writer );
			auto vtx_worldPosition = writer.declInput< Vec3 >( cuT( "vtx_worldPosition" ) );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( ShaderProgram::MapDiffuse, MinTextureIndex );

			glsl::Utils utils{ writer };
			utils.declareRemoveGamma();

			// Outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			auto sampleSphericalMap = writer.implementFunction< Vec2 >( cuT( "SampleSphericalMap" ), [&]( Vec3 const & v )
			{
				auto uv = writer.declLocale( cuT( "uv" ), vec2( atan( v.z(), v.x() ), asin( v.y() ) ) );
				uv *= vec2( 0.1591_f, 0.3183_f );
				uv += 0.5_f;
				writer.returnStmt( uv );
			}, InVec3( &writer, cuT( "v" ) ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto uv = writer.declLocale( cuT( "uv" ), sampleSphericalMap( normalize( vtx_worldPosition ) ) );
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vec2( uv.x(), 1.0_r - uv.y() ) ).rgb(), 1.0_f );
				pxl_fragColor = vec4( utils.removeGamma( c3d_gamma, pxl_fragColor.xyz() ), pxl_fragColor.w() );
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
