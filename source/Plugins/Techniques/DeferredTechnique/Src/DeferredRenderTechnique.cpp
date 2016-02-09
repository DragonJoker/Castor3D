#include "DeferredRenderTechnique.hpp"

#include <BlendStateManager.hpp>
#include <BufferDeclaration.hpp>
#include <BufferElementDeclaration.hpp>
#include <BufferElementGroup.hpp>
#include <CameraManager.hpp>
#include <ColourRenderBuffer.hpp>
#include <Context.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <DepthStencilStateManager.hpp>
#include <DynamicTexture.hpp>
#include <Engine.hpp>
#include <FrameBuffer.hpp>
#include <FrameVariableBuffer.hpp>
#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <LightManager.hpp>
#include <MatrixBuffer.hpp>
#include <OneFrameVariable.hpp>
#include <Pipeline.hpp>
#include <PointFrameVariable.hpp>
#include <RasteriserState.hpp>
#include <RenderBufferAttachment.hpp>
#include <RenderSystem.hpp>
#include <RenderTarget.hpp>
#include <Scene.hpp>
#include <SceneNode.hpp>
#include <ShaderManager.hpp>
#include <TextureAttachment.hpp>
#include <VertexBuffer.hpp>
#include <Viewport.hpp>

#include <Logger.hpp>

#if C3D_HAS_GL_RENDERER

#	include <GlslSource.hpp>
#	include <GlRenderSystem.hpp>
using namespace GlRender;

#endif

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

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
		: RenderTechniqueBase( cuT( "deferred" ), p_renderTarget, p_renderSystem, p_params )
		, m_viewport( Viewport::Ortho( *p_renderSystem->GetEngine(), 0, 1, 0, 1, 0, 1 ) )
	{
		Logger::LogInfo( cuT( "Using deferred shading" ) );
		m_geometryPassFrameBuffer = m_renderSystem->CreateFrameBuffer();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_lightPassTextures[i] = m_renderSystem->CreateDynamicTexture( eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
			m_lightPassTextures[i]->SetRenderTarget( p_renderTarget.shared_from_this() );
			m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->CreateAttachment( m_lightPassTextures[i] );
		}

		m_lightPassShaderProgram = GetEngine()->GetShaderManager().GetNewProgram();

		m_geometryPassDsState = GetEngine()->GetDepthStencilStateManager().Create( cuT( "GeometricPassState" ) );
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

		m_lightPassDsState = GetEngine()->GetDepthStencilStateManager().Create( cuT( "LightPassState" ) );
		m_lightPassDsState->SetStencilTest( true );
		m_lightPassDsState->SetStencilReadMask( 0xFFFFFFFF );
		m_lightPassDsState->SetStencilWriteMask( 0 );
		m_lightPassDsState->SetStencilFrontRef( 1 );
		m_lightPassDsState->SetStencilBackRef( 1 );
		m_lightPassDsState->SetStencilFrontFunc( eSTENCIL_FUNC_EQUAL );
		m_lightPassDsState->SetStencilBackFunc( eSTENCIL_FUNC_EQUAL );
		m_lightPassDsState->SetDepthTest( true );
		m_lightPassDsState->SetDepthMask( eWRITING_MASK_ZERO );

		m_lightPassBlendState = GetEngine()->GetBlendStateManager().Create( cuT( "LightPassState" ) );

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

		m_vertexBuffer = std::make_unique< VertexBuffer >( *m_renderSystem->GetEngine(), m_declaration );
		uint32_t l_stride = m_declaration.GetStride();
		m_vertexBuffer->Resize( sizeof( l_data ) );
		uint8_t * l_buffer = m_vertexBuffer->data();
		std::memcpy( l_buffer, l_data, sizeof( l_data ) );

		for ( auto && l_vertex : m_arrayVertex )
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

	RenderTechniqueBaseSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		bool l_return = m_geometryPassFrameBuffer->Create( 0 );

		for ( int i = 0; i < eDS_TEXTURE_COUNT && l_return; i++ )
		{
			l_return = m_lightPassTextures[i]->Create();
			m_lightPassTextures[i]->SetSampler( GetEngine()->GetLightsSampler() );
		}

		if ( l_return )
		{
			m_lightPassShaderProgram->CreateFrameVariable( ShaderProgram::Lights, eSHADER_TYPE_PIXEL );

			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_return; i++ )
			{
				m_lightPassShaderProgram->CreateFrameVariable( DS_TEXTURE_NAME[i], eSHADER_TYPE_PIXEL )->SetValue( m_lightPassTextures[i].get() );
			}

			m_lightPassMatrices = GetEngine()->GetShaderManager().CreateMatrixBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL | MASK_SHADER_TYPE_VERTEX );
			FrameVariableBufferSPtr l_scene = GetEngine()->GetShaderManager().CreateSceneBuffer( *m_lightPassShaderProgram, MASK_SHADER_TYPE_PIXEL );
			m_lightPassScene = l_scene;

			m_vertexBuffer->Create();
			eSHADER_MODEL l_model = GetEngine()->GetRenderSystem()->GetMaxShaderModel();
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
			m_lightPassTextures[i]->Destroy();
		}

		m_geometryPassFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = true;

		for ( int i = 0; i < eDS_TEXTURE_DEPTH && l_return; i++ )
		{
			m_lightPassTextures[i]->SetType( eTEXTURE_TYPE_2D );
			m_lightPassTextures[i]->SetImage( m_size, ePIXEL_FORMAT_ARGB32F );
			l_return = m_lightPassTextures[i]->Initialise( p_index++ );
		}

		if ( l_return )
		{
			m_lightPassTextures[eDS_TEXTURE_DEPTH]->SetType( eTEXTURE_TYPE_2D );
			m_lightPassTextures[eDS_TEXTURE_DEPTH]->SetImage( m_size, ePIXEL_FORMAT_DEPTH32 );
			l_return = m_lightPassTextures[eDS_TEXTURE_DEPTH]->Initialise( p_index++ );
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
				l_return = m_geometryPassFrameBuffer->Attach( eATTACHMENT_POINT_COLOUR, i, m_geometryPassTexAttachs[i], eTEXTURE_TARGET_2D );
			}

			if ( l_return )
			{
				l_return = m_geometryPassFrameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_geometryPassTexAttachs[eDS_TEXTURE_DEPTH], eTEXTURE_TARGET_2D );
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
		m_geometryBuffers = m_renderSystem->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *m_lightPassShaderProgram, m_vertexBuffer.get(), nullptr, nullptr, nullptr );
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

	bool RenderTechnique::DoBeginRender()
	{
		return m_geometryPassFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );;
	}

	bool RenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, double p_dFrameTime )
	{
		m_renderTarget->GetDepthStencilState()->Apply();
		m_renderTarget->GetRasteriserState()->Apply();
		//m_geometryPassDsState->Apply();
		return RenderTechniqueBase::DoRender( m_size, p_scene, p_camera, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		m_geometryPassFrameBuffer->Unbind();

#if DEBUG_BUFFERS

		int l_width = int( m_size.width() );
		int l_height = int( m_size.height() );
		int l_thirdWidth = int( l_width / 3.0f );
		int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
		int l_thirdHeight = int( l_height / 3.0f );
		int l_twothirdHeight = int( 2.0f * l_height / 3.0f );
		m_geometryPassTexAttachs[eDS_TEXTURE_POSITION]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, 0, l_thirdWidth, l_thirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_AMBIENT]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_thirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_DIFFUSE]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, 0, l_width, l_thirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_NORMALS]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_thirdHeight, l_thirdWidth, l_twothirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_TANGENT]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_thirdHeight, l_twoThirdWidth, l_twothirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_BITANGENT]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, l_thirdHeight, l_width, l_twothirdHeight ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_SPECULAR]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( 0, l_twothirdHeight, l_thirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_EMISSIVE]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_thirdWidth, l_twothirdHeight, l_twoThirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
		m_geometryPassTexAttachs[eDS_TEXTURE_DEPTH]->Blit( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( 0, 0, l_width, l_height ), Castor::Rectangle( l_twoThirdWidth, l_twothirdHeight, l_width, l_height ), eINTERPOLATION_MODE_LINEAR );

#else

		Pipeline & l_pipeline = m_renderSystem->GetPipeline();
		ContextRPtr l_pContext = m_renderSystem->GetCurrentContext();

		m_renderTarget->GetFrameBuffer()->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
		m_renderTarget->GetFrameBuffer()->SetClearColour( m_renderSystem->GetTopScene()->GetBackgroundColour() );
		m_renderTarget->GetFrameBuffer()->Clear();

		m_renderTarget->GetDepthStencilState()->Apply();
		m_renderTarget->GetRasteriserState()->Apply();
		m_lightPassBlendState->Apply();

		m_viewport.SetSize( m_size );
		m_viewport.Render( l_pipeline );
		l_pContext->CullFace( eFACE_BACK );

		if ( m_pShaderCamera )
		{
			bool l_return = true;
			//Point3r l_position = m_renderTarget->GetCamera()->GetParent()->GetDerivedPosition();
			//m_pShaderCamera->SetValue( l_position );
			l_pipeline.ApplyMatrices( *m_lightPassMatrices.lock(), 0xFFFFFFFFFFFFFFFF );
			auto & l_sceneBuffer = *m_lightPassScene.lock();
			m_renderSystem->GetTopScene()->GetLightManager().BindLights( *m_lightPassShaderProgram, l_sceneBuffer );
			m_renderSystem->GetTopScene()->GetCameraManager().BindCamera( l_sceneBuffer );
			m_lightPassShaderProgram->Bind();

			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_return; i++ )
			{
				l_return = m_lightPassTextures[i]->Bind();
			}

			if ( l_return )
			{
				m_geometryBuffers->Draw( m_arrayVertex.size(), 0 );

				for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
				{
					m_lightPassTextures[i]->Unbind();
				}
			}

			m_lightPassShaderProgram->Unbind();
			m_renderSystem->GetTopScene()->GetLightManager().UnbindLights( *m_lightPassShaderProgram, *m_lightPassScene.lock() );
		}

#endif
	}

	String RenderTechnique::DoGetPixelShaderSource( uint32_t p_flags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlPixelShaderSource( p_flags );
		}
#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

	String RenderTechnique::DoGetLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlLightPassVertexShaderSource( p_uiProgramFlags );
		}

#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

	String RenderTechnique::DoGetLightPassPixelShaderSource( uint32_t p_flags )const
	{
		if ( !m_renderSystem )
		{
			CASTOR_EXCEPTION( "No renderer selected" );
		}

#if C3D_HAS_GL_RENDERER

		if ( m_renderSystem->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			return DoGetGlLightPassPixelShaderSource( p_flags );
		}

#endif

		CASTOR_EXCEPTION( "No renderer selected" );
	}

#if C3D_HAS_GL_RENDERER

	String RenderTechnique::DoGetGlPixelShaderSource( uint32_t p_flags )const
	{
#define CHECK_FLAG( channel ) ( ( p_flags & ( channel ) ) == ( channel ) )

		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();

		// UBOs
		UBO_MATRIX( l_writer );
		UBO_SCENE( l_writer );
		UBO_PASS( l_writer );

		// Fragment Intputs
		IN( l_writer, Vec3, vtx_vertex );
		IN( l_writer, Vec3, vtx_normal );
		IN( l_writer, Vec3, vtx_tangent );
		IN( l_writer, Vec3, vtx_bitangent );
		IN( l_writer, Vec3, vtx_texture );

		Optional< Sampler2D > c3d_mapColour( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapColour" ), CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) ) );
		Optional< Sampler2D > c3d_mapAmbient( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapAmbient" ), CHECK_FLAG( eTEXTURE_CHANNEL_AMBIENT ) ) );
		Optional< Sampler2D > c3d_mapDiffuse( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapDiffuse" ), CHECK_FLAG( eTEXTURE_CHANNEL_DIFFUSE ) ) );
		Optional< Sampler2D > c3d_mapNormal( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapNormal" ), CHECK_FLAG( eTEXTURE_CHANNEL_NORMAL ) ) );
		Optional< Sampler2D > c3d_mapOpacity( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapOpacity" ), CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) ) );
		Optional< Sampler2D > c3d_mapSpecular( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapSpecular" ), CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) ) );
		Optional< Sampler2D > c3d_mapHeight( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapHeight" ), CHECK_FLAG( eTEXTURE_CHANNEL_HEIGHT ) ) );
		Optional< Sampler2D > c3d_mapGloss( l_writer.GetUniform< Sampler2D >( cuT( "c3d_mapGloss" ), CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) ) );

		uint32_t l_index = 0;
		FRAG_OUTPUT( l_writer, Vec4, out_c3dPosition, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dAmbient, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dDiffuse, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dNormal, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dTangent, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dBitangent, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dSpecular, l_index++ );
		FRAG_OUTPUT( l_writer, Vec4, out_c3dEmissive, l_index++ );

		std::function< void() > l_main = [&]()
		{
			LOCALE_ASSIGN( l_writer, Float, l_fAlpha, c3d_fMatOpacity );

			if ( CHECK_FLAG( eTEXTURE_CHANNEL_OPACITY ) )
			{
				l_fAlpha *= texture2D( c3d_mapOpacity, vtx_texture.XY ).R;
			}

			IF( l_writer, cuT( "l_fAlpha > 0" ) )
			{
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, vtx_vertex );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, normalize( vtx_normal ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, normalize( vtx_tangent ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, normalize( vtx_bitangent ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, c3d_v4MatDiffuse.XYZ );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, c3d_v4MatAmbient.XYZ );
				LOCALE_ASSIGN( l_writer, Vec4, l_v4Specular, vec4( c3d_v4MatSpecular.XYZ, 0 ) );
				LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, c3d_v4MatEmissive.XYZ );

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_COLOUR ) )
				{
					l_v3Ambient *= texture2D( c3d_mapColour, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_AMBIENT ) )
				{
					l_v3Ambient *= texture2D( c3d_mapAmbient, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_DIFFUSE ) )
				{
					l_v3Diffuse *= texture2D( c3d_mapDiffuse, vtx_texture.XY ).XYZ;
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_NORMAL ) )
				{
					l_v3Normal += normalize( texture2D( c3d_mapNormal, vtx_texture.XY ).XYZ * 2.0f - 1.0f );
					l_v3Tangent -= l_v3Normal * dot( l_v3Tangent, l_v3Normal );
					l_v3Bitangent = cross( l_v3Normal, l_v3Tangent );
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_SPECULAR ) )
				{
					if ( CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) )
					{
						l_v4Specular = vec4( c3d_v4MatSpecular.XYZ * texture2D( c3d_mapSpecular, vtx_texture.XY ).XYZ, texture2D( c3d_mapGloss, vtx_texture.XY ).X );
					}
					else
					{
						l_v4Specular = vec4( c3d_v4MatSpecular.XYZ * texture2D( c3d_mapSpecular, vtx_texture.XY ).XYZ, c3d_fMatShininess );
					}
				}
				else if ( CHECK_FLAG( eTEXTURE_CHANNEL_GLOSS ) )
				{
					l_v4Specular = vec4( c3d_v4MatSpecular.XYZ, texture2D( c3d_mapGloss, vtx_texture.XY ).X );
				}
				else
				{
					l_v4Specular = vec4( c3d_v4MatSpecular.XYZ, c3d_fMatShininess );
				}

				if ( CHECK_FLAG( eTEXTURE_CHANNEL_HEIGHT ) )
				{
					LOCALE_ASSIGN( l_writer, Vec3, l_v3MapHeight, texture2D( c3d_mapHeight, vtx_texture.XY ).XYZ );
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

#undef CHECK_FLAG
	}

	String RenderTechnique::DoGetGlLightPassVertexShaderSource( uint32_t p_uiProgramFlags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_VERTEX );
		l_writer << GLSL::Version() << Endl();

		// Shader inputs
		ATTRIBUTE( l_writer, Vec2, vertex );
		ATTRIBUTE( l_writer, Vec2, texture );
		UBO_MATRIX( l_writer );

		// Shader outputs
		OUT( l_writer, Vec2, vtx_texture );

		l_writer.ImplementFunction< void >( cuT( "main" ), [&]()
		{
			vtx_texture = texture;
			BUILTIN( l_writer, Vec4, gl_Position ) = c3d_mtxProjection * vec4( vertex.X, vertex.Y, 0.0, 1.0 );
		} );

		return l_writer.Finalise();
	}

	String RenderTechnique::DoGetGlLightPassPixelShaderSource( uint32_t p_flags )const
	{
		using namespace GLSL;

		GlslWriter l_writer( static_cast< GlRenderSystem * >( m_renderSystem )->GetOpenGl(), eSHADER_TYPE_PIXEL );
		l_writer << GLSL::Version() << Endl();

		IN( l_writer, Vec2, vtx_texture );

		if ( l_writer.GetOpenGl().HasTbo() )
		{
			UNIFORM( l_writer, SamplerBuffer, c3d_sLights );
		}
		else
		{
			UNIFORM( l_writer, Sampler1D, c3d_sLights );
		}

		UNIFORM( l_writer, Sampler2D, c3d_mapPosition );
		UNIFORM( l_writer, Sampler2D, c3d_mapAmbient );
		UNIFORM( l_writer, Sampler2D, c3d_mapDiffuse );
		UNIFORM( l_writer, Sampler2D, c3d_mapNormals );
		UNIFORM( l_writer, Sampler2D, c3d_mapTangent );
		UNIFORM( l_writer, Sampler2D, c3d_mapBitangent );
		UNIFORM( l_writer, Sampler2D, c3d_mapSpecular );
		UNIFORM( l_writer, Sampler2D, c3d_mapEmissive );
		UNIFORM( l_writer, Sampler2D, c3d_mapDepth );
		UBO_SCENE( l_writer );

		FRAG_OUTPUT( l_writer, Vec4, pxl_v4FragColor, 0 );

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
			LOCALE_ASSIGN( l_writer, Float, l_fMatShininess, l_v4Specular.W );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3MapSpecular, l_v4Specular.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Position, l_v4Position.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Normal, l_v4Normal.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Bitangent, l_v4Bitangent.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Tangent, l_v4Tangent.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Specular, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Diffuse, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Ambient, vec3( Float( &l_writer, 0 ), 0, 0 ) );
			LOCALE_ASSIGN( l_writer, Vec3, l_v3Emissive, l_v4Emissive.XYZ );
			LOCALE_ASSIGN( l_writer, Vec3, l_eye, l_v3Position - c3d_v3CameraPosition );
			LOCALE_ASSIGN( l_writer, Vec3, l_worldEye, vec3( c3d_v3CameraPosition.X, c3d_v3CameraPosition.Y, c3d_v3CameraPosition.Z ) );

			LOCALE_ASSIGN( l_writer, Int, l_begin, Int( 0 ) );
			LOCALE_ASSIGN( l_writer, Int, l_end, c3d_iLightsCount.X );

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeDirectionalLight( l_lighting->GetDirectionalLight( i ), l_worldEye, l_fMatShininess,
													 FragmentInput{ l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
													 l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.Y;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputePointLight( l_lighting->GetPointLight( i ), l_worldEye, l_fMatShininess,
											   FragmentInput{ l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
											   l_output );
			}
			ROF;

			l_begin = l_end;
			l_end += c3d_iLightsCount.Z;

			FOR( l_writer, Int, i, l_begin, cuT( "i < l_end" ), cuT( "++i" ) )
			{
				OutputComponents l_output{ l_v3Ambient, l_v3Diffuse, l_v3Specular };
				l_lighting->ComputeSpotLight( l_lighting->GetSpotLight( i ), l_worldEye, l_fMatShininess,
											  FragmentInput{ l_v3Position, l_v3Normal, l_v3Tangent, l_v3Bitangent },
											  l_output );
			}
			ROF;

			pxl_v4FragColor = vec4( l_writer.Paren( l_writer.Paren( Int( &l_writer, 1 ) - l_v4Emissive.W ) *
													l_writer.Paren( l_writer.Paren( l_v3Ambient + l_v4Ambient.XYZ ) +
																	l_writer.Paren( l_v3Diffuse * l_v4Diffuse.XYZ ) +
																	l_writer.Paren( l_v3Specular * l_v4Specular.XYZ ) +
																	l_v3Emissive ) ) +
									l_writer.Paren( l_v4Emissive.W * l_v4Diffuse.XYZ ), 1 );
		} );

		return l_writer.Finalise();
	}

#endif
}
