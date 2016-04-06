#include "DirectRenderTechnique.hpp"

#include <Camera.hpp>
#include <DepthStencilRenderBuffer.hpp>
#include <DepthStencilState.hpp>
#include <FrameBuffer.hpp>
#include <Light.hpp>
#include <MaterialManager.hpp>
#include <OverlayManager.hpp>
#include <PanelOverlay.hpp>
#include <RasteriserState.hpp>
#include <RenderTarget.hpp>
#include <RenderSystem.hpp>
#include <Scene.hpp>
#include <ShaderProgram.hpp>
#include <TextOverlay.hpp>

#include <FontManager.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Direct
{
	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "direct" ), p_renderTarget, p_renderSystem, p_params )
	{
		Logger::LogInfo( "Using Direct rendering" );
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
		return true;
	}

	void RenderTechnique::DoDestroy()
	{
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		return true;
	}

	void RenderTechnique::DoCleanup()
	{
	}

	bool RenderTechnique::DoBeginRender( Scene & p_scene )
	{
		bool l_return = m_frameBuffer.m_frameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_return )
		{
			m_frameBuffer.m_frameBuffer->SetClearColour( p_scene.GetBackgroundColour() );
			m_frameBuffer.m_frameBuffer->Clear();
		}

		return l_return;
	}

	void RenderTechnique::DoRender( stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, uint32_t p_frameTime )
	{
		m_renderTarget->GetDepthStencilState()->Apply();
		Castor3D::RenderTechnique::DoRender( m_size, p_nodes, p_camera, p_frameTime );
	}

	void RenderTechnique::DoEndRender( Scene & p_scene )
	{
		m_frameBuffer.m_frameBuffer->Unbind();
	}
}
