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

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_bufferVertex.data() )[i++ * m_declaration.stride()] );
		}

		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		auto & l_program = *DoCreateProgram();
		auto & l_engine = *GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();
		l_program.Initialise();
		m_vertexBuffer = std::make_shared< VertexBuffer >( l_engine
			, m_declaration );
		m_vertexBuffer->Resize( uint32_t( m_arrayVertex.size()
			* m_declaration.stride() ) );
		m_vertexBuffer->LinkCoords( m_arrayVertex.begin()
			, m_arrayVertex.end() );
		m_vertexBuffer->Initialise( BufferAccessType::eStatic
			, BufferAccessNature::eDraw );
		m_geometryBuffers = l_renderSystem.CreateGeometryBuffers( Topology::eTriangles
			, l_program );
		m_geometryBuffers->Initialise( { *m_vertexBuffer }
		, nullptr );

		DepthStencilState l_dsState;
		l_dsState.SetDepthTest( false );
		l_dsState.SetDepthMask( WritingMask::eZero );
		m_pipeline = l_renderSystem.CreateRenderPipeline( std::move( l_dsState )
			, RasteriserState{}
			, BlendState{}
			, MultisampleState{}
			, l_program
			, PipelineFlags{} );
		m_pipeline->AddUniformBuffer( m_matrixUbo.GetUbo() );

		m_frameBuffer = l_renderSystem.CreateFrameBuffer();

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

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex.reset();
		}
	}

	void BrdfPrefilter::Render( TextureLayoutSPtr p_dstTexture )
	{
		m_viewport.Apply();
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		auto l_attach = m_frameBuffer->CreateAttachment( p_dstTexture );
		l_attach->SetLayer( 0 );
		l_attach->SetTarget( TextureType::eTwoDimensions );
		l_attach->Attach( AttachmentPoint::eColour, 0u );
		m_frameBuffer->SetDrawBuffer( l_attach );
		REQUIRE( m_frameBuffer->IsComplete() );
		m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth );
		m_matrixUbo.Update( m_viewport.GetProjection() );
		m_pipeline->Apply();
		m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
		m_frameBuffer->Unbind();
	}

	ShaderProgramSPtr BrdfPrefilter::DoCreateProgram()
	{
		auto & l_renderSystem = *GetEngine()->GetRenderSystem();
		GLSL::Shader l_vtx;
		{
			using namespace GLSL;
			GlslWriter l_writer{ l_renderSystem.CreateGlslWriter() };

			// Inputs
			auto position = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			UBO_MATRIX( l_writer );

			// Outputs
			auto vtx_texture = l_writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			std::function< void() > l_main = [&]()
			{
				vtx_texture = position;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			};

			l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
			l_vtx = l_writer.Finalise();
		}

		GLSL::Shader l_pxl;
		{
			using namespace GLSL;
			GlslWriter l_writer{ l_renderSystem.CreateGlslWriter() };

			// Inputs
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Outputs
			auto plx_v4FragColor = l_writer.DeclOutput< Vec2 >( cuT( "pxl_FragColor" ) );

			auto l_radicalInverse = l_writer.ImplementFunction< Float >( cuT( "RadicalInverse_VdC" )
				, [&]( UInt const & p_bits )
				{
					auto l_bits = l_writer.DeclLocale( cuT( "l_bits" )
						, p_bits );
					l_bits = l_writer.Paren( l_bits << 16u ) | l_writer.Paren( l_bits >> 16u );
					l_bits = l_writer.Paren( l_writer.Paren( l_bits & 0x55555555_ui ) << 1u ) | l_writer.Paren( l_writer.Paren( l_bits & 0xAAAAAAAA_ui ) >> 1u );
					l_bits = l_writer.Paren( l_writer.Paren( l_bits & 0x33333333_ui ) << 2u ) | l_writer.Paren( l_writer.Paren( l_bits & 0xCCCCCCCC_ui ) >> 2u );
					l_bits = l_writer.Paren( l_writer.Paren( l_bits & 0x0F0F0F0F_ui ) << 4u ) | l_writer.Paren( l_writer.Paren( l_bits & 0xF0F0F0F0_ui ) >> 4u );
					l_bits = l_writer.Paren( l_writer.Paren( l_bits & 0x00FF00FF_ui ) << 8u ) | l_writer.Paren( l_writer.Paren( l_bits & 0xFF00FF00_ui ) >> 8u );
					l_writer.Return( l_writer.Cast< Float >( l_bits ) * 2.3283064365386963e-10 ); // / 0x100000000
				}
				, InUInt{ &l_writer, cuT( "p_bits" ) } );

			auto l_hammersley = l_writer.ImplementFunction< Vec2 >( cuT( "Hammersley" )
				, [&]( UInt const & p_i
					, UInt const & p_n )
				{
					l_writer.Return( vec2( l_writer.Cast< Float >( p_i ) / l_writer.Cast< Float >( p_n ), l_radicalInverse( p_i ) ) );
				}
				, InUInt{ &l_writer, cuT( "p_i" ) }
				, InUInt{ &l_writer, cuT( "p_n" ) } );

			auto l_importanceSample = l_writer.ImplementFunction< Vec3 >( cuT( "ImportanceSampleGGX" )
				, [&]( Vec2 const & p_xi
					, Vec3 const & p_n
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto constexpr PI = 3.1415926535897932384626433832795028841968;
					auto l_a = l_writer.DeclLocale( cuT( "l_a" )
						, p_roughness * p_roughness );

					auto l_phi = l_writer.DeclLocale( cuT( "l_phi" )
						, 2.0_f * PI * p_xi.x() );
					auto l_cosTheta = l_writer.DeclLocale( cuT( "l_cosTheta" )
						, sqrt( l_writer.Paren( 1.0 - p_xi.y() ) / l_writer.Paren( 1.0 + l_writer.Paren( l_a * l_a - 1.0 ) * p_xi.y() ) ) );
					auto l_sinTheta = l_writer.DeclLocale( cuT( "l_sinTheta" )
						, sqrt( 1.0 - l_cosTheta * l_cosTheta ) );

					// from spherical coordinates to cartesian coordinates
					auto l_H = l_writer.DeclLocale< Vec3 >( cuT( "l_H" ) );
					l_H.x() = cos( l_phi ) * l_sinTheta;
					l_H.y() = sin( l_phi ) * l_sinTheta;
					l_H.z() = l_cosTheta;

					// from tangent-space vector to world-space sample vector
					auto l_up = l_writer.DeclLocale( cuT( "l_up" )
						, l_writer.Ternary( GLSL::abs( p_n.z() ) < 0.999, vec3( 0.0_f, 0.0, 1.0 ), vec3( 1.0_f, 0.0, 0.0 ) ) );
					auto l_tangent = l_writer.DeclLocale( cuT( "l_tangent" )
						, normalize( cross( l_up, p_n ) ) );
					auto l_bitangent = l_writer.DeclLocale( cuT( "l_bitangent" )
						, cross( p_n, l_tangent ) );

					auto l_sampleVec = l_writer.DeclLocale( cuT( "l_sampleVec" )
						, l_tangent * l_H.x() + l_bitangent * l_H.y() + p_n * l_H.z() );
					l_writer.Return( normalize( l_sampleVec ) );
				}
				, InVec2{ &l_writer, cuT( "p_xi" ) }
				, InVec3{ &l_writer, cuT( "p_n" ) }
				, InFloat{ &l_writer, cuT( "p_roughness" ) } );

			auto l_geometrySchlickGGX = l_writer.ImplementFunction< Float >( cuT( "GeometrySchlickGGX" )
				, [&]( Float const & p_product
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto r = l_writer.DeclLocale( cuT( "r" )
						, p_roughness );
					auto k = l_writer.DeclLocale( cuT( "k" )
						, l_writer.Paren( r * r ) / 2.0_f );

					auto l_nominator = l_writer.DeclLocale( cuT( "l_num" )
						, p_product );
					auto l_denominator = l_writer.DeclLocale( cuT( "l_denom" )
						, p_product * l_writer.Paren( 1.0_f - k ) + k );

					l_writer.Return( l_nominator / l_denominator );
				}
				, InFloat( &l_writer, cuT( "p_product" ) )
				, InFloat( &l_writer, cuT( "p_roughness" ) ) );

			auto l_geometrySmith = l_writer.ImplementFunction< Float >( cuT( "GeometrySmith" )
				, [&]( Float const & p_NdotV
					, Float const & p_NdotL
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto l_ggx2 = l_writer.DeclLocale( cuT( "ggx2" )
						, l_geometrySchlickGGX( p_NdotV, p_roughness ) );
					auto l_ggx1 = l_writer.DeclLocale( cuT( "ggx1" )
						, l_geometrySchlickGGX( p_NdotL, p_roughness ) );

					l_writer.Return( l_ggx1 * l_ggx2 );
				}
				, InFloat( &l_writer, cuT( "p_NdotV" ) )
				, InFloat( &l_writer, cuT( "p_NdotL" ) )
				, InFloat( &l_writer, cuT( "p_roughness" ) ) );

			auto l_integrateBRDF = l_writer.ImplementFunction< Vec2 >( cuT( "IntegrateBRDF" )
				, [&]( Float const & p_NdotV
					, Float const & p_roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto l_V = l_writer.DeclLocale< Vec3 >( cuT( "l_V" ) );
					l_V.x() = sqrt( 1.0 - p_NdotV * p_NdotV );
					l_V.y() = 0.0_f;
					l_V.z() = p_NdotV;
					auto l_N = l_writer.DeclLocale( cuT( "l_N" )
						, vec3( 0.0_f, 0.0, 1.0 ) );

					auto l_A = l_writer.DeclLocale( cuT( "l_A" )
						, 0.0_f );
					auto l_B = l_writer.DeclLocale( cuT( "l_B" )
						, 0.0_f );

					auto l_sampleCount = l_writer.DeclLocale( cuT( "l_sampleCount" )
						, 1024_ui );

					FOR( l_writer, UInt, i, 0, "i < l_sampleCount", "++i" )
					{
						auto l_xi = l_writer.DeclLocale( cuT( "l_xi" )
							, l_hammersley( i, l_sampleCount ) );
						auto l_H = l_writer.DeclLocale( cuT( "l_H" )
							, l_importanceSample( l_xi, l_N, p_roughness ) );
						auto l_L = l_writer.DeclLocale( cuT( "l_L" )
							, normalize( vec3( 2.0_f ) * dot( l_V, l_H ) * l_H - l_V ) );

						auto l_NdotL = l_writer.DeclLocale( cuT( "l_NdotL" )
							, max( l_L.z(), 0.0 ) );
						auto l_NdotH = l_writer.DeclLocale( cuT( "l_NdotH" )
							, max( l_H.z(), 0.0 ) );
						auto l_VdotH = l_writer.DeclLocale( cuT( "l_VdotH" )
							, max( dot( l_V, l_H ), 0.0 ) );

						IF( l_writer, "l_NdotL > 0.0" )
						{
							auto l_G = l_writer.DeclLocale( cuT( "l_G" )
								, l_geometrySmith( p_NdotV, max( dot( l_N, l_L ), 0.0 ), p_roughness ) );
							auto l_G_Vis = l_writer.DeclLocale( cuT( "l_G_Vis" )
								, l_writer.Paren( l_G * l_VdotH ) / l_writer.Paren( l_NdotH * p_NdotV ) );
							auto l_Fc = l_writer.DeclLocale( cuT( "l_Fc" )
								, pow( 1.0 - l_VdotH, 5.0 ) );

							l_A += l_writer.Paren( 1.0_f - l_Fc ) * l_G_Vis;
							l_B += l_Fc * l_G_Vis;
						}
						FI;
					}
					ROF;

					l_A /= l_writer.Cast< Float >( l_sampleCount );
					l_B /= l_writer.Cast< Float >( l_sampleCount );
					l_writer.Return( vec2( l_A, l_B ) );
				}
				, InFloat( &l_writer, cuT( "p_NdotV" ) )
				, InFloat( &l_writer, cuT( "p_roughness" ) ) );

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					plx_v4FragColor.xy() = l_integrateBRDF( vtx_texture.x(), vtx_texture.y() );
				} );

			l_pxl = l_writer.Finalise();
		}

		auto & l_cache = GetEngine()->GetShaderProgramCache();
		auto l_program = l_cache.GetNewProgram( false );
		l_program->CreateObject( ShaderType::eVertex );
		l_program->CreateObject( ShaderType::ePixel );
		l_program->SetSource( ShaderType::eVertex, l_vtx );
		l_program->SetSource( ShaderType::ePixel, l_pxl );
		l_program->Initialise();
		return l_program;
	}
}
