#include "ReflectionPass.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Scene.hpp>
#include <Shader/MatrixUbo.hpp>
#include <Shader/SceneUbo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include <Graphics/Image.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	namespace
	{
		static uint32_t constexpr c_environmentStart = 5u;

		VertexBufferSPtr DoCreateVbo( Engine & p_engine )
		{
			auto l_declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float l_data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_vertexBuffer = std::make_shared< VertexBuffer >( p_engine, l_declaration );
			uint32_t l_stride = l_declaration.stride();
			l_vertexBuffer->Resize( uint32_t( sizeof( l_data ) ) );
			uint8_t * l_buffer = l_vertexBuffer->data();
			std::memcpy( l_buffer, l_data, sizeof( l_data ) );
			l_vertexBuffer->Initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return l_vertexBuffer;
		}

		GeometryBuffersSPtr DoCreateVao( Engine & p_engine
			, ShaderProgram & p_program
			, VertexBuffer & p_vbo )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_result = l_renderSystem.CreateGeometryBuffers( Topology::eTriangles
				, p_program );
			l_result->Initialise( { p_vbo }, nullptr );
			return l_result;
		}

		GLSL::Shader DoCreateVertexProgram( RenderSystem & p_renderSystem )
		{
			using namespace GLSL;
			auto l_writer = p_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( l_writer );
			auto position = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = l_writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = l_writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = l_writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
			{
				vtx_texture = texture;
				gl_Position = c3d_mtxProjection * vec4( position.x(), position.y(), 0.0, 1.0 );
			} );
			return l_writer.Finalise();
		}

		GLSL::Shader DoCreatePixelProgram( RenderSystem & p_renderSystem )
		{
			using namespace GLSL;
			auto l_writer = p_renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( l_writer );
			UBO_GPINFO( l_writer );
			UBO_HDR_CONFIG( l_writer );
			auto c3d_mapDepth = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eDepth ) );
			auto c3d_mapNormal = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData1 ) );
			auto c3d_mapDiffuse = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData2 ) );
			auto c3d_mapEmissive = l_writer.DeclUniform< Sampler2D >( GetTextureName( DsTexture::eData4 ) );
			auto c3d_mapPostLight = l_writer.DeclUniform< Sampler2D >( cuT( "c3d_mapPostLight" ) );
			auto c3d_mapEnvironment = l_writer.DeclUniform< SamplerCube >( ShaderProgram::MapEnvironment, 16u );
			auto c3d_fresnelBias = l_writer.DeclUniform< Float >( cuT( "c3d_fresnelBias" ), 0.10_f );
			auto c3d_fresnelScale = l_writer.DeclUniform< Float >( cuT( "c3d_fresnelScale" ), 0.25_f );
			auto c3d_fresnelPower = l_writer.DeclUniform< Float >( cuT( "c3d_fresnelPower" ), 0.30_f );
			auto vtx_texture = l_writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );

			// Shader outputs
			auto pxl_v4FragColor = l_writer.DeclFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

			GLSL::Utils l_utils{ l_writer };
			l_utils.DeclareCalcWSPosition();
			Declare_DecodeMaterial( l_writer );

			l_writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
			{
				auto l_v4Normal = l_writer.DeclLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormal, vtx_texture ) );
				auto l_flags = l_writer.DeclLocale( cuT( "l_flags" ), l_v4Normal.w() );
				auto l_envMapIndex = l_writer.DeclLocale( cuT( "l_envMapIndex" ), 0_i );
				auto l_receiver = l_writer.DeclLocale( cuT( "l_receiver" ), 0_i );
				auto l_reflection = l_writer.DeclLocale( cuT( "l_reflection" ), 0_i );
				auto l_refraction = l_writer.DeclLocale( cuT( "l_refraction" ), 0_i );
				DecodeMaterial( l_writer
					, l_flags
					, l_receiver
					, l_reflection
					, l_refraction
					, l_envMapIndex );
				auto l_postLight = l_writer.DeclLocale( cuT( "l_postLight" ), texture( c3d_mapPostLight, vtx_texture ).xyz() );

				IF( l_writer, l_envMapIndex < 1_i )
				{
					l_writer.Discard();
				}
				FI;

				IF( l_writer, cuT( "l_reflection == 0 && l_refraction == 0" ) )
				{
					l_writer.Discard();
				}
				FI;

				pxl_v4FragColor = vec4( 0.0_f, 0.0_f, 0.0_f, 1.0_f );
				auto l_position = l_writer.DeclLocale( cuT( "l_position" )
					, l_utils.CalcWSPosition( vtx_texture, c3d_mtxInvViewProj ) );
				auto l_normal = l_writer.DeclLocale( cuT( "l_normal" )
					, normalize( l_v4Normal.xyz() ) );
				auto l_incident = l_writer.DeclLocale( cuT( "l_incident" )
					, normalize( l_position - c3d_v3CameraPosition ) );
				l_envMapIndex = l_envMapIndex - 1_i;
				auto l_diffuse = l_writer.DeclLocale( cuT( "l_diffuse" ), texture( c3d_mapDiffuse, vtx_texture ).xyz() );
				auto l_reflectedColour = l_writer.DeclLocale( cuT( "l_reflectedColour" ), vec3( 0.0_f, 0, 0 ) );
				auto l_refractedColour = l_writer.DeclLocale( cuT( "l_refractedColour" ), l_diffuse / 2.0 );

				IF( l_writer, l_reflection != 0_i )
				{
					auto l_reflect = l_writer.DeclLocale( cuT( "l_reflect" )
						, reflect( l_incident, l_normal ) );
					l_reflectedColour = texture( c3d_mapEnvironment[l_envMapIndex], l_reflect ).xyz() * length( l_postLight.xyz() );
				}
				FI;

				IF( l_writer, l_refraction != 0_i )
				{
					auto l_ratio = l_writer.DeclLocale( cuT( "l_ratio" )
						, texture( c3d_mapEmissive, vtx_texture ).w() );
					auto l_refract = l_writer.DeclLocale( cuT( "l_refract" )
						, refract( l_incident, l_normal, l_ratio ) );
					l_refractedColour = texture( c3d_mapEnvironment[l_envMapIndex], l_refract ).xyz() * l_diffuse / length( l_diffuse );
				}
				FI;

				IF( l_writer, cuT( "l_reflection != 0 && l_refraction == 0" ) )
				{
					pxl_v4FragColor.xyz() = l_reflectedColour * l_diffuse / length( l_diffuse );
				}
				ELSE
				{
					auto l_refFactor = l_writer.DeclLocale( cuT( "l_refFactor" )
					, clamp( c3d_fresnelBias + c3d_fresnelScale * pow( 1.0_f + dot( l_incident, l_normal ), c3d_fresnelPower ), 0.0_f, 1.0_f ) );
					pxl_v4FragColor.xyz() = mix( l_refractedColour, l_reflectedColour, l_refFactor );
				}
				FI;
			} );
			return l_writer.Finalise();
		}

		ShaderProgramSPtr DoCreateProgram( Engine & p_engine )
		{
			auto & l_renderSystem = *p_engine.GetRenderSystem();
			auto l_vtx = DoCreateVertexProgram( l_renderSystem );
			auto l_pxl = DoCreatePixelProgram( l_renderSystem );
			auto l_result = p_engine.GetShaderProgramCache().GetNewProgram( false );
			l_result->CreateObject( ShaderType::eVertex );
			l_result->CreateObject( ShaderType::ePixel );
			l_result->SetSource( ShaderType::eVertex, l_vtx );
			l_result->SetSource( ShaderType::ePixel, l_pxl );
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eDepth ), ShaderType::ePixel )->SetValue( 0u );
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData1 ), ShaderType::ePixel )->SetValue( 1u );
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData2 ), ShaderType::ePixel )->SetValue( 2u );
			l_result->CreateUniform< UniformType::eSampler >( GetTextureName( DsTexture::eData4 ), ShaderType::ePixel )->SetValue( 3u );
			l_result->CreateUniform< UniformType::eSampler >( cuT( "c3d_mapPostLight" ), ShaderType::ePixel )->SetValue( 4u );
			int const c = int( c_environmentStart );
			l_result->CreateUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel, 16u )->SetValues(
			{
				c + 0, c + 1, c + 2, c + 3, c + 4, c + 5, c + 6, c + 7,
				c + 8, c + 9, c + 10, c + 11, c + 12, c + 13, c + 14, c + 15,
			} );
			l_result->Initialise();
			return l_result;
		}

		RenderPipelineUPtr DoCreateRenderPipeline( Engine & p_engine
			, ShaderProgram & p_program
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfoUbo
			, HdrConfigUbo & p_configUbo )
		{
			RasteriserState l_rsState;
			l_rsState.SetCulledFaces( Culling::eNone );
			DepthStencilState l_dsState;
			l_dsState.SetDepthTest( false );
			l_dsState.SetDepthMask( WritingMask::eZero );
			BlendState l_blState;
			auto l_result = p_engine.GetRenderSystem()->CreateRenderPipeline( std::move( l_dsState )
				, std::move( l_rsState )
				, std::move( l_blState )
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
			l_result->AddUniformBuffer( p_matrixUbo.GetUbo() );
			l_result->AddUniformBuffer( p_sceneUbo.GetUbo() );
			l_result->AddUniformBuffer( p_gpInfoUbo.GetUbo() );
			l_result->AddUniformBuffer( p_configUbo.GetUbo() );
			return l_result;
		}
	}

	ReflectionPass::ReflectionPass( Engine & p_engine
		, Size const & p_size )
		: OwnedBy< Engine >{ p_engine }
		, m_size{ p_size }
		, m_result{ p_engine }
		, m_frameBuffer{ p_engine.GetRenderSystem()->CreateFrameBuffer() }
		, m_viewport{ p_engine }
		, m_vertexBuffer{ DoCreateVbo( p_engine ) }
		, m_program{ DoCreateProgram( p_engine ) }
		, m_geometryBuffers{ DoCreateVao( p_engine, *m_program, *m_vertexBuffer ) }
		, m_matrixUbo{ p_engine }
		, m_sceneUbo{ p_engine }
		, m_gpInfo{ p_engine }
		, m_configUbo{ p_engine }
		, m_pipeline{ DoCreateRenderPipeline( p_engine, *m_program, m_matrixUbo, m_sceneUbo, m_gpInfo, m_configUbo ) }
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		m_matrixUbo.Update( m_viewport.GetProjection() );

		m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eTransparentBlack ) );
		bool l_return = m_frameBuffer->Create();

		if ( l_return )
		{
			l_return = m_frameBuffer->Initialise( p_size );
		}

		if ( l_return )
		{
			auto l_texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA16F32F
				, p_size );
			l_texture->GetImage().InitialiseSource();

			m_result.SetIndex( 0u );
			m_result.SetTexture( l_texture );
			m_result.SetSampler( p_engine.GetLightsSampler() );
			m_result.Initialise();

			m_resultAttach = m_frameBuffer->CreateAttachment( l_texture );

			m_frameBuffer->Bind();
			m_frameBuffer->Attach( AttachmentPoint::eColour
				, m_resultAttach
				, l_texture->GetType() );
			ENSURE( m_frameBuffer->IsComplete() );
			m_frameBuffer->SetDrawBuffers();
			m_frameBuffer->Unbind();
		}
	}

	ReflectionPass::~ReflectionPass()
	{
		m_frameBuffer->Bind();
		m_frameBuffer->DetachAll();
		m_frameBuffer->Unbind();
		m_frameBuffer->Cleanup();
		m_frameBuffer->Destroy();
		m_resultAttach.reset();
		m_result.Cleanup();

		m_configUbo.GetUbo().Cleanup();
		m_sceneUbo.GetUbo().Cleanup();
		m_matrixUbo.GetUbo().Cleanup();
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_vertexBuffer->Cleanup();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void ReflectionPass::Render( GeometryPassResult & p_gp
		, Castor3D::TextureLayout const & p_lp
		, Scene const & p_scene
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj )
	{
		m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer->Clear( BufferComponent::eColour );
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		m_sceneUbo.UpdateCameraPosition( p_camera );
		m_configUbo.Update( p_scene.GetHdrConfig() );
		auto & l_maps = p_scene.GetEnvironmentMaps();
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Bind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Bind( 0u );
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Bind( 1u );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Bind( 1u );
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Bind( 2u );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Bind( 2u );
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Bind( 3u );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Bind( 3u );
		p_lp.Bind( 4u );
		p_gp[size_t( DsTexture::eData3 )]->GetSampler()->Bind( 4u );
		auto l_index = c_environmentStart;

		for ( auto & l_map : l_maps )
		{
			l_map.get().GetTexture().GetTexture()->Bind( l_index );
			l_map.get().GetTexture().GetSampler()->Bind( l_index );
			++l_index;
		}

		m_pipeline->Apply();
		m_geometryBuffers->Draw( 6u, 0 );
		l_index = c_environmentStart;

		for ( auto & l_map : l_maps )
		{
			l_map.get().GetTexture().GetTexture()->Unbind( l_index );
			l_map.get().GetTexture().GetSampler()->Unbind( l_index );
			++l_index;
		}

		p_gp[size_t( DsTexture::eData3 )]->GetTexture()->Unbind( 4u );
		p_lp.Unbind( 4u );
		p_gp[size_t( DsTexture::eData4 )]->GetTexture()->Unbind( 3u );
		p_gp[size_t( DsTexture::eData4 )]->GetSampler()->Unbind( 3u );
		p_gp[size_t( DsTexture::eData2 )]->GetTexture()->Unbind( 2u );
		p_gp[size_t( DsTexture::eData2 )]->GetSampler()->Unbind( 2u );
		p_gp[size_t( DsTexture::eData1 )]->GetTexture()->Unbind( 1u );
		p_gp[size_t( DsTexture::eData1 )]->GetSampler()->Unbind( 1u );
		p_gp[size_t( DsTexture::eDepth )]->GetTexture()->Unbind( 0u );
		p_gp[size_t( DsTexture::eDepth )]->GetSampler()->Unbind( 0u );
		m_frameBuffer->Unbind();
	}
}
