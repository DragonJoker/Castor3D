#include "DeferredRenderTechnique.hpp"

#include <CameraCache.hpp>
#include <Engine.hpp>
#include <LightCache.hpp>
#include <SceneCache.hpp>
#include <SceneNodeCache.hpp>
#include <ShaderCache.hpp>
#include <TargetCache.hpp>

#include <FrameBuffer/ColourRenderBuffer.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Mesh/Buffer/BufferElementDeclaration.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/Context.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
#include <Shader/PointFrameVariable.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Texture/TextureLayout.hpp>

#include <Log/Logger.hpp>

#include <GlslSource.hpp>

#define DEBUG_BUFFERS 0

using namespace Castor;
using namespace Castor3D;

namespace Deferred
{
	namespace
	{
		String GetTextureName( DsTexture p_texture )
		{
			static std::array< String, size_t( DsTexture::eCount ) > Values
			{
				{
					cuT( "c3d_mapPosition" ),
					cuT( "c3d_mapDiffuse" ),
					cuT( "c3d_mapNormals" ),
					cuT( "c3d_mapTangent" ),
					cuT( "c3d_mapSpecular" ),
					cuT( "c3d_mapEmissive" ),
				}
			};

			return Values[size_t( p_texture )];
		}

		PixelFormat GetTextureFormat( DsTexture p_texture )
		{
			static std::array< PixelFormat, size_t( DsTexture::eCount ) > Values
			{
				{
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
					PixelFormat::eRGBA16F32F,
				}
			};

			return Values[size_t( p_texture )];
		}

		AttachmentPoint GetTextureAttachmentPoint( DsTexture p_texture )
		{
			static std::array< AttachmentPoint, size_t( DsTexture::eCount ) > Values
			{
				{
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
					AttachmentPoint::eColour,
				}
			};

			return Values[size_t( p_texture )];
		}

		uint32_t GetTextureAttachmentIndex( DsTexture p_texture )
		{
			static std::array< uint32_t, size_t( DsTexture::eCount ) > Values
			{
				{
					0,
					1,
					2,
					3,
					4,
					5,
				}
			};

			return Values[size_t( p_texture )];
		}
	}

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "deferred" ), p_renderTarget, p_renderSystem, p_params )
		, m_viewport( *p_renderSystem.GetEngine() )
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		Logger::LogInfo( cuT( "Using deferred shading" ) );
		m_geometryPassFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_geometryPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		m_lightPassDepthBuffer = m_geometryPassFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
		m_geometryPassDepthAttach = m_geometryPassFrameBuffer->CreateAttachment( m_lightPassDepthBuffer );

		for ( auto & program : m_lightPassShaderPrograms )
		{
			program.m_program = GetEngine()->GetShaderProgramCache().GetNewProgram( false );
			program.m_program->CreateObject( ShaderType::eVertex );
			program.m_program->CreateObject( ShaderType::ePixel );
			GetEngine()->GetShaderProgramCache().CreateMatrixBuffer( *program.m_program, 0u, ShaderTypeFlag::ePixel | ShaderTypeFlag::eVertex );
			program.m_matrixUbo = program.m_program->FindFrameVariableBuffer( ShaderProgram::BufferMatrix );
			GetEngine()->GetShaderProgramCache().CreateSceneBuffer( *program.m_program, 0u, ShaderTypeFlag::ePixel );
			program.m_sceneUbo = program.m_program->FindFrameVariableBuffer( ShaderProgram::BufferScene );
			program.m_sceneUbo->GetVariable( ShaderProgram::CameraPos, program.m_camera );
			program.m_program->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::Lights, ShaderType::ePixel );

			for ( int i = 0; i < int( DsTexture::eCount ); i++ )
			{
				program.m_program->CreateFrameVariable< OneIntFrameVariable >( GetTextureName( DsTexture( i ) ), ShaderType::ePixel )->SetValue( i + 1 );
			}
		}

		m_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec2 ),
			BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 ),
		} );

		real l_data[] =
		{
			0, 0, 0, 0,
			1, 1, 1, 1,
			0, 1, 0, 1,
			0, 0, 0, 0,
			1, 0, 1, 0,
			1, 1, 1, 1,
		};

		m_vertexBuffer = std::make_shared< VertexBuffer >( *m_renderSystem.GetEngine(), m_declaration );
		uint32_t l_stride = m_declaration.stride();
		m_vertexBuffer->Resize( sizeof( l_data ) );
		uint8_t * l_buffer = m_vertexBuffer->data();
		std::memcpy( l_buffer, l_data, sizeof( l_data ) );

		for ( auto & l_vertex : m_arrayVertex )
		{
			l_vertex = std::make_shared< BufferElementGroup >( l_buffer );
			l_buffer += l_stride;
		}
	}

	RenderTechnique::~RenderTechnique()
	{
		for ( auto & program : m_lightPassShaderPrograms )
		{
			program.m_geometryBuffers.reset();
			program.m_matrixUbo.reset();
			program.m_sceneUbo.reset();
			program.m_program.reset();
		}
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		bool l_return = m_geometryPassFrameBuffer->Create();

		if ( l_return )
		{
			m_lightPassDepthBuffer->Create();
			ShaderModel l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			uint8_t l_sceneFlags{ 0u };

			for ( auto & program : m_lightPassShaderPrograms )
			{
				program.m_program->SetSource( ShaderType::eVertex, l_model, DoGetLightPassVertexShaderSource( 0u, 0u, l_sceneFlags ) );
				program.m_program->SetSource( ShaderType::ePixel, l_model, DoGetLightPassPixelShaderSource( 0u, 0u, l_sceneFlags ) );

				DepthStencilState l_dsstate;
				l_dsstate.SetDepthTest( false );
				l_dsstate.SetDepthMask( WritingMask::eZero );
				program.m_pipeline = GetEngine()->GetRenderSystem()->CreateRenderPipeline( std::move( l_dsstate ), RasteriserState{}, BlendState{}, MultisampleState{}, *program.m_program, PipelineFlags{} );

				++l_sceneFlags;
			}
		}

		return l_return;
	}

	void RenderTechnique::DoDestroy()
	{
		for ( auto & program : m_lightPassShaderPrograms )
		{
			program.m_pipeline->Cleanup();
			program.m_pipeline.reset();
		}

		m_lightPassDepthBuffer->Destroy();
		m_geometryPassFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = true;

		for ( uint32_t i = 0; i < uint32_t( DsTexture::eCount ); i++ )
		{
			auto l_texture = m_renderSystem.CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead | AccessType::eWrite, GetTextureFormat( DsTexture( i ) ), m_size );
			l_texture->GetImage().InitialiseSource();

			m_lightPassTextures[i] = std::make_unique< TextureUnit >( *GetEngine() );
			m_lightPassTextures[i]->SetIndex( i + 1 );
			m_lightPassTextures[i]->SetTexture( l_texture );
			m_lightPassTextures[i]->SetSampler( GetEngine()->GetLightsSampler() );
			m_lightPassTextures[i]->Initialise();

			m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->CreateAttachment( l_texture );
			p_index++;
		}

		if ( l_return )
		{
			m_lightPassDepthBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Bind( FrameBufferMode::eConfig );
		}

		if ( l_return )
		{
			for ( int i = 0; i < size_t( DsTexture::eCount ) && l_return; i++ )
			{
				l_return = m_geometryPassFrameBuffer->Attach( GetTextureAttachmentPoint( DsTexture( i ) )
															  , GetTextureAttachmentIndex( DsTexture( i ) )
															  , m_geometryPassTexAttachs[i]
															  , m_lightPassTextures[i]->GetType() );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->Attach( AttachmentPoint::eDepth, m_geometryPassDepthAttach );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->IsComplete();
			}

			m_geometryPassFrameBuffer->Unbind();
		}

		m_vertexBuffer->Initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
		m_viewport.Initialise();

		for ( auto & program : m_lightPassShaderPrograms )
		{
			program.m_program->Initialise();
			program.m_geometryBuffers = m_renderSystem.CreateGeometryBuffers( Topology::eTriangles, *program.m_program );
			program.m_geometryBuffers->Initialise( { *m_vertexBuffer }, nullptr );
		}

		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		for ( auto & program : m_lightPassShaderPrograms )
		{
			program.m_geometryBuffers->Cleanup();
			program.m_geometryBuffers.reset();
			program.m_program->Cleanup();
		}

		m_viewport.Cleanup();
		m_vertexBuffer->Cleanup();
		m_geometryPassFrameBuffer->Bind( FrameBufferMode::eConfig );
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();

		for ( auto & l_unit : m_lightPassTextures )
		{
			l_unit->Cleanup();
			l_unit.reset();
		}
	}

	bool RenderTechnique::DoBeginRender()
	{
		return true;
	}

	bool RenderTechnique::DoBeginOpaqueRendering()
	{
		GetEngine()->SetPerObjectLighting( false );
		bool l_return = m_geometryPassFrameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw );

		if ( l_return )
		{
			m_geometryPassFrameBuffer->Clear();
		}

		return l_return;
	}

	void RenderTechnique::DoEndOpaqueRendering()
	{
		m_geometryPassFrameBuffer->Unbind();
		// Render the light pass.

#if DEBUG_BUFFERS

		int l_width = int( m_size.width() );
		int l_height = int( m_size.height() );
		int l_thirdWidth = int( l_width / 3.0f );
		int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
		int l_thirdHeight = int( l_height / 3.0f );
		int l_twothirdHeight = int( 2.0f * l_height / 3.0f );
		m_geometryPassTexAttachs[size_t( DsTexture::ePosition )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_thirdHeight, l_thirdWidth, l_twothirdHeight ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eDiffuse )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_twothirdHeight, l_thirdWidth, l_height ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eNormals )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_thirdHeight ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eTangent )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_thirdHeight, l_twoThirdWidth, l_twothirdHeight ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eSpecular )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_twothirdHeight, l_twoThirdWidth, l_height ), InterpolationMode::eLinear );
		m_geometryPassTexAttachs[size_t( DsTexture::eEmissive )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, 0, l_width, l_thirdHeight ), InterpolationMode::eLinear );

#else

		if ( m_frameBuffer.m_frameBuffer->Bind( FrameBufferMode::eAutomatic, FrameBufferTarget::eDraw ) )
		{
			auto & l_scene = *m_renderTarget.GetScene();
			auto & l_camera = *m_renderTarget.GetCamera();
			m_frameBuffer.m_frameBuffer->SetClearColour(l_scene.GetBackgroundColour() );
			m_frameBuffer.m_frameBuffer->Clear();

			auto & l_program = m_lightPassShaderPrograms[l_scene.GetFlags()];
			l_program.m_pipeline->Apply();

			m_viewport.Resize( m_size );
			m_viewport.Update();
			l_program.m_pipeline->SetProjectionMatrix( m_viewport.GetProjection() );

			if ( l_program.m_camera )
			{
				l_program.m_pipeline->ApplyMatrices( *l_program.m_matrixUbo, 0xFFFFFFFFFFFFFFFF );
				auto & l_sceneBuffer = *l_program.m_sceneUbo;
				l_scene.GetLightCache().FillShader( l_sceneBuffer );
				l_scene.GetLightCache().BindLights();
				l_scene.GetFog().FillShader( l_sceneBuffer );
				l_scene.FillShader( l_sceneBuffer );
				l_camera.FillShader( l_sceneBuffer );

				m_lightPassTextures[size_t( DsTexture::ePosition )]->Bind();
				m_lightPassTextures[size_t( DsTexture::eDiffuse )]->Bind();
				m_lightPassTextures[size_t( DsTexture::eNormals )]->Bind();
				m_lightPassTextures[size_t( DsTexture::eTangent )]->Bind();
				m_lightPassTextures[size_t( DsTexture::eSpecular )]->Bind();
				m_lightPassTextures[size_t( DsTexture::eEmissive )]->Bind();

				l_program.m_program->BindUbos();
				l_program.m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
				l_program.m_program->UnbindUbos();

				m_lightPassTextures[size_t( DsTexture::eEmissive )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::eSpecular )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::eTangent )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::eNormals )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::eDiffuse )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::ePosition )]->Unbind();

				l_scene.GetLightCache().UnbindLights();
			}

			m_frameBuffer.m_frameBuffer->Unbind();
		}

		m_geometryPassFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer, Rectangle{ Position{}, m_size }, uint32_t( BufferComponent::eDepth ) );
		m_frameBuffer.m_frameBuffer->Bind();

#endif

	}

	bool RenderTechnique::DoBeginTransparentRendering()
	{
		GetEngine()->SetPerObjectLighting( true );
		return true;
	}

	void RenderTechnique::DoEndTransparentRendering()
	{
		m_frameBuffer.m_frameBuffer->Unbind();
	}

	void RenderTechnique::DoEndRender()
	{
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return true;
	}

	String RenderTechnique::DoGetOpaquePixelShaderSource(
		FlagCombination< TextureChannel > const & p_textureFlags,
		FlagCombination< ProgramFlag > const & p_programFlags,
		uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Inputs
		auto vtx_worldSpacePosition = l_writer.GetInput< Vec3 >( cuT( "vtx_worldSpacePosition" ) );
		auto vtx_normal = l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) );
		auto vtx_tangent = l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) );
		auto vtx_bitangent = l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) );
		auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );
		auto vtx_instance = l_writer.GetInput< Int >( cuT( "vtx_instance" ) );

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_textureFlags, TextureChannel::eColour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_textureFlags, TextureChannel::eAmbient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::eDiffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::eNormal ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::eSpecular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::eEmissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::eHeight ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::eGloss ) ) );

		auto gl_FragCoord( l_writer.GetBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		uint32_t l_index = 0;
		auto out_c3dPosition = l_writer.GetFragData< Vec4 >( cuT( "out_c3dPosition" ), l_index++ );
		auto out_c3dDiffuse = l_writer.GetFragData< Vec4 >( cuT( "out_c3dDiffuse" ), l_index++ );
		auto out_c3dNormal = l_writer.GetFragData< Vec4 >( cuT( "out_c3dNormal" ), l_index++ );
		auto out_c3dTangent = l_writer.GetFragData< Vec4 >( cuT( "out_c3dTangent" ), l_index++ );
		auto out_c3dSpecular = l_writer.GetFragData< Vec4 >( cuT( "out_c3dSpecular" ), l_index++ );
		auto out_c3dEmissive = l_writer.GetFragData< Vec4 >( cuT( "out_c3dEmissive" ), l_index++ );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), c3d_v4MatAmbient.xyz() );
			auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), c3d_v4MatDiffuse.xyz() );
			auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), c3d_v4MatSpecular.xyz() );
			auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), c3d_fMatShininess );
			auto l_v3Emissive = l_writer.GetLocale( cuT( "l_v3Emissive" ), c3d_v4MatEmissive.xyz() );
			auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), vtx_worldSpacePosition );
			auto l_v3Tangent = l_writer.GetLocale( cuT( "l_v3Tangent" ), normalize( vtx_tangent ) );

			ComputePreLightingMapContributions( l_writer, l_v3Normal, l_fMatShininess, p_textureFlags, p_programFlags, p_sceneFlags );
			ComputePostLightingMapContributions( l_writer, l_v3Ambient, l_v3Diffuse, l_v3Specular, l_v3Emissive, p_textureFlags, p_programFlags, p_sceneFlags );

			auto l_wvPosition = l_writer.GetLocale( cuT( "l_wvPosition" ), l_writer.Paren( c3d_mtxView * vec4( vtx_worldSpacePosition, 1.0 ) ).xyz() );
			out_c3dPosition = vec4( l_v3Position, l_v3Ambient.x() );
			out_c3dDiffuse = vec4( l_v3Diffuse, length( l_wvPosition ) );
			out_c3dNormal = vec4( l_v3Normal, l_v3Ambient.y() );
			out_c3dTangent = vec4( l_v3Tangent, l_v3Ambient.z() );
			out_c3dSpecular = vec4( l_v3Specular, l_fMatShininess );
			out_c3dEmissive = vec4( l_v3Emissive, l_wvPosition.z() );
		} );

		return l_writer.Finalise();
	}

	void RenderTechnique::DoUpdateOpaquePipeline( RenderPipeline & p_pipeline, DepthMapArray & p_depthMaps )const
	{
	}

	String RenderTechnique::DoGetLightPassVertexShaderSource(
		FlagCombination< TextureChannel > const & p_textureFlags,
		FlagCombination< ProgramFlag > const & p_programFlags,
		uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_MATRIX( l_writer );
		auto vertex = l_writer.GetAttribute< Vec2 >( ShaderProgram::Position );
		auto texture = l_writer.GetAttribute< Vec2 >( ShaderProgram::Texture );

		// Shader outputs
		auto vtx_texture = l_writer.GetOutput< Vec2 >( cuT( "vtx_texture" ) );
		auto gl_Position = l_writer.GetBuiltin< Vec4 >( cuT( "gl_Position" ) );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			vtx_texture = texture;
			gl_Position = c3d_mtxProjection * vec4( vertex.x(), vertex.y(), 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetLightPassPixelShaderSource(
		FlagCombination< TextureChannel > const & p_textureFlags,
		FlagCombination< ProgramFlag > const & p_programFlags,
		uint8_t p_sceneFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// Shader inputs
		UBO_SCENE( l_writer );
		auto vtx_texture = l_writer.GetInput< Vec2 >( cuT( "vtx_texture" ) );

		if ( l_writer.HasTextureBuffers() )
		{
			auto c3d_sLights = l_writer.GetUniform< SamplerBuffer >( ShaderProgram::Lights );
		}
		else
		{
			auto c3d_sLights = l_writer.GetUniform< Sampler1D >( ShaderProgram::Lights );
		}

		auto c3d_mapPosition = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::ePosition ) );
		auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eDiffuse ) );
		auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eNormals ) );
		auto c3d_mapTangent = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eTangent ) );
		auto c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eSpecular ) );
		auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::eEmissive ) );

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, CheckFlag( p_programFlags, ProgramFlag::eShadows ) ? ShadowType::ePoisson : ShadowType::eNone );
		GLSL::Fog l_fog{ p_sceneFlags, l_writer };

		// Shader outputs
		auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			auto l_v4Normal = l_writer.GetLocale( cuT( "l_v4Normal" ), texture( c3d_mapNormals, vtx_texture ) );
			auto l_v4Tangent = l_writer.GetLocale( cuT( "l_v4Tangent" ), texture( c3d_mapTangent, vtx_texture ) );
			auto l_v3Normal = l_writer.GetLocale( cuT( "l_v3Normal" ), l_v4Normal.xyz() );
			auto l_v3Tangent = l_writer.GetLocale( cuT( "l_v3Tangent" ), l_v4Tangent.xyz() );

			IF( l_writer, l_v3Normal != l_v3Tangent )
			{
				auto l_v4Position = l_writer.GetLocale( cuT( "l_v4Position" ), texture( c3d_mapPosition, vtx_texture ) );
				auto l_v4Diffuse = l_writer.GetLocale( cuT( "l_v4Diffuse" ), texture( c3d_mapDiffuse, vtx_texture ) );
				auto l_v4Specular = l_writer.GetLocale( cuT( "l_v4Specular" ), texture( c3d_mapSpecular, vtx_texture ) );
				auto l_v4Emissive = l_writer.GetLocale( cuT( "l_v4Emissive" ), texture( c3d_mapEmissive, vtx_texture ) );
				auto l_v3MapAmbient = l_writer.GetLocale( cuT( "l_v3MapAmbient" ), c3d_v4AmbientLight.xyz() + vec3( l_v4Position.w(), l_v4Normal.w(), l_v4Tangent.w() ) );
				auto l_v3MapDiffuse = l_writer.GetLocale( cuT( "l_v3MapDiffuse" ), l_v4Diffuse.xyz() );
				auto l_v3MapSpecular = l_writer.GetLocale( cuT( "l_v3MapSpecular" ), l_v4Specular.xyz() );
				auto l_v3MapEmissive = l_writer.GetLocale( cuT( "l_v3MapEmissive" ), l_v4Emissive.xyz() );
				auto l_fMatShininess = l_writer.GetLocale( cuT( "l_fMatShininess" ), l_v4Specular.w() );
				auto l_v3Position = l_writer.GetLocale( cuT( "l_v3Position" ), l_v4Position.xyz() );
				auto l_v3Specular = l_writer.GetLocale( cuT( "l_v3Specular" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Diffuse = l_writer.GetLocale( cuT( "l_v3Diffuse" ), vec3( 0.0_f, 0, 0 ) );
				auto l_v3Ambient = l_writer.GetLocale( cuT( "l_v3Ambient" ), vec3( 0.0_f, 0, 0 ) );
				auto l_worldEye = l_writer.GetLocale( cuT( "l_worldEye" ), vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );
				auto l_dist = l_writer.GetLocale( cuT( "l_dist" ), l_v4Diffuse.w() );
				auto l_y = l_writer.GetLocale( cuT( "l_y" ), l_v4Emissive.w() );

				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeCombinedLighting( l_worldEye
													 , l_fMatShininess
													 , FragmentInput( l_v3Position, l_v3Normal )
													 , l_output );

				pxl_v4FragColor = vec4( l_writer.Paren( l_writer.Paren( l_v3Ambient * l_v3MapAmbient.xyz() ) +
														l_writer.Paren( l_v3Diffuse * l_v3MapDiffuse.xyz() ) +
														l_writer.Paren( l_v3Specular * l_v3MapSpecular.xyz() ) +
														l_v3MapEmissive ), 1.0 );

				if ( p_sceneFlags != 0 )
				{
					l_fog.ApplyFog( pxl_v4FragColor, l_dist, l_y );
				}
			}
			ELSE
			{
				l_writer.Discard();
			}
			FI
		} );

		return l_writer.Finalise();
	}
}
