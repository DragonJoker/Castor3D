#include "EnvironmentPrefilter.hpp"

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
	EnvironmentPrefilter::EnvironmentPrefilter( Engine & engine
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
		, m_configUbo{ cuT( "Config" )
			, *engine.getRenderSystem()
			, 10u }
		, m_roughnessUniform{ *m_configUbo.createUniform< UniformType::eFloat >( cuT( "c3d_roughness" ) ) }
	{
		uint32_t i = 0;

		for ( auto & vertex : m_arrayVertex )
		{
			vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_viewport.initialise();
		m_viewport.setPerspective( Angle::fromDegrees( 90.0_r ), 1.0_r, 0.1_r, 10.0_r );
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
		m_pipeline->addUniformBuffer( m_configUbo );

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

		if ( !getEngine()->getSamplerCache().has( cuT( "EnvironmentPrefilter" ) ) )
		{
			m_sampler = getEngine()->getSamplerCache().add( cuT( "EnvironmentPrefilter" ) );
			m_sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			m_sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			m_sampler->setInterpolationMode( InterpolationFilter::eMip, InterpolationMode::eLinear );
			m_sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
			m_sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
			m_sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
			m_sampler->initialise();
		}
		else
		{
			m_sampler = getEngine()->getSamplerCache().find( cuT( "EnvironmentPrefilter" ) );
		}
	}

	EnvironmentPrefilter::~EnvironmentPrefilter()
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
		m_configUbo.cleanup();

		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void EnvironmentPrefilter::render( TextureLayout const & p_srcTexture
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
		p_srcTexture.bind( MinTextureIndex );
		m_sampler->bind( MinTextureIndex );
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		REQUIRE( m_frameBuffer->isComplete() );

		for ( unsigned int mip = 0; mip < glsl::Utils::MaxIblReflectionLod + 1; ++mip )
		{
			Size mipSize{ uint32_t( m_size.getWidth() * std::pow( 0.5, mip ) )
				, uint32_t( m_size.getHeight() * std::pow( 0.5, mip ) ) };
			std::array< FrameBufferAttachmentSPtr, 6 > attachs
			{
				{
					m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::ePositiveX, mip ),
					m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::eNegativeX, mip ),
					m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::ePositiveY, mip ),
					m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::eNegativeY, mip ),
					m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::ePositiveZ, mip ),
					m_frameBuffer->createAttachment( p_dstTexture, CubeMapFace::eNegativeZ, mip ),
				}
			};

			m_depthBuffer->resize( mipSize );
			m_viewport.resize( mipSize );
			m_viewport.apply();
			m_roughnessUniform.setValue( mip / float( glsl::Utils::MaxIblReflectionLod ) );
			m_configUbo.update();
			m_configUbo.bindTo( 10u );

			for ( uint32_t i = 0u; i < 6u; ++i )
			{
				attachs[i]->attach( AttachmentPoint::eColour, 0u );
				REQUIRE( m_frameBuffer->isComplete() );
				m_frameBuffer->setDrawBuffer( attachs[i] );
				m_frameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth );
				m_matrixUbo.update( views[i], m_viewport.getProjection() );
				m_pipeline->apply();
				m_geometryBuffers->draw( uint32_t( m_arrayVertex.size() ), 0u );
			}
		}

		m_frameBuffer->unbind();
		m_sampler->unbind( MinTextureIndex );
		p_srcTexture.unbind( MinTextureIndex );
	}

	ShaderProgramSPtr EnvironmentPrefilter::doCreateProgram()
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
			Ubo config{ writer, cuT( "Config" ), 0u };
			auto c3d_roughness = config.declMember< Float >( cuT( "c3d_roughness" ) );
			config.end();

			// Outputs
			auto plx_v4FragColor = writer.declOutput< Vec4 >( cuT( "pxl_FragColor" ) );

			auto distributionGGX = writer.implementFunction< Float >( cuT( "DistributionGGX" )
				, [&]( Vec3 const & p_N
					, Vec3 const & p_H
					, Float const & p_roughness )
				{
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.declLocale( cuT( "a" )
						, p_roughness * p_roughness );
					auto a2 = writer.declLocale( cuT( "a2" )
						, a * a );
					auto NdotH = writer.declLocale( cuT( "NdotH" )
						, max( dot( p_N, p_H ), 0.0 ) );
					auto NdotH2 = writer.declLocale( cuT( "NdotH2" )
						, NdotH * NdotH );

					auto nom = writer.declLocale( cuT( "nom" )
						, a2 );
					auto denom = writer.declLocale( cuT( "denom" )
						, ( NdotH2 * writer.paren( a2 - 1.0 ) + 1.0 ) );
					denom = denom * denom * PI;

					writer.returnStmt( nom / denom );
				}
				, InVec3{ &writer, cuT( "p_N" ) }
				, InVec3{ &writer, cuT( "p_H" ) }
				, InFloat{ &writer, cuT( "p_roughness" ) } );

			auto radicalInverse = writer.implementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & p_bits )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto bits = writer.declLocale( cuT( "bits" )
						, p_bits );
					bits = writer.paren( bits << 16u ) | writer.paren( bits >> 16u );
					bits = writer.paren( writer.paren( bits & 0x55555555_ui ) << 1u ) | writer.paren( writer.paren( bits & 0xAAAAAAAA_ui ) >> 1u );
					bits = writer.paren( writer.paren( bits & 0x33333333_ui ) << 2u ) | writer.paren( writer.paren( bits & 0xCCCCCCCC_ui ) >> 2u );
					bits = writer.paren( writer.paren( bits & 0x0F0F0F0F_ui ) << 4u ) | writer.paren( writer.paren( bits & 0xF0F0F0F0_ui ) >> 4u );
					bits = writer.paren( writer.paren( bits & 0x00FF00FF_ui ) << 8u ) | writer.paren( writer.paren( bits & 0xFF00FF00_ui ) >> 8u );
					writer.returnStmt( writer.cast< Float >( bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ &writer, cuT( "p_bits" ) } );

			auto hammersley = writer.implementFunction< Vec2 >( cuT( "Hammersley" )
				, [&]( UInt const & p_i
					, UInt const & p_n )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					writer.returnStmt( vec2( writer.cast< Float >( p_i ) / writer.cast< Float >( p_n ), radicalInverse( p_i ) ) );
				}
				, InUInt{ &writer, cuT( "p_i" ) }
				, InUInt{ &writer, cuT( "p_n" ) } );

			auto importanceSample = writer.implementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
				, [&]( Vec2 const & p_xi
					, Vec3 const & p_n
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.declLocale( cuT( "a" )
						, p_roughness * p_roughness );

					auto phi = writer.declLocale( cuT( "phi" )
						, 2.0_f * PI * p_xi.x() );
					auto cosTheta = writer.declLocale( cuT( "cosTheta" )
						, sqrt( writer.paren( 1.0 - p_xi.y() ) / writer.paren( 1.0 + writer.paren( a * a - 1.0 ) * p_xi.y() ) ) );
					auto sinTheta = writer.declLocale( cuT( "sinTheta" )
						, sqrt( 1.0 - cosTheta * cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto H = writer.declLocale< Vec3 >( cuT( "H" ) );
					H.x() = cos( phi ) * sinTheta;
					H.y() = sin( phi ) * sinTheta;
					H.z() = cosTheta;

					// from tangent-space vector to world-space sample vector
					auto up = writer.declLocale( cuT( "up" )
						, writer.ternary( glsl::abs( p_n.z() ) < 0.999, vec3( 0.0_f, 0.0, 1.0 ), vec3( 1.0_f, 0.0, 0.0 ) ) );
					auto tangent = writer.declLocale( cuT( "tangent" )
						, normalize( cross( up, p_n ) ) );
					auto bitangent = writer.declLocale( cuT( "bitangent" )
						, cross( p_n, tangent ) );

					auto sampleVec = writer.declLocale( cuT( "sampleVec" )
						, tangent * H.x() + bitangent * H.y() + p_n * H.z() );
					writer.returnStmt( normalize( sampleVec ) );
				}
				, InVec2{ &writer, cuT( "p_xi" ) }
				, InVec3{ &writer, cuT( "p_n" ) }
				, InFloat{ &writer, cuT( "p_roughness" ) } );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto constexpr PI = 3.1415926535897932384626433832795028841968;
				// From https://learnopengl.com/#!PBR/Lighting
				auto N = writer.declLocale( cuT( "N" )
					, normalize( vtx_position ) );
				auto R = writer.declLocale( cuT( "R" )
					, N );
				auto V = writer.declLocale( cuT( "V" )
					, R );

				auto sampleCount = writer.declLocale( cuT( "sampleCount" )
					, 1024_ui );
				auto totalWeight = writer.declLocale( cuT( "totalWeight" )
					, 0.0_f );
				auto prefilteredColor = writer.declLocale( cuT( "prefilteredColor" )
					, vec3( 0.0_f ) );

				FOR( writer, UInt, i, 0, "i < sampleCount", "++i" )
				{
					auto xi = writer.declLocale( cuT( "xi" )
						, hammersley( i, sampleCount ) );
					auto H = writer.declLocale( cuT( "H" )
						, importanceSample( xi, N, c3d_roughness ) );
					auto L = writer.declLocale( cuT( "L" )
						, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

					auto NdotL = writer.declLocale( cuT( "NdotL" )
						, max( dot( N, L ), 0.0 ) );

					IF( writer, "NdotL > 0.0" )
					{
						auto D = writer.declLocale( cuT( "D" )
							, distributionGGX( N, H, c3d_roughness ) );
						auto NdotH = writer.declLocale( cuT( "NdotH" )
							, max( dot( N, H ), 0.0 ) );
						auto HdotV = writer.declLocale( cuT( "HdotV" )
							, max( dot( H, V ), 0.0 ) );
						auto pdf = writer.declLocale( cuT( "pdf" )
							, D * NdotH / writer.paren( 4.0_f * HdotV ) + 0.0001 );

						auto resolution = writer.declLocale( cuT( "resolution" )
							, Float( int( m_size.getWidth() ) ) ); // resolution of source cubemap (per face)
						auto saTexel = writer.declLocale( cuT( "saTexel" )
							, 4.0_f * PI / writer.paren( 6.0 * resolution * resolution ) );
						auto saSample = writer.declLocale( cuT( "saSample" )
							, 1.0_f / writer.paren( writer.cast< Float >( sampleCount ) * pdf + 0.0001 ) );
						auto mipLevel = writer.declLocale( cuT( "mipLevel" )
							, writer.ternary( c3d_roughness == 0.0_f, 0.0_f, 0.5_f * log2( saSample / saTexel ) ) );

						prefilteredColor += texture( c3d_mapDiffuse, L, mipLevel ).rgb() * NdotL;
						totalWeight += NdotL;
					}
					FI;
				}
				ROF;

				prefilteredColor = prefilteredColor / totalWeight;
				plx_v4FragColor = vec4( prefilteredColor, 1.0 );
			} );

			pxl = writer.finalise();
		}

		auto & cache = getEngine()->getShaderProgramCache();
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
