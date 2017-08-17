#include "BrdfPrefilter.hpp"

#include "Engine.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Skybox.hpp"
#include "PBR/IblTextures.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	BrdfPrefilter::BrdfPrefilter( Engine & engine
		, castor::Size const & p_size )
		: OwnedBy< Engine >{ engine }
		, m_matrixUbo{ engine }
		, m_viewport{ engine }
		, m_bufferVertex
		{
			{
				0, 0,
				1, 1,
				0, 1,
				0, 0,
				1, 0,
				1, 1
			}
		}
		, m_declaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 }
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
		m_viewport.resize( m_size );
		m_viewport.update();
		auto & program = *doCreateProgram();
		auto & renderSystem = *engine.getRenderSystem();
		program.initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( engine
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
		dsState.setDepthMask( WritingMask::eZero );
		m_pipeline = renderSystem.createRenderPipeline( std::move( dsState )
			, RasteriserState{}
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

		m_frameBuffer->create();
		m_frameBuffer->initialise( m_size );
		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach );
		REQUIRE( m_frameBuffer->isComplete() );
		m_frameBuffer->unbind();
	}

	BrdfPrefilter::~BrdfPrefilter()
	{
		m_frameBuffer->bind();
		m_frameBuffer->detachAll();
		m_frameBuffer->unbind();
		m_frameBuffer->cleanup();
		m_frameBuffer->destroy();
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

	void BrdfPrefilter::render( TextureLayoutSPtr p_dstTexture )
	{
		m_viewport.apply();
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		auto attach = m_frameBuffer->createAttachment( p_dstTexture );
		attach->setLayer( 0 );
		attach->setTarget( TextureType::eTwoDimensions );
		attach->attach( AttachmentPoint::eColour, 0u );
		m_frameBuffer->setDrawBuffer( attach );
		REQUIRE( m_frameBuffer->isComplete() );
		m_frameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth );
		m_matrixUbo.update( m_viewport.getProjection() );
		m_pipeline->apply();
		m_geometryBuffers->draw( uint32_t( m_arrayVertex.size() ), 0 );
		m_frameBuffer->unbind();
	}

	ShaderProgramSPtr BrdfPrefilter::doCreateProgram()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec2 >( ShaderProgram::Position );
			UBO_MATRIX( writer );

			// Outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			};

			writer.implementFunction< void >( cuT( "main" ), main );
			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ) );

			// Outputs
			auto plx_v4FragColor = writer.declOutput< Vec2 >( cuT( "pxl_FragColor" ) );

			auto radicalInverse = writer.implementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & p_bits )
				{
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

			auto geometrySchlickGGX = writer.implementFunction< Float >( cuT( "GeometrySchlickGGX" )
				, [&]( Float const & p_product
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = writer.declLocale( cuT( "r" )
						, p_roughness );
					auto k = writer.declLocale( cuT( "k" )
						, writer.paren( r * r ) / 2.0_f );

					auto nominator = writer.declLocale( cuT( "num" )
						, p_product );
					auto denominator = writer.declLocale( cuT( "denom" )
						, p_product * writer.paren( 1.0_f - k ) + k );

					writer.returnStmt( nominator / denominator );
				}
				, InFloat( &writer, cuT( "p_product" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			auto geometrySmith = writer.implementFunction< Float >( cuT( "GeometrySmith" )
				, [&]( Float const & p_NdotV
					, Float const & p_NdotL
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = writer.declLocale( cuT( "ggx2" )
						, geometrySchlickGGX( p_NdotV, p_roughness ) );
					auto ggx1 = writer.declLocale( cuT( "ggx1" )
						, geometrySchlickGGX( p_NdotL, p_roughness ) );

					writer.returnStmt( ggx1 * ggx2 );
				}
				, InFloat( &writer, cuT( "p_NdotV" ) )
				, InFloat( &writer, cuT( "p_NdotL" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			auto integrateBRDF = writer.implementFunction< Vec2 >( cuT( "IntegrateBRDF" )
				, [&]( Float const & p_NdotV
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto V = writer.declLocale< Vec3 >( cuT( "V" ) );
					V.x() = sqrt( 1.0 - p_NdotV * p_NdotV );
					V.y() = 0.0_f;
					V.z() = p_NdotV;
					auto N = writer.declLocale( cuT( "N" )
						, vec3( 0.0_f, 0.0, 1.0 ) );

					auto A = writer.declLocale( cuT( "A" )
						, 0.0_f );
					auto B = writer.declLocale( cuT( "B" )
						, 0.0_f );

					auto sampleCount = writer.declLocale( cuT( "sampleCount" )
						, 1024_ui );

					FOR( writer, UInt, i, 0, "i < sampleCount", "++i" )
					{
						auto xi = writer.declLocale( cuT( "xi" )
							, hammersley( i, sampleCount ) );
						auto H = writer.declLocale( cuT( "H" )
							, importanceSample( xi, N, p_roughness ) );
						auto L = writer.declLocale( cuT( "L" )
							, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

						auto NdotL = writer.declLocale( cuT( "NdotL" )
							, max( L.z(), 0.0 ) );
						auto NdotH = writer.declLocale( cuT( "NdotH" )
							, max( H.z(), 0.0 ) );
						auto VdotH = writer.declLocale( cuT( "VdotH" )
							, max( dot( V, H ), 0.0 ) );

						IF( writer, "NdotL > 0.0" )
						{
							auto G = writer.declLocale( cuT( "G" )
								, geometrySmith( p_NdotV, max( dot( N, L ), 0.0 ), p_roughness ) );
							auto G_Vis = writer.declLocale( cuT( "G_Vis" )
								, writer.paren( G * VdotH ) / writer.paren( NdotH * p_NdotV ) );
							auto Fc = writer.declLocale( cuT( "Fc" )
								, pow( 1.0 - VdotH, 5.0 ) );

							A += writer.paren( 1.0_f - Fc ) * G_Vis;
							B += Fc * G_Vis;
						}
						FI;
					}
					ROF;

					A /= writer.cast< Float >( sampleCount );
					B /= writer.cast< Float >( sampleCount );
					writer.returnStmt( vec2( A, B ) );
				}
				, InFloat( &writer, cuT( "p_NdotV" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					plx_v4FragColor.xy() = integrateBRDF( vtx_texture.x(), vtx_texture.y() );
				} );

			pxl = writer.finalise();
		}

		auto & cache = getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program->createObject( ShaderType::eVertex );
		program->createObject( ShaderType::ePixel );
		program->setSource( ShaderType::eVertex, vtx );
		program->setSource( ShaderType::ePixel, pxl );
		program->initialise();
		return program;
	}
}
