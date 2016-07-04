#include "DeferredRenderTechnique.hpp"

#include <BlendStateCache.hpp>
#include <CameraCache.hpp>
#include <DepthStencilStateCache.hpp>
#include <Engine.hpp>
#include <LightCache.hpp>
#include <RasteriserStateCache.hpp>
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
#include <Render/Viewport.hpp>
#include <Shader/FrameVariableBuffer.hpp>
#include <Shader/OneFrameVariable.hpp>
#include <Shader/PointFrameVariable.hpp>
#include <Texture/TextureLayout.hpp>

#include <Logger.hpp>

#include <GlslSource.hpp>

#define DEBUG_BUFFERS 0

using namespace Castor;
using namespace Castor3D;

namespace Deferred
{
	String DS_TEXTURE_NAME[] =
	{
		cuT( "c3d_mapPosition" ),
		cuT( "c3d_mapAmbient" ),
		cuT( "c3d_mapDiffuse" ),
		cuT( "c3d_mapNormals" ),
		cuT( "c3d_mapTangent" ),
		cuT( "c3d_mapBitangent" ),
		cuT( "c3d_mapSpecular" ),
		cuT( "c3d_mapEmissive" ),
		cuT( "c3d_mapDepth" ),
	};

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "deferred" ), p_renderTarget, p_renderSystem, p_params )
		, m_viewport( *p_renderSystem.GetEngine() )
	{
		m_viewport.SetOrtho( 0, 1, 0, 1, 0, 1 );
		Logger::LogInfo( cuT( "Using deferred shading" ) );
		m_geometryPassFrameBuffer = m_renderSystem.CreateFrameBuffer();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			auto l_texture = m_renderSystem.CreateTexture( TextureType::TwoDimensions, eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
			m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->CreateAttachment( l_texture );
			m_lightPassTextures[i] = std::make_shared< TextureUnit >( *GetEngine() );
			m_lightPassTextures[i]->SetIndex( i );
			m_lightPassTextures[i]->SetTexture( l_texture );
			m_lightPassTextures[i]->SetSampler( GetEngine()->GetLightsSampler() );
		}

		m_lightPassShaderProgram = GetEngine()->GetShaderCache().GetNewProgram();

		m_geometryPassDsState = GetEngine()->GetDepthStencilStateCache().Add( cuT( "GeometricPassState" ) );
		m_geometryPassDsState->SetStencilTest( true );
		m_geometryPassDsState->SetStencilReadMask( 0xFFFFFFFF );
		m_geometryPassDsState->SetStencilWriteMask( 0xFFFFFFFF );
		m_geometryPassDsState->SetStencilFrontRef( 1 );
		m_geometryPassDsState->SetStencilBackRef( 1 );
		m_geometryPassDsState->SetStencilFrontFunc( eSTENCIL_FUNC_NEVER );
		m_geometryPassDsState->SetStencilBackFunc( eSTENCIL_FUNC_NEVER );
		m_geometryPassDsState->SetStencilFrontFailOp( eSTENCIL_OP_REPLACE );
		m_geometryPassDsState->SetStencilBackFailOp( eSTENCIL_OP_REPLACE );
		m_geometryPassDsState->SetStencilFrontDepthFailOp( eSTENCIL_OP_KEEP );
		m_geometryPassDsState->SetStencilBackDepthFailOp( eSTENCIL_OP_KEEP );
		m_geometryPassDsState->SetStencilFrontPassOp( eSTENCIL_OP_KEEP );
		m_geometryPassDsState->SetStencilBackPassOp( eSTENCIL_OP_KEEP );
		m_geometryPassDsState->SetDepthTest( true );
		m_geometryPassDsState->SetDepthMask( eWRITING_MASK_ALL );

		m_lightPassDsState = GetEngine()->GetDepthStencilStateCache().Add( cuT( "LightPassState" ) );
		m_lightPassDsState->SetStencilTest( true );
		m_lightPassDsState->SetStencilReadMask( 0xFFFFFFFF );
		m_lightPassDsState->SetStencilWriteMask( 0 );
		m_lightPassDsState->SetStencilFrontRef( 1 );
		m_lightPassDsState->SetStencilBackRef( 1 );
		m_lightPassDsState->SetStencilFrontFunc( eSTENCIL_FUNC_EQUAL );
		m_lightPassDsState->SetStencilBackFunc( eSTENCIL_FUNC_EQUAL );
		m_lightPassDsState->SetDepthTest( true );
		m_lightPassDsState->SetDepthMask( eWRITING_MASK_ZERO );

		m_lightPassBlendState = GetEngine()->GetBlendStateCache().Add( cuT( "LightPassState" ) );

		m_declaration = BufferDeclaration(
		{
			BufferElementDeclaration( ShaderProgram::Position, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_2FLOATS ),
			BufferElementDeclaration( ShaderProgram::Texture, eELEMENT_USAGE_TEXCOORDS, eELEMENT_TYPE_2FLOATS ),
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

		GetEngine()->SetPerObjectLighting( false );
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
			m_lightPassShaderProgram->CreateFrameVariable< OneIntFrameVariable >( ShaderProgram::Lights, eSHADER_TYPE_PIXEL );

			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_return; i++ )
			{
				m_lightPassShaderProgram->CreateFrameVariable< OneIntFrameVariable >( DS_TEXTURE_NAME[i], eSHADER_TYPE_PIXEL )->SetValue( i );
			}

			m_lightPassMatrices = GetEngine()->GetShaderCache().CreateMatrixBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL | MASK_SHADER_TYPE_VERTEX );
			FrameVariableBufferSPtr l_scene = GetEngine()->GetShaderCache().CreateSceneBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL );
			m_lightPassScene = l_scene;

			m_vertexBuffer->Create();
			eSHADER_MODEL l_model = GetEngine()->GetRenderSystem()->GetGpuInformations().GetMaxShaderModel();
			m_lightPassShaderProgram->SetSource( eSHADER_TYPE_VERTEX, l_model, DoGetLightPassVertexShaderSource( 0 ) );
			m_lightPassShaderProgram->SetSource( eSHADER_TYPE_PIXEL, l_model, DoGetLightPassPixelShaderSource( 0 ) );
		}

		return l_return;
	}

	void RenderTechnique::DoDestroy()
	{
		m_vertexBuffer->Destroy();

		for ( int i = 0; i < eDS_TEXTURE_DEPTH; i++ )
		{
			m_lightPassTextures[i]->Cleanup();
		}

		m_geometryPassFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = true;

		for ( int i = 0; i < eDS_TEXTURE_DEPTH && l_return; i++ )
		{
			m_lightPassTextures[i]->GetTexture()->GetImage().SetSource( m_size, ePIXEL_FORMAT_ARGB16F32F );
			m_lightPassTextures[i]->Initialise();
			p_index++;
		}

		if ( l_return )
		{
			m_lightPassTextures[eDS_TEXTURE_DEPTH]->GetTexture()->GetImage().SetSource( m_size, ePIXEL_FORMAT_DEPTH32 );
			m_lightPassTextures[eDS_TEXTURE_DEPTH]->Initialise();
			p_index++;
		}

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		}

		if ( l_return )
		{
			for ( int i = 0; i < eDS_TEXTURE_DEPTH && l_return; i++ )
			{
				l_return = m_geometryPassFrameBuffer->Attach( eATTACHMENT_POINT_COLOUR, i, m_geometryPassTexAttachs[i], m_lightPassTextures[i]->GetType() );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_geometryPassTexAttachs[eDS_TEXTURE_DEPTH], m_lightPassTextures[eDS_TEXTURE_DEPTH]->GetType() );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->IsComplete();
			}

			m_geometryPassFrameBuffer->Unbind();
		}

		m_geometryPassDsState->Initialise();
		m_lightPassDsState->Initialise();
		m_lightPassBlendState->Initialise();

		m_lightPassShaderProgram->Initialise();
		m_lightPassMatrices = m_lightPassShaderProgram->FindFrameVariableBuffer( ShaderProgram::BufferMatrix );
		FrameVariableBufferSPtr l_scene = m_lightPassShaderProgram->FindFrameVariableBuffer( ShaderProgram::BufferScene );
		l_scene->GetVariable( ShaderProgram::CameraPos, m_pShaderCamera );
		m_lightPassScene = l_scene;
		m_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
		m_geometryBuffers = m_renderSystem.CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_lightPassShaderProgram );
		m_geometryBuffers->Initialise( m_vertexBuffer, nullptr, nullptr, nullptr, nullptr );
		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		m_geometryBuffers.reset();
		m_pShaderCamera.reset();
		m_geometryPassDsState->Cleanup();
		m_lightPassDsState->Cleanup();
		m_lightPassBlendState->Cleanup();
		m_vertexBuffer->Cleanup();
		m_lightPassShaderProgram->Cleanup();
		m_geometryPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();


		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_lightPassTextures[i]->Cleanup();
		}
	}

	bool RenderTechnique::DoBeginRender( Scene & p_scene )
	{
		bool l_return = m_geometryPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_return )
		{
			m_geometryPassFrameBuffer->SetClearColour( p_scene.GetBackgroundColour() );
			m_geometryPassFrameBuffer->Clear();
		}

		return l_return;
	}

	void RenderTechnique::DoRender( stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, uint32_t p_frameTime )
	{
		m_renderTarget.GetDepthStencilState()->Apply();
		//m_geometryPassDsState->Apply();
		Castor3D::RenderTechnique::DoRender( m_size, p_nodes, p_camera, p_frameTime );
	}

	void RenderTechnique::DoEndRender( Scene & p_scene )
	{
		m_geometryPassFrameBuffer->Unbind();

#if DEBUG_BUFFERS

		int l_width = int( m_size.width() );
		int l_height = int( m_size.height() );
		int l_thirdWidth = int( l_width / 3.0f );
		int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
		int l_thirdHeight = int( l_height / 3.0f );
		int l_twothirdHeight = int( 2.0f * l_height / 3.0f );
		m_geometryPassTexAttachs[eDS_TEXTURE_POSITION]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, 0, l_thirdWidth, l_thirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[eDS_TEXTURE_AMBIENT]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_thirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[eDS_TEXTURE_DIFFUSE]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, 0, l_width, l_thirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[eDS_TEXTURE_NORMALS]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_thirdHeight, l_thirdWidth, l_twothirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[eDS_TEXTURE_TANGENT]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_thirdHeight, l_twoThirdWidth, l_twothirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[eDS_TEXTURE_BITANGENT]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, l_thirdHeight, l_width, l_twothirdHeight ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[eDS_TEXTURE_SPECULAR]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_twothirdHeight, l_thirdWidth, l_height ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[eDS_TEXTURE_EMISSIVE]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_twothirdHeight, l_twoThirdWidth, l_height ), InterpolationMode::Linear );
		m_geometryPassTexAttachs[eDS_TEXTURE_DEPTH]->Blit( m_frameBuffer.m_frameBuffer, Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, l_twothirdHeight, l_width, l_height ), InterpolationMode::Linear );

#else

		if ( m_frameBuffer.m_frameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW ) )
		{
			Pipeline & l_pipeline = m_renderSystem.GetCurrentContext()->GetPipeline();

			m_frameBuffer.m_frameBuffer->SetClearColour( p_scene.GetBackgroundColour() );
			m_frameBuffer.m_frameBuffer->Clear();

			m_renderTarget.GetDepthStencilState()->Apply();
			m_renderTarget.GetRasteriserState()->Apply();
			m_lightPassBlendState->Apply();

			m_viewport.Resize( m_size );
			m_viewport.Render( l_pipeline );

			if ( m_pShaderCamera )
			{
				bool l_return = true;
				//Point3r l_position = m_renderTarget.GetCamera()->GetParent()->GetDerivedPosition();
				//m_pShaderCamera->SetValue( l_position );
				l_pipeline.ApplyMatrices( *m_lightPassMatrices.lock(), 0xFFFFFFFFFFFFFFFF );
				auto & l_sceneBuffer = *m_lightPassScene.lock();
				p_scene.GetLightCache().BindLights( *m_lightPassShaderProgram, l_sceneBuffer );
				BindCamera( p_scene.GetCameraCache(), l_sceneBuffer );
				m_lightPassShaderProgram->Bind();

				for ( int i = 0; i < eDS_TEXTURE_COUNT && l_return; i++ )
				{
					m_lightPassTextures[i]->Bind();
				}

				if ( l_return )
				{
					m_geometryBuffers->Draw( uint32_t( m_arrayVertex.size() ), 0 );

					for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
					{
						m_lightPassTextures[i]->Unbind();
					}
				}

				m_lightPassShaderProgram->Unbind();
				p_scene.GetLightCache().UnbindLights( *m_lightPassShaderProgram, *m_lightPassScene.lock() );
			}
		}

		m_frameBuffer.m_frameBuffer->Unbind();

#endif
	}

	String RenderTechnique::DoGetPixelShaderSource( uint32_t p_flags )const
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
		auto vtx_texture = l_writer.GetInput< Vec3 >( cuT( "vtx_texture" ) );

		auto c3d_mapColour( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapColour, CheckFlag( p_flags, TextureChannel::Colour ) ) );
		auto c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapAmbient, CheckFlag( p_flags, TextureChannel::Ambient ) ) );
		auto c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapDiffuse, CheckFlag( p_flags, TextureChannel::Diffuse ) ) );
		auto c3d_mapNormal( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapNormal, CheckFlag( p_flags, TextureChannel::Normal ) ) );
		auto c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapOpacity, CheckFlag( p_flags, TextureChannel::Opacity ) ) );
		auto c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapSpecular, CheckFlag( p_flags, TextureChannel::Specular ) ) );
		auto c3d_mapEmissive( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapEmissive, CheckFlag( p_flags, TextureChannel::Emissive ) ) );
		auto c3d_mapHeight( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapHeight, CheckFlag( p_flags, TextureChannel::Height ) ) );
		auto c3d_mapGloss( l_writer.GetUniform< Sampler2D >( ShaderProgram::MapGloss, CheckFlag( p_flags, TextureChannel::Gloss ) ) );

		uint32_t l_index = 0;
		auto out_c3dPosition = l_writer.GetFragData< Vec4 >( cuT( "out_c3dPosition" ), l_index++ );
		auto out_c3dAmbient = l_writer.GetFragData< Vec4 >( cuT( "out_c3dAmbient" ), l_index++ );
		auto out_c3dDiffuse = l_writer.GetFragData< Vec4 >( cuT( "out_c3dDiffuse" ), l_index++ );
		auto out_c3dNormal = l_writer.GetFragData< Vec4 >( cuT( "out_c3dNormal" ), l_index++ );
		auto out_c3dTangent = l_writer.GetFragData< Vec4 >( cuT( "out_c3dTangent" ), l_index++ );
		auto out_c3dBitangent = l_writer.GetFragData< Vec4 >( cuT( "out_c3dBitangent" ), l_index++ );
		auto out_c3dSpecular = l_writer.GetFragData< Vec4 >( cuT( "out_c3dSpecular" ), l_index++ );
		auto out_c3dEmissive = l_writer.GetFragData< Vec4 >( cuT( "out_c3dEmissive" ), l_index++ );

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );

			if ( CheckFlag( p_flags, TextureChannel::Opacity ) )
			{
				l_fAlpha *= texture2D( c3d_mapOpacity, vtx_texture.SWIZZLE_XY ).SWIZZLE_R;
			}

			IF( l_writer, cuT( "l_fAlpha > 0" ) )
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, vtx_vertex );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vtx_normal ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, normalize( vtx_tangent ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, normalize( vtx_bitangent ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, c3d_v4MatDiffuse.SWIZZLE_XYZ );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, c3d_v4MatAmbient.SWIZZLE_XYZ );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Specular, vec4( c3d_v4MatSpecular.SWIZZLE_XYZ, 0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.SWIZZLE_XYZ );

				if ( CheckFlag( p_flags, TextureChannel::Colour ) )
				{
					l_v3Ambient = texture2D( c3d_mapColour, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;

					if ( CheckFlag( p_flags, TextureChannel::Ambient ) )
					{
						l_v3Ambient *= texture2D( c3d_mapAmbient, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
					}
				}
				else if ( CheckFlag( p_flags, TextureChannel::Ambient ) )
				{
					l_v3Ambient = texture2D( c3d_mapAmbient, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
				}

				if ( CheckFlag( p_flags, TextureChannel::Emissive ) )
				{
					l_v3Emissive = texture2D( c3d_mapEmissive, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
				}

				if ( CheckFlag( p_flags, TextureChannel::Diffuse ) )
				{
					l_v3Diffuse = texture2D( c3d_mapDiffuse, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ;
				}

				if ( CheckFlag( p_flags, TextureChannel::Normal ) )
				{
					l_v3Normal += normalize( texture2D( c3d_mapNormal, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ * 2.0f - 1.0f );
					l_v3Tangent -= l_v3Normal * dot( l_v3Tangent, l_v3Normal );
					l_v3Bitangent = cross( l_v3Normal, l_v3Tangent );
				}

				if ( CheckFlag( p_flags, TextureChannel::Specular ) )
				{
					if ( CheckFlag( p_flags, TextureChannel::Gloss ) )
					{
						l_v4Specular = vec4( c3d_v4MatSpecular.SWIZZLE_XYZ * texture2D( c3d_mapSpecular, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ, texture2D( c3d_mapGloss, vtx_texture.SWIZZLE_XY ).SWIZZLE_X );
					}
					else
					{
						l_v4Specular = vec4( c3d_v4MatSpecular.SWIZZLE_XYZ * texture2D( c3d_mapSpecular, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ, c3d_fMatShininess );
					}
				}
				else if ( CheckFlag( p_flags, TextureChannel::Gloss ) )
				{
					l_v4Specular = vec4( c3d_v4MatSpecular.SWIZZLE_XYZ, texture2D( c3d_mapGloss, vtx_texture.SWIZZLE_XY ).SWIZZLE_X );
				}
				else
				{
					l_v4Specular = vec4( c3d_v4MatSpecular.SWIZZLE_XYZ, c3d_fMatShininess );
				}

				if ( CheckFlag( p_flags, TextureChannel::Height ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapHeight, texture2D( c3d_mapHeight, vtx_texture.SWIZZLE_XY ).SWIZZLE_XYZ );
				}

				out_c3dPosition = vec4( l_v3Position, 1 );
				out_c3dAmbient = vec4( l_v3Ambient, 1 );
				out_c3dDiffuse = vec4( l_v3Diffuse, l_fAlpha );
				out_c3dNormal = vec4( l_v3Normal, 1 );
				out_c3dTangent = vec4( l_v3Tangent, 1 );
				out_c3dBitangent = vec4( l_v3Bitangent, 1 );
				out_c3dSpecular = vec4( l_v4Specular );
				out_c3dEmissive = vec4( l_v3Emissive, 0 );
			}
			ELSE
			{
				l_writer.Discard();
			}
			FI
		};

		l_writer.ImplementFunction< void >( cuT( "main" ), l_main );
		return l_writer.Finalise();
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return true;
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
			gl_Position = c3d_mtxProjection * vec4( vertex.SWIZZLE_X, vertex.SWIZZLE_Y, 0.0, 1.0 );
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

		auto c3d_mapPosition = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapPosition" ) );
		auto c3d_mapAmbient = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapAmbient" ) );
		auto c3d_mapDiffuse = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapDiffuse" ) );
		auto c3d_mapNormals = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapNormals" ) );
		auto c3d_mapTangent = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapTangent" ) );
		auto c3d_mapBitangent = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapBitangent" ) );
		auto c3d_mapSpecular = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapSpecular" ) );
		auto c3d_mapEmissive = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapEmissive" ) );
		auto c3d_mapDepth = l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapDepth" ) );

		// Shader outputs
		auto pxl_v4FragColor = l_writer.GetFragData< Vec4 >( cuT( "pxl_v4FragColor" ), 0 );

		std::unique_ptr< LightingModel > l_lighting = l_writer.CreateLightingModel( PhongLightingModel::Name, p_flags );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Position, texture2D( c3d_mapPosition, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Ambient, texture2D( c3d_mapAmbient, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Diffuse, texture2D( c3d_mapDiffuse, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Normal, texture2D( c3d_mapNormals, vtx_texture ) / 2.0 );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Tangent, texture2D( c3d_mapTangent, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Bitangent, texture2D( c3d_mapBitangent, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Specular, texture2D( c3d_mapSpecular, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Emissive, texture2D( c3d_mapEmissive, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Vec4, l_v4Depth, texture2D( c3d_mapDepth, vtx_texture ) );
			LOCALE_ASSIGN( l_writer, Float, l_fMatShininess, l_v4Specular.SWIZZLE_W );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3MapSpecular, l_v4Specular.SWIZZLE_XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, l_v4Position.SWIZZLE_XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, l_v4Normal.SWIZZLE_XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, l_v4Bitangent.SWIZZLE_XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, l_v4Tangent.SWIZZLE_XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, l_v4Emissive.SWIZZLE_XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_eye, l_v3Position - c3d_v3CameraPosition );
			LOCALE_ASSIGN( l_writer, Vec3, l_worldEye, vec3( c3d_v3CameraPosition.SWIZZLE_X, c3d_v3CameraPosition.SWIZZLE_Y, c3d_v3CameraPosition.SWIZZLE_Z ) );

			LOCALE_ASSIGN( l_writer, Int, l_begin, Int( 0 ) );
			LOCALE_ASSIGN( l_writer, Int, l_end, c3d_iLightsCount.SWIZZLE_X );

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( l_lighting->GetDirectionalLight( i ), l_worldEye, l_fMatShininess,
													 FragmentInput { l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
													 l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.SWIZZLE_Y;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( l_lighting->GetPointLight( i ), l_worldEye, l_fMatShininess,
											   FragmentInput { l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
											   l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.SWIZZLE_Z;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output { l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( l_lighting->GetSpotLight( i ), l_worldEye, l_fMatShininess,
											  FragmentInput { l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
											  l_output );
			}
			ROF;

			pxl_v4FragColor = vec4( l_writer.Paren( l_writer.Paren( Int( &l_writer, 1 ) - l_v4Emissive.SWIZZLE_W ) *
													l_writer.Paren( l_writer.Paren( l_v3Ambient + l_v4Ambient.SWIZZLE_XYZ ) +
															l_writer.Paren( l_v3Diffuse * l_v4Diffuse.SWIZZLE_XYZ ) +
															l_writer.Paren( l_v3Specular * l_v4Specular.SWIZZLE_XYZ ) +
															l_v3Emissive ) ) +
									l_writer.Paren( l_v4Emissive.SWIZZLE_W * l_v4Diffuse.SWIZZLE_XYZ ), 1 );
		} );

		return l_writer.Finalise();
	}
}
