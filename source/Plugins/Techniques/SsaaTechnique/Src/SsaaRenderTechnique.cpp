#include "SsaaRenderTechnique.hpp"

#include <BlendState.hpp>
#include <Camera.hpp>
#include <ColourRenderBuffer.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <DepthStencilState.hpp>
#include <DynamicTexture.hpp>
#include <Engine.hpp>
#include <FrameBuffer.hpp>
#include <Parameter.hpp>
#include <RasteriserState.hpp>
#include <RenderBufferAttachment.hpp>
#include <RenderSystem.hpp>
#include <RenderTarget.hpp>
#include <Scene.hpp>
#include <TextureAttachment.hpp>
#include <StaticTexture.hpp>
#include <ShaderProgram.hpp>
#include <PixelBufferBase.hpp>

#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Ssaa
{
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "ssaa" ), p_renderTarget, p_renderSystem, p_params )
		, m_samplesCount( 1 )
	{
		String l_count;

		if ( p_params.Get( cuT( "samples_count" ), l_count ) )
		{
			m_samplesCount = string::to_int( l_count );
		}

		Logger::LogInfo( std::stringstream() << "Using SSAA, " << m_samplesCount << " samples" );
		m_ssFrameBuffer = m_renderSystem->CreateFrameBuffer();
		m_ssColorBuffer = m_renderTarget->CreateDynamicTexture( eACCESS_TYPE_READ, eACCESS_TYPE_READ | eACCESS_TYPE_WRITE );
		m_ssColorAttach = m_ssFrameBuffer->CreateAttachment( m_ssColorBuffer );
		m_ssDepthBuffer = m_ssFrameBuffer->CreateDepthStencilRenderBuffer( ePIXEL_FORMAT_DEPTH24 );
		m_ssDepthAttach = m_ssFrameBuffer->CreateAttachment( m_ssDepthBuffer );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoCreate()
	{
		bool l_return = m_ssFrameBuffer->Create( 0 );
		l_return &= m_ssColorBuffer->Create();
		l_return &= m_ssDepthBuffer->Create();
		return l_return;
	}

	void RenderTechnique::DoDestroy()
	{
		m_ssColorBuffer->Destroy();
		m_ssDepthBuffer->Destroy();
		m_ssFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = true;
		m_size = Size( m_renderTarget->GetSize().width() * m_samplesCount, m_renderTarget->GetSize().height() * m_samplesCount );

		m_ssColorBuffer->SetType( eTEXTURE_TYPE_2D );
		m_ssColorBuffer->SetImage( m_size, m_renderTarget->GetPixelFormat() );
		l_return = m_ssColorBuffer->Initialise();

		if ( l_return )
		{
			l_return = m_ssDepthBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			l_return = m_ssFrameBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			l_return = m_ssFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

			if ( l_return )
			{
				l_return = m_ssFrameBuffer->Attach( eATTACHMENT_POINT_COLOUR, m_ssColorAttach, eTEXTURE_TARGET_2D );

				if ( l_return )
				{
					l_return = m_ssFrameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_ssDepthAttach );
				}

				m_ssFrameBuffer->Unbind();
			}
		}

		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		m_ssFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_ssFrameBuffer->DetachAll();
		m_ssFrameBuffer->Unbind();
		m_ssFrameBuffer->Cleanup();
		m_ssColorBuffer->Cleanup();
		m_ssDepthBuffer->Cleanup();
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_ssFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	bool RenderTechnique::DoRender( stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, double p_dFrameTime )
	{
		m_renderTarget->GetDepthStencilState()->Apply();
		m_renderTarget->GetRasteriserState()->Apply();
		return Castor3D::RenderTechnique::DoRender( m_size, p_nodes, p_camera, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		m_ssFrameBuffer->Unbind();
		m_ssFrameBuffer->StretchInto( m_renderTarget->GetFrameBuffer(), Castor::Rectangle( Position( 0, 0 ), m_size ), Castor::Rectangle( Position( 0, 0 ), m_renderTarget->GetSize() ), eBUFFER_COMPONENT_COLOUR, eINTERPOLATION_MODE_LINEAR );
//		GetEngine()->GetRenderSystem()->GetCurrentContext()->RenderTextureToCurrentBuffer( m_renderTarget->GetSize(), m_ssColorBuffer );
		m_renderTarget->GetDepthStencilState()->Apply();
		m_renderTarget->GetRasteriserState()->Apply();
		m_renderTarget->GetFrameBuffer()->Bind();
	}
}
