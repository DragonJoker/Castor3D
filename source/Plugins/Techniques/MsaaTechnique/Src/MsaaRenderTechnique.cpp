#include "MsaaRenderTechnique.hpp"

#include <FrameBuffer.hpp>
#include <RasteriserState.hpp>
#include <RenderTarget.hpp>
#include <ColourRenderBuffer.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <RenderBufferAttachment.hpp>
#include <TextureAttachment.hpp>
#include <DepthStencilStateManager.hpp>
#include <RasteriserStateManager.hpp>
#include <RenderSystem.hpp>
#include <Engine.hpp>
#include <Parameter.hpp>
#include <ShaderProgram.hpp>
#include <DynamicTexture.hpp>

#include <Image.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Msaa
{
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "msaa" ), p_renderTarget, p_renderSystem, p_params )
		, m_samplesCount( 0 )
	{
		String l_count;

		if ( p_params.Get( cuT( "samples_count" ), l_count ) )
		{
			m_samplesCount = string::to_int( l_count );
		}

		Logger::LogInfo( StringStream() << cuT( "Using MSAA, " ) << m_samplesCount << cuT( " samples" ) );
		m_msFrameBuffer = m_renderSystem->CreateFrameBuffer();
		m_pMsColorBuffer = m_msFrameBuffer->CreateColourRenderBuffer( m_renderTarget->GetPixelFormat() );
		m_pMsDepthBuffer = m_msFrameBuffer->CreateDepthStencilRenderBuffer( m_renderTarget->GetDepthFormat() );
		m_pMsColorAttach = m_msFrameBuffer->CreateAttachment( m_pMsColorBuffer );
		m_pMsDepthAttach = m_msFrameBuffer->CreateAttachment( m_pMsDepthBuffer );
		RasteriserStateSPtr l_pRasteriser = GetEngine()->GetRasteriserStateManager().Create( cuT( "MSAA_RT" ) );
		l_pRasteriser->SetMultisample( true );
		m_wpMsRasteriserState = l_pRasteriser;
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
		bool l_bReturn = m_msFrameBuffer->Create( m_samplesCount );
		m_pMsColorBuffer->SetSamplesCount( m_samplesCount );
		m_pMsDepthBuffer->SetSamplesCount( m_samplesCount );
		l_bReturn &= m_pMsColorBuffer->Create();
		l_bReturn &= m_pMsDepthBuffer->Create();
		return l_bReturn;
	}

	void RenderTechnique::DoDestroy()
	{
		m_pMsColorBuffer->Destroy();
		m_pMsDepthBuffer->Destroy();
		m_msFrameBuffer->Destroy();
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_bReturn = true;
		m_rect = Castor::Rectangle( Position(), m_size );

		if ( l_bReturn )
		{
			l_bReturn = m_pMsColorBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_pMsDepthBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_msFrameBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

			if ( l_bReturn )
			{
				l_bReturn = m_msFrameBuffer->Attach( eATTACHMENT_POINT_COLOUR, m_pMsColorAttach );

				if ( l_bReturn )
				{
					l_bReturn = m_msFrameBuffer->Attach( eATTACHMENT_POINT_DEPTH, m_pMsDepthAttach );
				}

				m_msFrameBuffer->Unbind();
			}
		}

		return l_bReturn;
	}

	void RenderTechnique::DoCleanup()
	{
		m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_msFrameBuffer->DetachAll();
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->Cleanup();
		m_pMsColorBuffer->Cleanup();
		m_pMsDepthBuffer->Cleanup();
	}

	bool RenderTechnique::DoBeginRender()
	{
		return m_msFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
	}

	bool RenderTechnique::DoRender( stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, double p_dFrameTime )
	{
		m_renderTarget->GetDepthStencilState()->Apply();

		if ( !m_samplesCount )
		{
			m_renderTarget->GetRasteriserState()->Apply();
		}
		else
		{
			m_wpMsRasteriserState.lock()->Apply();
		}

		return Castor3D::RenderTechnique::DoRender( m_size, p_nodes, p_camera, p_dFrameTime );
	}

	void RenderTechnique::DoEndRender()
	{
		m_msFrameBuffer->Unbind();
		m_msFrameBuffer->BlitInto( m_renderTarget->GetFrameBuffer(), m_rect, eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH );
		m_renderTarget->GetFrameBuffer()->Bind();
	}
}
