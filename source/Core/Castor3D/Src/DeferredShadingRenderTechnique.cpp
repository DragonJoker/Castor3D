#include "DeferredShadingRenderTechnique.hpp"
#include "FrameBuffer.hpp"
#include "ColourRenderBuffer.hpp"
#include "DepthStencilRenderBuffer.hpp"
#include "DepthStencilState.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "RenderTarget.hpp"
#include "DepthStencilState.hpp"
#include "RasteriserState.hpp"
#include "Engine.hpp"
#include "RenderSystem.hpp"
#include "BufferElementDeclaration.hpp"
#include "BufferDeclaration.hpp"
#include "BufferElementGroup.hpp"
#include "VertexBuffer.hpp"
#include "GeometryBuffers.hpp"
#include "Viewport.hpp"
#include "Context.hpp"
#include "ShaderProgram.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"
#include "FrameVariableBuffer.hpp"
#include "Pipeline.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"
#include "ShaderManager.hpp"
#include "Camera.hpp"
#include "DynamicTexture.hpp"
#include "IndexBuffer.hpp"
#include "MatrixBuffer.hpp"
#include "VertexBuffer.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	String g_strNames[] =
	{
		cuT( "c3d_mapPosition" ),
		cuT( "c3d_mapDiffuse" ),
		cuT( "c3d_mapNormals" ),
		cuT( "c3d_mapTangent" ),
		cuT( "c3d_mapBitangent" ),
		cuT( "c3d_mapSpecular" ),
	};

	DeferredShadingRenderTechnique::DeferredShadingRenderTechnique()
		:	RenderTechniqueBase( cuT( "deferred" ) )
	{
	}

	DeferredShadingRenderTechnique::DeferredShadingRenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
		:	RenderTechniqueBase( cuT( "deferred" ), p_renderTarget, p_pRenderSystem, p_params )
	{
		Logger::LogMessage( cuT( "Using deferred shading" ) );
		m_pDsFrameBuffer = m_pRenderTarget->CreateFrameBuffer();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_pDsTextures[i] = m_pRenderSystem->CreateDynamicTexture();
			m_pDsTextures[i]->SetRenderTarget( true );
			m_pDsTexAttachs[i] = m_pRenderTarget->CreateAttachment( m_pDsTextures[i] );
		}

		m_pDsBufDepth = m_pDsFrameBuffer->CreateDepthStencilRenderBuffer( ePIXEL_FORMAT_DEPTH24S8 );
		m_pDsDepthAttach = m_pRenderTarget->CreateAttachment( m_pDsBufDepth );
		m_pDsShaderProgram = m_pEngine->GetShaderManager().GetNewProgram();
		m_pDsGeometricState = m_pEngine->CreateDepthStencilState( cuT( "GeometricPassState" ) );
		m_pDsLightsState = m_pEngine->CreateDepthStencilState( cuT( "LightPassState" ) );

		m_pDsGeometricState->SetStencilTest( true );
		m_pDsGeometricState->SetStencilReadMask( 0xFFFFFFFF );
		m_pDsGeometricState->SetStencilWriteMask( 0xFFFFFFFF );
		m_pDsGeometricState->SetStencilFrontRef( 1 );
		m_pDsGeometricState->SetStencilBackRef( 1 );
		m_pDsGeometricState->SetStencilFrontFunc( eSTENCIL_FUNC_NEVER );
		m_pDsGeometricState->SetStencilBackFunc( eSTENCIL_FUNC_NEVER );
		m_pDsGeometricState->SetStencilFrontFailOp( eSTENCIL_OP_REPLACE );
		m_pDsGeometricState->SetStencilBackFailOp( eSTENCIL_OP_REPLACE );
		m_pDsGeometricState->SetStencilFrontDepthFailOp( eSTENCIL_OP_KEEP );
		m_pDsGeometricState->SetStencilBackDepthFailOp( eSTENCIL_OP_KEEP );
		m_pDsGeometricState->SetStencilFrontPassOp( eSTENCIL_OP_KEEP );
		m_pDsGeometricState->SetStencilBackPassOp( eSTENCIL_OP_KEEP );
		m_pDsGeometricState->SetDepthTest( true );
		m_pDsGeometricState->SetDepthMask( eWRITING_MASK_ALL );

		m_pDsLightsState->SetStencilTest( true );
		m_pDsLightsState->SetStencilReadMask( 0xFFFFFFFF );
		m_pDsLightsState->SetStencilWriteMask( 0 );
		m_pDsLightsState->SetStencilFrontRef( 1 );
		m_pDsLightsState->SetStencilBackRef( 1 );
		m_pDsLightsState->SetStencilFrontFunc( eSTENCIL_FUNC_EQUAL );
		m_pDsLightsState->SetStencilBackFunc( eSTENCIL_FUNC_EQUAL );
		m_pDsLightsState->SetDepthTest( true );
		m_pDsLightsState->SetDepthMask( eWRITING_MASK_ZERO );

		BufferElementDeclaration l_vertexDeclarationElements[] =
		{
			BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION,	eELEMENT_TYPE_3FLOATS ),
			BufferElementDeclaration( 0, eELEMENT_USAGE_TEXCOORDS0,	eELEMENT_TYPE_2FLOATS ),
		};

		real l_pBuffer[] =
		{
			0, 0, 0, 0, 0,
			0, 1, 0, 0, 1,
			1, 1, 0, 1, 1,
			1, 0, 0, 1, 0,
		};

		std::memcpy( m_pBuffer, l_pBuffer, sizeof( l_pBuffer ) );
		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
		uint32_t l_uiStride = m_pDeclaration->GetStride();
		uint32_t i = 0;

		std::for_each( m_arrayVertex.begin(), m_arrayVertex.end(), [&]( BufferElementGroupSPtr & p_vertex )
		{
			p_vertex = std::make_shared< BufferElementGroup >( &reinterpret_cast< uint8_t * >( m_pBuffer )[i++ * m_pDeclaration->GetStride()] );
		} );

		VertexBufferUPtr l_pVtxBuffer = std::make_unique< VertexBuffer >( m_pRenderSystem, l_vertexDeclarationElements );
		IndexBufferUPtr l_pIdxBuffer = std::make_unique< IndexBuffer >( m_pRenderSystem );

		l_pVtxBuffer->Resize( 4 * l_uiStride );
		m_arrayVertex[0]->LinkCoords( &l_pVtxBuffer->data()[0 * l_uiStride], l_uiStride );
		m_arrayVertex[1]->LinkCoords( &l_pVtxBuffer->data()[1 * l_uiStride], l_uiStride );
		m_arrayVertex[2]->LinkCoords( &l_pVtxBuffer->data()[2 * l_uiStride], l_uiStride );
		m_arrayVertex[3]->LinkCoords( &l_pVtxBuffer->data()[3 * l_uiStride], l_uiStride );

		l_pIdxBuffer->AddElement( 0 );
		l_pIdxBuffer->AddElement( 2 );
		l_pIdxBuffer->AddElement( 1 );
		l_pIdxBuffer->AddElement( 0 );
		l_pIdxBuffer->AddElement( 3 );
		l_pIdxBuffer->AddElement( 2 );
		m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), std::move( l_pIdxBuffer ), nullptr );

		m_pViewport = std::make_shared< Viewport >( m_pRenderSystem->GetEngine(), Size( 10, 10 ), eVIEWPORT_TYPE_2D );
		m_pViewport->SetLeft( real( 0.0 ) );
		m_pViewport->SetRight( real( 1.0 ) );
		m_pViewport->SetTop( real( 1.0 ) );
		m_pViewport->SetBottom( real( 0.0 ) );
		m_pViewport->SetNear( real( 0.0 ) );
		m_pViewport->SetFar( real( 1.0 ) );
	}

	DeferredShadingRenderTechnique::~DeferredShadingRenderTechnique()
	{
		m_pViewport.reset();
		m_pGeometryBuffers.reset();
		m_pDsShaderProgram.reset();
		m_pDeclaration.reset();
	}

	RenderTechniqueBaseSPtr DeferredShadingRenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueBaseSPtr( new DeferredShadingRenderTechnique( p_renderTarget, p_pRenderSystem, p_params ) );
	}

	bool DeferredShadingRenderTechnique::DoCreate()
	{
		m_pRenderSystem->GetMainContext()->SetDeferredShading( true );
		bool l_bReturn = m_pDsFrameBuffer->Create( 0 );

		for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
		{
			l_bReturn = m_pDsTextures[i]->Create();
			m_pDsTextures[i]->SetSampler( m_sampler );
		}

		l_bReturn &= m_pDsBufDepth->Create();

		if ( l_bReturn )
		{
			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
			{
				m_pDsShaderProgram->CreateFrameVariable( g_strNames[i], eSHADER_TYPE_PIXEL )->SetValue( m_pDsTextures[i].get() );
			}

			m_pGeometryBuffers->GetVertexBuffer().Create();
			m_pGeometryBuffers->GetIndexBuffer().Create();

			for ( int i = 0; i < eSHADER_MODEL_COUNT; i++ )
			{
				eSHADER_MODEL l_eModel = eSHADER_MODEL( i );

				if ( m_pRenderSystem->CheckSupport( l_eModel ) )
				{
					m_pDsShaderProgram->SetSource( eSHADER_TYPE_VERTEX,	l_eModel, m_pDsShaderProgram->GetVertexShaderSource( ePROGRAM_FLAG_DEFERRED, true ) );
					m_pDsShaderProgram->SetSource( eSHADER_TYPE_PIXEL,	l_eModel, m_pDsShaderProgram->GetPixelShaderSource( eTEXTURE_CHANNEL_LGHTPASS ) );
				}
			}
		}

		return l_bReturn;
	}

	void DeferredShadingRenderTechnique::DoDestroy()
	{
		m_pGeometryBuffers->GetVertexBuffer().Destroy();
		m_pGeometryBuffers->GetIndexBuffer().Destroy();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_pDsTextures[i]->Destroy();
		}

		m_pDsBufDepth->Destroy();
		m_pDsFrameBuffer->Destroy();
	}

	bool DeferredShadingRenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_bReturn = true;

		for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
		{
			m_pDsTextures[i]->SetDimension(	eTEXTURE_DIMENSION_2D );

			if ( i != eDS_TEXTURE_POSITION )
			{
				m_pDsTextures[i]->SetImage(	m_size, ePIXEL_FORMAT_A8R8G8B8 );
			}
			else
			{
				m_pDsTextures[i]->SetImage(	m_size, ePIXEL_FORMAT_ARGB32F );
			}

			m_pDsTextures[i]->Initialise( p_index++ );
		}

		m_pDsBufDepth->Initialise( m_size );

		if ( m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG ) )
		{
			for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
			{
				l_bReturn &= m_pDsTexAttachs[i]->Attach( eATTACHMENT_POINT( eATTACHMENT_POINT_COLOUR0 + i ),	m_pDsFrameBuffer, eTEXTURE_TARGET_2D );
			}

			l_bReturn &= m_pDsDepthAttach->Attach( eATTACHMENT_POINT_DEPTH,	m_pDsFrameBuffer );
			m_pDsFrameBuffer->Unbind();
		}

		m_pDsGeometricState->Initialise();
		m_pDsLightsState->Initialise();

		m_pDsShaderProgram->Initialise();
		m_pDsShaderProgram->GetSceneBuffer()->GetVariable( ShaderProgramBase::CameraPos, m_pShaderCamera );
		m_pGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, m_pDsShaderProgram );
		m_pGeometryBuffers->GetIndexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, m_pDsShaderProgram );
		m_pGeometryBuffers->Initialise();
		return l_bReturn;
	}

	void DeferredShadingRenderTechnique::DoCleanup()
	{
		m_pShaderCamera.reset();
		m_pDsGeometricState->Cleanup();
		m_pDsLightsState->Cleanup();
		m_pGeometryBuffers->Cleanup();
		m_pDsShaderProgram->Cleanup();
		m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_pDsTexAttachs[i]->Detach();
		}

		m_pDsDepthAttach->Detach();
		m_pDsFrameBuffer->Unbind();

		for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
		{
			m_pDsTextures[i]->Cleanup();
		}

		m_pDsBufDepth->Cleanup();
	}

	bool DeferredShadingRenderTechnique::DoBeginRender()
	{
		return m_pDsFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );;
	}

	bool DeferredShadingRenderTechnique::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_pDsGeometricState->Apply();
		return RenderTechniqueBase::DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void DeferredShadingRenderTechnique::DoEndRender()
	{
		Size const & l_size = m_pRenderTarget->GetSize();
		Size l_halfSize( l_size.width() / 2, l_size.height() / 2 );
		Pipeline * l_pPipeline = m_pRenderSystem->GetPipeline();
		ContextRPtr l_pContext = m_pRenderSystem->GetCurrentContext();
		m_pDsFrameBuffer->Unbind();

		if ( m_pViewport )
		{
			bool l_bReturn = true;
			m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
			m_pDsLightsState->Apply();
//			m_pRenderTarget->GetDepthStencilState()->Apply();
			m_pRenderTarget->GetRasteriserState()->Apply();
			m_pRenderTarget->GetRenderer()->BeginScene();
			l_pContext->SetClearColour( m_pRenderSystem->GetTopScene()->GetBackgroundColour() );
			l_pContext->Clear( eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH | eBUFFER_COMPONENT_STENCIL );
			m_pViewport->SetSize( m_size );
			m_pViewport->Render();
			l_pContext->CullFace( eFACE_BACK );
			l_pPipeline->MatrixMode( eMTXMODE_MODEL );
			l_pPipeline->LoadIdentity();
			l_pPipeline->MatrixMode( eMTXMODE_VIEW );
			l_pPipeline->LoadIdentity();

			if ( m_pShaderCamera )
			{
				Point3r l_position = m_pRenderTarget->GetCamera()->GetParent()->GetDerivedPosition();
				m_pShaderCamera->SetValue( l_position );
				m_pDsShaderProgram->Begin( 0, 1 );
				l_pPipeline->ApplyMatrices( *m_pDsShaderProgram );

#if DEBUG_BUFFERS
				int l_width = int( m_size.width() );
				int l_height = int( m_size.height() );
				int l_thirdWidth = int( l_width / 3.0f );
				int l_twoThirdWidth = int( 2.0f * l_width / 3.0f );
				int l_halfHeight = int( l_height / 2.0f );
				m_pDsTexAttachs[eDS_TEXTURE_POSITION]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( 0, 0, l_thirdWidth, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_DIFFUSE]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_thirdWidth, 0, l_twoThirdWidth, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_NORMALS]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_twoThirdWidth, 0, l_width, l_halfHeight ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_TANGENT]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( 0, l_halfHeight, l_thirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_BITANGENT]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_thirdWidth, l_halfHeight, l_twoThirdWidth, l_height ), eINTERPOLATION_MODE_LINEAR );
				m_pDsTexAttachs[eDS_TEXTURE_SPECULAR]->Blit( m_pFrameBuffer, Rectangle( 0, 0, l_width, l_height ), Rectangle( l_twoThirdWidth, l_halfHeight, l_width, l_height ), eINTERPOLATION_MODE_LINEAR );
#else

				for ( int i = 0; i < eDS_TEXTURE_COUNT && l_bReturn; i++ )
				{
					l_bReturn = m_pDsTextures[i]->Bind();
				}

				if ( l_bReturn )
				{
					m_pGeometryBuffers->Draw( eTOPOLOGY_TRIANGLES, m_pDsShaderProgram, m_pGeometryBuffers->GetIndexBuffer().GetSize(), 0 );

					for ( int i = 0; i < eDS_TEXTURE_COUNT; i++ )
					{
						m_pDsTextures[i]->Unbind();
					}
				}

#endif

				m_pDsShaderProgram->End();
			}

			m_pRenderTarget->GetRenderer()->EndScene();
			m_pFrameBuffer->Unbind();
		}
	}
}
