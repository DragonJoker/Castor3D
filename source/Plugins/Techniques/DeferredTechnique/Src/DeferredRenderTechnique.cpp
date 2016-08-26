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
#include <Render/Pipeline.hpp>
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
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "deferred" ), p_renderTarget, p_renderSystem, p_params )
		, m_viewport( *p_renderSystem.GetEngine() )
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		Logger::LogInfo( cuT( "Using deferred shading" ) );
		m_geometryPassFrameBuffer = m_renderSystem.CreateFrameBuffer();
		uint32_t l_index{ 0u };

		for ( auto & l_unit : m_lightPassTextures )
		{
			auto l_texture = m_renderSystem.CreateTexture( TextureType::TwoDimensions, AccessType::None, AccessType::Read | AccessType::Write );
			m_geometryPassTexAttachs[l_index] = m_geometryPassFrameBuffer->CreateAttachment( l_texture );
			l_unit = std::make_shared< TextureUnit >( *GetEngine() );
			l_unit->SetIndex( ++l_index );
			l_unit->SetTexture( l_texture );
			l_unit->SetSampler( GetEngine()->GetLightsSampler() );
		}
		
		m_lightPassDepthBuffer = m_geometryPassFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::D32F );
		m_geometryPassDepthAttach = m_geometryPassFrameBuffer->CreateAttachment( m_lightPassDepthBuffer );

		m_lightPassShaderProgram = GetEngine()->GetShaderProgramCache().GetNewProgram();

		DepthStencilState l_dsstate;
		l_dsstate.SetDepthTest( true );
		l_dsstate.SetDepthMask( WritingMask::Zero );
		m_lightPassPipeline = p_renderSystem.CreatePipeline( std::move( l_dsstate ), RasteriserState{}, BlendState{}, MultisampleState{} );

		m_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::Position ), ElementType::Vec2 ),
			BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::TexCoords ), ElementType::Vec2 ),
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
		uint32_t l_stride = m_declaration.GetStride();
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
		m_geometryBuffers.reset();
		m_lightPassShaderProgram.reset();
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
			m_lightPassShaderProgram->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::Lights, ShaderType::Pixel );

			for ( int i = 0; i < int( DsTexture::Count ); i++ )
			{
				m_lightPassShaderProgram->CreateFrameVariable< OneIntFrameVariable >( GetTextureName( DsTexture( i ) ), ShaderType::Pixel )->SetValue( i + 1 );
			}

			m_lightPassMatrices = GetEngine()->GetShaderProgramCache().CreateMatrixBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL | MASK_SHADER_TYPE_VERTEX );
			FrameVariableBufferSPtr l_scene = GetEngine()->GetShaderProgramCache().CreateSceneBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL );
			m_lightPassScene = l_scene;

			m_vertexBuffer->Create();
			eSHADER_MODEL l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			m_lightPassShaderProgram->SetSource( ShaderType::Vertex, l_model, DoGetLightPassVertexShaderSource( 0 ) );
			m_lightPassShaderProgram->SetSource( ShaderType::Pixel, l_model, DoGetLightPassPixelShaderSource( 0 ) );
		}

		return l_return;
	}

	void RenderTechnique::DoDestroy()
	{
		m_vertexBuffer->Destroy();

		for ( auto & l_unit : m_lightPassTextures )
		{
			l_unit->Cleanup();
		}
		
		m_lightPassDepthBuffer->Destroy();
		m_geometryPassFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = true;
		
		for ( size_t i = 0; i < size_t( DsTexture::Count ); i++ )
		{
			m_lightPassTextures[i]->GetTexture()->GetImage().SetSource( m_size, GetTextureFormat( DsTexture( i ) ) );
			m_lightPassTextures[i]->Initialise();
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
			l_return = m_geometryPassFrameBuffer->Bind( FrameBufferMode::Config );
		}

		if ( l_return )
		{
			for ( int i = 0; i < size_t( DsTexture::Count ) && l_return; i++ )
			{
				l_return = m_geometryPassFrameBuffer->Attach( GetTextureAttachmentPoint( DsTexture( i ) )
															  , GetTextureAttachmentIndex( DsTexture( i ) )
															  , m_geometryPassTexAttachs[i]
															  , m_lightPassTextures[i]->GetType() );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->Attach( AttachmentPoint::Depth, m_geometryPassDepthAttach );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->IsComplete();
			}

			m_geometryPassFrameBuffer->Unbind();
		}

		m_lightPassShaderProgram->Initialise();
		m_lightPassMatrices = m_lightPassShaderProgram->FindFrameVariableBuffer( ShaderProgram::BufferMatrix );
		FrameVariableBufferSPtr l_scene = m_lightPassShaderProgram->FindFrameVariableBuffer( ShaderProgram::BufferScene );
		l_scene->GetVariable( ShaderProgram::CameraPos, m_pShaderCamera );
		m_lightPassScene = l_scene;
		m_vertexBuffer->Initialise( BufferAccessType::Static, BufferAccessNature::Draw );
		m_geometryBuffers = m_renderSystem.CreateGeometryBuffers( Topology::Triangles, *m_lightPassShaderProgram );
		m_geometryBuffers->Initialise( m_vertexBuffer, nullptr, nullptr, nullptr, nullptr );
		m_viewport.Initialise();
		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		m_viewport.Cleanup();
		m_geometryBuffers.reset();
		m_pShaderCamera.reset();
		m_vertexBuffer->Cleanup();
		m_lightPassShaderProgram->Cleanup();
		m_geometryPassFrameBuffer->Bind( FrameBufferMode::Config );
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();

		for ( auto & l_unit : m_lightPassTextures )
		{
			l_unit->Cleanup();
		}
	}

	bool RenderTechnique::DoBeginRender( Scene & p_scene, Camera & p_camera )
	{
		m_camera = &p_camera;
		m_scene = &p_scene;
		return true;
	}

	bool RenderTechnique::DoBeginOpaqueRendering()
	{
		static Colour l_colour{ Colour::from_components( 0, 0, 0, 0 ) };
		GetEngine()->SetPerObjectLighting( false );
		bool l_return = m_geometryPassFrameBuffer->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw );

		if ( l_return )
		{
			m_geometryPassFrameBuffer->SetClearColour( l_colour );
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
		m_geometryPassTexAttachs[size_t( DsTexture::Position )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_thirdHeight, l_thirdWidth, l_twothirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[size_t( DsTexture::Diffuse )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_twothirdHeight, l_thirdWidth, l_height ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[size_t( DsTexture::Normals )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_thirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[size_t( DsTexture::Tangent )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_thirdHeight, l_twoThirdWidth, l_twothirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[size_t( DsTexture::Specular )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_twothirdHeight, l_twoThirdWidth, l_height ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[size_t( DsTexture::Emissive )]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, 0, l_width, l_thirdHeight ), InterpolationMode::Linear );

#else

		if ( m_frameBuffer.m_frameBuffer->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw ) )
		{
			m_frameBuffer.m_frameBuffer->SetClearColour( m_scene->GetBackgroundColour() );
			m_frameBuffer.m_frameBuffer->Clear();

			m_lightPassPipeline->Apply();

			m_viewport.Resize( m_size );
			m_viewport.Update();
			m_lightPassPipeline->SetProjectionMatrix( m_viewport.GetProjection() );

			if ( m_pShaderCamera )
			{
				m_lightPassPipeline->ApplyMatrices( *m_lightPassMatrices.lock(), 0xFFFFFFFFFFFFFFFF );
				auto & l_sceneBuffer = *m_lightPassScene.lock();
				m_scene->GetLightCache().BindLights( *m_lightPassShaderProgram, l_sceneBuffer );
				m_camera->FillShader( l_sceneBuffer );

				m_lightPassTextures[size_t( DsTexture::Position )]->Bind();
				m_lightPassTextures[size_t( DsTexture::Diffuse )]->Bind();
				m_lightPassTextures[size_t( DsTexture::Normals )]->Bind();
				m_lightPassTextures[size_t( DsTexture::Tangent )]->Bind();
				m_lightPassTextures[size_t( DsTexture::Specular )]->Bind();
				m_lightPassTextures[size_t( DsTexture::Emissive )]->Bind();

				m_lightPassShaderProgram->Bind();
				m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );
				m_lightPassShaderProgram->Unbind();
				
				m_lightPassTextures[size_t( DsTexture::Emissive )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::Specular )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::Tangent )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::Normals )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::Diffuse )]->Unbind();
				m_lightPassTextures[size_t( DsTexture::Position )]->Unbind();

				m_scene->GetLightCache().UnbindLights( *m_lightPassShaderProgram, *m_lightPassScene.lock() );
			}
		}

		m_frameBuffer.m_frameBuffer->Unbind();
		m_geometryPassFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer, Rectangle{ Position{}, m_size }, uint32_t( BufferComponent::Depth ) );

#endif

	}

	bool RenderTechnique::DoBeginTransparentRendering()
	{
		GetEngine()->SetPerObjectLighting( true );
		return m_frameBuffer.m_frameBuffer->Bind( FrameBufferMode::Automatic, FrameBufferTarget::Draw );
	}

	void RenderTechnique::DoEndTransparentRendering()
	{
		m_frameBuffer.m_frameBuffer->Unbind();
	}

	void RenderTechnique::DoEndRender( Scene & p_scene, Camera & p_camera )
	{
		m_camera = nullptr;
		m_scene = nullptr;
	}

	bool RenderTechnique::DoWriteInto (TextFile & p_file)
	{
		return true;
	}

	String RenderTechnique::DoGetOpaquePixelShaderSource( uint32_t p_textureFlags, uint32_t p_programFlags )const
	{
		using namespace GLSL;
		GlslWriter l_writer = GetEngine()->GetRenderSystem()->CreateGlslWriter();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		auto vtx_vertex( l_writer.GetInput< Vec3 >( cuT( "vtx_vertex" ) ) );
		auto vtx_normal( l_writer.GetInput< Vec3 >( cuT( "vtx_normal" ) ) );
		auto vtx_tangent( l_writer.GetInput< Vec3 >( cuT( "vtx_tangent" ) ) );
		auto vtx_bitangent( l_writer.GetInput< Vec3 >( cuT( "vtx_bitangent" ) ) );
		auto vtx_texture( l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) ) );

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_textureFlags, TextureChannel::Colour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_textureFlags, TextureChannel::Ambient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_textureFlags, TextureChannel::Diffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_textureFlags, TextureChannel::Normal ) ) );
		auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_textureFlags, TextureChannel::Opacity ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_textureFlags, TextureChannel::Specular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_textureFlags, TextureChannel::Emissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_textureFlags, TextureChannel::Height ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_textureFlags, TextureChannel::Gloss ) ) );

		uint32_t l_index = 0;
		auto out_c3dPosition = l_writer.GetFragData< Vec4 >( cuT( "out_c3dPosition" ), l_index++ );
		auto out_c3dDiffuse = l_writer.GetFragData< Vec4 >( cuT( "out_c3dDiffuse" ), l_index++ );
		auto out_c3dNormal = l_writer.GetFragData< Vec4 >( cuT( "out_c3dNormal" ), l_index++ );
		auto out_c3dTangent = l_writer.GetFragData< Vec4 >( cuT( "out_c3dTangent" ), l_index++ );
		auto out_c3dSpecular = l_writer.GetFragData< Vec4 >( cuT( "out_c3dSpecular" ), l_index++ );
		auto out_c3dEmissive = l_writer.GetFragData< Vec4 >( cuT( "out_c3dEmissive" ), l_index++ );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vec3( vtx_normal.x(), vtx_normal.y(), vtx_normal.z() ) ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, c3d_v4MatAmbient.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, c3d_v4MatDiffuse.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, c3d_v4MatSpecular.xyz() );
			LOCALE_ASSIGN( l_writer, Float, l_fMatShininess, c3d_fMatShininess );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, vtx_vertex );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, normalize( vtx_tangent ) );

			if ( CheckFlag( p_textureFlags, TextureChannel::Normal ) )
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_v3MapNormal, texture2D( c3d_mapNormal, vtx_texture.xy() ).xyz() );
				l_v3MapNormal = Float( &l_writer, 2.0f ) * l_v3MapNormal - vec3( Int( &l_writer, 1 ), 1.0, 1.0 );
				l_v3Normal = normalize( mat3( vtx_tangent, vtx_bitangent, vtx_normal ) * l_v3MapNormal );
				//l_v3Tangent -= l_v3Normal * dot( l_v3Tangent, l_v3Normal );
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Gloss ) )
			{
				l_fMatShininess = texture2D( c3d_mapGloss, vtx_texture.xy() ).r();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Emissive ) )
			{
				l_v3Emissive = texture2D( c3d_mapEmissive, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Colour ) )
			{
				l_v3Ambient *= texture2D( c3d_mapColour, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Ambient ) )
			{
				l_v3Ambient *= texture2D( c3d_mapAmbient, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Diffuse ) )
			{
				l_v3Diffuse *= texture2D( c3d_mapDiffuse, vtx_texture.xy() ).xyz();
			}

			if ( CheckFlag( p_textureFlags, TextureChannel::Specular ) )
			{
				l_v3Specular *= texture2D( c3d_mapSpecular, vtx_texture.xy() ).xyz();
			}

			out_c3dPosition = vec4( l_v3Position, l_v3Ambient.x() );
			out_c3dDiffuse = vec4( l_v3Diffuse, 0 );
			out_c3dNormal = vec4( l_v3Normal, l_v3Ambient.y() );
			out_c3dTangent = vec4( l_v3Tangent, l_v3Ambient.z() );
			out_c3dSpecular = vec4( l_v3Specular, l_fMatShininess );
			out_c3dEmissive = vec4( l_v3Emissive, 0 );
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
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

	String RenderTechnique::DoGetLightPassPixelShaderSource( uint32_t p_flags )const
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

		auto c3d_mapPosition = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::Position ) );
		auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::Diffuse ) );
		auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::Normals ) );
		auto c3d_mapTangent = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::Tangent ) );
		auto c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::Specular ) );
		auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( GetTextureName( DsTexture::Emissive ) );

		// Shader outputs
		auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );
		auto gl_FragDepth = l_writer.GetBuiltin< Float >( cuT( "gl_FragDepth" ) );

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, p_flags );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Normal, texture2D( c3d_mapNormals, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Tangent, texture2D( c3d_mapTangent, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, l_v4Normal.xyz() );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, l_v4Tangent.xyz() );

			IF (l_writer, l_v3Normal != l_v3Tangent )
			{
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Position, texture2D( c3d_mapPosition, vtx_texture ) );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Diffuse, texture2D( c3d_mapDiffuse, vtx_texture ) );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Specular, texture2D( c3d_mapSpecular, vtx_texture ) );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Emissive, texture2D( c3d_mapEmissive, vtx_texture ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3MapAmbient, vec3( l_v4Position.w(), l_v4Normal.w(), l_v4Tangent.w() ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3MapDiffuse, l_v4Diffuse.xyz() );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3MapSpecular, l_v4Specular.xyz() );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3MapEmissive, l_v4Emissive.xyz() );
				LOCALE_ASSIGN( l_writer, Float, l_fMatShininess, l_v4Specular.w() );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, l_v4Position.xyz() );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, cross( l_v3Tangent.xyz(), l_v3Normal.xyz() ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( &l_writer, 0 ), 0, 0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( &l_writer, 0 ), 0, 0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( &l_writer, 0 ), 0, 0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_worldEye, vec3( c3d_v3CameraPosition.x(), c3d_v3CameraPosition.y(), c3d_v3CameraPosition.z() ) );

				LOCALE_ASSIGN( l_writer, Int, l_begin, Int( 0 ) );
				LOCALE_ASSIGN( l_writer, Int, l_end, c3d_iLightsCount.x() );

				FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
				{
					OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
					l_lighting->ComputeDirectionalLight( l_lighting->GetDirectionalLight( i ), l_worldEye, l_fMatShininess,
														 FragmentInput { l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
														 l_output );
				}
				ROF;

				l_begin = l_end;
				l_end += c3d_iLightsCount.y();

				FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
				{
					OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
					l_lighting->ComputePointLight( l_lighting->GetPointLight( i ), l_worldEye, l_fMatShininess,
												   FragmentInput { l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
												   l_output );
				}
				ROF;

				l_begin = l_end;
				l_end += c3d_iLightsCount.z();

				FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
				{
					OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
					l_lighting->ComputeSpotLight( l_lighting->GetSpotLight( i ), l_worldEye, l_fMatShininess,
												  FragmentInput { l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
												  l_output );
				}
				ROF;

				pxl_v4FragColor = vec4( l_writer.Paren( l_writer.Paren( l_v3Ambient * l_v3MapAmbient.xyz() ) +
														l_writer.Paren( l_v3Diffuse * l_v3MapDiffuse.xyz() ) +
														l_writer.Paren( l_v3Specular * l_v3MapSpecular.xyz() ) +
														l_v3MapEmissive ), 1.0 );
			}
			ELSE
			{
				l_writer.Discard();
			}
			FI
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::GetTextureName( DsTexture p_texture )
	{
		static std::array< String, size_t( DsTexture::Count ) > Values
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

	PixelFormat RenderTechnique::GetTextureFormat( DsTexture p_texture )
	{
		static std::array< PixelFormat, size_t( DsTexture::Count ) > Values
		{
			{
				PixelFormat::RGBA16F32F,
				PixelFormat::RGBA16F32F,
				PixelFormat::RGBA16F32F,
				PixelFormat::RGBA16F32F,
				PixelFormat::RGBA16F32F,
				PixelFormat::RGBA16F32F,
			}
		};

		return Values[size_t( p_texture )];
	}

	AttachmentPoint RenderTechnique::GetTextureAttachmentPoint( DsTexture p_texture )
	{
		static std::array< AttachmentPoint, size_t( DsTexture::Count ) > Values
		{
			{
				AttachmentPoint::Colour,
				AttachmentPoint::Colour,
				AttachmentPoint::Colour,
				AttachmentPoint::Colour,
				AttachmentPoint::Colour,
				AttachmentPoint::Colour,
			}
		};

		return Values[size_t( p_texture )];
	}

	uint32_t RenderTechnique::GetTextureAttachmentIndex( DsTexture p_texture )
	{
		static std::array< uint32_t, size_t( DsTexture::Count ) > Values
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
