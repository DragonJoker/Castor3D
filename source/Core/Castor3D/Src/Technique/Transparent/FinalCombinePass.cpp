#include "FinalCombinePass.hpp"

#include <Engine.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Miscellaneous/SsaoConfig.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Fog.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>
#include <GlslUtils.hpp>

#include <random>

using namespace Castor;
using namespace Castor3D;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		VertexBufferSPtr DoCreateVbo( Engine & engine )
		{
			auto declaration = BufferDeclaration(
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
				BufferElementDeclaration{ ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 },
			} );

			float data[] =
			{
				0, 0, 0, 0,
				1, 1, 1, 1,
				0, 1, 0, 1,
				0, 0, 0, 0,
				1, 0, 1, 0,
				1, 1, 1, 1
			};

			auto & renderSystem = *engine.GetRenderSystem();
			auto vertexBuffer = std::make_shared< VertexBuffer >( engine, declaration );
			uint32_t stride = declaration.stride();
			vertexBuffer->Resize( uint32_t( sizeof( data ) ) );
			uint8_t * buffer = vertexBuffer->GetData();
			std::memcpy( buffer, data, sizeof( data ) );
			vertexBuffer->Initialise( BufferAccessType::eStatic
				, BufferAccessNature::eDraw );
			return vertexBuffer;
		}

		GeometryBuffersSPtr DoCreateVao( Engine & engine
			, ShaderProgram & p_program
			, VertexBuffer & p_vbo )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			auto result = renderSystem.CreateGeometryBuffers( Topology::eTriangles
				, p_program );
			result->Initialise( { p_vbo }, nullptr );
			return result;
		}

		GLSL::Shader DoGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_MATRIX( writer );
			auto position = writer.DeclAttribute< Vec2 >( ShaderProgram::Position );
			auto texture = writer.DeclAttribute< Vec2 >( ShaderProgram::Texture );

			// Shader outputs
			auto vtx_texture = writer.DeclOutput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_Position = writer.DeclBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.ImplementFunction< void >( cuT( "main" )
				, [&]()
				{
					vtx_texture = texture;
					gl_Position = c3d_mtxProjection * vec4( position, 0.0, 1.0 );
				} );
			return writer.Finalise();
		}
		
		GLSL::Shader DoGetPixelProgram( Engine & engine
			, GLSL::FogType p_fogType )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			using namespace GLSL;
			auto writer = renderSystem.CreateGlslWriter();

			// Shader inputs
			UBO_SCENE( writer );
			UBO_GPINFO( writer );
			auto c3d_mapDepth = writer.DeclUniform< Sampler2D >( GetTextureName( WbTexture::eDepth ) );
			auto c3d_mapAccumulation = writer.DeclUniform< Sampler2D >( GetTextureName( WbTexture::eAccumulation ) );
			auto c3d_mapRevealage = writer.DeclUniform< Sampler2D >( GetTextureName( WbTexture::eRevealage ) );
			auto vtx_texture = writer.DeclInput< Vec2 >( cuT( "vtx_texture" ) );
			auto gl_FragCoord = writer.DeclBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.DeclOutput< Vec4 >( cuT( "pxl_fragColor" ) );

			GLSL::Utils utils{ writer };
			utils.DeclareCalcVSPosition();

			GLSL::Fog fog{ p_fogType, writer };

			auto maxComponent = writer.ImplementFunction< Float >( cuT( "maxComponent" )
				, [&]( Vec3 const & v )
			{
				writer.Return( max( max( v.x(), v.y() ), v.z() ) );
			}, InVec3{ &writer, cuT( "v" ) } );

			writer.ImplementFunction< Void >( cuT( "main" )
				, [&]()
				{
				auto coord = writer.DeclLocale( cuT( "coord" )
					, ivec2( gl_FragCoord.xy() ) );
					auto revealage = writer.DeclLocale( cuT( "revealage" )
						, texelFetch( c3d_mapRevealage, coord, 0 ).r() );

					IF( writer, revealage == 1.0_f )
					{
						// Save the blending and color texture fetch cost
						writer.Discard();
					}
					FI;

					auto accum = writer.DeclLocale( cuT( "accum" )
						, texelFetch( c3d_mapAccumulation, coord, 0 ) );

					// Suppress overflow
					IF( writer, GLSL::isinf( maxComponent( GLSL::abs( accum.rgb() ) ) ) )
					{
						accum.rgb() = vec3( accum.a() );
					}
					FI;

					auto averageColor = writer.DeclLocale( cuT( "averageColor" )
						, accum.rgb() / max( accum.a(), 0.00001_f ) );

					pxl_fragColor = vec4( averageColor.rgb(), 1.0_f - revealage );

					if ( p_fogType != FogType::eDisabled )
					{
						auto position = writer.DeclLocale( cuT( "position" ), utils.CalcVSPosition( vtx_texture, c3d_mtxInvProj ) );
						fog.ApplyFog( pxl_fragColor, length( position ), position.z() );
					}
				} );
			return writer.Finalise();
		}
		
		ShaderProgramSPtr DoCreateProgram( Engine & engine
			, GLSL::FogType p_fogType )
		{
			auto & renderSystem = *engine.GetRenderSystem();
			auto vtx = DoGetVertexProgram( engine );
			auto pxl = DoGetPixelProgram( engine, p_fogType );
			ShaderProgramSPtr program = engine.GetShaderProgramCache().GetNewProgram( false );
			program->CreateObject( ShaderType::eVertex );
			program->CreateObject( ShaderType::ePixel );
			program->CreateUniform< UniformType::eSampler >( GetTextureName( WbTexture::eDepth ), ShaderType::ePixel )->SetValue( 0u );
			program->CreateUniform< UniformType::eSampler >( GetTextureName( WbTexture::eAccumulation ), ShaderType::ePixel )->SetValue( 1u );
			program->CreateUniform< UniformType::eSampler >( GetTextureName( WbTexture::eRevealage ), ShaderType::ePixel )->SetValue( 2u );

			program->SetSource( ShaderType::eVertex, vtx );
			program->SetSource( ShaderType::ePixel, pxl );
			program->Initialise();
			return program;
		}

		RenderPipelineUPtr DoCreateRenderPipeline( Engine & engine
			, ShaderProgram & p_program
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfoUbo & p_gpInfoUbo )
		{
			DepthStencilState dsstate;
			dsstate.SetDepthTest( false );
			dsstate.SetDepthMask( WritingMask::eZero );
			RasteriserState rsstate;
			rsstate.SetCulledFaces( Culling::eNone );
			BlendState bdState;
			bdState.EnableBlend( true );
			bdState.SetAlphaBlend( BlendOperation::eAdd, BlendOperand::eSrcAlpha, BlendOperand::eInvSrcAlpha );
			bdState.SetRgbBlend( BlendOperation::eAdd, BlendOperand::eSrcAlpha, BlendOperand::eInvSrcAlpha );
			auto pipeline = engine.GetRenderSystem()->CreateRenderPipeline( std::move( dsstate )
				, std::move( rsstate )
				, std::move( bdState )
				, MultisampleState{}
				, p_program
				, PipelineFlags{} );
			pipeline->AddUniformBuffer( p_matrixUbo.GetUbo() );
			pipeline->AddUniformBuffer( p_sceneUbo.GetUbo() );
			pipeline->AddUniformBuffer( p_gpInfoUbo.GetUbo() );
			return pipeline;
		}
	}

	//*********************************************************************************************

	FinalCombineProgram::FinalCombineProgram( Engine & engine
		, VertexBuffer & p_vbo
		, MatrixUbo & p_matrixUbo
		, SceneUbo & p_sceneUbo
		, GpInfoUbo & p_gpInfoUbo
		, GLSL::FogType p_fogType )
		: m_program{ DoCreateProgram( engine, p_fogType ) }
		, m_geometryBuffers{ DoCreateVao( engine, *m_program, p_vbo ) }
		, m_pipeline{ DoCreateRenderPipeline( engine, *m_program, p_matrixUbo, p_sceneUbo, p_gpInfoUbo ) }
	{
	}

	FinalCombineProgram::~FinalCombineProgram()
	{
		m_pipeline->Cleanup();
		m_pipeline.reset();
		m_geometryBuffers->Cleanup();
		m_geometryBuffers.reset();
		m_geometryBuffers.reset();
		m_program.reset();
	}

	void FinalCombineProgram::Render()const
	{
		m_pipeline->Apply();
		m_geometryBuffers->Draw( 6u, 0 );
	}

	//*********************************************************************************************

	FinalCombinePass::FinalCombinePass( Engine & engine
		, Size const & p_size )
		: m_size{ p_size }
		, m_viewport{ engine }
		, m_vertexBuffer{ DoCreateVbo( engine ) }
		, m_matrixUbo{ engine }
		, m_sceneUbo{ engine }
		, m_gpInfo{ engine }
		, m_programs
		{
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eDisabled },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eLinear },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eExponential },
			FinalCombineProgram{ engine, *m_vertexBuffer, m_matrixUbo, m_sceneUbo, m_gpInfo, GLSL::FogType::eSquaredExponential }
		}
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		m_viewport.Initialise();
		m_viewport.Resize( m_size );
		m_viewport.Update();
		m_matrixUbo.Update( m_viewport.GetProjection() );
	}

	FinalCombinePass::~FinalCombinePass()
	{
		m_matrixUbo.GetUbo().Cleanup();
		m_sceneUbo.GetUbo().Cleanup();
		m_gpInfo.GetUbo().Cleanup();
		m_vertexBuffer->Cleanup();
	}

	void FinalCombinePass::Render( WeightedBlendPassResult const & p_r
		, FrameBuffer const & p_frameBuffer
		, Camera const & p_camera
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj
		, Fog const & p_fog )
	{
		m_gpInfo.Update( m_size
			, p_camera
			, p_invViewProj
			, p_invView
			, p_invProj );
		m_sceneUbo.Update( p_camera, p_fog );
		p_frameBuffer.Bind( FrameBufferTarget::eDraw );
		p_frameBuffer.SetDrawBuffers();

		auto & program = m_programs[size_t( p_fog.GetType() )];

		m_viewport.Apply();
		p_r[size_t( WbTexture::eDepth )]->GetTexture()->Bind( 0u );
		p_r[size_t( WbTexture::eDepth )]->GetSampler()->Bind( 0u );
		p_r[size_t( WbTexture::eAccumulation )]->GetTexture()->Bind( 1u );
		p_r[size_t( WbTexture::eAccumulation )]->GetSampler()->Bind( 1u );
		p_r[size_t( WbTexture::eRevealage )]->GetTexture()->Bind( 2u );
		p_r[size_t( WbTexture::eRevealage )]->GetSampler()->Bind( 2u );
		program.Render();
		p_r[size_t( WbTexture::eRevealage )]->GetTexture()->Unbind( 2u );
		p_r[size_t( WbTexture::eRevealage )]->GetSampler()->Unbind( 2u );
		p_r[size_t( WbTexture::eAccumulation )]->GetTexture()->Unbind( 1u );
		p_r[size_t( WbTexture::eAccumulation )]->GetSampler()->Unbind( 1u );
		p_r[size_t( WbTexture::eDepth )]->GetTexture()->Unbind( 0u );
		p_r[size_t( WbTexture::eDepth )]->GetSampler()->Unbind( 0u );
	}

	//*********************************************************************************************
}
