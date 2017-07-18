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

using namespace Castor;

namespace Castor3D
{
	BrdfPrefilter::BrdfPrefilter( Engine & p_engine
		, Castor::Size const & p_size )
		: OwnedBy< Engine >{ p_engine }
		, m_matrixUbo{ p_engine }
		, m_viewport{ p_engine }
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

		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		auto & program = *DoCreateProgram();
		auto & engine = *GetEngine();
		auto & renderSystem = *engine.GetRenderSystem();
		program.Initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( engine
			, m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin()
			, m_arrayVertex.end() );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );
		m_geometryBuffers = renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, program );
		m_geometryBuffers->Initialise( { *m_vertexBuffer }
		, nullptr );

		DepthStencilState dsState;
		dsState.SetDepthTest( false );
		dsState.SetDepthMask( WritingMask::eZero );
		m_pipeline = renderSystem.CreateRenderPipeline( std::move( dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_frameBuffer = renderSystem.CreateFrameBuffer();

		m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD24 );
		m_depthBuffer->Create();
		m_depthBuffer->Initialise( m_size );
		m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );

		m_frameBuffer->Create();
		m_frameBuffer->Initialise( m_size );
		m_frameBuffer->Bind();
		m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
		REQUIRE( m_frameBuffer->IsComplete() );
		m_frameBuffer->Unbind();
	}

	BrdfPrefilter::~BrdfPrefilter()
	{
		m_frameBuffer->Bind();
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_frameBuffer->Destroy();
		m_frameBuffer.reset();
		m_depthAttach.reset();
		m_depthBuffer->Cleanup();
		m_depthBuffer->Destroy();
		m_depthBuffer.reset();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_viewport.Cleanup();
		m_matrixUbo.GetUbo().Cleanup();

		for ( auto & vertex : m_arrayVertex )
		{
			vertex.reset();
		}
	}

	void BrdfPrefilter::Render( TextureLayoutSPtr p_dstTexture )
	{
		m_viewport.Apply();
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		auto attach = m_frameBuffer->CreateAttachment( p_dstTexture );
		attach->SetLayer( 0 );
		attach->SetTarget( TextureType::eTwoDimensions );
		attach->Attach( AttachmentPoint::eColour, 0u );
		m_frameBuffer->SetDrawBuffer( attach );
		REQUIRE( m_frameBuffer->IsComplete() );
		m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth );
		m_matrixUbo.Update( m_viewport.GetProjection() );
		m_pipeline->Apply();
		m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
		m_frameBuffer->Unbind();
	}

	ShaderProgramSPtr BrdfPrefilter::DoCreateProgram()
	{
		auto & renderSystem = *GetEngine()->GetRenderSystem();
		GLSL::Shader vtx;
		{
			using namespace GLSL;
			GlslWriter writer{ renderSystem.CreateGlslWriter() };

			// Inputs
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			UBO_MATRIX( writer );

			// Outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > main = [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			};

			writer.ImplementFunction< void >( cuT( "main" ), main );
			vtx = writer.Finalise();
		}

		GLSL::Shader pxl;
		{
			using namespace GLSL;
			GlslWriter writer{ renderSystem.CreateGlslWriter() };

			// Inputs
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Outputs
			auto plx_v4FragColor = writer.DeclOutput< Vec2 >( cuT( "pxl_FragColor" ) );

			auto radicalInverse = writer.ImplementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & p_bits )
				{
					auto bits = writer.DeclLocale( cuT( "bits" )
						, p_bits );
					bits = writer.Paren( bits << 16u ) | writer.Paren( bits >> 16u );
					bits = writer.Paren( writer.Paren( bits & 0x55555555_ui ) << 1u ) | writer.Paren( writer.Paren( bits & 0xAAAAAAAA_ui ) >> 1u );
					bits = writer.Paren( writer.Paren( bits & 0x33333333_ui ) << 2u ) | writer.Paren( writer.Paren( bits & 0xCCCCCCCC_ui ) >> 2u );
					bits = writer.Paren( writer.Paren( bits & 0x0F0F0F0F_ui ) << 4u ) | writer.Paren( writer.Paren( bits & 0xF0F0F0F0_ui ) >> 4u );
					bits = writer.Paren( writer.Paren( bits & 0x00FF00FF_ui ) << 8u ) | writer.Paren( writer.Paren( bits & 0xFF00FF00_ui ) >> 8u );
					writer.Return( writer.Cast< Float >( bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ &writer, cuT( "p_bits" ) } );

			auto hammersley = writer.ImplementFunction< Vec2 >( cuT( "Hammersley" )
				, [&]( UInt const & p_i
					, UInt const & p_n )
				{
					writer.Return( vec2( writer.Cast< Float >( p_i ) / writer.Cast< Float >( p_n ), radicalInverse( p_i ) ) );
				}
				, InUInt{ &writer, cuT( "p_i" ) }
				, InUInt{ &writer, cuT( "p_n" ) } );

			auto importanceSample = writer.ImplementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
				, [&]( Vec2 const & p_xi
					, Vec3 const & p_n
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto a = writer.DeclLocale( cuT( "a" )
						, p_roughness * p_roughness );

					auto phi = writer.DeclLocale( cuT( "phi" )
						, 2.0_f * PI * p_xi.x() );
					auto cosTheta = writer.DeclLocale( cuT( "cosTheta" )
						, sqrt( writer.Paren( 1.0 - p_xi.y() ) / writer.Paren( 1.0 + writer.Paren( a * a - 1.0 ) * p_xi.y() ) ) );
					auto sinTheta = writer.DeclLocale( cuT( "sinTheta" )
						, sqrt( 1.0 - cosTheta * cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto H = writer.DeclLocale< Vec3 >( cuT( "H" ) );
					H.x() = cos( phi ) * sinTheta;
					H.y() = sin( phi ) * sinTheta;
					H.z() = cosTheta;

					// from tangent-space vector to world-space sample vector
					auto up = writer.DeclLocale( cuT( "up" )
						, writer.Ternary( GLSL::abs( p_n.z() ) < 0.999, vec3( 0.0_f, 0.0, 1.0 ), vec3( 1.0_f, 0.0, 0.0 ) ) );
					auto tangent = writer.DeclLocale( cuT( "tangent" )
						, normalize( cross( up, p_n ) ) );
					auto bitangent = writer.DeclLocale( cuT( "bitangent" )
						, cross( p_n, tangent ) );

					auto sampleVec = writer.DeclLocale( cuT( "sampleVec" )
						, tangent * H.x() + bitangent * H.y() + p_n * H.z() );
					writer.Return( normalize( sampleVec ) );
				}
				, InVec2{ &writer, cuT( "p_xi" ) }
				, InVec3{ &writer, cuT( "p_n" ) }
				, InFloat{ &writer, cuT( "p_roughness" ) } );

			auto geometrySchlickGGX = writer.ImplementFunction< Float >( cuT( "GeometrySchlickGGX" )
				, [&]( Float const & p_product
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = writer.DeclLocale( cuT( "r" )
						, p_roughness );
					auto k = writer.DeclLocale( cuT( "k" )
						, writer.Paren( r * r ) / 2.0_f );

					auto nominator = writer.DeclLocale( cuT( "num" )
						, p_product );
					auto denominator = writer.DeclLocale( cuT( "denom" )
						, p_product * writer.Paren( 1.0_f - k ) + k );

					writer.Return( nominator / denominator );
				}
				, InFloat( &writer, cuT( "p_product" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			auto geometrySmith = writer.ImplementFunction< Float >( cuT( "GeometrySmith" )
				, [&]( Float const & p_NdotV
					, Float const & p_NdotL
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggx2 = writer.DeclLocale( cuT( "ggx2" )
						, geometrySchlickGGX( p_NdotV, p_roughness ) );
					auto ggx1 = writer.DeclLocale( cuT( "ggx1" )
						, geometrySchlickGGX( p_NdotL, p_roughness ) );

					writer.Return( ggx1 * ggx2 );
				}
				, InFloat( &writer, cuT( "p_NdotV" ) )
				, InFloat( &writer, cuT( "p_NdotL" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			auto integrateBRDF = writer.ImplementFunction< Vec2 >( cuT( "IntegrateBRDF" )
				, [&]( Float const & p_NdotV
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto V = writer.DeclLocale< Vec3 >( cuT( "V" ) );
					V.x() = sqrt( 1.0 - p_NdotV * p_NdotV );
					V.y() = 0.0_f;
					V.z() = p_NdotV;
					auto N = writer.DeclLocale( cuT( "N" )
						, vec3( 0.0_f, 0.0, 1.0 ) );

					auto A = writer.DeclLocale( cuT( "A" )
						, 0.0_f );
					auto B = writer.DeclLocale( cuT( "B" )
						, 0.0_f );

					auto sampleCount = writer.DeclLocale( cuT( "sampleCount" )
						, 1024_ui );

					FOR( writer, UInt, i, 0, "i < sampleCount", "++i" )
					{
						auto xi = writer.DeclLocale( cuT( "xi" )
							, hammersley( i, sampleCount ) );
						auto H = writer.DeclLocale( cuT( "H" )
							, importanceSample( xi, N, p_roughness ) );
						auto L = writer.DeclLocale( cuT( "L" )
							, normalize( vec3( 2.0_f ) * dot( V, H ) * H - V ) );

						auto NdotL = writer.DeclLocale( cuT( "NdotL" )
							, max( L.z(), 0.0 ) );
						auto NdotH = writer.DeclLocale( cuT( "NdotH" )
							, max( H.z(), 0.0 ) );
						auto VdotH = writer.DeclLocale( cuT( "VdotH" )
							, max( dot( V, H ), 0.0 ) );

						IF( writer, "NdotL > 0.0" )
						{
							auto G = writer.DeclLocale( cuT( "G" )
								, geometrySmith( p_NdotV, max( dot( N, L ), 0.0 ), p_roughness ) );
							auto G_Vis = writer.DeclLocale( cuT( "G_Vis" )
								, writer.Paren( G * VdotH ) / writer.Paren( NdotH * p_NdotV ) );
							auto Fc = writer.DeclLocale( cuT( "Fc" )
								, pow( 1.0 - VdotH, 5.0 ) );

							A += writer.Paren( 1.0_f - Fc ) * G_Vis;
							B += Fc * G_Vis;
						}
						FI;
					}
					ROF;

					A /= writer.Cast< Float >( sampleCount );
					B /= writer.Cast< Float >( sampleCount );
					writer.Return( vec2( A, B ) );
				}
				, InFloat( &writer, cuT( "p_NdotV" ) )
				, InFloat( &writer, cuT( "p_roughness" ) ) );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					plx_v4FragColor.xy() = integrateBRDF( vtx_texture.x(), vtx_texture.y() );
				} );

			pxl = writer.Finalise();
		}

		auto & cache = GetEngine()->GetShaderProgramCache();
		auto program = cache.GetNewProgram( false );
		program->CreateObject( ShaderType::eVertex );
		program->CreateObject( ShaderType::ePixel );
		program->SetSource( ShaderType::eVertex, vtx );
		program->SetSource( ShaderType::ePixel, pxl );
		program->Initialise();
		return program;
	}
}
