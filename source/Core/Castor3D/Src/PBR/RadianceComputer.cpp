#include "RadianceComputer.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
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
	RadianceComputer::RadianceComputer( Engine & engine
		, castor::Size const & p_size )
		: OwnedBy< Engine >{ engine }
		, m_matrixUbo{ engine }
		, m_viewport{ engine }
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
		, m_size{ p_size }
	{
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_viewport.initialise();
		m_viewport.setPerspective( Angle::fromDegrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
		m_viewport.resize( m_size );
		m_viewport.update();
		auto & program = *doCreateProgram();
		auto & renderSystem = *getEngine()->getRenderSystem();
		m_vertexBuffer = std::make_shared< VertexBuffer >( *getEngine()
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

		m_frameBuffer = renderSystem.createFrameBuffer();

		m_depthBuffer = m_frameBuffer->createDepthStencilRenderBuffer( PixelFormat::eD24 );
		m_depthBuffer->create();
		m_depthBuffer->initialise( m_size );
		m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );

		m_frameBuffer->initialise();
		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
		REQUIRE( m_frameBuffer->isComplete() );
		m_frameBuffer->unbind();

		if ( !getEngine()->getSamplerCache().has( cuT( "RadianceComputer" ) ) )
		{
			m_sampler = getEngine()->getSamplerCache().add( cuT( "RadianceComputer" ) );
			m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_sampler->initialise();
		}
		else
		{
			m_sampler = getEngine()->getSamplerCache().find( cuT( "RadianceComputer" ) );
		}
	}

	RadianceComputer::~RadianceComputer()
	{
		m_sampler.reset();
		m_frameBuffer->bind();
		m_frameBuffer->detachAll();
		m_frameBuffer->unbind();
		m_frameBuffer->cleanup();
		m_frameBuffer.reset();
		m_depthAttach.reset();
		m_depthBuffer->cleanup();
		m_depthBuffer->destroy();
		m_depthBuffer.reset();
		m_pipeline->cleanup();
		m_pipeline.reset();
		m_vertexBuffer->cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->cleanup();
		m_geometryBuffers.reset();
		m_viewport.cleanup();
		m_matrixUbo.getUbo().cleanup();

		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void RadianceComputer::render( TextureLayout const & p_srcTexture
		, TextureLayoutSPtr p_dstTexture )
	{
		static Matrix4x4r const views[] =
		{
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ -1.0f, +0.0f, +0.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, +1.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, -1.0f, +0.0f }, Point3r{ 0.0f, +0.0f, -1.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, +1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } ),
			matrix::lookAt( Point3r{ 0.0f, 0.0f, 0.0f }, Point3r{ +0.0f, +0.0f, -1.0f }, Point3r{ 0.0f, -1.0f, +0.0f } )
		};
		REQUIRE( p_dstTexture->getDimensions() == m_size );
		std::array< FrameBufferAttachmentSPtr, 6 > attachs
		{
			{
				m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::ePositiveX ),
				m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::eNegativeX ),
				m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::ePositiveY ),
				m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::eNegativeY ),
				m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::ePositiveZ ),
				m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::eNegativeZ ),
			}
		};

		m_viewport.apply();
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		p_srcTexture.bind( 0u );
		m_sampler->bind( 0u );

		for ( uint32_t i = 0u; i < 6u; ++i )
		{
			attachs[i]->attach( AttachmentPoint::eColour, 0u );
			m_frameBuffer->setDrawBuffer( attachs[i] );
			m_frameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth );
			REQUIRE( m_frameBuffer->isComplete() );
			m_matrixUbo.update( views[i], m_viewport.getProjection() );
			m_pipeline->apply();
			m_geometryBuffers->draw( uint32_t( m_arrayVertex.size() ), 0u );
		}

		m_sampler->unbind( 0u );
		p_srcTexture.unbind( 0u );
		m_frameBuffer->unbind();
	}

	ShaderProgramSPtr RadianceComputer::doCreateProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( ShaderProgram::Position );
			UBO_MATRIX( writer );

			// Outputs
			auto vtx_position = writer.declOutput< Vec3 >( cuT( "vtx_position" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				vtx_position = position;
				auto view = writer.declLocale( cuT( "normal" )
					, mat4( mat3( c3d_mtxView ) ) );
				gl_Position = writer.paren( c3d_mtxProjection * view * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto vtx_position = writer.declInput< Vec3 >( cuT( "vtx_position" ) );
			auto c3d_mapDiffuse = writer.declUniform< SamplerCube >( ShaderProgram::MapDiffuse );

			// Outputs
			auto plx_v4FragColor = writer.declOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				// From https://learnopengl.com/#!PBR/Lighting
				// the sample direction equals the hemisphere's orientation 
				auto normal = writer.declLocale( cuT( "normal" )
					, normalize( vtx_position ) );

				auto irradiance = writer.declLocale( cuT( "irradiance" )
					, vec3( 0.0_f ) );

				auto up = writer.declLocale( cuT( "up" )
					, vec3( 0.0_f, 1.0, 0.0 ) );
				auto right = writer.declLocale( cuT( "right" )
					, cross( up, normal ) );
				up = cross( normal, right );

				auto sampleDelta = writer.declLocale( cuT( "sampleDelta" )
					, 0.025_f );
				auto nrSamples = writer.declLocale( cuT( "nrSamples" )
					, 0_i );
				auto PI = writer.declLocale( cuT( "PI" )
					, 3.14159265359_f );

				FOR( writer, Float, phi, 0.0, "phi < 2.0 * PI", "phi += sampleDelta" )
				{
					FOR( writer, Float, theta, 0.0, "theta < 0.5 * PI", "theta += sampleDelta" )
					{
						// spherical to cartesian (in tangent space)
						auto tangentSample = writer.declLocale( cuT( "tangentSample" )
							, vec3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
						// tangent space to world
						auto sampleVec = writer.declLocale( cuT( "sampleVec" )
							, right * tangentSample.x() + up * tangentSample.y() + normal * tangentSample.z() );

						irradiance += texture( c3d_mapDiffuse, sampleVec ).rgb() * cos( theta ) * sin( theta );
						nrSamples = nrSamples + 1;
					}
					ROF;
				}
				ROF;

				irradiance = irradiance * PI * writer.paren( 1.0_f / writer.cast< Float >( nrSamples ) );
				plx_v4FragColor = vec4( irradiance, 1.0 );
			} );

			pxl = writer.finalise();
		}

		auto & cache = getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->createUniform< UniformType::eInt >( ShaderProgram::MapDiffuse, ShaderType::ePixel );
		program->initialise();
		return program;
	}
}
